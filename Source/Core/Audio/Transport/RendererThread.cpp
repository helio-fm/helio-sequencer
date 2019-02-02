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
#include "Instrument.h"
#include "SerializationKeys.h"
#include "Workspace.h"
#include "AudioCore.h"

RendererThread::RendererThread(Transport &parentTrasport) :
    Thread("RendererThread"),
    transport(parentTrasport),
    writer(nullptr),
    percentsDone(0.f) {}

RendererThread::~RendererThread()
{
    this->stop();
}

float RendererThread::getPercentsComplete() const
{
    const ScopedReadLock lock(this->percentsLock);
    return this->percentsDone;
}

void RendererThread::startRecording(const File &file)
{
    this->transport.rebuildSequencesIfNeeded();
    const ProjectSequences sequences = this->transport.getSequences();
    
    if (sequences.empty())
    {
        return;
    }

    this->stop();

    double sampleRate = sequences.getSampleRate();
    int numChannels = sequences.getNumOutputChannels();

    // Create an OutputStream to write to our destination file...
    file.deleteFile();
    ScopedPointer<FileOutputStream> fileStream(file.createOutputStream());

    if (fileStream != nullptr)
    {
        {
            const ScopedWriteLock pl(this->percentsLock);
            this->percentsDone = 0.f;
        }
        
        // 16 bits per sample should be enough for anybody :)
        // ..wanna fight about it? https://people.xiph.org/~xiphmont/demo/neil-young.html
        const int bitDepth = 16;

        if (file.getFileExtension().endsWithIgnoreCase("wav"))
        {
            WavAudioFormat wavFormat;
            const ScopedLock sl(this->writerLock);
            this->writer = wavFormat.createWriterFor(fileStream, sampleRate, numChannels, bitDepth, {}, 0);
        }
        else if (file.getFileExtension().endsWithIgnoreCase("flac"))
        {
            FlacAudioFormat flacFormat;
            const ScopedLock sl(this->writerLock);
            this->writer = flacFormat.createWriterFor(fileStream, sampleRate, numChannels, bitDepth, {}, 0);
        }

        if (writer != nullptr)
        {
            DBG(file.getFullPathName());
            fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)
            this->startThread(9);
        }
    }
}

void RendererThread::stop()
{
    if (this->isThreadRunning())
    {
        this->stopThread(500);
    }

    {
        const ScopedLock sl(this->writerLock);
        this->writer = nullptr;
    }
}

bool RendererThread::isRecording() const
{
    //return (this->writer != nullptr) && this->isThreadRunning();
    return this->isThreadRunning();
}

//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

struct RenderBuffer final
{
    Instrument *instrument;
    AudioSampleBuffer sampleBuffer;
    MidiBuffer midiBuffer;
};

