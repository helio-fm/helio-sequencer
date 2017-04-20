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
#include "TransportListener.h"
#include "Arpeggiator.h"
#include "CommandIDs.h"

class PianoRoll;
class ProjectTreeItem;
class CommandItemComponent;
class SourceCodeEditor;
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Common/PlayButton.h"
#include "../Themes/ShadowDownwards.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Themes/SeparatorHorizontal.h"

class ArpeggiatorEditorPanel  : public Component,
                                public ChangeBroadcaster,
                                private TransportListener,
                                private ChangeListener,
                                public ButtonListener,
                                public ComboBoxListener,
                                public LabelListener
{
public:

    ArpeggiatorEditorPanel (ProjectTreeItem &parentProject, PianoRoll &targetRoll);

    ~ArpeggiatorEditorPanel();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void labelTextChanged (Label* labelThatHasChanged) override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;


private:

    //[UserVariables]

    void reloadData();
    void updateControls(const Arpeggiator &arp);
    void applyArpToSelectedNotes(const Arpeggiator &arp);
    void updateCurrentArp();
    void syncDataToManager();

    PianoRoll &roll;
    ProjectTreeItem &project;

    CodeDocument document;
    Arpeggiator currentArp;

    bool hasMadeChanges;

    //===----------------------------------------------------------------------===//
    // TransportListener
    //

    void onSeek(const double newPosition, const double currentTimeMs, const double totalTimeMs) override;
    void onTempoChanged(const double newTempo) override;
    void onTotalTimeChanged(const double timeMs) override;
    void onPlay() override;
    void onStop() override;

    //===----------------------------------------------------------------------===//
    // ChangeListener
    //

    void changeListenerCallback(ChangeBroadcaster *source) override;

    XmlTokeniser tokeniser;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> bg;
    ScopedPointer<CodeEditorComponent> editor;
    ScopedPointer<CommandItemComponent> resetButton;
    ScopedPointer<PlayButton> playButton;
    ScopedPointer<ShadowDownwards> shadowDown;
    ScopedPointer<SeparatorHorizontal> s2;
    ScopedPointer<ToggleButton> reverseToggleButton;
    ScopedPointer<ComboBox> arpsList;
    ScopedPointer<ToggleButton> relativeMappingToggleButton;
    ScopedPointer<ToggleButton> limitToChordToggleButton;
    ScopedPointer<SeparatorHorizontal> s1;
    ScopedPointer<Label> label;
    ScopedPointer<ComboBox> scaleList;
    ScopedPointer<TextButton> addArpButton;
    ScopedPointer<SeparatorHorizontal> s3;
    ScopedPointer<Label> nameLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArpeggiatorEditorPanel)
};
