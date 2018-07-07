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
class ProjectTreeItem;

#include "TransportListener.h"
#include "MenuPanel.h"
//[/Headers]

#include "../../Themes/SeparatorHorizontalReversed.h"
#include "../../Themes/ShadowDownwards.h"
#include "../../Themes/SeparatorHorizontal.h"
#include "../../Themes/ShadowUpwards.h"
#include "../../Common/PlayButton.h"

class SequencerSidebarRight final : public Component,
                                    protected TransportListener,
                                    protected AsyncUpdater,
                                    protected ListBoxModel,
                                    protected ChangeListener,
                                    protected Timer
{
public:

    SequencerSidebarRight(ProjectTreeItem &parent);
    ~SequencerSidebarRight();

    //[UserMethods]
    void setLinearMode();
    void setPatternMode();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;

    // Binary resources:
    static const char* gray1x1_png;
    static const int gray1x1_pngSize;

private:

    //[UserVariables]

    //===------------------------------------------------------------------===//
    // SequencerSidebarRight
    //===------------------------------------------------------------------===//

    ProjectTreeItem &project;

    Atomic<double> lastSeekTime;
    Atomic<double> lastTotalTime;
    Atomic<double> timerStartSeekTime;
    Atomic<double> timerStartSystemTime;

    enum MenuMode
    {
        PianoRollTools,
        PatternRollTools
    };

    MenuMode menuMode;
    MenuPanel::Menu menu;

    void updateModeButtons();
    void emitAnnotationsCallout(Component *newAnnotationsMenu);
    void recreateMenu();

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool,  Component *) override;
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

    void onSeek(double absolutePosition, double currentTimeMs,
        double totalTimeMs) override;
    void onTempoChanged(double msPerQuarter) override;
    void onTotalTimeChanged(double timeMs) override;
    void onPlay() override;
    void onStop() override;

    //[/UserVariables]

    ScopedPointer<ListBox> listBox;
    ScopedPointer<SeparatorHorizontalReversed> headLine;
    ScopedPointer<ShadowUpwards> shadow;
    ScopedPointer<SeparatorHorizontal> separator;
    ScopedPointer<Label> totalTime;
    ScopedPointer<Label> currentTime;
    ScopedPointer<ShadowDownwards> headShadow;
    ScopedPointer<MenuItemComponent> annotationsButton;
    ScopedPointer<PlayButton> playButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerSidebarRight)
};
