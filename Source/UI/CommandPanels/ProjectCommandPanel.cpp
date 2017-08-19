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
#include "ProjectCommandPanel.h"
#include "ProjectTreeItem.h"
#include "Icons.h"
#include "App.h"
#include "RenderDialog.h"
#include "ModalDialogInput.h"
#include "ModalDialogConfirmation.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
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


ProjectCommandPanel::ProjectCommandPanel(ProjectTreeItem &parentProject, AnimationType animationType) :
    project(parentProject),
    haveSetBatchCheckpoint(false),
    layerNameString(TRANS("defaults::newlayer::name"))
{
    this->initMainMenu(animationType);
}

ProjectCommandPanel::~ProjectCommandPanel()
{
}

void ProjectCommandPanel::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
        case CommandIDs::Back:
            this->initMainMenu(CommandPanel::SlideRight);
            return;
            
        case CommandIDs::ProjectRenderMenu:
            this->initRenderMenu();
            return;
            
        case CommandIDs::ProjectBatchMenu:
            this->initBatchMenu();
            return;
            
        case CommandIDs::AddAutomation:
            this->initAutomationsMenu(CommandPanel::SlideLeft);
            return;

        case CommandIDs::ProjectAutomationsMenu:
            this->initAutomationsMenu(CommandPanel::SlideRight);
            return;
            
        case CommandIDs::RenderToFLAC:
            this->proceedToRenderDialog("FLAC");
            return;

        case CommandIDs::RenderToOGG:
            this->proceedToRenderDialog("OGG");
            return;

        case CommandIDs::RenderToWAV:
            this->proceedToRenderDialog("WAV");
            return;
            
        case CommandIDs::BatchChangeInstrument:
            this->initInstrumentSelection();
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
                App::Helio()->showTooltip(TRANS("menu::project::addtempo::failed"));
            }
            else
            {
                const String autoLayerParams = this->createAutoLayerTempate(TRANS("defaults::tempotrack::name"),
                                                                            MidiTrack::tempoController);
                
                this->project.getUndoStack()->beginNewTransaction();
                this->project.getUndoStack()->perform(new AutomationTrackInsertAction(this->project,
                                                                                        autoLayerParams,
                                                                                        TRANS("defaults::tempotrack::name")));
            }

            this->focusRollAndExit();
            return;
        }
            
        case CommandIDs::AddLayer:
        {
            Component *inputDialog =
            new ModalDialogInput(*this,
                                 this->layerNameString,
                                 TRANS("dialog::addlayer::caption"),
                                 TRANS("dialog::addlayer::proceed"),
                                 TRANS("dialog::addlayer::cancel"),
                                 CommandIDs::AddLayerConfirmed,
                                 CommandIDs::Cancel);
            
            App::Layout().showModalNonOwnedDialog(inputDialog);
            return;
        }
            
        case CommandIDs::AddLayerConfirmed:
        {
            this->project.setOpen(true);
            
            this->project.getUndoStack()->beginNewTransaction();
            this->project.getUndoStack()->perform(new PianoTrackInsertAction(this->project,
                                                                                     this->createPianoLayerTempate(""),
                                                                                     this->layerNameString));
            
            this->focusRollAndExit();
            return;
        }
            
        case CommandIDs::Cancel:
            return;
            
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
            this->getParentComponent()->exitModalState(0);
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
            this->project.getDocument()->exportAs("*.mid;*.midi", TreeItem::createSafeName(this->project.getName() + ".mid"));
