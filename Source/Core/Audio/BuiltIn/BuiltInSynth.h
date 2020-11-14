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

class BuiltInSynth final : public Synthesiser
{
public:

    BuiltInSynth();

    // what we want here is to make all built-in temperaments
    // work out of the box with the built-in instrument, so that
    // all features are easily previewed even before the user
    // sets up any instruments and keyboard mappings;
    // for that we need to let the instrument know which temperament
    // the project is currently in, and even simpler - we only need
    // an octave size, because this is what matters from the sequencer's
    // perspective, and we will only support EDOs, because all built-in
    // temperaments and EDO's; hopefully someday I'll come up with
    // a better approach, or just get rid of this hack;
    void setPeriodSize(int periodSize);

protected:

    void handleSustainPedal(int midiChannel, bool isDown) override;
    void handleSostenutoPedal(int midiChannel, bool isDown) override;

    static constexpr auto numVoices = 16;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BuiltInSynth)
};
