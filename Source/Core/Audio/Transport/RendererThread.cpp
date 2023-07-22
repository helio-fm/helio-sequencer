/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "RendererThread.h"
#include "Workspace.h"
#include "AudioCore.h"

RendererThread::RendererThread(Transport &transport) :
    Thread("RendererThread"),
    transport(transport) {}

RendererThread::~RendererThread()
{
    this->stop();
}

float RendererThread::getPercentsComplete() const noexcept
{
    return this->percentsDone.get();
}

bool RendererThread::startRendering(const URL &target, RenderFormat format,
    Transport::PlaybackContext::Ptr playbackContext,
    int waveformThumbnailResolution)
{
    this->stop();

    this->format = format;
    this->context = playbackContext;

    this->waveformThumbnail.clearQuick();
    this->waveformThumbnail.resize(waveformThumbnailResolution);

    // keep the url copy alive while rendering,
    // since on iOS it contains a security bookmark:
    this->renderTarget = target;

    try
    {
        if (this->renderTarget.isLocalFile() &&
            this->renderTarget.getLocalFile().exists())
        {
            const auto deleted = this->renderTarget.getLocalFile().deleteFile();
            if (!deleted) {
                // the file exists but is probably inaccessible:
                return false;
            }
        }
    }
    catch (...)
    {
        return false;
    }

    if (auto outStream = this->renderTarget.createOutputStream())
    {
        this->percentsDone = 0.f;
        
        // 16 bits per sample should be enough for anybody :)
        // ..wanna fight about it? https://people.xiph.org/~xiphmont/demo/neil-young.html
        const int bitDepth = 16;

        if (this->format == RenderFormat::WAV)
        {
            WavAudioFormat wavFormat;
            const ScopedLock sl(this->writerLock);
            this->writer.reset(wavFormat.createWriterFor(outStream.release(),
                this->context->sampleRate, this->context->numOutputChannels, bitDepth, {}, 0));
        }
        else if (this->format == RenderFormat::FLAC)
        {
            FlacAudioFormat flacFormat;
            const ScopedLock sl(this->writerLock);
            this->writer.reset(flacFormat.createWriterFor(outStream.release(),
                this->context->sampleRate, this->context->numOutputChannels, bitDepth, {}, 0));
        }

        if (writer != nullptr)
        {
            DBG(this->renderTarget.getLocalFile().getFullPathName());
            this->startThread(9);
        }

        return true;
    }

    return false;
}

void RendererThread::stop()
{
    if (this->isThreadRunning())
    {
        this->stopThread(500);
    }

    {
        const ScopedLock lock(this->writerLock);
        this->writer = nullptr;
    }
}

bool RendererThread::isRendering() const
{
    return this->isThreadRunning();
}

//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

struct RenderBuffer final
{
    Instrument *instrument;
    AudioBuffer<float> sampleBuffer;
    MidiBuffer midiBuffer;
};