#endif
            this->getParentComponent()->exitModalState(0);
            return;
        }

        case CommandIDs::UnloadProject:
            App::Workspace().unloadProjectById(this->project.getId());
            this->getParentComponent()->exitModalState(0);
            return;

        case CommandIDs::DeleteProject:
        {
            Component *confirmationDialog =
            new ModalDialogConfirmation(*this,
                                        TRANS("dialog::deleteproject::caption"),
                                        TRANS("dialog::deleteproject::proceed"),
                                        TRANS("dialog::deleteproject::cancel"),
                                        CommandIDs::DeleteProjectConfirmed1,
                                        CommandIDs::Cancel);
            
            App::Layout().showModalNonOwnedDialog(confirmationDialog);
            return;
        }
            
        case CommandIDs::DeleteProjectConfirmed1:
        {
            Component *inputDialog =
            new ModalDialogInput(*this,
                                 this->projectNameRemovalConfirmation,
                                 TRANS("dialog::deleteproject::confirm::caption"),
                                 TRANS("dialog::deleteproject::confirm::proceed"),
                                 TRANS("dialog::deleteproject::confirm::cancel"),
                                 CommandIDs::DeleteProjectConfirmed2,
                                 CommandIDs::Cancel);
            
            App::Layout().showModalNonOwnedDialog(inputDialog);
            return;
        }

        case CommandIDs::DeleteProjectConfirmed2:
        {
            if (this->projectNameRemovalConfirmation == this->project.getName())
            {
                this->project.deletePermanently();
            }
            else
            {
                App::Layout().showTooltip(TRANS("menu::project::delete::cancelled"));
            }

            this->getParentComponent()->exitModalState(0);
            return;
        }
    }
    
    const Array<Instrument *> &instruments = App::Workspace().getAudioCore().getInstruments();
    
    if (commandId >= CommandIDs::BatchSetInstrument &&
        commandId <= (CommandIDs::BatchSetInstrument + instruments.size()))
    {
        const int instrumentIndex = commandId - CommandIDs::BatchSetInstrument;
        if (instrumentIndex >= 0 && instrumentIndex < instruments.size())
        {
            Logger::writeToLog(instruments[instrumentIndex]->getIdAndHash());
            
            const Array<MidiTrackTreeItem *> tracks =
            this->project.findChildrenOfType<MidiTrackTreeItem>();
            
            if (tracks.size() > 0)
            {
                this->project.getUndoStack()->beginNewTransaction();
                
                for (auto track : tracks)
                {
                    const String trackId = track->getTrackId().toString();
                    const String instrumentId = instruments[instrumentIndex]->getIdAndHash();
                    this->project.getUndoStack()->perform(new MidiTrackChangeInstrumentAction(this->project, trackId, instrumentId));
                }
            }
            
            this->focusRollAndExit();
            return;
        }
    }
    
    if (commandId >= CommandIDs::ProjectInstrumentsMenu &&
        commandId < (CommandIDs::ProjectInstrumentsMenu + instruments.size()))
    {
        const int instrumentIndex = (commandId - CommandIDs::ProjectInstrumentsMenu);
        this->lastSelectedInstrument = instruments[instrumentIndex];
        this->initAutomationsControllersMenu();
        return;
    }
    
    if (commandId >= CommandIDs::AddCustomController &&
        commandId < (CommandIDs::AddCustomController + NUM_CONTROLLERS_TO_SHOW))
    {
        const int controllerNumber = (commandId - CommandIDs::AddCustomController);
        const String instrumentId = this->lastSelectedInstrument ? this->lastSelectedInstrument->getIdAndHash() : "";
        const String layerName = TreeItem::createSafeName(MidiMessage::getControllerName(controllerNumber));
        const String autoLayerParams = this->createAutoLayerTempate(layerName, controllerNumber, instrumentId);
        
        this->project.getUndoStack()->beginNewTransaction();
        this->project.getUndoStack()->perform(new AutomationTrackInsertAction(this->project,
                                                                                autoLayerParams,
                                                                                layerName));
        
        this->focusRollAndExit();
        return;
    }
}

void ProjectCommandPanel::proceedToRenderDialog(const String &extension)
{
    const File initialPath = File::getSpecialLocation(File::userMusicDirectory);
    const String renderFileName = this->project.getName() + "." + extension.toLowerCase();
    const String safeRenderName = File::createLegalFileName(renderFileName);

#if HELIO_DESKTOP
    FileChooser fc(TRANS("dialog::render::caption"),
                   File(initialPath.getChildFile(safeRenderName)), ("*." + extension), true);
    
    if (fc.browseForFileToSave(true))
    {
        App::Helio()->showModalComponent(new RenderDialog(this->project, fc.getResult(), extension));
    }
#else
    App::Helio()->showModalComponent(new RenderDialog(this->project, initialPath.getChildFile(safeRenderName), extension));
#endif
    
    this->getParentComponent()->exitModalState(0);
}

String ProjectCommandPanel::createPianoLayerTempate(const String &name) const
{
    ScopedPointer<MidiTrackTreeItem> newItem = new PianoTrackTreeItem(name);
    ScopedPointer<XmlElement> parameters = newItem->serialize();
    return parameters->createDocument("", false, false, "UTF-8", 1024);
}

String ProjectCommandPanel::createAutoLayerTempate(const String &name, int controllerNumber, const String &instrumentId) const
{
    ScopedPointer<MidiTrackTreeItem> newItem = new AutomationTrackTreeItem(name);
    auto itemLayer = static_cast<AutomationSequence *>(newItem->getSequence());
    
    newItem->setTrackControllerNumber(controllerNumber);
    newItem->setTrackInstrumentId(instrumentId);
    newItem->setTrackColour(Colours::royalblue);
    
    // init with one event
    const float defaultCV = newItem->isOnOffTrack() ? 1.f : 0.5f;
    const float firstBeat = this->project.getProjectRangeInBeats().getX();
    itemLayer->insert(AutomationEvent(itemLayer, firstBeat, defaultCV), false);
    
    ScopedPointer<XmlElement> parameters = newItem->serialize();
    return parameters->createDocument("", false, false, "UTF-8", 1024);
}

void ProjectCommandPanel::initMainMenu(AnimationType animationType)
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::layer, CommandIDs::AddLayer, TRANS("menu::project::addlayer")));

