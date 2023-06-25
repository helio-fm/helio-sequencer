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

class AudioMonitor;
class WaveformAudioMonitorComponent;
class SpectrogramAudioMonitorComponent;
class MenuItemComponent;

class ShadowUpwards;
class ShadowDownwards;
class SeparatorHorizontal;
class SeparatorHorizontalReversed;

#include "MenuPanel.h"
#include "ComponentFader.h"
#include "UserInterfaceFlags.h"
#include "ModeIndicatorComponent.h"

class SequencerSidebarLeft final : public ModeIndicatorOwnerComponent,
                                   protected UserInterfaceFlags::Listener,
                                   protected ListBoxModel
{
public:

    SequencerSidebarLeft();
    ~SequencerSidebarLeft();

    void setAudioMonitor(AudioMonitor *audioMonitor);
    void handleChangeMode() override;

    void setLinearMode();
    void setPatternMode();

    void paint(Graphics &g) override;
    void resized() override;

private:

    //===------------------------------------------------------------------===//
    // UserInterfaceFlags::Listener
    //===------------------------------------------------------------------===//

    void onProjectMapLargeModeFlagChanged(bool showFullMap) override;
    void onEditorPanelVisibilityFlagChanged(bool visible) override;
    void onScalesHighlightingFlagChanged(bool enabled) override;
    void onNoteNameGuidesFlagChanged(bool enabled) override;

    bool scalesHighlightingEnabled = true;
    bool noteNameGuidesEnabled = false;
    bool velocityMapVisible = false;
    bool miniMapVisible = false;

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    enum class MenuMode : int8
    {
        None,
        PianoRollTools,
        PatternRollTools
    };

    ComponentFader buttonFader;

    MenuMode menuMode = MenuMode::None;
    MenuPanel::Menu menu;
    void recreateMenu();

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool, Component *) override;
    void paintListBoxItem(int, Graphics &, int, int, bool) override {}

    UniquePointer<ShadowUpwards> footShadow;
    UniquePointer<SeparatorHorizontalReversed> headRule;
    UniquePointer<ShadowDownwards> headShadow;
    UniquePointer<SeparatorHorizontal> footRule;
    UniquePointer<ModeIndicatorTrigger> modeIndicatorSelector;
    UniquePointer<ModeIndicatorComponent> modeIndicator;
    UniquePointer<MenuItemComponent> switchPatternModeButton;
    UniquePointer<MenuItemComponent> switchLinearModeButton;
    UniquePointer<ListBox> listBox;

    ComponentAnimator animator;
    void switchMonitorsAnimated(Component *oldOne, Component *newOne);

    UniquePointer<WaveformAudioMonitorComponent> waveformMonitor;
    UniquePointer<SpectrogramAudioMonitorComponent> spectrogramMonitor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencerSidebarLeft)
};
