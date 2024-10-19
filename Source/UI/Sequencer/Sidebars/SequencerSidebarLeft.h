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

class AudioMonitor;
class WaveformAudioMonitorComponent;
class MenuItemComponent;
class ShadowUpwards;
class ShadowDownwards;
class SeparatorHorizontal;
class SeparatorHorizontalReversed;

#include "MenuPanel.h"
#include "ComponentFader.h"
#include "UserInterfaceFlags.h"
#include "ModeIndicatorComponent.h"
#include "SwipeController.h"
#include "MenuPanel.h"
#include "ColourIDs.h"

class SequencerSidebarLeft final : public Component,
    public SwipeController::Listener,
    private UserInterfaceFlags::Listener,
    private ListBoxModel
{
public:

    SequencerSidebarLeft();
    ~SequencerSidebarLeft();

    void setAudioMonitor(AudioMonitor *audioMonitor);

    void setLinearMode();
    void setPatternMode();

    void paint(Graphics &g) override;
    void resized() override;

private:

    //===------------------------------------------------------------------===//
    // SwipeController::Listener
    //===------------------------------------------------------------------===//

    int getHorizontalSwipeAnchor() override;
    void onHorizontalSwipe(int anchor, int distance) override;

    //===------------------------------------------------------------------===//
    // UserInterfaceFlags::Listener
    //===------------------------------------------------------------------===//

    void onLockZoomLevelFlagChanged(bool zoomLocked) override;
    void onProjectMapLargeModeFlagChanged(bool showFullMap) override;
    void onEditorPanelVisibilityFlagChanged(bool visible) override;
    void onScalesHighlightingFlagChanged(bool enabled) override;
    void onNoteNameGuidesFlagChanged(bool enabled) override;

    bool zoomLevelLocked = false;
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

private:

    const Colour borderColour = findDefaultColour(ColourIDs::Common::borderLineLight);

    UniquePointer<SwipeController> swipeController;

    UniquePointer<ShadowUpwards> footShadow;
    UniquePointer<SeparatorHorizontalReversed> headRule;
    UniquePointer<ShadowDownwards> headShadow;
    UniquePointer<SeparatorHorizontal> footRule;
    UniquePointer<MenuItemComponent> switchPatternModeButton;
    UniquePointer<MenuItemComponent> switchLinearModeButton;
    UniquePointer<ListBox> listBox;

    UniquePointer<WaveformAudioMonitorComponent> waveformMonitor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencerSidebarLeft)
};
