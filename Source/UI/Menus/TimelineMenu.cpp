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
#include "TimelineMenu.h"
#include "ProjectNode.h"
#include "MainLayout.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "AnnotationEvent.h"
#include "TimeSignatureEvent.h"
#include "PianoTrackNode.h"
#include "ProjectTimeline.h"
#include "MidiSequence.h"
#include "ModalDialogInput.h"
#include "CommandIDs.h"

template<typename T>
const T *findSelectedEventOfType(MidiSequence *const sequence, HybridRoll *const roll,
    double seekPosition, double seekThreshold)
{
    const T *selectedEvent = nullptr;

    for (int i = 0; i < sequence->size(); ++i)
    {
        if (T *event = dynamic_cast<T *>(sequence->getUnchecked(i)))
        {
            const double eventSeekPosition = roll->getTransportPositionByBeat(event->getBeat());
            if (fabs(eventSeekPosition - seekPosition) < seekThreshold)
            {
                selectedEvent = event;
                break;
            }
        }
    }

    return selectedEvent;
}

TimelineMenu::TimelineMenu(ProjectNode &parentProject) :
    project(parentProject)
{
    const AnnotationEvent *selectedAnnotation = nullptr;
    const KeySignatureEvent *selectedKeySignature = nullptr;
    const TimeSignatureEvent *selectedTimeSignature = nullptr;
    const ProjectTimeline *timeline = this->project.getTimeline();

    if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
    {
        const double seekPosition = this->project.getTransport().getSeekPosition();
        const double seekThreshold = (1.0 / double(roll->getNumBeats())) / 10.0;
        const auto annotationsSequence = timeline->getAnnotations()->getSequence();
        const auto keySignaturesSequence = timeline->getKeySignatures()->getSequence();
        const auto timeSignaturesSequence = timeline->getTimeSignatures()->getSequence();
        selectedAnnotation = findSelectedEventOfType<AnnotationEvent>(annotationsSequence, roll, seekPosition, seekThreshold);
        selectedKeySignature = findSelectedEventOfType<KeySignatureEvent>(keySignaturesSequence, roll, seekPosition, seekThreshold);
        selectedTimeSignature = findSelectedEventOfType<TimeSignatureEvent>(timeSignaturesSequence, roll, seekPosition, seekThreshold);
    }
    
    ReferenceCountedArray<MenuItem> cmds;
    
    if (selectedAnnotation == nullptr)
    {
        cmds.add(MenuItem::item(Icons::create,
            CommandIDs::AddAnnotation,
            TRANS("menu::annotation::add"))->closesMenu());
    }

    if (selectedKeySignature == nullptr)
    {
        cmds.add(MenuItem::item(Icons::create,
            CommandIDs::AddKeySignature,
            TRANS("menu::keysignature::add"))->closesMenu());
    }

    if (selectedTimeSignature == nullptr)
    {
        cmds.add(MenuItem::item(Icons::create,
            CommandIDs::AddTimeSignature,
            TRANS("menu::timesignature::add"))->closesMenu());
    }

    if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
    {
        const auto annotationsSequence = timeline->getAnnotations()->getSequence();

        for (int i = 0; i < annotationsSequence->size(); ++i)
        {
            if (AnnotationEvent *annotation =
                dynamic_cast<AnnotationEvent *>(annotationsSequence->getUnchecked(i)))
            {
                //double outTimeMs = 0.0;
                //double outTempo = 0.0;
                //const double seekPos = roll->getTransportPositionByBeat(annotation->getBeat());
                //this->project.getTransport().calcTimeAndTempoAt(seekPos, outTimeMs, outTempo);
                
                cmds.add(MenuItem::item(Icons::annotation, annotation->getDescription())->
                    //withSubLabel(Transport::getTimeString(outTimeMs))->
                    colouredWith(annotation->getTrackColour())->withAction([this, i]()
                {
                    const auto timeline = this->project.getTimeline();
                    const auto annotations = timeline->getAnnotations()->getSequence();
                    if (auto roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
                    {
                        if (auto annotation = dynamic_cast<AnnotationEvent *>(annotations->getUnchecked(i)))
                        {
                            const double seekPosition = roll->getTransportPositionByBeat(annotation->getBeat());
                            this->project.getTransport().seekToPosition(seekPosition);
                            roll->scrollToSeekPosition();
                        }
                    }

                    this->dismiss();
                }));
            }
        }
    }
    else
    {
        jassertfalse;
    }
    
    this->updateContent(cmds, SlideDown);
}
