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
#include "ProjectMenu.h"
#include "ProjectNode.h"
#include "ProjectTimeline.h"
#include "ProjectMetadataActions.h"
#include "ModalDialogInput.h"
#include "ModalDialogConfirmation.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "VersionControlNode.h"
#include "PatternEditorNode.h"
#include "KeySignaturesSequence.h"
#include "AudioCore.h"
#include "PianoSequence.h"
#include "RollBase.h"
#include "SequencerOperations.h"
#include "PatternOperations.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "Pattern.h"
#include "UndoStack.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "Config.h"

ProjectMenu::ProjectMenu(ProjectNode &parentProject, AnimationType animationType) :
    project(parentProject)
{
    this->instruments = App::Workspace().getAudioCore().getInstrumentsExceptInternal();
    this->showMainMenu(animationType);
}

void ProjectMenu::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::ProjectTransposeUp:
            PatternOperations::transposeProject(this->project, 1, this->transactionId);
            return;
        case CommandIDs::ProjectTransposeDown:
            PatternOperations::transposeProject(this->project, -1, this->transactionId);
            return;
        case CommandIDs::DeleteProject:
        {
            auto confirmationDialog = ModalDialogConfirmation::Presets::deleteProject();
            auto &project = this->project;
            confirmationDialog->onOk = [&project]()
            {
                auto inputDialog = ModalDialogInput::Presets::deleteProjectConfirmation(project.getName());
                inputDialog->onOk = [&project](const String &text)
                {
                    jassert(text == project.getName());
                    App::Workspace().unloadProject(project.getId(), true, true);
                };

                App::showModalComponent(move(inputDialog));
            };

            App::showModalComponent(move(confirmationDialog));
            return;
        }
        default:
            break;
    }
}

void ProjectMenu::showMainMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::piano, CommandIDs::SwitchToEditMode,
        TRANS(I18n::Menu::Project::editorLinear))->closesMenu());

    menu.add(MenuItem::item(Icons::patterns, CommandIDs::SwitchToArrangeMode,
        TRANS(I18n::Menu::Project::editorPattern))->closesMenu());

    menu.add(MenuItem::item(Icons::versionControl, CommandIDs::SwitchToVersioningMode,
        TRANS(I18n::Menu::Project::editorVcs))->closesMenu());

    menu.add(MenuItem::item(Icons::create,
        TRANS(I18n::Menu::Project::addItems))->withSubmenu()->withAction([this]()
        {
            this->showCreateItemsMenu(MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::render,
        TRANS(I18n::Menu::Project::render))->
        withSubmenu()->
        withAction([this]()
        {
            this->showRenderMenu();
        }));

    menu.add(MenuItem::item(Icons::refactor,
        TRANS(I18n::Menu::Project::refactor))->
        withSubmenu()->
        withAction([this]()
        {
            this->showBatchActionsMenu(MenuPanel::SlideLeft);
        }));
    
    menu.add(MenuItem::item(Icons::close,
        TRANS(I18n::Menu::Project::unload))->
        closesMenu()->
        withAction([this]()
        {
            App::Workspace().unloadProject(this->project.getId(), false, false);
        }));

    menu.add(MenuItem::item(Icons::remove,
        CommandIDs::DeleteProject, TRANS(I18n::Menu::Project::deleteConfirm)));

    this->updateContent(menu, animationType);
}

void ProjectMenu::showCreateItemsMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS(I18n::Menu::back))->withAction([this]()
        {
            this->showMainMenu(MenuPanel::SlideRight);
        }));

    menu.add(MenuItem::item(Icons::browse, CommandIDs::ImportMidi,
        TRANS(I18n::Menu::Project::importMidi))->closesMenu());

    menu.add(MenuItem::item(Icons::pianoTrack,
        TRANS(I18n::Menu::Project::addTrack))->withSubmenu()->withAction([this]()
        {
            this->showNewTrackMenu(MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::automationTrack,
        TRANS(I18n::Menu::Project::addAutomation))->withSubmenu()->withAction([this]()
        {
            this->showNewAutomationMenu(MenuPanel::SlideLeft);
        }));

    this->updateContent(menu, animationType);
}

void ProjectMenu::showNewTrackMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS(I18n::Menu::back))->withAction([this]()
        {
            this->showCreateItemsMenu(MenuPanel::SlideRight);
        }));

    for (const auto *instrument : this->instruments)
    {
        const auto instrumentId = instrument->getIdAndHash();
        menu.add(MenuItem::item(Icons::instrument,
            instrument->getName())->withAction([this, instrumentId]()
            {
                const float firstBeat = this->project.getProjectBeatRange().getStart();
                ProjectMenu::showNewTrackDialog(this->project, instrumentId, firstBeat);
            }));
    }

    this->updateContent(menu, animationType);
}

