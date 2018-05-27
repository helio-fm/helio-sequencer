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

#include "../../Themes/PanelBackgroundC.h"
#include "../../Themes/SeparatorHorizontalReversed.h"
#include "../../Themes/LighterShadowUpwards.h"
#include "../../Themes/SeparatorHorizontal.h"
#include "../../Common/PlayButton.h"
#include "../../Themes/LighterShadowDownwards.h"

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
    void paintOverChildren(Graphics& g) override;
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

    MenuPanel::Menu commandDescriptions;

    void updateModeButtons();
    void emitAnnotationsCallout(Component *newAnnotationsMenu);
    void recreateCommandDescriptions();

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
    void onTempoChanged(double newTempo) override;
    void onTotalTimeChanged(double timeMs) override;
    void onPlay() override;
    void onStop() override;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> bodyBg;
    ScopedPointer<ListBox> listBox;
    ScopedPointer<SeparatorHorizontalReversed> headLine;
    ScopedPointer<LighterShadowUpwards> shadow;
    ScopedPointer<SeparatorHorizontal> separator;
    ScopedPointer<Label> totalTime;
    ScopedPointer<Label> currentTime;
    ScopedPointer<PlayButton> playButton;
    ScopedPointer<LighterShadowDownwards> headShadow;
    ScopedPointer<MenuItemComponent> annotationsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerSidebarRight)
};
