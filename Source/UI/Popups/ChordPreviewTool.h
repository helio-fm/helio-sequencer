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

class Transport;
class PianoRoll;
class MidiSequence;
class PianoSequence;
class KeySignaturesSequence;

#include "Note.h"
#include "Clip.h"
#include "Chord.h"
#include "Scale.h"
#include "PopupMenuComponent.h"
#include "PopupCustomButton.h"

class ChordPreviewTool final : public PopupMenuComponent,
                               public PopupButtonOwner
{
public:

    ChordPreviewTool(PianoRoll &caller, WeakReference<PianoSequence> target,
        const Clip &clip, WeakReference<KeySignaturesSequence> harmonicContext);

    ~ChordPreviewTool();

    void onPopupsResetState(PopupButton *button) override;

    void onPopupButtonFirstAction(PopupButton *button) override;
    void onPopupButtonSecondAction(PopupButton *button) override;

    void onPopupButtonStartDragging(PopupButton *) override {}
    bool onPopupButtonDrag(PopupButton *button) override;
    void onPopupButtonEndDragging(PopupButton *) override {}

    void resized() override;
    void parentHierarchyChanged() override;
    void handleCommandMessage(int commandId) override;
    bool keyPressed(const KeyPress &key) override;
    void inputAttemptWhenModal() override;

private:

    const PianoRoll &roll;
    WeakReference<PianoSequence> sequence;
    const Clip clip;

    bool hasMadeChanges = false;
    void undoChangesIfAny();

    static constexpr auto noteLength = float(Globals::beatsPerBar);
    static constexpr auto noteVelocity = 0.35f;

    // detected on the fly as the user drags the tool around:
    int targetKey = 0;
    float targetBeat = 0;
    Note::Key root;
    Scale::Ptr scale;
    // using project context:
    WeakReference<KeySignaturesSequence> harmonicContext;
    bool detectKeyBeatAndContext();

    Scale::Ptr defaultScale;
    Array<Chord::Ptr> defaultChords;

    OwnedArray<PopupCustomButton> chordButtons;

    Chord::Ptr findChordFor(PopupButton *button) const;
    void buildChord(const Chord::Ptr chord);
    void buildNewNote(bool shouldSendMidiMessage);

    void stopSound();

    UniquePointer<PopupCustomButton> newChord;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordPreviewTool)
};
