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

#pragma once

#include "SpectrumAnalyzer.h"

// 256 == we don't need that high resolution on a spectrum
#define AUDIO_MONITOR_SPECTRUM_SIZE                 256
#define AUDIO_MONITOR_NUM_CHANNELS                  2
#define AUDIO_MONITOR_SAMPLE_RATE                   44100
#define AUDIO_MONITOR_CLIP_THRESHOLD                0.995f
#define AUDIO_MONITOR_OVERSATURATION_THRESHOLD      0.5f
#define AUDIO_MONITOR_OVERSATURATION_RATE           4.f

class AudioMonitor final : public AudioIODeviceCallback
{
public:
    
    AudioMonitor();

    //===------------------------------------------------------------------===//
    // AudioIODeviceCallback
    //===------------------------------------------------------------------===//

    void audioDeviceAboutToStart(AudioIODevice *device) override;
    void audioDeviceIOCallback(const float **inputChannelData, int numInputChannels,
        float **outputChannelData, int numOutputChannels, int numSamples) override;
    void audioDeviceStopped() override;
    
    //===------------------------------------------------------------------===//
    // Clipping warnings
    //===------------------------------------------------------------------===//
    
    class ClippingListener
    {
    public:
        virtual ~ClippingListener() {}
        virtual void onClippingWarning() = 0;
        virtual void onOversaturationWarning() = 0;
    };
    
    void addClippingListener(ClippingListener *const listener);
    void removeClippingListener(ClippingListener *const listener);
    ListenerList<ClippingListener> &getListeners() noexcept;

    //===------------------------------------------------------------------===//
    // Volume data
    //===------------------------------------------------------------------===//
    
    float getPeak(int channel) const;
    float getRootMeanSquare(int channel) const;
    
    //===------------------------------------------------------------------===//
    // Spectrum data
    //===------------------------------------------------------------------===//
    
    float getInterpolatedSpectrumAtFrequency(float frequency) const;
    
private:

    SpectrumFFT fft;

    Atomic<float> spectrum[AUDIO_MONITOR_NUM_CHANNELS][AUDIO_MONITOR_SPECTRUM_SIZE];
    Atomic<float> peak[AUDIO_MONITOR_NUM_CHANNELS];
    Atomic<float> rms[AUDIO_MONITOR_NUM_CHANNELS];

    Atomic<int> spectrumSize;
    Atomic<double> sampleRate;

    ListenerList<ClippingListener> clippingListeners;

    UniquePointer<AsyncUpdater> asyncClippingWarning;
    UniquePointer<AsyncUpdater> asyncOversaturationWarning;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMonitor)
    JUCE_DECLARE_WEAK_REFERENCEABLE(AudioMonitor)
};
