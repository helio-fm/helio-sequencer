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

#include "BuiltInSynthAudioPlugin.h"

// A lightweight piano sampler with the only purpose of providing a default instrument
// that doesn't sound too much crappy when user opens the app at the very first time,
// and doesn't have any custom instruments added yet.
// So it's as simple and small as possible.

class BuiltInSynthPiano : public BuiltInSynthAudioPlugin
{
public:

    explicit BuiltInSynthPiano();

    const String getName() const override;
    void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages) override;
    void reset() override;

protected:

    void initVoices() override;
    void initSampler() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BuiltInSynthPiano)
};
