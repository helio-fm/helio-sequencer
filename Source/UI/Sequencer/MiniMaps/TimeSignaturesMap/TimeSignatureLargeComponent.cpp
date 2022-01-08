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
        // don't allow dragging track-based time signatures at the moment
        // (todo someday: allow dragging them within the track range)
        if (this->event.getSequence() != nullptr)
        {
            this->dragger.startDraggingComponent(this, e);
            this->draggingHadCheckpoint = false;
            this->draggingState = true;
            this->anchor = this->event;
        }
    }
    else
    {
        this->editor.alternateActionFor(this);
        // this->editor.showContextMenuFor(this);
    }
}

void TimeSignatureLargeComponent::mouseDrag(const MouseEvent &e)
{
    if (this->draggingState &&
        e.mods.isLeftButtonDown() && e.getDistanceFromDragStart() > 4)
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);
        this->dragger.dragComponent(this, e, nullptr);
        const float newBeat = this->editor.getBeatByXPosition(this->getX());
        const bool beatHasChanged = (this->event.getBeat() != newBeat);

        if (beatHasChanged)
        {
            const bool firstChangeIsToCome = !this->draggingHadCheckpoint;
            auto *sequence = static_cast<TimeSignaturesSequence *>(this->event.getSequence());
            jassert(sequence != nullptr);

            if (!this->draggingHadCheckpoint)
            {
                sequence->checkpoint();
                this->draggingHadCheckpoint = true;
            }

            // Drag-and-copy logic:
            if (firstChangeIsToCome && e.mods.isAnyModifierKeyDown())
            {
                sequence->insert(this->event.withNewId(), true);
            }

            sequence->change(this->event, this->event.withBeat(newBeat), true);
        }
        else
        {
            this->editor.applyTimeSignatureBounds(this, nullptr);
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
    this->signatureLabel->setText(this->event.toString(), dontSendNotification);
    this->signatureLabel->setColour(Label::textColourId, textColour);
    this->textWidth = float(this->signatureLabel->getFont()
        .getStringWidth(this->signatureLabel->getText()));
}