#if HELIO_DESKTOP
    cmds.add(CommandItem::withParams(Icons::automation, CommandIDs::AddAutomation, TRANS("menu::project::addautomation"))->withSubmenu());
    cmds.add(CommandItem::withParams(Icons::open, CommandIDs::ImportMidi, TRANS("menu::project::import::midi")));
    cmds.add(CommandItem::withParams(Icons::render, CommandIDs::ProjectRenderMenu, TRANS("menu::project::render"))->withSubmenu());
#endif

    const Array<MidiTrackTreeItem *> &layers = this->project.findChildrenOfType<MidiTrackTreeItem>();
    const Array<Instrument *> &instruments = App::Workspace().getAudioCore().getInstruments();
    if (instruments.size() > 1 && layers.size() > 0)
    {
        cmds.add(CommandItem::withParams(Icons::saxophone, CommandIDs::BatchChangeInstrument, TRANS("menu::project::change::instrument"))->withSubmenu());
    }

    cmds.add(CommandItem::withParams(Icons::group, CommandIDs::ProjectBatchMenu, TRANS("menu::project::refactor"))->withSubmenu());
    
#if JUCE_IOS
    cmds.add(CommandItem::withParams(Icons::commit, CommandIDs::ExportMidi, TRANS("menu::project::render::midi")));
#endif
    
    cmds.add(CommandItem::withParams(Icons::close, CommandIDs::UnloadProject, TRANS("menu::project::unload")));
    cmds.add(CommandItem::withParams(Icons::trash, CommandIDs::DeleteProject, TRANS("menu::project::delete")));
    this->updateContent(cmds, animationType);
}

void ProjectCommandPanel::initAutomationsMenu(AnimationType animationType)
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back")));
    cmds.add(CommandItem::withParams(Icons::automation, CommandIDs::AddTempoController, TRANS("menu::project::addtempo")));

    const Array<Instrument *> &instruments = App::Workspace().getAudioCore().getInstruments();
    
    for (int i = 0; i < instruments.size(); ++i)
    {
        cmds.add(CommandItem::withParams(Icons::saxophone, CommandIDs::ProjectInstrumentsMenu + i, instruments[i]->getName())->withSubmenu());
    }

    this->updateContent(cmds, animationType);
}

void ProjectCommandPanel::initAutomationsControllersMenu()
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::ProjectAutomationsMenu, TRANS("menu::back")));
    
    for (int i = 0; i < NUM_CONTROLLERS_TO_SHOW; ++i)
    {
        const String controllerName = MidiMessage::getControllerName(i);
        
        if (controllerName.isNotEmpty())
        {
            cmds.add(CommandItem::withParams(Icons::automation, CommandIDs::AddCustomController + i, String(i) + ": " + TRANS(controllerName)));
        }
    }
    
    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void ProjectCommandPanel::initRenderMenu()
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back")));
    cmds.add(CommandItem::withParams(Icons::render, CommandIDs::RenderToWAV, TRANS("menu::project::render::wav")));
    cmds.add(CommandItem::withParams(Icons::render, CommandIDs::RenderToOGG, TRANS("menu::project::render::ogg")));
    cmds.add(CommandItem::withParams(Icons::render, CommandIDs::RenderToFLAC, TRANS("menu::project::render::flac")));
    cmds.add(CommandItem::withParams(Icons::commit, CommandIDs::ExportMidi, TRANS("menu::project::render::midi")));
    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void ProjectCommandPanel::initBatchMenu()
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back")));
    cmds.add(CommandItem::withParams(Icons::up, CommandIDs::RefactorTransposeUp, TRANS("menu::project::refactor::halftoneup")));
    cmds.add(CommandItem::withParams(Icons::down, CommandIDs::RefactorTransposeDown, TRANS("menu::project::refactor::halftonedown")));
    //cmds.add(CommandItem::withParams(Icons::group, CommandIDs::RefactorRemoveOverlaps, TRANS("menu::project::refactor::cleanup")));
    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void ProjectCommandPanel::initInstrumentSelection()
{
    ReferenceCountedArray<CommandItem> cmds;
    cmds.add(CommandItem::withParams(Icons::left, CommandIDs::Back, TRANS("menu::back")));
    const Array<Instrument *> &info = App::Workspace().getAudioCore().getInstruments();
    
    for (int i = 0; i < info.size(); ++i)
    {
        cmds.add(CommandItem::withParams(Icons::saxophone, CommandIDs::BatchSetInstrument + i, info[i]->getName()));
    }
    
    this->updateContent(cmds, CommandPanel::SlideLeft);
}

void ProjectCommandPanel::focusRollAndExit()
{
    if (HybridRoll *roll = this->project.getLastFocusedRoll())
    {
        roll->grabKeyboardFocus();
    }
    
    if (Component *parent = this->getParentComponent())
    {
        parent->exitModalState(0);
    }
}
