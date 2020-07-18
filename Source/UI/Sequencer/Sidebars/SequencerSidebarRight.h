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
class ProjectNode;

#include "TransportListener.h"
#include "MenuPanel.h"
//[/Headers]

#include "../../Themes/SeparatorHorizontalReversed.h"
#include "../../Themes/ShadowUpwards.h"
#include "../../Themes/SeparatorHorizontal.h"
#include "../../Themes/ShadowDownwards.h"
#include "../../Common/TransportControlComponent.h"

class SequencerSidebarRight final : public Component,
                                    protected TransportListener,
                                    protected AsyncUpdater,
                                    protected ListBoxModel,
                                    protected ChangeListener,
                                    protected Timer
{
public:

    SequencerSidebarRight(ProjectNode &parent);
    ~SequencerSidebarRight();

    //[UserMethods]
    void setLinearMode();
    void setPatternMode();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    //===------------------------------------------------------------------===//
    // SequencerSidebarRight
    //===------------------------------------------------------------------===//

    ProjectNode &project;

    Atomic<double> lastSeekTime;
    Atomic<double> lastTotalTime;
    Atomic<double> timerStartSeekTime;
    Atomic<double> timerStartSystemTime;

    enum class MenuMode : int8
    {
        None,
        PianoRollTools,
        PatternRollTools
    };

    MenuMode menuMode = MenuMode::None;
    MenuPanel::Menu menu;

    void updateModeButtons();
    void emitAnnotationsCallout(Component *newAnnotationsMenu);
    void recreateMenu();

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool, Component *) override;
    void paintListBoxItem(int, Graphics &, int, int, bool) override {}

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//

    void handleAsyncUpdate() override;

    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void timerCallback() override;

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onSeek(float beatPosition, double currentTimeMs, double totalTimeMs) override;
    void onTempoChanged(double msPerQuarter) override {}
    void onTotalTimeChanged(double timeMs) override;
    void onLoopModeChanged(bool hasLoop, float startBeat, float endBeat) override;

    void onPlay() override;
    void onStop() override;
    void onRecord() override;
    void onRecordFailed(const Array<MidiDeviceInfo> &devices) override;

    //[/UserVariables]

    UniquePointer<ListBox> listBox;
    UniquePointer<SeparatorHorizontalReversed> headLine;
    UniquePointer<ShadowUpwards> shadow;
    UniquePointer<SeparatorHorizontal> separator;
    UniquePointer<Label> totalTime;
    UniquePointer<Label> currentTime;
    UniquePointer<ShadowDownwards> headShadow;
    UniquePointer<MenuItemComponent> annotationsButton;
    UniquePointer<TransportControlComponent> transportControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerSidebarRight)
};