void RendererThread::run()
{
    auto sequences = this->transport.buildPlaybackCache(false);
    constexpr auto bufferSize = 512;

    // assuming that number of channels and sample rate is equal for all instruments
    const int numOutChannels = sequences.getNumOutputChannels();
    const int numInChannels = sequences.getNumInputChannels();
    const double sampleRate = sequences.getSampleRate();
    const double totalTimeMs = this->context->totalTimeMs;
    const double msPerQuarter = this->context->startBeatTempo;
    const double totalFrames = totalTimeMs / 1000.0 * sampleRate;
    double secPerQuarter = msPerQuarter / 1000.0;

    // create a list of unique instruments with audio buffers for them
    OwnedArray<RenderBuffer> subBuffers;
    Array<Instrument *> uniqueInstruments;
    uniqueInstruments.addArray(sequences.getUniqueInstruments());

    for (int i = 0; i < uniqueInstruments.size(); ++i)
    {
        Instrument *instrument = uniqueInstruments[i];
        auto *subBuffer = new RenderBuffer();
        subBuffer->instrument = instrument;
        subBuffer->sampleBuffer = AudioBuffer<float>(numOutChannels, bufferSize);
        subBuffers.add(subBuffer);
        //DBG("Adding instrument: " + String(instrument->getName()));
    }

    // release resources, prepare to play, etc
    for (auto *subBuffer : subBuffers)
    {
        auto *graph = subBuffer->instrument->getProcessorGraph();
        graph->setPlayConfigDetails(numInChannels, numOutChannels, sampleRate, bufferSize);
        graph->releaseResources();
        graph->setProcessingPrecision(AudioProcessor::singlePrecision);
        graph->prepareToPlay(graph->getSampleRate(), bufferSize);
        graph->setNonRealtime(true);
    }

    // let the processor graphs handle their async updates
    Thread::sleep(200);

    // the render loop itself
    sequences.seekToStart();
    
    CachedMidiMessage nextMessage;
    bool hasNextMessage = sequences.getNextMessage(nextMessage);
    if (!hasNextMessage)
    {
        jassertfalse;
        return;
    }
    
    // TODO: add double precision rendering someday (for processor graphs who support it)
    AudioBuffer<float> mixingBuffer(numOutChannels, bufferSize);
    
    const auto firstEventTimestamp = nextMessage.message.getTimeStamp();

    double prevEventTimeStamp = firstEventTimestamp;
    double prevEventTick = firstEventTimestamp * secPerQuarter;
    double nextEventTick = prevEventTick;
    double nextEventTickDelta = 0.0;

    const double firstFrame = prevEventTick * sampleRate;
    const double lastFrame = firstFrame + totalFrames;

    double currentFrame = firstFrame;
    int messageFrame = 0;

    // send MidiStart to everyone
    auto midiStart = MidiMessage::midiStart();
    midiStart.setTimeStamp(firstEventTimestamp);
    for (auto *subBuffer : subBuffers)
    {
        subBuffer->midiBuffer.addEvent(midiStart, messageFrame);
    }

    while (currentFrame < lastFrame)
    {
        if (this->threadShouldExit())
        {
            break;
        }
        
        // fill up the midi buffers
        while (hasNextMessage &&
            (nextEventTick * sampleRate) >= currentFrame &&
            (nextEventTick * sampleRate) < (currentFrame + bufferSize))
        {
            // basically a sample number, which needs to be in range [0 .. bufferSize)
            messageFrame = int((nextEventTick * sampleRate) - currentFrame);

            if (nextMessage.message.isTempoMetaEvent())
            {
                secPerQuarter = nextMessage.message.getTempoSecondsPerQuarterNote();

                // send this to everybody (need to do that for drum-machines) - TODO test
                for (auto *subBuffer : subBuffers)
                {
                    subBuffer->midiBuffer.addEvent(nextMessage.message, messageFrame);
                }
            }
            else
            {
                for (auto *subBuffer : subBuffers)
                {
                    if (nextMessage.instrument == subBuffer->instrument)
                    {
                        subBuffer->midiBuffer.addEvent(nextMessage.message, messageFrame);
                    }
                }
            }

            prevEventTick += nextEventTickDelta;
            prevEventTimeStamp = nextMessage.message.getTimeStamp();
            
            hasNextMessage = sequences.getNextMessage(nextMessage);
            nextEventTickDelta = (nextMessage.message.getTimeStamp() - prevEventTimeStamp) * secPerQuarter;
            nextEventTick = prevEventTick + nextEventTickDelta;
        }

        // call processBlock for every instrument
        for (auto *subBuffer : subBuffers)
        {
            auto *graph = subBuffer->instrument->getProcessorGraph();
            {
                const ScopedLock lock(graph->getCallbackLock());
                
                graph->processBlock(subBuffer->sampleBuffer, subBuffer->midiBuffer);
                subBuffer->midiBuffer.clear();
                //Thread::yield();
            }
        }

        // mix them down to the render buffer
        mixingBuffer.clear();

        for (auto *subBuffer : subBuffers)
        {
            for (int channel = 0; channel < numOutChannels; ++channel)
            {
                mixingBuffer.addFrom(channel, 0,
                    subBuffer->sampleBuffer, channel, 0,
                    bufferSize,
                    1.f);
            }
        }

        // write the resulting buffer to disk
        // (make a few attempts and bail out if failed)
        {
            const ScopedLock lock(this->writerLock);
            
            bool writtenSuccessfully = false;
            for (int i = 0; i < 10 && !writtenSuccessfully; ++i)
            {
                writtenSuccessfully =
                    this->writer->writeFromAudioSampleBuffer(mixingBuffer, 0, mixingBuffer.getNumSamples());
            }

            if (! writtenSuccessfully)
            {
                jassert(false);
                break;
            }
        }

        // finally, update counters
        currentFrame += bufferSize;

        const auto peakLevel = mixingBuffer.getMagnitude(0, mixingBuffer.getNumSamples());

        this->percentsDone = float((currentFrame - firstFrame) / totalFrames);

        jassert(this->waveformThumbnail.size() > 0);
        const auto waveformFrameIndex = int(float(this->waveformThumbnail.size() - 1) * this->percentsDone.get());
        if (waveformFrameIndex >= 0 && waveformFrameIndex < this->waveformThumbnail.size())
        {
            this->waveformThumbnail.set(waveformFrameIndex, jmax(this->waveformThumbnail[waveformFrameIndex], peakLevel));
        }

        // DBG("% done: " + String(this->percentsDone.get()));
    }

    // setNonRealtime false
    for (auto *subBuffer : subBuffers)
    {
        auto *graph = subBuffer->instrument->getProcessorGraph();
        graph->setNonRealtime(false);
        graph->reset();
        graph->releaseResources();
    }

    // some plugins tend to make a weird post-rendering "tail" sound,
    // and here is an attepmt to fix that by giving them time to do
    // whatever processing they need to do after resetting
    Thread::sleep(200);
    
    {
        const ScopedLock sl(this->writerLock);
        this->writer = nullptr;
    }

    // dispose the URL object, so that its security bookmark can be released by iOS
    this->renderTarget = {};

    App::Workspace().getAudioCore().setAwake();
}

const Array<float, CriticalSection> &RendererThread::getWaveformThumbnail() const
{
    return this->waveformThumbnail;
}