void ProjectMenu::showNewAutomationMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS(I18n::Menu::back))->withAction([this]()
        {
            this->showCreateItemsMenu(MenuPanel::SlideRight);
        }));

    menu.add(MenuItem::item(Icons::automationTrack,
        TRANS(I18n::Menu::Project::addTempo))->
        closesMenu()->
        withAction([this]()
        {
            String outTrackId;
            String instrumentId; // empty, it doesn't matter for the master tempo track
            const auto autoTracks = this->project.findChildrenOfType<AutomationTrackNode>();
            const auto autoTrackParams =
                SequencerOperations::createAutoTrackTemplate(this->project,
                    TRANS(I18n::Defaults::tempoTrackName), MidiTrack::tempoController,
                    instrumentId, outTrackId);

            this->project.getUndoStack()->beginNewTransaction();
            this->project.getUndoStack()->perform(new AutomationTrackInsertAction(this->project,
                &this->project, autoTrackParams, TRANS(I18n::Defaults::tempoTrackName)));
        }));

    for (auto *instrument : this->instruments)
    {
        menu.add(MenuItem::item(Icons::instrument,
            instrument->getName())->withSubmenu()->withAction([this, instrument]()
            {
                this->showControllersMenuForInstrument(instrument);
            }));
    }

    this->updateContent(menu, animationType);
}

void ProjectMenu::showControllersMenuForInstrument(const WeakReference<Instrument> instrument)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS(I18n::Menu::back))->withAction([this]()
        {
            this->showNewAutomationMenu(MenuPanel::SlideRight);
        }));


    static constexpr auto numControllersToShow = 80;
    for (int controllerNumber = 0; controllerNumber < numControllersToShow; ++controllerNumber)
    {
        const String controllerName = MidiMessage::getControllerName(controllerNumber);
        if (controllerName.isNotEmpty())
        {
            menu.add(MenuItem::item(Icons::automationTrack,
                String(controllerNumber) + ": " + TRANS(controllerName))->
                closesMenu()->
                withAction([this, controllerNumber, instrument]()
                {
                    String outTrackId;
                    const String instrumentId = instrument ? instrument->getIdAndHash() : "";
                    const String trackName =
                        TreeNode::createSafeName(MidiMessage::getControllerName(controllerNumber));
                    const auto autoTrackParams =
                        SequencerOperations::createAutoTrackTemplate(this->project,
                            trackName, controllerNumber, instrumentId, outTrackId);

                    this->project.getUndoStack()->beginNewTransaction();
                    this->project.getUndoStack()->perform(new AutomationTrackInsertAction(this->project,
                        &this->project, autoTrackParams, trackName));
                }));
        }
    }
    
    this->updateContent(menu, MenuPanel::SlideLeft);
}

void ProjectMenu::showRenderMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back,
        TRANS(I18n::Menu::back))->withAction([this]()
        {
            this->showMainMenu(MenuPanel::SlideRight);
        }));

#if !JUCE_ANDROID
    // WavAudioFormatWriter doesn't seem to work on my devices,
    // it fails to write the header due to non-seekable file stream,
    // see the assertion in WavAudioFormatWriter::writeHeader

    menu.add(MenuItem::item(Icons::render, CommandIDs::RenderToWAV,
        TRANS(I18n::Menu::Project::renderWav))->closesMenu());
#endif

    menu.add(MenuItem::item(Icons::render, CommandIDs::RenderToFLAC,
        TRANS(I18n::Menu::Project::renderFlac))->closesMenu());

    menu.add(MenuItem::item(Icons::commit, CommandIDs::ExportMidi,
        TRANS(I18n::Menu::Project::renderMidi))->closesMenu());

    this->updateContent(menu, MenuPanel::SlideLeft);
}

void ProjectMenu::showBatchActionsMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS(I18n::Menu::back))->withAction([this]()
        {
            this->showMainMenu(MenuPanel::SlideRight);
        }));

    menu.add(MenuItem::item(Icons::up,
        CommandIDs::ProjectTransposeUp,
        TRANS(I18n::Menu::Project::transposeUp)));

    menu.add(MenuItem::item(Icons::down,
        CommandIDs::ProjectTransposeDown,
        TRANS(I18n::Menu::Project::transposeDown)));

    menu.add(MenuItem::item(Icons::automationTrack, CommandIDs::ProjectSetOneTempo,
        TRANS(I18n::Menu::setOneTempo))->closesMenu());

    menu.add(MenuItem::item(Icons::automationTrack, CommandIDs::ProjectTempoUp1Bpm,
        TRANS(I18n::Menu::tempoUp1Bpm)));

    menu.add(MenuItem::item(Icons::automationTrack, CommandIDs::ProjectTempoDown1Bpm,
        TRANS(I18n::Menu::tempoDown1Bpm)));

    const auto tracks = this->project.findChildrenOfType<MidiTrackNode>();
    if (this->instruments.size() > 1 && tracks.size() > 0)
    {
        menu.add(MenuItem::item(Icons::instrument,
            TRANS(I18n::Menu::Project::changeInstrument))->withSubmenu()->withAction([this]()
            {
                this->showSetInstrumentMenu();
            }));
    }

    // todo better icons
    if (App::Config().getUiFlags()->areExperimentalFeaturesEnabled())
    {
        // hide this option by default, it seems to be rather confusing compared to the next one
        menu.add(MenuItem::item(Icons::arpeggiate,
            TRANS(I18n::Menu::Project::changeTemperament))->withSubmenu()->withAction([this]()
        {
            this->showTemperamentsMenu(false);
        }));
    }

    menu.add(MenuItem::item(Icons::arpeggiate,
        TRANS(I18n::Menu::Project::convertTemperament))->withSubmenu()->withAction([this]()
    {
        this->showTemperamentsMenu(true);
    }));

    this->updateContent(menu, animationType);
}

