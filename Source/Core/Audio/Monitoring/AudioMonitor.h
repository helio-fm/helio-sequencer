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

#define AUDIO_MONITOR_MAX_CHANNELS      2
#define AUDIO_MONITOR_MAX_SPECTRUMSIZE  512

class AudioMonitor final : public AudioIODeviceCallback
{
public:
    
    AudioMonitor();

    //===------------------------------------------------------------------===//
    // AudioIODeviceCallback
    //===------------------------------------------------------------------===//

    void audioDeviceAboutToStart(AudioIODevice *device) override;
    void audioDeviceIOCallback(const float **inputChannelData,
                               int numInputChannels,
                               float **outputChannelData,
                               int numOutputChannels,
                               int numSamples) override;
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

    Atomic<float> spectrum[AUDIO_MONITOR_MAX_CHANNELS][AUDIO_MONITOR_MAX_SPECTRUMSIZE];
    Atomic<float> peak[AUDIO_MONITOR_MAX_CHANNELS];
    Atomic<float> rms[AUDIO_MONITOR_MAX_CHANNELS];

    Atomic<int> spectrumSize;
    Atomic<double> sampleRate;

    ListenerList<ClippingListener> clippingListeners;

    ScopedPointer<AsyncUpdater> asyncClippingWarning;
    ScopedPointer<AsyncUpdater> asyncOversaturationWarning;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMonitor)
    JUCE_DECLARE_WEAK_REFERENCEABLE(AudioMonitor)
};
