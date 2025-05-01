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
#include "SequencerSidebarRight.h"

#include "SeparatorHorizontalReversed.h"
#include "ShadowUpwards.h"
#include "SeparatorHorizontal.h"
#include "ShadowDownwards.h"
#include "TransportControlComponent.h"
#include "ProjectNode.h"
#include "Transport.h"
#include "PlayerThread.h"
#include "PianoRoll.h"
#include "MenuItemComponent.h"
#include "ProjectTimeline.h"
#include "ModalCallout.h"
#include "SequencerOperations.h"
#include "MenuPanel.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "Config.h"
#include "CommandIDs.h"
#include "Icons.h"

SequencerSidebarRight::SequencerSidebarRight(ProjectNode &parent) : project(parent)
{
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);

#if PLATFORM_DESKTOP
    this->setInterceptsMouseClicks(false, true);
#elif PLATFORM_MOBILE
    this->setInterceptsMouseClicks(true, true);
    this->swipeController = make<SwipeController>(*this);
    this->addMouseListener(this->swipeController.get(), true);
#endif

    this->headRule = make<SeparatorHorizontalReversed>();
    this->addAndMakeVisible(this->headRule.get());

    this->footShadow = make<ShadowUpwards>(ShadowType::Light);
    this->addAndMakeVisible(this->footShadow.get());

    this->footRule = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->footRule.get());

    this->headShadow = make<ShadowDownwards>(ShadowType::Light);
    this->addAndMakeVisible(this->headShadow.get());

    this->repriseButton = make<MenuItemComponent>(this, nullptr,
        MenuItem::item(Icons::reprise, CommandIDs::ToggleLoopOverSelection)->
        withTooltip(TRANS(I18n::Tooltips::togglePlaybackLoop)));
    this->repriseButton->setChecked(this->project.getTransport().hasPlaybackLoop());
    this->addAndMakeVisible(this->repriseButton.get());

    this->transportControl = make<TransportControlComponent>(nullptr);
    this->addAndMakeVisible(this->transportControl.get());

    this->listBox = make<ListBox>();
    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setRowHeight(Globals::UI::sidebarRowHeight);
    this->listBox->getViewport()->setScrollBarPosition(true, true);
    this->listBox->getViewport()->setScrollOnDragMode(Viewport::ScrollOnDragMode::never);
    this->addAndMakeVisible(this->listBox.get());

    MenuPanelUtils::disableKeyboardFocusForAllChildren(this);

    this->project.getTransport().addTransportListener(this);
    this->project.getEditMode().addListener(this);
    App::Config().getUiFlags()->addListener(this);

    this->setSize(App::Config().getUiFlags()->getRightSidebarWidth(), 16);
}

SequencerSidebarRight::~SequencerSidebarRight()
{
    App::Config().getUiFlags()->removeListener(this);
    this->project.getEditMode().removeListener(this);
    this->project.getTransport().removeTransportListener(this);
}

