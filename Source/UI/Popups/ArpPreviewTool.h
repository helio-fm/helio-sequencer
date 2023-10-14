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

class Lasso;
class MidiTrack;
class PianoRoll;

#include "Arpeggiator.h"
#include "MenuPanel.h"
#include "Scale.h"
#include "Note.h"

class ArpPreviewTool final : public MenuPanel
{
public:

    ArpPreviewTool(PianoRoll &roll, Note::Key keyContext,
        Scale::Ptr scaleContext, bool advancedMode);

    static ArpPreviewTool *createWithinContext(PianoRoll &roll,
        WeakReference<MidiTrack> keySignatures);

    void handleCommandMessage(int commandId) override;

private:

    void dismissAsync();
    void undoIfNeeded();

    MenuPanel::Menu mainMenu;
    MenuPanel::Menu createOptionsMenu(Arpeggiator::Ptr arp);

    struct Options final
    {
        bool reversed = false;
        bool limitToChord = false;
        float randomness = 0.f;
        float durationMultiplier = 1.f;
    };

    void previewArp(Arpeggiator::Ptr arp, const Options options, bool forceRecreate);

    PianoRoll &roll;

    Note::Key keyContext;
    Scale::Ptr scaleContext;

    bool advancedMode = false;

    bool hasMadeChanges = false;
    Arpeggiator::Ptr lastChosenArp = nullptr;
    Options lastOptions;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArpPreviewTool)
};
