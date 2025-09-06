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

#include "Common.h"
#include "SequencerSidebarLeft.h"
#include "AudioMonitor.h"
#include "WaveformAudioMonitorComponent.h"
#include "MenuItemComponent.h"
#include "ShadowUpwards.h"
#include "ShadowDownwards.h"
#include "SeparatorHorizontal.h"
#include "SeparatorHorizontalReversed.h"
#include "HelioTheme.h"
#include "IconComponent.h"

#include "AudioCore.h"
#include "CommandIDs.h"
#include "Config.h"
#include "Icons.h"

SequencerSidebarLeft::SequencerSidebarLeft()
{
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);

    if (App::mayHaveDisplayNotch())
    {
        this->setInterceptsMouseClicks(true, true);
        this->swipeController = make<SwipeController>(*this);
        this->addMouseListener(this->swipeController.get(), true);
    }
    else
    {
        this->setInterceptsMouseClicks(false, true);
    }

    this->footShadow = make<ShadowUpwards>(ShadowType::Light);
    this->addAndMakeVisible(this->footShadow.get());

    this->headRule = make<SeparatorHorizontalReversed>();
    this->addAndMakeVisible(this->headRule.get());

    this->headShadow = make<ShadowDownwards>(ShadowType::Light);
    this->addAndMakeVisible(this->headShadow.get());

    this->footRule = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->footRule.get());

    this->switchPatternModeButton = make<MenuItemComponent>(this, nullptr,
        MenuItem::item(Icons::patterns, CommandIDs::SwitchBetweenRolls)->
            withTooltip(TRANS(I18n::Tooltips::switchRolls)));

    this->addAndMakeVisible(this->switchPatternModeButton.get());

    this->switchLinearModeButton = make<MenuItemComponent>(this, nullptr,
        MenuItem::item(Icons::piano, CommandIDs::SwitchBetweenRolls)->
            withTooltip(TRANS(I18n::Tooltips::switchRolls)));

    this->addAndMakeVisible(switchLinearModeButton.get());

    this->listBox = make<ListBox>();
    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setRowHeight(Globals::UI::sidebarRowHeight);
    this->listBox->getViewport()->setScrollBarPosition(false, true);
    this->listBox->getViewport()->setScrollOnDragMode(Viewport::ScrollOnDragMode::never);
    this->addAndMakeVisible(this->listBox.get());

    const auto *uiFlags = App::Config().getUiFlags();
    this->zoomLevelLocked = uiFlags->isZoomLevelLocked();
    this->miniMapVisible = uiFlags->isProjectMapInLargeMode();
    this->velocityMapVisible = uiFlags->isEditorPanelVisible();
    this->noteNameGuidesEnabled = uiFlags->areNoteNameGuidesEnabled();

    this->switchLinearModeButton->setVisible(false);
    this->switchPatternModeButton->setVisible(false);

    this->waveformMonitor = make<WaveformAudioMonitorComponent>(nullptr);

    this->addChildComponent(this->waveformMonitor.get());

    this->waveformMonitor->setVisible(true);

    MenuPanelUtils::disableKeyboardFocusForAllChildren(this);

    App::Config().getUiFlags()->addListener(this);

    this->setSize(App::Config().getUiFlags()->getLeftSidebarWidth(), 16);
}

SequencerSidebarLeft::~SequencerSidebarLeft()
{
    App::Config().getUiFlags()->removeListener(this);
}

void SequencerSidebarLeft::paint(Graphics &g)
{
    auto localBounds = this->getLocalBounds();
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getSidebarBackground(), {} });
    g.fillRect(localBounds);

    g.setColour(this->borderColour);
    g.fillRect(this->getWidth() - 1, 0, 1, this->getHeight());

    if (localBounds.getWidth() > Globals::UI::sidebarWidth)
    {
        localBounds.removeFromRight(Globals::UI::sidebarWidth);
        localBounds.removeFromBottom(Globals::UI::sidebarFooterHeight);
        theme.drawStripes(localBounds.toFloat(), g, 0.5f);
    }
}