void SequencerSidebarRight::paint(Graphics &g)
{
    auto localBounds = this->getLocalBounds();
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getSidebarBackground(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(this->borderColour);
    g.fillRect(0, 0, 1, this->getHeight());

    if (localBounds.getWidth() > Globals::UI::sidebarWidth)
    {
        localBounds.removeFromLeft(Globals::UI::sidebarWidth);
        localBounds.removeFromBottom(Globals::UI::sidebarFooterHeight);
        theme.drawStripes(localBounds.toFloat(), g, 0.5f);
    }
}

void SequencerSidebarRight::resized()
{
    constexpr auto toolbarSize = Globals::UI::sidebarWidth;
    constexpr auto headerSize = Globals::UI::rollHeaderHeight;
    constexpr auto footerSize = Globals::UI::sidebarFooterHeight;

    this->listBox->setBounds(0, headerSize - 1,
        toolbarSize, this->getHeight() - headerSize - footerSize + 1);

    constexpr auto shadowSize = 6;
    this->headShadow->setBounds(0, headerSize - 1, this->getWidth(), shadowSize);
    this->footShadow->setBounds(0,
        this->getHeight() - footerSize - shadowSize,
        this->getWidth(), shadowSize);

    this->headRule->setBounds(0, headerSize - 2, this->getWidth(), 2);
    this->footRule->setBounds(0,
        this->getHeight() - footerSize,
        this->getWidth(), 2);

    this->repriseButton->setBounds(0, 0, toolbarSize, headerSize - 1);

    constexpr auto transportControlSize = footerSize - 1;
    this->transportControl->setBounds(0,
        this->getHeight() - transportControlSize,
        this->getWidth(), transportControlSize);

    // a hack for themes changing:
    this->listBox->updateContent();
    this->repriseButton->resized();
}

void SequencerSidebarRight::recreateMenu()
{
    this->menu.clear();

    const auto &editMode = this->project.getEditMode();

    const bool defaultMode = editMode.isMode(RollEditMode::defaultMode);
    const bool drawMode = editMode.isMode(RollEditMode::drawMode) || editMode.isMode(RollEditMode::eraseMode);
    const bool scissorsMode = editMode.isMode(RollEditMode::knifeMode) || editMode.isMode(RollEditMode::mergeMode);

    // Selection tool is useless on the desktop
#if PLATFORM_MOBILE
    const bool selectionMode = editMode.isMode(RollEditMode::selectionMode);
    this->menu.add(MenuItem::item(Icons::selectionTool, CommandIDs::EditModeSelect)->toggledIf(selectionMode));
#endif

    this->menu.add(MenuItem::item(Icons::cursorTool, CommandIDs::EditModeDefault)->
        toggledIf(defaultMode)->withTooltip(TRANS(I18n::Tooltips::editModeCursor)));

    this->menu.add(MenuItem::item(Icons::drawTool, CommandIDs::EditModeDraw)->
        toggledIf(drawMode)->withTooltip(TRANS(I18n::Tooltips::editModePen)));

    this->menu.add(MenuItem::item(Icons::cutterTool, CommandIDs::EditModeKnife)->
        toggledIf(scissorsMode)->withTooltip(TRANS(I18n::Tooltips::editModeKnife)));

    // Dragging mode is not displayed (kinda useless),
    // but holding space will temporarily switch to it
    //const bool dragMode = editMode.isMode(RollEditMode::dragMode);
    //this->menu.add(MenuItem::item(Icons::dragTool, CommandIDs::EditModePan)->
    //    toggledIf(dragMode)->withTooltip(TRANS(I18n::Tooltips::editModeDrag)));

    // Undo/redo buttons make less sense on desktop because of hotkeys,
    // but they may still be useful at times because of how often they are needed
    this->menu.add(MenuItem::item(Icons::undo, CommandIDs::Undo)->withTooltip({}));
    this->menu.add(MenuItem::item(Icons::redo, CommandIDs::Redo)->withTooltip({}));

    if (this->menuMode == MenuMode::PianoRollTools)
    {
        this->menu.add(MenuItem::item(Icons::chordBuilder, CommandIDs::ShowChordPanel)->
            withTooltip(TRANS(I18n::Tooltips::chordTool)));

        if (!App::Config().getArpeggiators()->isEmpty())
        {
            this->menu.add(MenuItem::item(Icons::arpeggiate, CommandIDs::ShowArpeggiatorsPanel)->
                withTooltip(TRANS(I18n::Tooltips::arpeggiators)));
        }

#if PLATFORM_MOBILE
        this->menu.add(MenuItem::item(Icons::copy, CommandIDs::CopyEvents));
        this->menu.add(MenuItem::item(Icons::paste, CommandIDs::PasteEvents));
#endif
    }
    else if (this->menuMode == MenuMode::PatternRollTools)
    {
        this->menu.add(MenuItem::item(Icons::expand, CommandIDs::ShowNewTrackPanel)->
            withTooltip(TRANS(I18n::Tooltips::addTrack)));
    }

#if PLATFORM_MOBILE
    if (this->menuMode == MenuMode::PianoRollTools)
    {
        this->menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteEvents));
    }
    else if (this->menuMode == MenuMode::PatternRollTools)
    {
        this->menu.add(MenuItem::item(Icons::volumeUp, CommandIDs::ClipVolumeUp));
        this->menu.add(MenuItem::item(Icons::volumeDown, CommandIDs::ClipVolumeDown));
        this->menu.add(MenuItem::item(Icons::up, CommandIDs::ClipTransposeUp));
        this->menu.add(MenuItem::item(Icons::down, CommandIDs::ClipTransposeDown));
        this->menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteClips));
    }
