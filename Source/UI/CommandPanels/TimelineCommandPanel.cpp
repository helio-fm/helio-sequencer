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
#include "TimelineCommandPanel.h"
#include "ProjectTreeItem.h"
#include "MainLayout.h"
#include "Icons.h"
#include "MidiRoll.h"
#include "AnnotationEvent.h"
#include "TimeSignatureEvent.h"
#include "PianoLayerTreeItem.h"
#include "ProjectTimeline.h"
#include "MidiLayer.h"
#include "ModalDialogInput.h"
#include "App.h"

TimelineCommandPanel::TimelineCommandPanel(ProjectTreeItem &parentProject) :
    project(parentProject)
{
    const AnnotationEvent *selectedAnnotation = nullptr;
	const TimeSignatureEvent *selectedTimeSignature = nullptr;

	const ProjectTimeline *timeline = this->project.getTimeline();
    const double seekPosition = this->project.getTransport().getSeekPosition();
    
    if (MidiRoll *roll = dynamic_cast<MidiRoll *>(this->project.getLastFocusedRoll()))
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
    
    ReferenceCountedArray<CommandItem> cmds;
    
    if (selectedAnnotation == nullptr)
    {
        cmds.add(CommandItem::withParams(Icons::plus, CommandIDs::AddAnnotation, TRANS("menu::annotation::add")));
    }

	if (selectedTimeSignature == nullptr)
	{
		cmds.add(CommandItem::withParams(Icons::plus, CommandIDs::AddTimeSignature, TRANS("menu::timesignature::add")));
	}
    
    if (MidiRoll *roll = dynamic_cast<MidiRoll *>(this->project.getLastFocusedRoll()))
    {
        for (int i = 0; i < timeline->getAnnotations()->size(); ++i)
        {
            if (AnnotationEvent *annotation = dynamic_cast<AnnotationEvent *>(timeline->getAnnotations()->getUnchecked(i)))
            {
                const int commandIndex = (CommandIDs::JumpToAnnotation + i);
                
                double outTimeMs = 0.0;
                double outTempo = 0.0;
                const double seekPos = roll->getTransportPositionByBeat(annotation->getBeat());
                this->project.getTransport().calcTimeAndTempoAt(seekPos, outTimeMs, outTempo);
                
                cmds.add(CommandItem::withParams(Icons::annotation,
                                                 commandIndex,
                                                 annotation->getDescription())->
                         withSubLabel(Transport::getTimeString(outTimeMs))->
                         colouredWith(annotation->getColour()));
            }
        }
    }
    else
    {
        jassertfalse;
    }
    
    this->updateContent(cmds, SlideLeft);
}

TimelineCommandPanel::~TimelineCommandPanel()
{
}

void TimelineCommandPanel::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::AddAnnotation)
    {
        if (MidiRoll *roll = dynamic_cast<MidiRoll *>(this->project.getLastFocusedRoll()))
        {
            Component *inputDialog =
            new ModalDialogInput(*this,
                                 this->newEventData,
                                 TRANS("dialog::annotation::add::caption"),
                                 TRANS("dialog::annotation::add::proceed"),
                                 TRANS("dialog::annotation::add::cancel"),
                                 CommandIDs::AddAnnotationConfirmed,
                                 CommandIDs::Cancel);
            
            App::Layout().showModalNonOwnedDialog(inputDialog);
        }
    }
    else if (commandId == CommandIDs::AddAnnotationConfirmed)
    {
        if (MidiRoll *roll = dynamic_cast<MidiRoll *>(this->project.getLastFocusedRoll()))
        {
			roll->insertAnnotationWithinScreen(this->newEventData);
            this->newEventData.clear();
            this->getParentComponent()->exitModalState(0);
            roll->grabKeyboardFocus();
        }
    }
	else if (commandId == CommandIDs::AddTimeSignature)
	{
		if (MidiRoll *roll = dynamic_cast<MidiRoll *>(this->project.getLastFocusedRoll()))
		{
			Component *inputDialog =
				new ModalDialogInput(*this,
					this->newEventData,
					TRANS("dialog::timesignature::add::caption"),
					TRANS("dialog::timesignature::add::proceed"),
					TRANS("dialog::timesignature::add::cancel"),
					CommandIDs::AddTimeSignatureConfirmed,
					CommandIDs::Cancel);

			App::Layout().showModalNonOwnedDialog(inputDialog);
		}
	}
	else if (commandId == CommandIDs::AddTimeSignatureConfirmed)
	{
		if (MidiRoll *roll = dynamic_cast<MidiRoll *>(this->project.getLastFocusedRoll()))
		{
			int numerator;
			int denominator;
			TimeSignatureEvent::parseString(this->newEventData, numerator, denominator);
			roll->insertTimeSignatureWithinScreen(numerator, denominator);
			this->newEventData.clear();
			this->getParentComponent()->exitModalState(0);
			roll->grabKeyboardFocus();
		}
	}
    else if (commandId == CommandIDs::Cancel)
    {
        if (MidiRoll *roll = dynamic_cast<MidiRoll *>(this->project.getLastFocusedRoll()))
        {
            this->getParentComponent()->exitModalState(0);
            roll->grabKeyboardFocus();
        }
    }
    else
    {
        ProjectTimeline *timeline = this->project.getTimeline();
        const int annotationIndex = (commandId - CommandIDs::JumpToAnnotation);
        
        if (MidiRoll *roll =
            dynamic_cast<MidiRoll *>(this->project.getLastFocusedRoll()))
        {
            if (AnnotationEvent *annotation =
                dynamic_cast<AnnotationEvent *>(timeline->getAnnotations()->getUnchecked(annotationIndex)))
            {
                const double seekPosition = roll->getTransportPositionByBeat(annotation->getBeat());
                this->project.getTransport().seekToPosition(seekPosition);
                roll->scrollToSeekPosition();
            }
        }

        this->getParentComponent()->exitModalState(0);
    }
}
