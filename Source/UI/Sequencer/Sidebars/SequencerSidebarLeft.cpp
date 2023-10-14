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

#include "WaveformAudioMonitorComponent.h"
#include "SpectrogramAudioMonitorComponent.h"
#include "MenuItemComponent.h"
#include "ShadowUpwards.h"
#include "ShadowDownwards.h"
#include "SeparatorHorizontal.h"
#include "SeparatorHorizontalReversed.h"
#include "HelioTheme.h"
#include "IconComponent.h"

#include "AudioCore.h"
#include "ColourIDs.h"
#include "CommandIDs.h"
#include "Config.h"
#include "Icons.h"

SequencerSidebarLeft::SequencerSidebarLeft()
{
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);

    this->footShadow = make<ShadowUpwards>(ShadowType::Light);
    this->addAndMakeVisible(this->footShadow.get());

    this->headRule = make<SeparatorHorizontalReversed>();
    this->addAndMakeVisible(this->headRule.get());

    this->headShadow = make<ShadowDownwards>(ShadowType::Light);
    this->addAndMakeVisible(this->headShadow.get());

    this->footRule = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->footRule.get());

    this->modeIndicatorSelector = make<ModeIndicatorTrigger>();
    this->addAndMakeVisible(this->modeIndicatorSelector.get());

    this->modeIndicator = make<ModeIndicatorComponent>(2);
    this->addAndMakeVisible(this->modeIndicator.get());
    this->modeIndicator->setAlpha(0.f);

    this->switchPatternModeButton = make<MenuItemComponent>(this, nullptr,
        MenuItem::item(Icons::patterns, CommandIDs::SwitchBetweenRolls)->
            withTooltip(TRANS(I18n::Tooltips::switchRolls)));

    this->addAndMakeVisible(this->switchPatternModeButton.get());

    this->switchLinearModeButton = make<MenuItemComponent>(this, nullptr,
        MenuItem::item(Icons::piano, CommandIDs::SwitchBetweenRolls)->
            withTooltip(TRANS(I18n::Tooltips::switchRolls)));

    this->addAndMakeVisible(switchLinearModeButton.get());

    this->listBox = make<ListBox>();
    this->addAndMakeVisible(this->listBox.get());

    const auto *uiFlags = App::Config().getUiFlags();
    this->miniMapVisible = uiFlags->isProjectMapInLargeMode();
    this->velocityMapVisible = uiFlags->isEditorPanelVisible();
    this->noteNameGuidesEnabled = uiFlags->isNoteNameGuidesEnabled();
    this->scalesHighlightingEnabled = uiFlags->isScalesHighlightingEnabled();

    this->recreateMenu();
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setRowHeight(Globals::UI::sidebarRowHeight);
    this->listBox->setModel(this);

    this->switchLinearModeButton->setVisible(false);
    this->switchPatternModeButton->setVisible(false);

    // todo save the default monitor option in global UI flags?
    this->waveformMonitor = make<WaveformAudioMonitorComponent>(nullptr);
    this->spectrogramMonitor = make<SpectrogramAudioMonitorComponent>(nullptr);

    this->addChildComponent(this->waveformMonitor.get());
    this->addChildComponent(this->spectrogramMonitor.get());

    this->waveformMonitor->setVisible(true);

    MenuPanelUtils::disableKeyboardFocusForAllChildren(this);

    App::Config().getUiFlags()->addListener(this);
}

SequencerSidebarLeft::~SequencerSidebarLeft()
{
    App::Config().getUiFlags()->removeListener(this);
}

void SequencerSidebarLeft::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getSidebarBackground(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
    g.fillRect(this->getWidth() - 1, 0, 1, this->getHeight());
}