void SequencerSidebarLeft::resized()
{
    constexpr auto toolbarSize = Globals::UI::sidebarWidth;
    constexpr auto headerSize = Globals::UI::rollHeaderHeight;
    constexpr auto footerSize = Globals::UI::sidebarFooterHeight;

    this->listBox->setBounds(this->getWidth() - toolbarSize,
        headerSize - 1,
        toolbarSize,
        this->getHeight() - headerSize - footerSize + 1);

    constexpr auto shadowSize = 6;
    this->headShadow->setBounds(0, headerSize - 1, this->getWidth(), shadowSize);
    this->footShadow->setBounds(0,
        this->getHeight() - footerSize - shadowSize,
        this->getWidth(), shadowSize);

    this->headRule->setBounds(0, headerSize - 2, this->getWidth(), 2);
    this->footRule->setBounds(0,
        this->getHeight() - footerSize,
        this->getWidth(), 2);

    constexpr auto audioMonitorHeight = footerSize - 2;

    this->waveformMonitor->setBounds(0,
        this->getHeight() - audioMonitorHeight,
        this->getWidth(), audioMonitorHeight);

    this->switchPatternModeButton->setBounds(this->getWidth() - toolbarSize, 0,
        toolbarSize, headerSize - 1);
    this->switchLinearModeButton->setBounds(this->getWidth() - toolbarSize, 0,
        toolbarSize, headerSize - 1);
}

void SequencerSidebarLeft::setAudioMonitor(AudioMonitor *audioMonitor)
{
    this->waveformMonitor->setTargetAnalyzer(audioMonitor);
}

void SequencerSidebarLeft::setLinearMode()
{
    this->buttonFader.cancelAllAnimations(false);
    this->buttonFader.fadeIn(this->switchPatternModeButton.get(), Globals::UI::fadeInLong);
    this->buttonFader.fadeOut(this->switchLinearModeButton.get(), Globals::UI::fadeOutLong);

    if (this->menuMode != MenuMode::PianoRollTools)
    {
        this->menuMode = MenuMode::PianoRollTools;
        this->recreateMenu();
        this->listBox->updateContent();
    }
}

void SequencerSidebarLeft::setPatternMode()
{
    this->buttonFader.cancelAllAnimations(false);
    this->buttonFader.fadeIn(this->switchLinearModeButton.get(), Globals::UI::fadeInLong);
    this->buttonFader.fadeOut(this->switchPatternModeButton.get(), Globals::UI::fadeOutLong);

    if (this->menuMode != MenuMode::PatternRollTools)
    {
        this->menuMode = MenuMode::PatternRollTools;
        this->recreateMenu();
        this->listBox->updateContent();
    }
}

//===----------------------------------------------------------------------===//
// SwipeController::Listener
//===----------------------------------------------------------------------===//

int SequencerSidebarLeft::getHorizontalSwipeAnchor()
{
    return this->getWidth();
}

void SequencerSidebarLeft::onHorizontalSwipe(int anchor, int distance)
{
    constexpr auto minWidth = Globals::UI::sidebarWidth;
    constexpr auto maxWidth = Globals::UI::sidebarWidth * 2;

    const auto newWidth = jlimit(minWidth, maxWidth, anchor + distance);
    App::Config().getUiFlags()->setLeftSidebarWidth(newWidth);

    this->setSize(newWidth, this->getHeight());
    if (auto *parentComponent = this->getParentComponent())
    {
        parentComponent->resized();
    }
}

//===----------------------------------------------------------------------===//
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

void SequencerSidebarLeft::onLockZoomLevelFlagChanged(bool zoomLocked)
{
    this->zoomLevelLocked = zoomLocked;
    this->recreateMenu();
    this->listBox->updateContent();
    this->repaint();
}

void SequencerSidebarLeft::onProjectMapLargeModeFlagChanged(bool showFullMap)
{
    this->miniMapVisible = showFullMap;
    this->recreateMenu();
    this->listBox->updateContent();
}