void RendererThread::run()
{
    // step 0. init.
    this->transport.rebuildSequencesIfNeeded();
    ProjectSequences sequences = this->transport.getSequences();
    const int bufferSize = 512;

    // assuming that number of channels and sample rate is equal for all instruments
    const int numOutChannels = sequences.getNumOutputChannels();
    const int numInChannels = sequences.getNumInputChannels();
    const double sampleRate = sequences.getSampleRate();
    
    double totalTimeMs = 0.0;
    double tempoAtTheEndOfTrack = 0.0;
    this->transport.calcTimeAndTempoAt(1.0, totalTimeMs, tempoAtTheEndOfTrack);
    
    double startTimeMs = 0.0;
    double msPerQuarter = 0.0;
    this->transport.calcTimeAndTempoAt(0.0, startTimeMs, msPerQuarter);
    double secPerQuarter = msPerQuarter / 1000.0;

    double currentFrame = 0.0;
    const double lastFrame = totalTimeMs / 1000.0 * sampleRate;

    // step 1. create a list of unique instruments with audio buffers for them.
    OwnedArray<RenderBuffer> subBuffers;
    Array<Instrument *> uniqueInstruments(sequences.getUniqueInstruments());

    for (int i = 0; i < uniqueInstruments.size(); ++i)
    {
        Instrument *instrument = uniqueInstruments[i];
        auto subBuffer = new RenderBuffer();
        subBuffer->instrument = instrument;
        subBuffer->sampleBuffer = AudioSampleBuffer(numOutChannels, bufferSize);
        subBuffers.add(subBuffer);
        //DBG("Adding instrument: " + String(instrument->getName()));
    }

    // step 2. release resources, prepare to play, etc.
    for (auto *subBuffer : subBuffers)
    {
        AudioProcessorGraph *graph = subBuffer->instrument->getProcessorGraph();
        graph->setPlayConfigDetails(numInChannels, numOutChannels, sampleRate, bufferSize);
        graph->releaseResources();
        // TODO:
        //graph->setProcessingPrecision(AudioProcessor::singlePrecision);
        graph->prepareToPlay(graph->getSampleRate(), bufferSize);
        graph->setNonRealtime(true);
    }

    // let processor graphs call handle their async updates
    Thread::sleep(200);

    // step 3. render loop itself.
    sequences.seekToTime(0.0);
    
    MessageWrapper nextMessage;
    bool hasNextMessage = sequences.getNextMessage(nextMessage);
    jassert(hasNextMessage);
    
    // TODO: add double precision rendering someday (for processor graphs who support it)
    AudioSampleBuffer mixingBuffer(numOutChannels, bufferSize);
    
    double lastEventTick = 0.0;
    double prevEventTimeStamp = 0.0;
    double nextEventTickDelta = (nextMessage.message.getTimeStamp() - prevEventTimeStamp) * secPerQuarter;
    double nextEventTick = lastEventTick + nextEventTickDelta;

    int messageFrame = int((nextEventTick * sampleRate) - currentFrame);

    // And here we go: send MidiStart
    for (auto *subBuffer : subBuffers)
    {
        subBuffer->midiBuffer.addEvent(MidiMessage::midiStart(), messageFrame);
    }

    while (currentFrame < lastFrame)
    {
        if (this->threadShouldExit())
        {
            break;
        }
        
        // step 3a. fill up the midi buffers.
        while (hasNextMessage &&
               ( (nextEventTick * sampleRate) >= currentFrame &&
                 (nextEventTick * sampleRate) < (currentFrame + bufferSize) ))
        {
            messageFrame = int((nextEventTick * sampleRate) - currentFrame);

            if (nextMessage.message.isTempoMetaEvent())
            {
                secPerQuarter = nextMessage.message.getTempoSecondsPerQuarterNote();

                // Sends this to everybody (need to do that for drum-machines) - TODO test
                for (auto subBuffer : subBuffers)
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
                        //DBG("Adding message with frame " + String(messageFrame));
                        subBuffer->midiBuffer.addEvent(nextMessage.message, messageFrame);
                    }
                }
            }

            lastEventTick += nextEventTickDelta;
            prevEventTimeStamp = nextMessage.message.getTimeStamp();
            
            hasNextMessage = sequences.getNextMessage(nextMessage);
            nextEventTickDelta = (nextMessage.message.getTimeStamp() - prevEventTimeStamp) * secPerQuarter;
            nextEventTick = lastEventTick + nextEventTickDelta;
        }

        // step 3b. call processBlock for every instrument.
        for (auto subBuffer : subBuffers)
        {
            AudioProcessorGraph *graph = subBuffer->instrument->getProcessorGraph();
            {
                const ScopedLock lock(graph->getCallbackLock());
                
                //DBG("processBlock num midi events: " + String(subBuffer->midiBuffer.getNumEvents()));
                graph->processBlock(subBuffer->sampleBuffer, subBuffer->midiBuffer);
                subBuffer->midiBuffer.clear();

                //Thread::yield();
            }
        }

        // step 3c. mix them down to the render buffer.
        mixingBuffer.clear();

        for (auto subBuffer : subBuffers)
        {
            for (int j = 0; j < numOutChannels; ++j)
            {
                mixingBuffer.addFrom(j, 0,
                    subBuffer->sampleBuffer, j, 0,
                    bufferSize,
                    1.0f);
            }
        }

        // step 3d. write resulting buffer to disk.
        {
            const ScopedLock sl(this->writerLock);
            bool writedSuccessfullty = false;
            
            while (! writedSuccessfullty)
            {
                writedSuccessfullty =
                this->writer->writeFromAudioSampleBuffer(mixingBuffer, 0, mixingBuffer.getNumSamples());
            }
        }

        // step 3e. finally, update counters.
        currentFrame += bufferSize;

        {
            const ScopedWriteLock pl(this->percentsLock);
            this->percentsDone = float(currentFrame / lastFrame);
            //DBG("this->percentsDone : " + String(this->percentsDone));
        }
    }

    // step 4. setNonRealtime false.
    for (auto subBuffer : subBuffers)
    {
        AudioProcessorGraph *graph = subBuffer->instrument->getProcessorGraph();
        graph->setNonRealtime(false);
    }
    
    {
        const ScopedLock sl(this->writerLock);
        this->writer = nullptr;
    }
    
    if (! this->threadShouldExit())
    {
        // dirty hack
        App::Workspace().getAudioCore().unmute();
        App::Workspace().getAudioCore().unmute();
    }
}
