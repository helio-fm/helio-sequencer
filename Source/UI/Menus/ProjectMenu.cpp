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

#include "Common.h"
#include "ProjectMenu.h"
#include "ProjectTreeItem.h"
#include "Icons.h"
#include "App.h"
#include "RenderDialog.h"
#include "ModalDialogInput.h"
#include "ModalDialogConfirmation.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "VersionControlTreeItem.h"
#include "PatternEditorTreeItem.h"
#include "AutomationSequence.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Instrument.h"
#include "PianoSequence.h"
#include "HybridRoll.h"
#include "Document.h"
#include "SuccessTooltip.h"
#include "MidiTrackActions.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"
#include "UndoStack.h"
#include "App.h"
#include "Workspace.h"
#include "CommandIDs.h"

#define NUM_CONTROLLERS_TO_SHOW 80

ProjectMenu::ProjectMenu(ProjectTreeItem &parentProject, AnimationType animationType) :
    project(parentProject)
{
    this->showMainMenu(animationType);
}

void ProjectMenu::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::RenderToFLAC:
            this->proceedToRenderDialog("FLAC");
            return;

        case CommandIDs::RenderToOGG:
            this->proceedToRenderDialog("OGG");
            return;

        case CommandIDs::RenderToWAV:
            this->proceedToRenderDialog("WAV");
            return;
        
        case CommandIDs::AddTempoController:
        {
            bool hasTempoTrack = false;
            Array<AutomationTrackTreeItem *> autos = this->project.findChildrenOfType<AutomationTrackTreeItem>();
            
            for (auto i : autos)
            {
                if (i->getTrackControllerNumber() == MidiTrack::tempoController)
                {
                    hasTempoTrack = true;
                    break;
                }
            }
            
            if (hasTempoTrack)
            {
                App::Layout().showTooltip(TRANS("menu::project::addtempo::failed"));
            }
            else
            {
                const auto autoLayerParams =
                    this->createAutoTrackTempate(TRANS("defaults::tempotrack::name"),
                        MidiTrack::tempoController);
                
                this->project.getUndoStack()->beginNewTransaction();
                this->project.getUndoStack()->perform(new AutomationTrackInsertAction(this->project,
                    &this->project, autoLayerParams,  TRANS("defaults::tempotrack::name")));
            }

            this->dismiss();
            return;
        }
            
        case CommandIDs::RefactorTransposeUp:
        {
            Array<MidiTrack *> tracks = this->project.getTracks();
            bool didCheckpoint = false;
            
            for (int i = 0; i < tracks.size(); ++i)
            {
                if (PianoSequence *pianoSequence =
                    dynamic_cast<PianoSequence *>(tracks.getUnchecked(i)->getSequence()))
                {
                    if (! didCheckpoint)
                    {
                        didCheckpoint = true;
                        pianoSequence->checkpoint();
                    }
                    
                    pianoSequence->transposeAll(1, false);
                }
            }
        }
            return;
            
        case CommandIDs::RefactorTransposeDown:
        {
            Array<MidiTrack *> tracks = this->project.getTracks();
            bool didCheckpoint = false;
            
            for (int i = 0; i < tracks.size(); ++i)
            {
                if (PianoSequence *pianoSequence =
                    dynamic_cast<PianoSequence *>(tracks.getUnchecked(i)->getSequence()))
                {
                    if (! didCheckpoint)
                    {
                        didCheckpoint = true;
                        pianoSequence->checkpoint();
                    }

                    pianoSequence->transposeAll(-1, false);
                }
            }
        }
            return;
            
        case CommandIDs::ImportMidi:
            this->project.getDocument()->import("*.mid;*.midi");
            this->dismiss();
            return;

        case CommandIDs::ExportMidi:
        {
#if JUCE_IOS
            const String safeName = TreeItem::createSafeName(this->project.getName()) + ".mid";
            File midiExport = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(safeName);
            this->project.exportMidi(midiExport);
            
            App::Helio()->showTooltip(TRANS("menu::project::render::savedto") + " '" + safeName + "'");
            App::Helio()->showModalComponent(new SuccessTooltip());
#else
            this->project.getDocument()->exportAs("*.mid;*.midi", this->project.getName() + ".mid");
#endif
            this->dismiss();
            return;
        }

        case CommandIDs::UnloadProject:
            App::Workspace().unloadProjectById(this->project.getId());
            this->dismiss();
            return;

        case CommandIDs::DeleteProject:
        {
            auto confirmationDialog = ModalDialogConfirmation::Presets::deleteProject();
            auto &project = this->project;
            confirmationDialog->onOk = [&project]()
            {
                auto inputDialog = ModalDialogInput::Presets::deleteProjectConfirmation();
                inputDialog->onOk = [&project](const String &text)
                {
                    if (text == project.getName())
                    {
                        project.deletePermanently();
                    }
                    else
                    {
                        App::Layout().showTooltip(TRANS("menu::project::delete::cancelled"));
                    }
                };

                App::Layout().showModalComponentUnowned(inputDialog.release());
            };

            App::Layout().showModalComponentUnowned(confirmationDialog.release());
            this->dismiss();
            return;
        }
    }
}

