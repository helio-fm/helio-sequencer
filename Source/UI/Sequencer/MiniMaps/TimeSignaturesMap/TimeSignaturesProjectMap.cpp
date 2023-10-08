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
#include "TimeSignaturesProjectMap.h"
#include "ProjectNode.h"
#include "MidiSequence.h"
#include "MidiTrackNode.h"
#include "ProjectTimeline.h"
#include "PlayerThread.h"
#include "RollBase.h"
#include "TrackStartIndicator.h"
#include "TrackEndIndicator.h"
#include "TimeSignatureDialog.h"
#include "TimeSignatureLargeComponent.h"
#include "TimeSignatureSmallComponent.h"

TimeSignaturesProjectMap::TimeSignaturesProjectMap(ProjectNode &project,
    SafePointer<RollBase> roll, Type type) :
    ScrolledComponent(roll),
    project(project),
    type(type)
{
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    this->trackStartIndicator = make<TrackStartIndicator>();
    this->addAndMakeVisible(this->trackStartIndicator.get());

    this->trackEndIndicator = make<TrackEndIndicator>();
    this->addAndMakeVisible(this->trackEndIndicator.get());

    this->updateTrackRangeIndicatorsAnchors();

    this->reloadTrackMap();

    this->project.addListener(this);
    this->project.getTimeline()->getTimeSignaturesAggregator()->addListener(this);
}

TimeSignaturesProjectMap::~TimeSignaturesProjectMap()
{
    this->project.getTimeline()->getTimeSignaturesAggregator()->removeListener(this);
    this->project.removeListener(this);
}

