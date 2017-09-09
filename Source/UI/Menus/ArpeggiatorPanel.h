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
#include "CommandIDs.h"

class Arpeggiator;
class PianoRoll;
class Transport;
class CommandItemComponent;
class SourceCodeEditor;
struct CommandItem;
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Common/PlayButton.h"
#include "../Themes/ShadowDownwards.h"

class ArpeggiatorPanel  : public Component,
                          public ChangeBroadcaster,
                          private TransportListener,
                          private ChangeListener,
                          private ListBoxModel
{
public:

    ArpeggiatorPanel (Transport &targetTransport, PianoRoll &targetRoll);

    ~ArpeggiatorPanel();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;


private:

    //[UserVariables]

    PianoRoll &roll;
    Transport &transport;

    ReferenceCountedArray<CommandItem> commandDescriptions;

    bool hasMadeChanges;
    int lastSelectedArpIndex;

    void applyArpToSelectedNotes(const Arpeggiator &arp);
    void reloadList(int selectedRow = -1);

    void startPlaybackLoop() const;
    void stopPlaybackLoop() const;


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

    //===----------------------------------------------------------------------===//
    // ListBoxModel
    //

    int getNumRows() override;

    void paintListBoxItem(int rowNumber,
                                  Graphics &g,
                                  int width, int height,
                                  bool rowIsSelected) override;

    Component *refreshComponentForRow(int rowNumber, bool isRowSelected,
                                              Component *existingComponentToUpdate) override;

    void listWasScrolled() override;



    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> bg;
    ScopedPointer<ListBox> listBox;
    ScopedPointer<CommandItemComponent> resetButton;
    ScopedPointer<PlayButton> playButton;
    ScopedPointer<ShadowDownwards> shadowDown;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArpeggiatorPanel)
};
