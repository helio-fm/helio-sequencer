/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

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
    void audioDeviceStopped() override {}
    
    //===------------------------------------------------------------------===//
    // Clipping warnings
    //===------------------------------------------------------------------===//
    
    class ClippingListener
    {
    public:
        virtual ~ClippingListener() = default;
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
        
private:

    static constexpr auto numChannels = 2;
    static constexpr auto defaultSampleRate = 44100;
    static constexpr auto clipThreshold = 0.995f;
    static constexpr auto oversaturationThreshold = 0.5f;
    static constexpr auto oversaturationRate = 4.f;

    Atomic<float> peak[numChannels];
    Atomic<float> rms[numChannels];

    Atomic<double> sampleRate = defaultSampleRate;

    ListenerList<ClippingListener> clippingListeners;

    UniquePointer<AsyncUpdater> asyncClippingWarning;
    UniquePointer<AsyncUpdater> asyncOversaturationWarning;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMonitor)
    JUCE_DECLARE_WEAK_REFERENCEABLE(AudioMonitor)
};
