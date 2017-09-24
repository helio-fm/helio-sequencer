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
#include "AudioMonitor.h"
#include "AudioCore.h"
#include "AudiobusOutput.h"

#define AUDIO_MONITOR_SPECTRUM_SIZE                 512
#define AUDIO_MONITOR_DEFAULT_SAMPLERATE            44100
#define AUDIO_MONITOR_CLIP_THRESHOLD                0.995f
#define AUDIO_MONITOR_OVERSATURATION_THRESHOLD      0.5f
#define AUDIO_MONITOR_OVERSATURATION_RATE           4.f

class ClippingWarningAsyncCallback : public AsyncUpdater
{
public:
    
    explicit ClippingWarningAsyncCallback(AudioMonitor &parentSpectrumCallback) :
    AudioMonitor(parentSpectrumCallback) {}
    
    void handleAsyncUpdate() override
    {
        this->AudioMonitor.getListeners().
        call(&AudioMonitor::ClippingListener::onClippingWarning);
    }

private:
    
    AudioMonitor &AudioMonitor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClippingWarningAsyncCallback)
};

class OversaturationWarningAsyncCallback : public AsyncUpdater
{
public:
    
    explicit OversaturationWarningAsyncCallback(AudioMonitor &parentSpectrumCallback) :
    AudioMonitor(parentSpectrumCallback) {}
    
    void handleAsyncUpdate() override
    {
        this->AudioMonitor.getListeners().
        call(&AudioMonitor::ClippingListener::onOversaturationWarning);
    }
    
private:
    
    AudioMonitor &AudioMonitor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OversaturationWarningAsyncCallback)
};

AudioMonitor::AudioMonitor() :
    fft(),
    spectrumSize(AUDIO_MONITOR_SPECTRUM_SIZE),
    sampleRate(AUDIO_MONITOR_DEFAULT_SAMPLERATE)
{
    zeromem(this->spectrum, sizeof(float) * AUDIO_MONITOR_MAX_CHANNELS * AUDIO_MONITOR_MAX_SPECTRUMSIZE);
    this->asyncClippingWarning = new ClippingWarningAsyncCallback(*this);
    this->asyncOversaturationWarning = new OversaturationWarningAsyncCallback(*this);
}

AudioMonitor::~AudioMonitor()
{
    this->masterReference.clear();
}

//===----------------------------------------------------------------------===//
// AudioIODeviceCallback
//===----------------------------------------------------------------------===//

void AudioMonitor::audioDeviceAboutToStart(AudioIODevice *device)
{
    this->sampleRate = device->getCurrentSampleRate();
}

void AudioMonitor::audioDeviceIOCallback(const float **inputChannelData,
                                         int numInputChannels,
                                         float **outputChannelData,
                                         int numOutputChannels,
                                         int numSamples)
{
    const int numChannels =
    jmin(AUDIO_MONITOR_MAX_CHANNELS, numOutputChannels);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        this->fft.computeSpectrum(outputChannelData[channel], 0, numSamples,
                                  this->spectrum[channel], this->spectrumSize,
                                  channel, numOutputChannels);
    }
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
#if AUDIO_MONITOR_COMPUTES_RMS
        float pcmSquaresSum = 0.f;
#endif
        
        float pcmPeak = 0.f;
        for (int samplePosition = 0; samplePosition < numSamples; ++samplePosition)
        {
            const float &pcmData = outputChannelData[channel][samplePosition];
            
#if AUDIO_MONITOR_COMPUTES_RMS
            pcmSquaresSum += (pcmData * pcmData);
#endif
            
            pcmPeak = jmax(pcmPeak, pcmData);
        }
        
#if AUDIO_MONITOR_COMPUTES_RMS
        const float rootMeanSquare = sqrtf(pcmSquaresSum / numSamples);
        this->rms[channel] = rootMeanSquare;
#endif

        this->peak[channel] = pcmPeak;
        
        if (pcmPeak > AUDIO_MONITOR_CLIP_THRESHOLD)
        {
            this->asyncClippingWarning->triggerAsyncUpdate();
        }
        
#if AUDIO_MONITOR_COMPUTES_RMS
        if (pcmPeak > AUDIO_MONITOR_OVERSATURATION_THRESHOLD &&
            (pcmPeak / rootMeanSquare) > AUDIO_MONITOR_OVERSATURATION_RATE)
        {
            this->asyncOversaturationWarning->triggerAsyncUpdate();
        }
#endif
    }
    
#if JUCE_IOS && HELIO_AUDIOBUS_SUPPORT
    AudiobusOutput::process();
#endif
    
    for (int i = 0; i < numOutputChannels; ++i)
    {
        FloatVectorOperations::clear(outputChannelData[i], numSamples);
    }
}

void AudioMonitor::audioDeviceStopped()
{
}

//===----------------------------------------------------------------------===//
// Spectrum data
//===----------------------------------------------------------------------===//

float AudioMonitor::getInterpolatedSpectrumAtFrequency(float frequency) const
{
    const float resolution = float(this->sampleRate / 2.f) / float(this->spectrumSize);
    
    const int index1 = roundFloatToInt(frequency / resolution);
    const int safeIndex1 = jlimit(0, this->spectrumSize, index1);
    const float f1 = index1 * resolution;
    const float y1 = (this->spectrum[0][safeIndex1].get() +
                      this->spectrum[1][safeIndex1].get()) / 2.f;
    
    const int index2 = index1 + 1;
    const int safeIndex2 = jlimit(0, this->spectrumSize, index2);
    const float f2 = index2 * resolution;
    const float y2 = (this->spectrum[0][safeIndex2].get() +
                      this->spectrum[1][safeIndex2].get()) / 2.f;
    
    return y1 + ((AudioCore::fastLog10(frequency) - AudioCore::fastLog10(f1)) /
                 (AudioCore::fastLog10(f2) - AudioCore::fastLog10(f1))) * (y2 - y1);
}

//===----------------------------------------------------------------------===//
// Clipping data
//===----------------------------------------------------------------------===//

void AudioMonitor::addClippingListener(ClippingListener *const listener)
{
    this->clippingListeners.add(listener);
}

void AudioMonitor::removeClippingListener(ClippingListener *const listener)
{
    this->clippingListeners.remove(listener);
}

ListenerList<AudioMonitor::ClippingListener> &AudioMonitor::getListeners() noexcept
{
    return this->clippingListeners;
}

//===----------------------------------------------------------------------===//
// Volume data
//===----------------------------------------------------------------------===//

float AudioMonitor::getPeak(int channel) const
{
    return this->peak[channel].get();
}

#if AUDIO_MONITOR_COMPUTES_RMS
float AudioMonitor::getRootMeanSquare(int channel) const
{
    return this->rms[channel].get();
}
#endif
