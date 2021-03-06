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
#include "HybridRoll.h"
#include "PianoTrackNode.h"
#include "ProjectTimeline.h"

template<typename T>
const T *findSelectedEventOfType(MidiSequence *const sequence, float seekBeat)
{
    const T *selectedEvent = nullptr;

    for (int i = 0; i < sequence->size(); ++i)
    {
        if (T *event = dynamic_cast<T *>(sequence->getUnchecked(i)))
        {
            if (fabsf(event->getBeat() - seekBeat) < 0.001f)
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

    if (nullptr != this->project.getLastFocusedRoll())
    {
        const auto seekBeat = this->project.getTransport().getSeekBeat();
        const auto annotationsSequence = timeline->getAnnotations()->getSequence();
        const auto keySignaturesSequence = timeline->getKeySignatures()->getSequence();
        const auto timeSignaturesSequence = timeline->getTimeSignatures()->getSequence();
        selectedAnnotation = findSelectedEventOfType<AnnotationEvent>(annotationsSequence, seekBeat);
        selectedKeySignature = findSelectedEventOfType<KeySignatureEvent>(keySignaturesSequence, seekBeat);
        selectedTimeSignature = findSelectedEventOfType<TimeSignatureEvent>(timeSignaturesSequence, seekBeat);
    }

    MenuPanel::Menu menu;

    if (selectedAnnotation == nullptr)
    {
        menu.add(MenuItem::item(Icons::create,
            CommandIDs::AddAnnotation,
            TRANS(I18n::Menu::annotationAdd))->closesMenu());
    }

    if (selectedKeySignature == nullptr)
    {
        menu.add(MenuItem::item(Icons::create,
            CommandIDs::AddKeySignature,
            TRANS(I18n::Menu::keySignatureAdd))->closesMenu());
    }

    if (selectedTimeSignature == nullptr)
    {
        menu.add(MenuItem::item(Icons::create,
            CommandIDs::AddTimeSignature,
            TRANS(I18n::Menu::timeSignatureAdd))->closesMenu());
    }

    if (nullptr != this->project.getLastFocusedRoll())
    {
        const auto annotationsSequence = timeline->getAnnotations()->getSequence();

        for (int i = 0; i < annotationsSequence->size(); ++i)
        {
            if (auto *annotation = dynamic_cast<AnnotationEvent *>(annotationsSequence->getUnchecked(i)))
            {
                //double outTimeMs = 0.0;
                //double outTempo = 0.0;
                //const double seekPos = roll->getTransportPositionByBeat(annotation->getBeat());
                //this->project.getTransport().calcTimeAndTempoAt(seekPos, outTimeMs, outTempo);
                
                menu.add(MenuItem::item(Icons::annotation,
                    annotation->getDescription())->
                    //withSubLabel(Transport::getTimeString(outTimeMs))->
                    colouredWith(annotation->getColour())->
                    closesMenu()->
                    withAction([this, i]()
                    {
                        if (auto *roll = this->project.getLastFocusedRoll())
                        {
                            const auto annotations = this->project.getTimeline()->getAnnotations()->getSequence();
                            if (auto *ae = dynamic_cast<AnnotationEvent *>(annotations->getUnchecked(i)))
                            {
                                this->project.getTransport().seekToBeat(ae->getBeat());
                                roll->scrollToPlayheadPosition();
                            }
                        }
                    }));
            }
        }
    }
    else
    {
        jassertfalse;
    }
    
    this->updateContent(menu, SlideDown);
}
