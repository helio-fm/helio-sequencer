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
#include "ProjectTimeline.h"
#include "PlayerThread.h"
#include "RollBase.h"
#include "TrackStartIndicator.h"
#include "TrackEndIndicator.h"
#include "TimeSignatureDialog.h"
#include "TimeSignatureLargeComponent.h"
#include "TimeSignatureSmallComponent.h"

TimeSignaturesProjectMap::TimeSignaturesProjectMap(ProjectNode &parentProject, RollBase &parentRoll, Type type) :
    project(parentProject),
    roll(parentRoll),
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
    const auto &timeSignatures =
        this->project.getTimeline()->getTimeSignaturesAggregator()->getAllOrdered();

    for (const auto &ts : timeSignatures)
    {
        if (auto *existingComponent = this->timeSignaturesMap[ts])
        {
            DBG("Updating existing time signature")
            existingComponent->updateContent(); // fixme will it work with track-based ts?
        }
        else
        {
            DBG("Adding new time signature")
            auto *newComponent = this->createComponent(ts);
            this->addAndMakeVisible(newComponent);
            newComponent->updateContent();

            this->timeSignatureComponents.addSorted(*newComponent, newComponent);
            this->timeSignaturesMap[ts] = newComponent;
        }
    }

    for (const auto *component : this->timeSignatureComponents)
    {
        const auto &event = component->getEvent();
        if (timeSignatures.indexOfSorted(event, event) < 0)
        {
            this->timeSignatureComponents.removeObject(component);
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

void TimeSignaturesProjectMap::onTimeSignatureMoved(TimeSignatureComponent *c) {}

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
    if (!this->project.getTransport().isPlaying())
    {
        App::showModalComponent(TimeSignatureDialog::editingDialog(*this, c->getEvent()));
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
    const int xRoll = int(float(x) / float(this->getWidth()) * float(this->roll.getWidth()));
    const float targetBeat = this->roll.getRoundBeatSnapByXPosition(xRoll);
    return jlimit(this->rollFirstBeat, this->rollLastBeat, targetBeat);
}

void TimeSignaturesProjectMap::reloadTrackMap()
{
    jassert(this->project.getTimeline() != nullptr);

    this->timeSignaturesMap.clear();
    this->timeSignatureComponents.clear();

    const auto &timeSignatures =
        this->project.getTimeline()->getTimeSignaturesAggregator()->getAllOrdered();

    for (const auto &ts : timeSignatures)
    {
        auto *component = this->createComponent(ts);
        this->addAndMakeVisible(component);
        component->updateContent();

        this->timeSignatureComponents.addSorted(*component, component);
        this->timeSignaturesMap[ts] = component;
    }

    this->resized();
}

void TimeSignaturesProjectMap::applyTimeSignatureBounds(TimeSignatureComponent *c, TimeSignatureComponent *nextOne)
{
    constexpr auto minWidth = 10.f;
    constexpr auto widthMargin = 12.f;
    constexpr auto componentsPadding = 10.f;

    const float rollLengthInBeats = this->rollLastBeat - this->rollFirstBeat;
    const float projectLengthInBeats = this->projectLastBeat - this->projectFirstBeat;

    const float beat = c->getBeat() - this->rollFirstBeat;
    const float mapWidth = float(this->getWidth()) * (projectLengthInBeats / rollLengthInBeats);

    const float x = mapWidth * (beat / projectLengthInBeats);
    const float nextBeat = (nextOne ? nextOne->getBeat() : this->rollLastBeat) - this->rollFirstBeat;
    const float nextX = mapWidth * (nextBeat / projectLengthInBeats);

    const float maxWidth = nextX - x;
    const float componentWidth = c->getTextWidth() + widthMargin;
    const float w = jlimit(minWidth, maxWidth, componentWidth);

    c->setRealBounds(Rectangle<float>(x, 0.f, w,
        float(TimeSignatureComponent::timeSignatureHeight)));
}

TimeSignatureComponent *TimeSignaturesProjectMap::createComponent(const TimeSignatureEvent &event)
{
    switch (this->type)
    {
        case Type::Large:
            return new TimeSignatureLargeComponent(*this, event);
        case Type::Small:
            return new TimeSignatureSmallComponent(*this, event);
        default:
            return nullptr;
    }
}