void ProjectMenu::proceedToRenderDialog(const String &extension)
{
    const File initialPath = File::getSpecialLocation(File::userMusicDirectory);
    const String renderFileName = this->project.getName() + "." + extension.toLowerCase();
    const String safeRenderName = File::createLegalFileName(renderFileName);

#if HELIO_DESKTOP
    FileChooser fc(TRANS("dialog::render::caption"),
                   File(initialPath.getChildFile(safeRenderName)), ("*." + extension), true);
    
    if (fc.browseForFileToSave(true))
    {
        App::Layout().showModalComponentUnowned(new RenderDialog(this->project, fc.getResult(), extension));
    }
#else
    App::Layout().showModalComponentUnowned(new RenderDialog(this->project, initialPath.getChildFile(safeRenderName), extension));
#endif
    
    this->dismiss();
}

ValueTree ProjectMenu::createPianoTrackTempate(const String &name, const String &instrumentId) const
{
    ScopedPointer<MidiTrackTreeItem> newItem = new PianoTrackTreeItem(name);
    newItem->setTrackInstrumentId(instrumentId, false);
    return newItem->serialize();
}

ValueTree ProjectMenu::createAutoTrackTempate(const String &name, int controllerNumber, const String &instrumentId) const
{
    ScopedPointer<MidiTrackTreeItem> newItem = new AutomationTrackTreeItem(name);
    auto itemLayer = static_cast<AutomationSequence *>(newItem->getSequence());
    
    newItem->setTrackControllerNumber(controllerNumber, false);
    newItem->setTrackInstrumentId(instrumentId, false);
    newItem->setTrackColour(Colours::royalblue, false);
    
    // init with a couple of events
    const float cv1 = newItem->isOnOffAutomationTrack() ? 1.f : 0.5f;
    const float cv2 = newItem->isOnOffAutomationTrack() ? 0.f : 0.5f;
    const float firstBeat = this->project.getProjectRangeInBeats().getX();
    itemLayer->insert(AutomationEvent(itemLayer, firstBeat, cv1), false);
    itemLayer->insert(AutomationEvent(itemLayer, firstBeat + BEATS_PER_BAR, cv2), false);

    return newItem->serialize();
}