void TimeSignaturesProjectMap::updateTrackRangeIndicatorsAnchors()
{
    const double rollLengthInBeats = double(this->rollLastBeat - this->rollFirstBeat);
    const double absStart = double(this->projectFirstBeat - this->rollFirstBeat) / rollLengthInBeats;
    const double absEnd = double(this->projectLastBeat - this->rollFirstBeat) / rollLengthInBeats;
    this->trackStartIndicator->setAnchoredAt(absStart);
    this->trackEndIndicator->setAnchoredAt(absEnd);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void TimeSignaturesProjectMap::resized()
{
    TimeSignatureComponent *previous = nullptr;

    for (int i = 0; i < this->timeSignatureComponents.size(); ++i)
    {
        auto *current = this->timeSignatureComponents.getUnchecked(i);

        if (previous != nullptr)
        {
            this->applyTimeSignatureBounds(previous, current);
        }

        if (i == (this->timeSignatureComponents.size() - 1))
        {
            this->applyTimeSignatureBounds(current, nullptr);
        }

        previous = current;
    }

    this->updateTrackRangeIndicatorsAnchors();
}

//===----------------------------------------------------------------------===//
// TimeSignaturesAggregator::Listener
//===----------------------------------------------------------------------===//

void TimeSignaturesProjectMap::onTimeSignaturesUpdated()
{
    const auto &sequenceToSyncWith =
        *this->project.getTimeline()->getTimeSignaturesAggregator()->getSequence();

    for (const auto *event : sequenceToSyncWith)
    {
        const auto &ts = static_cast<const TimeSignatureEvent &>(*event);

        if (auto *myComponent = this->timeSignaturesMap[ts])
        {
            myComponent->updateContent(ts);
            this->applyTimeSignatureBounds(myComponent, nullptr);
        }
        else
        {
            auto *newComponent = this->createComponent();
            this->addAndMakeVisible(newComponent);
            newComponent->updateContent(ts);
            this->applyTimeSignatureBounds(newComponent, nullptr);

            this->timeSignatureComponents.addSorted(*newComponent, newComponent);
            this->timeSignaturesMap[ts] = newComponent;
        }
    }

    jassert(this->timeSignatureComponents.size() == int(this->timeSignaturesMap.size()));
    jassert(this->timeSignatureComponents.size() >= sequenceToSyncWith.size());

    if (this->timeSignatureComponents.size() > sequenceToSyncWith.size())
    {
        // so yes, nested loops here suck, but we're only going to get
        // in this branch when something is deleted (not really often),
        // and we expect to have not that many time signatures, maybe tens at max
        for (int i = 0; i < this->timeSignatureComponents.size();)
        {
            bool shouldDelete = true;
            const auto &myTs = this->timeSignatureComponents.getUnchecked(i)->getEvent();
            for (const auto *theirEvent : sequenceToSyncWith)
            {
                const auto &theirTs = static_cast<const TimeSignatureEvent &>(*theirEvent);

                if (theirTs == myTs)
                {
                    shouldDelete = false;
                    break;
                }
            }

            if (shouldDelete)
            {
                this->timeSignaturesMap.erase(myTs);
                this->timeSignatureComponents.remove(i);
            }
            else
            {
                i++;
            }
        }
    }
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void TimeSignaturesProjectMap::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectFirstBeat = firstBeat;
    this->projectLastBeat = lastBeat;

    if (this->rollFirstBeat > firstBeat || this->rollLastBeat < lastBeat)
    {
        this->rollFirstBeat = jmin(firstBeat, this->rollFirstBeat);
        this->rollLastBeat = jmax(lastBeat, this->rollLastBeat);
        this->resized();
    }
    else
    {
        this->updateTrackRangeIndicatorsAnchors();
    }
}

void TimeSignaturesProjectMap::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    if (this->rollFirstBeat != firstBeat || this->rollLastBeat != lastBeat)
    {
        this->rollFirstBeat = firstBeat;
        this->rollLastBeat = lastBeat;
        this->resized();
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

void TimeSignaturesProjectMap::onTimeSignatureTapped(TimeSignatureComponent *c)
{
    const auto seekBeat = this->project.getTransport().getSeekBeat();
    const auto newSeekBeat = c->getBeat();
    const bool wasPlaying = this->project.getTransport().isPlaying();

    this->project.getTransport().stopPlaybackAndRecording();
    this->project.getTransport().seekToBeat(newSeekBeat);

    if (fabs(c->getBeat() - seekBeat) < 0.001f && !wasPlaying)
    {
        this->showDialogFor(c);
    }
}

void TimeSignaturesProjectMap::showDialogFor(TimeSignatureComponent *c)
{
    if (this->project.getTransport().isPlaying())
    {
        return;
    }

    if (auto *track = dynamic_cast<MidiTrackNode *>(c->getEvent().getTrack().get()))
    {
        // can't use c->getEvent() here, because is's auto-generated and has absolute beat,
        // so we take the track's time signature template instead
        jassert(track->getTimeSignatureOverride() != nullptr);
        App::showModalComponent(TimeSignatureDialog::editingDialog(*this, this->project, *track->getTimeSignatureOverride()));
    }
    else
    {
        App::showModalComponent(TimeSignatureDialog::editingDialog(*this, this->project, c->getEvent()));
    }
}

void TimeSignaturesProjectMap::alternateActionFor(TimeSignatureComponent *nc)
{
    // todo what?
    jassertfalse;

    // const bool isShiftPressed = Desktop::getInstance()
    //     .getMainMouseSource().getCurrentModifiers().isShiftDown();
    // const bool shouldClearSelection = !isShiftPressed;
    // this->roll.selectEventsInRange(startBeat, endBeat, shouldClearSelection);
}

float TimeSignaturesProjectMap::getBeatByXPosition(int x) const
{
    const int xRoll = int(float(x) / float(this->getWidth()) * float(this->roll->getWidth()));
    const float targetBeat = this->roll->getRoundBeatSnapByXPosition(xRoll);
    return jlimit(this->rollFirstBeat, this->rollLastBeat, targetBeat);
}

void TimeSignaturesProjectMap::reloadTrackMap()
{
    jassert(this->project.getTimeline() != nullptr);

    this->timeSignaturesMap.clear();
    this->timeSignatureComponents.clear();

    const auto &timeSignatures =
        *this->project.getTimeline()->getTimeSignaturesAggregator()->getSequence();

    for (const auto *event : timeSignatures)
    {
        const auto &ts = static_cast<const TimeSignatureEvent &>(*event);

        auto *component = this->createComponent();
        this->addAndMakeVisible(component);
        component->updateContent(ts);

        this->timeSignatureComponents.addSorted(*component, component);
        this->timeSignaturesMap[ts] = component;
    }

    this->resized();
}

void TimeSignaturesProjectMap::applyTimeSignatureBounds(TimeSignatureComponent *c, TimeSignatureComponent *nextOne)
{
    constexpr auto widthMargin = 8;
    const int defaultWidth = int(c->getTextWidth()) + widthMargin;

    int x = 0;
    int nextX = 0;
    switch (this->type)
    {
        case Type::Large:
            // for the timeline we can reuse the roll's methods which will
            // provide more precise x position which aligns with the grid lines:
            x = this->roll->getXPositionByBeat(c->getBeat());
            nextX = nextOne ? this->roll->getXPositionByBeat(nextOne->getBeat()) : x + defaultWidth;
            break;
        case Type::Small:
            // for the mini-map we have to compute the x position ourselves:
            const float rollLengthInBeats = this->rollLastBeat - this->rollFirstBeat;
            const float projectLengthInBeats = this->projectLastBeat - this->projectFirstBeat;

            const float beat = c->getBeat() - this->rollFirstBeat;
            const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

            x = int(mapWidth * (beat / projectLengthInBeats));
            const float nextBeat = (nextOne ? nextOne->getBeat() : this->rollLastBeat) - this->rollFirstBeat;
            nextX = int(mapWidth * (nextBeat / projectLengthInBeats));
            break;
    }

    const int maxWidth = nextX - x;
    const int w = jmin(maxWidth, defaultWidth);

    c->setBounds(x, 0, w, TimeSignatureComponent::timeSignatureHeight);
}

TimeSignatureComponent *TimeSignaturesProjectMap::createComponent()
{
    switch (this->type)
    {
        case Type::Large:
            return new TimeSignatureLargeComponent(*this);
        case Type::Small:
            return new TimeSignatureSmallComponent(*this);
        default:
            return nullptr;
    }
}
