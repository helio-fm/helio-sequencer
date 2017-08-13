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

//[Headers]
class NoteComponent;
class MidiSequence;
class PianoRoll;

#include "PopupMenuComponent.h"
//[/Headers]

#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"
#include "PopupCustomButton.h"

class NoNotesPopup  : public PopupMenuComponent,
                      public PopupButtonOwner
{
public:

    NoNotesPopup (PianoRoll *caller, MidiSequence *layer);

    ~NoNotesPopup();

    //[UserMethods]

    void onPopupsResetState(PopupButton *button) override;

    void onPopupButtonFirstAction(PopupButton *button) override;
    void onPopupButtonSecondAction(PopupButton *button) override;

    void onPopupButtonStartDragging(PopupButton *button) override;
    bool onPopupButtonDrag(PopupButton *button) override;
    void onPopupButtonEndDragging(PopupButton *button) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;
    void inputAttemptWhenModal() override;

    // Binary resources:
    static const char* acute_heptagram_svg;
    static const int acute_heptagram_svgSize;

private:

    //[UserVariables]

    PianoRoll *roll;
    MidiSequence *targetLayer;
    int targetKey;
    float targetBeat;
    Point<int> draggingStartPosition;
    Point<int> draggingEndPosition;
    bool detectKeyAndBeat(); // returns true if key changes

    bool hasMadeChanges;
    void buildChord(int n1, int n2, int n3);
    void buildNewNote(bool shouldSendMidiMessage);
    void cancelChangesIfAny();

	void stopSound();
	void sendMidiMessage(const MidiMessage &message);

    //[/UserVariables]

    ScopedPointer<PopupCustomButton> chordMinor1;
    ScopedPointer<PopupCustomButton> chordMajor1;
    ScopedPointer<PopupCustomButton> chordMinor2;
    ScopedPointer<PopupCustomButton> chordMajor2;
    ScopedPointer<PopupCustomButton> chordMinor3;
    ScopedPointer<PopupCustomButton> chordMajor3;
    ScopedPointer<PopupCustomButton> chordMinor4;
    ScopedPointer<PopupCustomButton> chordMajor4;
    ScopedPointer<PopupCustomButton> chordMinor5;
    ScopedPointer<PopupCustomButton> chordMajor5;
    ScopedPointer<PopupCustomButton> chordMinor6;
    ScopedPointer<PopupCustomButton> chordMajor6;
    ScopedPointer<PopupCustomButton> chordMinor7;
    ScopedPointer<PopupCustomButton> newNote;
    ScopedPointer<Label> labelI;
    ScopedPointer<Label> labelII;
    ScopedPointer<Label> labelIII;
    ScopedPointer<Label> labelIV;
    ScopedPointer<Label> labelV;
    ScopedPointer<Label> labelVI;
    ScopedPointer<Label> labelVII;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoNotesPopup)
};