void ProjectMenu::showMainMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::piano,
        TRANS("menu::project::editor::linear"))->withAction([this]()
        {
            if (this->project.getLastShownTrack() == nullptr)
            {
                this->project.selectChildOfType<PianoTrackTreeItem>();
            }
            else
            {
                this->project.getLastShownTrack()->setSelected(true, true);
            }
        }));

    menu.add(MenuItem::item(Icons::patterns,
        TRANS("menu::project::editor::pattern"))->withAction([this]()
        {
            this->project.selectChildOfType<PatternEditorTreeItem>();
        }));

    menu.add(MenuItem::item(Icons::versionControl,
        TRANS("menu::project::editor::vcs"))->withAction([this]()
        {
            this->project.selectChildOfType<VersionControlTreeItem>();
        }));

    menu.add(MenuItem::item(Icons::create,
        TRANS("menu::project::additems"))->withSubmenu()->withAction([this]()
        {
            this->showCreateItemsMenu(MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::render,
        TRANS("menu::project::render"))->withSubmenu()->withAction([this]()
        {
            this->showRenderMenu();
        }));

    menu.add(MenuItem::item(Icons::refactor,
        TRANS("menu::project::refactor"))->withSubmenu()->withAction([this]()
        {
            this->showBatchActionsMenu(MenuPanel::SlideLeft);
        }));
    
    menu.add(MenuItem::item(Icons::close, CommandIDs::UnloadProject, TRANS("menu::project::unload")));
    menu.add(MenuItem::item(Icons::remove, CommandIDs::DeleteProject, TRANS("menu::project::delete")));
    this->updateContent(menu, animationType);
}

void ProjectMenu::showCreateItemsMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS("menu::back"))->withTimer()->withAction([this]()
        {
            this->showMainMenu(MenuPanel::SlideRight);
        }));

#if HELIO_DESKTOP
    menu.add(MenuItem::item(Icons::browse,
        CommandIDs::ImportMidi,
        TRANS("menu::project::import::midi")));
#endif

    menu.add(MenuItem::item(Icons::pianoTrack,
        TRANS("menu::project::addlayer"))->withSubmenu()->withAction([this]()
        {
            this->showNewTrackMenu(MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::automationTrack,
        TRANS("menu::project::addautomation"))->withSubmenu()->withAction([this]()
        {
            this->showNewAutomationMenu(MenuPanel::SlideLeft);
        }));

    this->updateContent(menu, animationType);
}

void ProjectMenu::showNewTrackMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS("menu::back"))->withTimer()->withAction([this]()
        {
            this->showCreateItemsMenu(MenuPanel::SlideRight);
        }));

    const auto &instruments = App::Workspace().getAudioCore().getInstruments();
    for (int i = 0; i < instruments.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::instrument,
            instruments[i]->getName())->withAction([this, instrumentId = instruments[i]->getIdAndHash()]()
            {
                auto &project = this->project;
                const ValueTree trackTemplate = this->createPianoTrackTempate("", instrumentId);
                auto inputDialog = ModalDialogInput::Presets::newTrack();
                inputDialog->onOk = [trackTemplate, &project](const String &input)
                {
                    project.checkpoint();
                    project.getUndoStack()->perform(new PianoTrackInsertAction(project,
                        &project, trackTemplate, input));
                };

                App::Layout().showModalComponentUnowned(inputDialog.release());
                this->dismiss();
            }));
    }

    this->updateContent(menu, animationType);
}

void ProjectMenu::showNewAutomationMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS("menu::back"))->withTimer()->withAction([this]()
        {
            this->showCreateItemsMenu(MenuPanel::SlideRight);
        }));

    menu.add(MenuItem::item(Icons::automationTrack,
        CommandIDs::AddTempoController,
        TRANS("menu::project::addtempo")));

    const auto &instruments = App::Workspace().getAudioCore().getInstruments();
    for (int i = 0; i < instruments.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::instrument,
            instruments[i]->getName())->withSubmenu()->withAction([this, instrument = instruments[i]]()
            {
                this->showControllersMenuForInstrument(instrument);
            }));
    }

    this->updateContent(menu, animationType);
}

void ProjectMenu::showControllersMenuForInstrument(WeakReference<Instrument> instrument)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS("menu::back"))->withTimer()->withAction([this]()
        {
            this->showNewAutomationMenu(MenuPanel::SlideRight);
        }));

    for (int i = 0; i < NUM_CONTROLLERS_TO_SHOW; ++i)
    {
        const String controllerName = MidiMessage::getControllerName(i);
        if (controllerName.isNotEmpty())
        {
            menu.add(MenuItem::item(Icons::automationTrack,
                String(i) + ": " + TRANS(controllerName))->withAction([this, controllerNumber = i, instrument]()
                {
                    const String instrumentId = instrument ? instrument->getIdAndHash() : "";
                    const String trackName = TreeItem::createSafeName(MidiMessage::getControllerName(controllerNumber));
                    const ValueTree autoTrackParams = this->createAutoTrackTempate(trackName, controllerNumber, instrumentId);

                    this->project.getUndoStack()->beginNewTransaction();
                    this->project.getUndoStack()->perform(new AutomationTrackInsertAction(this->project,
                        &this->project, autoTrackParams, trackName));

                    this->dismiss();
                }));
        }
    }
    
    this->updateContent(menu, MenuPanel::SlideLeft);
}