void ProjectMenu::showTemperamentsMenu(bool convertTracks)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->showBatchActionsMenu(MenuPanel::SlideRight);
    }));

    const auto temperaments = App::Config().getTemperaments()->getAll();
    const auto currentTemperament = this->project.getProjectInfo()->getTemperament();

    for (const auto &otherTemperament : temperaments)
    {
        menu.add(MenuItem::item(Icons::arpeggiate,
            TRANS(otherTemperament->getName()))->
            disabledIf(otherTemperament->getPeriodSize() == currentTemperament->getPeriodSize())->
            closesMenu()->
            withAction([this, currentTemperament, otherTemperament, convertTracks]()
        {
            if (convertTracks)
            {
                const bool anyModKeyPressed = Desktop::getInstance()
                    .getMainMouseSource().getCurrentModifiers().isAnyModifierKeyDown();
                // by default, convert notes using the temperaments' chromatic maps,
                // which is more flexible (doesn't make assumptions about temperaments),
                // as an alt mode, when any mod key is pressed, convert notes assuming
                // equal temperaments and using just proportions, which is more accurate
                const bool defaultConversionMode = !anyModKeyPressed;
                const bool hasMadeChanges =
                    SequencerOperations::remapNotesToTemperament(this->project,
                        otherTemperament, defaultConversionMode, true);

                if (!hasMadeChanges)
                {
                    this->project.checkpoint();
                }
            }
            else
            {
                this->project.checkpoint();
            }

            // let's also update key signatures (todo move this code somewhere):
            auto *harmonicContext = this->project.getTimeline()->getKeySignaturesSequence();
            SequencerOperations::remapKeySignaturesToTemperament(harmonicContext,
                currentTemperament, otherTemperament, App::Config().getScales()->getAll(),
                false); // false == already did checkpoint earlier

            // finally, the temperament itself:
            this->project.getUndoStack()->perform(
                new ProjectTemperamentChangeAction(this->project, *otherTemperament));
        }));
    }

    this->updateContent(menu, MenuPanel::SlideLeft);
}

void ProjectMenu::showSetInstrumentMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->showBatchActionsMenu(MenuPanel::SlideRight);
    }));

    for (const auto *instrument : this->instruments)
    {
        const auto instrumentId = instrument->getIdAndHash();
        menu.add(MenuItem::item(Icons::instrument,
            instrument->getName())->
            disabledIf(!instrument->isValid())->
            closesMenu()->
            withAction([this, instrumentId]()
            {
                DBG(instrumentId);

                const auto tracks = this->project.findChildrenOfType<MidiTrackNode>();

                if (tracks.size() > 0)
                {
                    this->project.getUndoStack()->beginNewTransaction();

                    for (auto *track : tracks)
                    {
                        track->setTrackInstrumentId(instrumentId, true, sendNotification);
                    }
                }
            }));
    }
    
    this->updateContent(menu, MenuPanel::SlideLeft);
}

void ProjectMenu::showNewTrackDialog(ProjectNode &project,
    const String &instrumentId, float beatPosition)
{
    String outTrackId;
    const auto trackTemplate =
        SequencerOperations::createPianoTrackTemplate(project,
            "", beatPosition, instrumentId, outTrackId);

    auto inputDialog = ModalDialogInput::Presets::newTrack();
    inputDialog->onOk = [trackTemplate, outTrackId, &project](const String &input)
    {
        project.checkpoint();
        project.getUndoStack()->perform(new PianoTrackInsertAction(project,
            &project, trackTemplate, input));

        if (!dynamic_cast<PatternEditorNode *>(project.findActiveNode()))
        {
            // when in piano roll, select and auto-zoom the new track for convenience:
            if (auto *pianoTrack = project.findTrackById<PianoTrackNode>(outTrackId))
            {
                const auto *clip = pianoTrack->getPattern()->getClips().getFirst();
                project.setEditableScope(*clip, true);
            }
        }
    };

    App::showModalComponent(move(inputDialog));
}
