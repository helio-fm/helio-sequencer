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
#include "HybridRollCommandPanel.h"
#include "ProjectTreeItem.h"
#include "PlayerThread.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "PianoRoll.h"
#include "MidiSequence.h"
#include "InternalClipboard.h"
#include "CommandItemComponent.h"
#include "ProjectTimeline.h"
#include "AnnotationEvent.h"
#include "HelioCallout.h"
#include "TimelineCommandPanel.h"
#include "AnnotationCommandPanel.h"
#include "TimeSignatureCommandPanel.h"
#include "PianoRollToolbox.h"
#include "ArpeggiatorPanel.h"
#include "MoveToLayerCommandPanel.h"
#include "NotesTuningPanel.h"
#include "TriggersTrackMap.h"
#include "App.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "CommandIDs.h"

HybridRollCommandPanel::HybridRollCommandPanel(ProjectTreeItem &parent)
    : project(parent),
      lastSeekTime(0.0),
      lastTotalTime(0.0)
{
    this->recreateCommandDescriptions();

    this->setSize(MIDIROLL_COMMANDPANEL_WIDTH, 640);

    this->project.getTransport().addTransportListener(this);
    this->project.getEditMode().addChangeListener(this);
}

HybridRollCommandPanel::~HybridRollCommandPanel()
{
    this->project.getEditMode().removeChangeListener(this);
    this->project.getTransport().removeTransportListener(this);
}

void HybridRollCommandPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    switch (commandId)
    {
        case CommandIDs::ShowAnnotations:

        // если выбрана какая-то аннотация, показываем ее меню, если нет - показываем общее
        {
            const AnnotationEvent *selectedAnnotation = nullptr;
			const TimeSignatureEvent *selectedTimeSignature = nullptr;

			const ProjectTimeline *timeline = this->project.getTimeline();
            const double seekPosition = this->project.getTransport().getSeekPosition();

            if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
            {
				const double numBeats = double(roll->getNumBeats());
				const double seekThreshold = (1.0 / numBeats) / 10.0;

				for (int i = 0; i < timeline->getAnnotations()->size(); ++i)
                {
                    if (AnnotationEvent *annotation = dynamic_cast<AnnotationEvent *>(timeline->getAnnotations()->getUnchecked(i)))
                    {
                        const double annotationSeekPosition = roll->getTransportPositionByBeat(annotation->getBeat());
                        if (fabs(annotationSeekPosition - seekPosition) < seekThreshold)
                        {
                            selectedAnnotation = annotation;
                            break;
                        }
                    }
                }

				for (int i = 0; i < timeline->getTimeSignatures()->size(); ++i)
				{
					if (TimeSignatureEvent *ts = dynamic_cast<TimeSignatureEvent *>(timeline->getTimeSignatures()->getUnchecked(i)))
					{
						const double tsSeekPosition = roll->getTransportPositionByBeat(ts->getBeat());
						if (fabs(tsSeekPosition - seekPosition) < seekThreshold)
						{
							selectedTimeSignature = ts;
							break;
						}
					}
				}
            }

            // FIXME при нажатии что-то происходит со звуком, а индикатор едет дальше
            // (пока просто остановлю воспроизведение)
            this->project.getTransport().stopPlayback();

            if (selectedAnnotation != nullptr && MIDIROLL_COMMANDPANEL_SHOULD_SHOW_ANNOTATION_DETAILS)
            {
                this->emitAnnotationsCallout(new AnnotationCommandPanel(this->project, *selectedAnnotation));
            }
			else if (selectedTimeSignature != nullptr && MIDIROLL_COMMANDPANEL_SHOULD_SHOW_ANNOTATION_DETAILS)
			{
				this->emitAnnotationsCallout(new TimeSignatureCommandPanel(this->project, *selectedTimeSignature));
			}
            else
            {
                this->emitAnnotationsCallout(new TimelineCommandPanel(this->project));
            }
        }
            break;

        case CommandIDs::TransportStartPlayback:
            if (HybridRoll *roll = this->project.getLastFocusedRoll())
            {
                this->project.getTransport().startPlayback();
                roll->startFollowingIndicator();
            }
            break;

        case CommandIDs::TransportPausePlayback:
            if (HybridRoll *roll = this->project.getLastFocusedRoll())
            {
                this->project.getTransport().stopPlayback();
                roll->stopFollowingIndicator();
            }
            break;

        case CommandIDs::EditEvents:
            //if (HybridRoll *roll = this->project.getLastFocusedRoll())
            //{
            //    roll->showEditMenu();
            //}
            break;

        case CommandIDs::DeleteEvents:
            if (PianoRoll *roll = dynamic_cast<PianoRoll *>(this->project.getLastFocusedRoll()))
            {
                roll->deleteSelection();
            }
            break;

        case CommandIDs::CopyEvents:
            if (HybridRoll *roll = this->project.getLastFocusedRoll())
            {
                InternalClipboard::copy(*roll);
            }
            break;

        case CommandIDs::PasteEvents:
            if (HybridRoll *roll = this->project.getLastFocusedRoll())
            {
                InternalClipboard::paste(*roll);
            }
            break;


        case CommandIDs::CursorTool:
            this->project.getEditMode().setMode(HybridRollEditMode::defaultMode);
            break;

        case CommandIDs::DrawTool:
            this->project.getEditMode().setMode(HybridRollEditMode::drawMode);
            break;

        case CommandIDs::SelectionTool:
            this->project.getEditMode().setMode(HybridRollEditMode::selectionMode);
            break;

        case CommandIDs::ZoomTool:
            this->project.getEditMode().setMode(HybridRollEditMode::zoomMode);
            break;

        case CommandIDs::DragTool:
            this->project.getEditMode().setMode(HybridRollEditMode::dragMode);
            break;

        case CommandIDs::InsertSpaceTool:
            this->project.getEditMode().setMode(HybridRollEditMode::insertSpaceMode);
            break;

        case CommandIDs::WipeSpaceTool:
            this->project.getEditMode().setMode(HybridRollEditMode::wipeSpaceMode);
            break;

        case CommandIDs::ScissorsTool:
            this->project.getEditMode().setMode(HybridRollEditMode::scissorsMode);
            break;


        case CommandIDs::ZoomIn:
            if (HybridRoll *roll = this->project.getLastFocusedRoll())
            {
                roll->zoomInImpulse();
            }
            break;

        case CommandIDs::ZoomOut:
            if (HybridRoll *roll = this->project.getLastFocusedRoll())
            {
                roll->zoomOutImpulse();
            }
            break;


        case CommandIDs::Undo:
			this->project.undo();
            break;

        case CommandIDs::Redo:
			this->project.redo();
            break;

        case CommandIDs::ArpNotes:
            if (PianoRoll *roll = dynamic_cast<PianoRoll *>(this->project.getLastFocusedRoll()))
            {
                if (roll->getLassoSelection().getNumSelected() > 0)
                {
                    HelioCallout::emit(new ArpeggiatorPanel(roll->getTransport(), *roll), roll, true);
                }
                else
                {
                    App::Helio()->showTooltip(TRANS("warnings::emptyselection"));
                }
            }
            break;

        case CommandIDs::MoveEventsToLayer:
            if (PianoRoll *roll = dynamic_cast<PianoRoll *>(this->project.getLastFocusedRoll()))
            {
                if (roll->getLassoSelection().getNumSelected() > 0)
                {
                    HelioCallout::emit(new MoveToLayerCommandPanel(*roll, roll->getProject()), roll, true);
                }
                else
                {
                    App::Helio()->showTooltip(TRANS("warnings::emptyselection"));
                }
            }
            break;

        case CommandIDs::TweakNotesVolume:
            if (PianoRoll *roll = dynamic_cast<PianoRoll *>(this->project.getLastFocusedRoll()))
            {
                if (roll->getLassoSelection().getNumSelected() > 0)
                {
                    HelioCallout::emit(new NotesTuningPanel(roll->getProject(), *roll), roll, true);
                }
                else
                {
                    App::Helio()->showTooltip(TRANS("warnings::emptyselection"));
                }
            }
            break;

        default:
            break;
    }
    //[/UserCode_handleCommandMessage]
}

void HybridRollCommandPanel::childrenChanged()
{
    //[UserCode_childrenChanged] -- Add your code here...
    //this->updateButtonsImages();
    //[/UserCode_childrenChanged]
}

void HybridRollCommandPanel::mouseMove (const MouseEvent& e)
{
    //[UserCode_mouseMove] -- Add your code here...
    //[/UserCode_mouseMove]
}



//[MiscUserCode]

