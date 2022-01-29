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
#include "TimeSignaturesSequence.h"
#include "TimeSignaturesProjectMap.h"
#include "PianoTrackNode.h"
#include "RollBase.h"
#include "ColourIDs.h"
#include "CachedLabelImage.h"
#include "TimeSignatureLargeComponent.h"

TimeSignatureLargeComponent::TimeSignatureLargeComponent(TimeSignaturesProjectMap &parent) :
    TimeSignatureComponent(parent)
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->signatureLabel = make<Label>();
    this->addAndMakeVisible(this->signatureLabel.get());
    this->signatureLabel->setFont(Globals::UI::Fonts::M);
    this->signatureLabel->setJustificationType(Justification::topLeft);
    this->signatureLabel->setInterceptsMouseClicks(false, false);
    this->signatureLabel->setBounds(-2, 0, 48, TimeSignatureComponent::timeSignatureHeight);

    this->signatureLabel->setCachedComponentImage(new CachedLabelImage(*this->signatureLabel));

    constexpr auto topPadding = 0.f;
    constexpr auto triangleSize = 5.f;
    this->triangleShape.addTriangle(0.f, topPadding,
        triangleSize * 1.5f, topPadding,
        0.f, triangleSize + topPadding);

    this->setMouseCursor(MouseCursor::PointingHandCursor);
}

TimeSignatureLargeComponent::~TimeSignatureLargeComponent() = default;

void TimeSignatureLargeComponent::paint(Graphics &g)
{
    g.setColour(this->colour);
    g.fillPath(this->triangleShape);
}

void TimeSignatureLargeComponent::mouseDown(const MouseEvent &e)
{
    this->mouseDownWasTriggered = true;

    if (e.mods.isLeftButtonDown())
    {
        this->dragger.startDraggingComponent(this, e);
        this->draggingHadCheckpoint = false;
        this->draggingState = true;
        this->draggingAnchorBeat = this->editor.getBeatByXPosition(this->getX());
    }
    else
    {
        this->editor.alternateActionFor(this);
    }
}

void TimeSignatureLargeComponent::mouseDrag(const MouseEvent &e)
{
    if (this->draggingState &&
        e.mods.isLeftButtonDown() && e.getDistanceFromDragStart() > 4)
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->dragger.dragComponent(this, e, nullptr);
        const bool firstChangeIsToCome = !this->draggingHadCheckpoint;

        const float newBeat = this->editor.getBeatByXPosition(this->getX());
        if (this->draggingAnchorBeat == newBeat)
        {
            this->editor.applyTimeSignatureBounds(this, nullptr);
            return;
        }

        // dragging the track's time signatures:
        if (auto *track = dynamic_cast<MidiTrackNode *>(this->event.getTrack().get()))
        {
            jassert(this->event.getSequence() == nullptr);

            // we can only rely on delta beat here, because track's ts template
            // cannot be set to some absolute beat, it's beat range is from 0 to (sequence range - 1 bar)
            const auto deltaBeat = newBeat - this->draggingAnchorBeat;

            // take the track's time signature template's beat, not this->event's,
            // because this->event is auto-generated and has absolute beat
            const auto oldTemplateBeat = track->getTimeSignatureOverride()->getBeat();

            // check if the new event would go out of allowed range
            // (tech debt warning: setTimeSignatureOverride the same check)
            const auto barLength = track->getTimeSignatureOverride()->getBarLengthInBeats();
            const auto newTemplateBeat = jlimit(0.f,
                track->getSequence()->getLengthInBeats() - barLength,
                oldTemplateBeat + deltaBeat);

            if (newTemplateBeat == oldTemplateBeat)
            {
                this->editor.applyTimeSignatureBounds(this, nullptr);
                return;
            }

            if (!this->draggingHadCheckpoint)
            {
                track->getProject()->checkpoint();
                this->draggingHadCheckpoint = true;
            }

            track->setTimeSignatureOverride(this->event.withBeat(newTemplateBeat), true, sendNotification);
            this->draggingAnchorBeat = newBeat;
        }
        // dragging the timeline time signatures:
        else if (auto *sequence = dynamic_cast<TimeSignaturesSequence *>(this->event.getSequence()))
        {
            jassert(this->event.getTrack() == nullptr);

            if (!this->draggingHadCheckpoint)
            {
                sequence->checkpoint();
                this->draggingHadCheckpoint = true;
            }

            // drag-and-copy logic:
            if (firstChangeIsToCome && e.mods.isAnyModifierKeyDown())
            {
                sequence->insert(this->event.withNewId(), true);
            }

            sequence->change(this->event, this->event.withBeat(newBeat), true);
            this->draggingAnchorBeat = newBeat;
        }
    }
}

void TimeSignatureLargeComponent::mouseUp(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown())
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::PointingHandCursor);
            this->draggingState = false;
        }

        if (e.getDistanceFromDragStart() < 10 &&
            this->mouseDownWasTriggered &&
            !this->draggingHadCheckpoint)
        {
            this->editor.onTimeSignatureTapped(this);
        }
    }

    this->mouseDownWasTriggered = false;
}

void TimeSignatureLargeComponent::setRealBounds(const Rectangle<float> bounds)
{
    const auto intBounds = bounds.toType<int>();
    this->boundsOffset = {
        bounds.getX() - float(intBounds.getX()),
        bounds.getY(),
        bounds.getWidth() - float(intBounds.getWidth()),
        bounds.getHeight()};

    this->setBounds(intBounds);
}

float TimeSignatureLargeComponent::getTextWidth() const noexcept
{
    return this->textWidth;
}

void TimeSignatureLargeComponent::updateContent(const TimeSignatureEvent &newEvent)
{
    this->event = newEvent;

    this->colour = this->event.getTrackColour()
        .interpolatedWith(findDefaultColour(ColourIDs::Roll::headerSnaps), 0.75f);
    const auto textColour = this->event.getTrackColour()
        .interpolatedWith(findDefaultColour(Label::textColourId), 0.5f);
    this->signatureLabel->setColour(Label::textColourId, textColour);

    auto *cachedImage = static_cast<CachedLabelImage *>(this->signatureLabel->getCachedComponentImage());
    jassert(cachedImage != nullptr);
    cachedImage->forceInvalidate();

    this->signatureLabel->setText(this->event.toString(), dontSendNotification);
    this->textWidth = float(this->signatureLabel->getFont()
        .getStringWidth(this->signatureLabel->getText()));
}
