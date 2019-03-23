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
class GenericAudioMonitorComponent;
class WaveformAudioMonitorComponent;
class SpectrogramAudioMonitorComponent;
class ModeIndicatorComponent;
class MenuItemComponent;
class ProjectNode;
class AudioMonitor;

#include "ComponentFader.h"
#include "ModeIndicatorComponent.h"
#include "MenuPanel.h"
//[/Headers]

#include "../../Themes/ShadowUpwards.h"
#include "../../Themes/SeparatorHorizontalReversed.h"
#include "../../Themes/ShadowDownwards.h"
#include "../../Themes/SeparatorHorizontal.h"

class SequencerSidebarLeft final : public ModeIndicatorOwnerComponent,
                                   protected ListBoxModel
{
public:

    SequencerSidebarLeft(ProjectNode &project);
    ~SequencerSidebarLeft();

    //[UserMethods]
    void setAudioMonitor(AudioMonitor *audioMonitor);
    void handleChangeMode() override;

    void setLinearMode();
    void setPatternMode();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]
    ProjectNode &project;

    ComponentFader buttonFader;
    ComponentAnimator animator;
    void switchMonitorsAnimated(Component *oldOne, Component *newOne);

    ScopedPointer<GenericAudioMonitorComponent> genericMonitor;
    ScopedPointer<WaveformAudioMonitorComponent> waveformMonitor;
    ScopedPointer<SpectrogramAudioMonitorComponent> spectrogramMonitor;

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    MenuPanel::Menu menu;
    void recreateMenu();

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool, Component *) override;
    void paintListBoxItem(int, Graphics &, int, int, bool) override {}
    //[/UserVariables]

    UniquePointer<ShadowUpwards> shadow;
    UniquePointer<SeparatorHorizontalReversed> headLine;
    UniquePointer<ShadowDownwards> headShadow;
    UniquePointer<SeparatorHorizontal> separator;
    UniquePointer<ModeIndicatorTrigger> modeIndicatorSelector;
    UniquePointer<ModeIndicatorComponent> modeIndicator;
    UniquePointer<MenuItemComponent> switchPatternModeButton;
    UniquePointer<MenuItemComponent> switchLinearModeButton;
    UniquePointer<ListBox> listBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerSidebarLeft)
};