void ProjectMenu::showRenderMenu()
{
    MenuPanel::Menu menu;

    // TODO! save rendered wavs on mobile in the same way as midi export
#if HELIO_DESKTOP
    const bool noRender = false;
#else
    const bool noRender = true;
#endif

    menu.add(MenuItem::item(Icons::back,
        TRANS("menu::back"))->withTimer()->withAction([this]()
        {
            this->showMainMenu(MenuPanel::SlideRight);
        }));

    menu.add(MenuItem::item(Icons::render,
        CommandIDs::RenderToWAV,
        TRANS("menu::project::render::wav"))->disabledIf(noRender));

    menu.add(MenuItem::item(Icons::render,
        CommandIDs::RenderToOGG,
        TRANS("menu::project::render::ogg"))->disabledIf(noRender));

    menu.add(MenuItem::item(Icons::render,
        CommandIDs::RenderToFLAC,
        TRANS("menu::project::render::flac"))->disabledIf(noRender));

    menu.add(MenuItem::item(Icons::commit,
        CommandIDs::ExportMidi,
        TRANS("menu::project::render::midi")));

    this->updateContent(menu, MenuPanel::SlideLeft);
}

void ProjectMenu::showBatchActionsMenu(AnimationType animationType)
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS("menu::back"))->withTimer()->withAction([this]()
        {
            this->showMainMenu(MenuPanel::SlideRight);
        }));

    menu.add(MenuItem::item(Icons::up,
        CommandIDs::RefactorTransposeUp,
        TRANS("menu::project::refactor::halftoneup")));

    menu.add(MenuItem::item(Icons::down,
        CommandIDs::RefactorTransposeDown,
        TRANS("menu::project::refactor::halftonedown")));

    //menu.add(MenuItem::item(Icons::group, CommandIDs::RefactorRemoveOverlaps, TRANS("menu::project::refactor::cleanup")));

    const auto &tracks = this->project.findChildrenOfType<MidiTrackTreeItem>();
    const auto &instruments = App::Workspace().getAudioCore().getInstruments();
    if (instruments.size() > 1 && tracks.size() > 0)
    {
        menu.add(MenuItem::item(Icons::instrument,
            TRANS("menu::project::change::instrument"))->withSubmenu()->withAction([this]()
            {
                this->showSetInstrumentMenu();
            }));
    }

    this->updateContent(menu, animationType);
}

void ProjectMenu::showSetInstrumentMenu()
{
    MenuPanel::Menu menu;
    menu.add(MenuItem::item(Icons::back,
        TRANS("menu::back"))->withTimer()->withAction([this]()
        {
            this->showBatchActionsMenu(MenuPanel::SlideRight);
        }));

    const auto &instruments = App::Workspace().getAudioCore().getInstruments();
    for (int i = 0; i < instruments.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::instrument, instruments[i]->getName())->withAction([this, i, instruments]()
        {
            if (i >= 0 && i < instruments.size())
            {
                Logger::writeToLog(instruments[i]->getIdAndHash());

                const Array<MidiTrackTreeItem *> tracks =
                    this->project.findChildrenOfType<MidiTrackTreeItem>();

                if (tracks.size() > 0)
                {
                    this->project.getUndoStack()->beginNewTransaction();

                    for (auto *track : tracks)
                    {
                        const String instrumentId = instruments[i]->getIdAndHash();
                        this->project.getUndoStack()->
                            perform(new MidiTrackChangeInstrumentAction(this->project,
                                track->getTrackId(), instrumentId));
                    }
                }

                this->dismiss();
            }
        }));
    }
    
    this->updateContent(menu, MenuPanel::SlideLeft);
}