#endif

    this->menu.add(MenuItem::item(Icons::metronome, CommandIDs::ToggleMetronome)->
        withTooltip(TRANS(I18n::Tooltips::metronome))->
        toggledIf(this->isMetronomeEnabled));
}

//===----------------------------------------------------------------------===//
// SwipeController::Listener
//===----------------------------------------------------------------------===//

int SequencerSidebarRight::getHorizontalSwipeAnchor()
{
    return this->getWidth();
}

void SequencerSidebarRight::onHorizontalSwipe(int anchor, int distance)
{
    constexpr auto minWidth = Globals::UI::sidebarWidth;
    constexpr auto maxWidth = Globals::UI::sidebarWidth * 2;

    const auto newWidth = jlimit(minWidth, maxWidth, anchor - distance);
    App::Config().getUiFlags()->setRightSidebarWidth(newWidth);

    this->setSize(newWidth, this->getHeight());
    if (auto *parentComponent = this->getParentComponent())
    {
        parentComponent->resized();
    }
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *SequencerSidebarRight::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber >= this->menu.size())
    {
        return existingComponentToUpdate;
    }

    const auto description = this->menu.getUnchecked(rowNumber);
    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<MenuItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->update(description);
        }
    }
    else
    {
        auto *row = new MenuItemComponent(this, this->listBox->getViewport(), description);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

int SequencerSidebarRight::getNumRows()
{
    return this->menu.size();
}

//===----------------------------------------------------------------------===//
// TransportListener
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::onLoopModeChanged(bool hasLoop, float startBeat, float endBeat)
{
    this->repriseButton->setChecked(hasLoop);
}

void SequencerSidebarRight::onPlay()
{
    this->transportControl->showPlayingMode(true);
}

void SequencerSidebarRight::onRecord()
{
    this->transportControl->showRecordingMode(true);
}

void SequencerSidebarRight::onRecordFailed(const Array<MidiDeviceInfo> &devices)
{
    jassert(devices.size() != 1);

    if (!this->isShowing()) // shouldn't happen, but just in case it does
    {
        jassertfalse;
        return;
    }

    if (devices.isEmpty())
    {
        App::Layout().showTooltip(TRANS(I18n::Settings::midiNoDevicesFound));
        this->transportControl->showRecordingError();
    }
    else
    {
        this->transportControl->showRecordingMenu(devices);
    }
}

void SequencerSidebarRight::onStop()
{
    this->transportControl->showPlayingMode(false);
    this->transportControl->showRecordingMode(false);
}

//===----------------------------------------------------------------------===//
// RollEditMode::Listener
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::onChangeEditMode(const RollEditMode &mode)
{
    this->updateModeButtons();
}

//===----------------------------------------------------------------------===//
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::onMetronomeFlagChanged(bool enabled)
{
    this->isMetronomeEnabled = enabled;
    this->recreateMenu();
    this->listBox->updateContent();
}

//===----------------------------------------------------------------------===//
// SequencerSidebarRight
//===----------------------------------------------------------------------===//

void SequencerSidebarRight::updateModeButtons()
{
    this->recreateMenu();
    this->listBox->updateContent();
}

void SequencerSidebarRight::emitAnnotationsCallout(Component *newAnnotationsMenu)
{
    ModalCallout::emit(newAnnotationsMenu, this->repriseButton.get());
}

void SequencerSidebarRight::setLinearMode()
{
    if (this->menuMode != MenuMode::PianoRollTools)
    {
        this->menuMode = MenuMode::PianoRollTools;
        this->recreateMenu();
        this->listBox->updateContent();
    }
}

void SequencerSidebarRight::setPatternMode()
{
    if (this->menuMode != MenuMode::PatternRollTools)
    {
        this->menuMode = MenuMode::PatternRollTools;
        this->recreateMenu();
        this->listBox->updateContent();
    }
}