void HybridRollCommandPanel::recreateCommandDescriptions()
{
    this->commandDescriptions.clear();

    const bool defaultMode = this->project.getEditMode().isMode(HybridRollEditMode::defaultMode);
    const bool drawMode = this->project.getEditMode().isMode(HybridRollEditMode::drawMode);
    const bool selectionMode = this->project.getEditMode().isMode(HybridRollEditMode::selectionMode);
    const bool zoomMode = this->project.getEditMode().isMode(HybridRollEditMode::zoomMode);
    const bool dragMode = this->project.getEditMode().isMode(HybridRollEditMode::dragMode);
    const bool wipeSpaceMode = this->project.getEditMode().isMode(HybridRollEditMode::wipeSpaceMode);
    const bool insertSpaceMode = this->project.getEditMode().isMode(HybridRollEditMode::insertSpaceMode);
    const bool scissorsMode = this->project.getEditMode().isMode(HybridRollEditMode::scissorsMode);

    this->commandDescriptions.add(CommandItem::withParams(Icons::cursorTool, CommandIDs::CursorTool)->toggled(defaultMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::drawTool, CommandIDs::DrawTool)->toggled(drawMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::selectionTool, CommandIDs::SelectionTool)->toggled(selectionMode));
//    this->commandDescriptions.add(CommandItem::withParams(Icons::zoomTool, CommandIDs::ZoomTool)->toggled(zoomMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::dragTool, CommandIDs::DragTool)->toggled(dragMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::wipeScapeTool, CommandIDs::WipeSpaceTool)->toggled(wipeSpaceMode));
    this->commandDescriptions.add(CommandItem::withParams(Icons::insertSpaceTool, CommandIDs::InsertSpaceTool)->toggled(insertSpaceMode));

    this->commandDescriptions.add(CommandItem::withParams(Icons::zoomIn, CommandIDs::ZoomIn));
    this->commandDescriptions.add(CommandItem::withParams(Icons::zoomOut, CommandIDs::ZoomOut));
    this->commandDescriptions.add(CommandItem::withParams(Icons::undo, CommandIDs::Undo));
    this->commandDescriptions.add(CommandItem::withParams(Icons::redo, CommandIDs::Redo));

    this->commandDescriptions.add(CommandItem::withParams(Icons::volumeUp, CommandIDs::TweakNotesVolume));
    this->commandDescriptions.add(CommandItem::withParams(Icons::switcher, CommandIDs::MoveEventsToLayer));
//    this->commandDescriptions.add(CommandItem::withParams(Icons::ellipsis, CommandIDs::RefactorNotes));
    this->commandDescriptions.add(CommandItem::withParams(Icons::arpeggiator, CommandIDs::ArpNotes));

    this->commandDescriptions.add(CommandItem::withParams(Icons::copy, CommandIDs::CopyEvents));
    this->commandDescriptions.add(CommandItem::withParams(Icons::paste, CommandIDs::PasteEvents));
    this->commandDescriptions.add(CommandItem::withParams(Icons::trash, CommandIDs::DeleteEvents));
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//

int HybridRollCommandPanel::getNumRows()
{
    return this->commandDescriptions.size();
}

void HybridRollCommandPanel::paintListBoxItem(int rowNumber,
                                            Graphics &g,
                                            int width, int height,
                                            bool rowIsSelected)
{
    //
}


//===----------------------------------------------------------------------===//
// ChangeListener
//

void HybridRollCommandPanel::changeListenerCallback(ChangeBroadcaster *source)
{
    this->updateModeButtons();
}


//===----------------------------------------------------------------------===//
// Timer
//

void HybridRollCommandPanel::timerCallback()
{
    this->triggerAsyncUpdate();
}


//===----------------------------------------------------------------------===//
// TransportListener
//

void HybridRollCommandPanel::onSeek(const double newPosition,
                                  const double currentTimeMs, const double totalTimeMs)
{
    this->lastSeekTime = currentTimeMs; // todo locks?
    this->lastTotalTime = totalTimeMs;
    this->triggerAsyncUpdate();
}

void HybridRollCommandPanel::onTempoChanged(const double newTempo)
{

}

void HybridRollCommandPanel::onTotalTimeChanged(const double timeMs)
{
    this->lastTotalTime = timeMs;
}

void HybridRollCommandPanel::onPlay()
{
    this->timerStartSystemTime = Time::getMillisecondCounter();
    this->timerStartSeekTime = this->lastSeekTime;
    this->startTimer(100);
}

void HybridRollCommandPanel::onStop()
{
    this->stopTimer();
}