void SequencerSidebarLeft::resized()
{
    constexpr auto headerSize = Globals::UI::rollHeaderHeight;
    constexpr auto footerSize = Globals::UI::projectMapHeight;

    this->listBox->setBounds(0, headerSize - 1, this->getWidth(),
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

    this->spectrogramMonitor->setBounds(0,
        this->getHeight() - audioMonitorHeight,
        this->getWidth(), audioMonitorHeight);

    this->modeIndicatorSelector->setBounds(0,
        this->getHeight() - audioMonitorHeight,
        this->getWidth(), audioMonitorHeight);

    constexpr auto modeIndicatorSize = 6;
    this->modeIndicator->setBounds(0,
        this->getHeight() - modeIndicatorSize * 2,
        this->getWidth(), modeIndicatorSize);

    this->switchPatternModeButton->setBounds(0, 0, this->getWidth(), headerSize - 1);
    this->switchLinearModeButton->setBounds(0, 0, this->getWidth(), headerSize - 1);
}

void SequencerSidebarLeft::setAudioMonitor(AudioMonitor *audioMonitor)
{
    this->spectrogramMonitor->setTargetAnalyzer(audioMonitor);
    this->waveformMonitor->setTargetAnalyzer(audioMonitor);
}

void SequencerSidebarLeft::handleChangeMode()
{
    switch (this->modeIndicator->scrollToNextMode())
    {
    case 0:
        this->switchMonitorsAnimated(this->spectrogramMonitor.get(), this->waveformMonitor.get());
        break;
    case 1:
        this->switchMonitorsAnimated(this->waveformMonitor.get(), this->spectrogramMonitor.get());
        break;
    default:
        break;
    }
}

void SequencerSidebarLeft::switchMonitorsAnimated(Component *oldOne, Component *newOne)
{
    jassert(newOne->getY() == oldOne->getY());
    auto slideTime = Globals::UI::fadeOutLong;
    const int w = this->getWidth();
    const int y = newOne->getY();
    this->animator.animateComponent(oldOne,
        oldOne->getBounds().translated(-w, 0), 0.f, slideTime, true, 0.0, 1.0);
    oldOne->setVisible(false);
    newOne->setAlpha(0.f);
    newOne->setVisible(true);
    newOne->setTopLeftPosition(w, y);
    this->animator.animateComponent(newOne,
        newOne->getBounds().translated(-w, 0), 1.f, slideTime, false, 1.0, 0.0);
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
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

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

void SequencerSidebarLeft::onScalesHighlightingFlagChanged(bool enabled)
{
    this->scalesHighlightingEnabled = enabled;
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
        withTooltip(TRANS(I18n::Tooltips::zoomOut)));

    this->menu.add(MenuItem::item(Icons::zoomIn, CommandIDs::ZoomIn)->
        withTooltip(TRANS(I18n::Tooltips::zoomIn)));

    this->menu.add(MenuItem::item(Icons::zoomToFit, CommandIDs::ZoomEntireClip)->
        withTooltip(TRANS(I18n::Tooltips::zoomToFit)));

    // Jump to playhead position (or start following playhead when playing)
    //this->menu.add(MenuItem::item(Icons::playhead, CommandIDs::FollowFlayhead));

    // Jump to the next/previous anchor,
    // i.e. any timeline event or active track's start in the piano roll mode,
    // and next/previous clip in the pattern roll mode:
    this->menu.add(MenuItem::item(Icons::timelinePrevious, CommandIDs::TimelineJumpPrevious)->
        withTooltip(TRANS(I18n::Tooltips::jumpToPrevAnchor)));

    this->menu.add(MenuItem::item(Icons::timelineNext, CommandIDs::TimelineJumpNext)->
        withTooltip(TRANS(I18n::Tooltips::jumpToNextAnchor)));

    // TODO add some controls to switch focus between tracks?

    // toggling the mini-map makes sense on mobile platforms, because of smaller screens,
    // but on the desktop it's more like a one-off choice, so let's not clog up the ui
    // (this action will still be available via hotkey, 'b' by default):
#if PLATFORM_MOBILE

    this->menu.add(MenuItem::item(Icons::bottomBar, CommandIDs::ToggleBottomMiniMap)->
        toggledIf(this->miniMapVisible)->
        withTooltip(TRANS(I18n::Tooltips::toggleMiniMap)));

#endif

    this->menu.add(MenuItem::item(Icons::volumePanel, CommandIDs::ToggleVolumePanel)->
        toggledIf(this->velocityMapVisible)->
        withTooltip(TRANS(I18n::Tooltips::toggleVolumePanel)));

    if (this->menuMode == MenuMode::PianoRollTools)
    {
        this->menu.add(MenuItem::item(Icons::paint, CommandIDs::ToggleScalesHighlighting)->
            toggledIf(this->scalesHighlightingEnabled)->
            withTooltip(TRANS(I18n::Tooltips::toggleScalesHighlighting)));

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