void SequencerSidebarLeft::onEditorPanelVisibilityFlagChanged(bool visible)
{
    this->velocityMapVisible = visible;
    this->recreateMenu();
    this->listBox->updateContent();
}

void SequencerSidebarLeft::onNoteNameGuidesFlagChanged(bool enabled)
{
    this->noteNameGuidesEnabled = enabled;
    this->recreateMenu();
    this->listBox->updateContent();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

void SequencerSidebarLeft::recreateMenu()
{
    this->menu.clearQuick();
    this->menu.add(MenuItem::item(Icons::zoomOut, CommandIDs::ZoomOut)->
        disabledIf(this->zoomLevelLocked)->
        withTooltip(TRANS(I18n::Tooltips::zoomOut)));

    this->menu.add(MenuItem::item(Icons::zoomIn, CommandIDs::ZoomIn)->
        disabledIf(this->zoomLevelLocked)->
        withTooltip(TRANS(I18n::Tooltips::zoomIn)));

    this->menu.add(MenuItem::item(Icons::zoomToFit, CommandIDs::ZoomEntireClip)->
        disabledIf(this->zoomLevelLocked)->
        withTooltip(TRANS(I18n::Tooltips::zoomToFit)));

    this->menu.add(MenuItem::item(Icons::lockZoom, CommandIDs::ToggleLockZoomLevel)->
        toggledIf(this->zoomLevelLocked)->
        withTooltip(TRANS(I18n::Tooltips::lockZoom)));

    // Jump to playhead position (or start following playhead when playing)
    //this->menu.add(MenuItem::item(Icons::playhead, CommandIDs::FollowPlayhead));

    // Jump to the next/previous anchor,
    // i.e. any timeline event or active track's start in the piano roll mode,
    // and next/previous clip in the pattern roll mode:
    this->menu.add(MenuItem::item(Icons::timelinePrevious, CommandIDs::TimelineJumpPrevious)->
        withTooltip(TRANS(I18n::Tooltips::jumpToPrevAnchor)));

    this->menu.add(MenuItem::item(Icons::timelineNext, CommandIDs::TimelineJumpNext)->
        withTooltip(TRANS(I18n::Tooltips::jumpToNextAnchor)));

    // TODO add some controls to switch focus between tracks?

    // only display this button on tablets: on desktop the app will show the full map
    // and on phones it will show the scroller, so let's not clog up the ui;
    // switching between two modes is still available via hotkey 'b' or via clicking on the map:
#if PLATFORM_MOBILE

    if (!App::isRunningOnPhone())
    {
        this->menu.add(MenuItem::item(Icons::bottomBar, CommandIDs::ToggleBottomMiniMap)->
            toggledIf(this->miniMapVisible)->
            withTooltip(TRANS(I18n::Tooltips::toggleMiniMap)));
    }

#endif

    this->menu.add(MenuItem::item(Icons::volumePanel, CommandIDs::ToggleVolumePanel)->
        toggledIf(this->velocityMapVisible)->
        withTooltip(TRANS(I18n::Tooltips::toggleVolumePanel)));

    if (this->menuMode == MenuMode::PianoRollTools)
    {
        this->menu.add(MenuItem::item(Icons::tag, CommandIDs::ToggleNoteNameGuides)->
            toggledIf(this->noteNameGuidesEnabled)->
            withTooltip(TRANS(I18n::Tooltips::toggleNoteGuides)));
    }
}

Component *SequencerSidebarLeft::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber >= this->menu.size())
    {
        return existingComponentToUpdate;
    }

    const MenuItem::Ptr itemDescription = this->menu[rowNumber];

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<MenuItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->update(itemDescription);
        }
    }
    else
    {
        auto *row = new MenuItemComponent(this, this->listBox->getViewport(), itemDescription);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

int SequencerSidebarLeft::getNumRows()
{
    return this->menu.size();
}
