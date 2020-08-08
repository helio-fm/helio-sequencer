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
#include "HybridRoll.h"
#include "ColourIDs.h"
#include "CachedLabelImage.h"
#include "TimeSignatureLargeComponent.h"

TimeSignatureLargeComponent::TimeSignatureLargeComponent(TimeSignaturesProjectMap &parent,
    const TimeSignatureEvent &targetEvent) :
    TimeSignatureComponent(parent, targetEvent),
    anchor(targetEvent)
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->numeratorLabel = make<Label>();
    this->addAndMakeVisible(this->numeratorLabel.get());
    this->numeratorLabel->setFont({ 18.f });
    this->numeratorLabel->setJustificationType(Justification::centredLeft);
    this->numeratorLabel->setBounds(-2, 4, 32, 14);
    this->numeratorLabel->setInterceptsMouseClicks(false, false);

    this->denominatorLabel = make<Label>();
    this->addAndMakeVisible(this->denominatorLabel.get());
    this->denominatorLabel->setFont({ 18.f });
    this->denominatorLabel->setJustificationType(Justification::centredLeft);
    this->denominatorLabel->setBounds(-2, 17, 32, 14);
    this->denominatorLabel->setInterceptsMouseClicks(false, false);

    this->setMouseCursor(MouseCursor::PointingHandCursor);

    this->numeratorLabel->setBufferedToImage(true);
    this->numeratorLabel->setCachedComponentImage(new CachedLabelImage(*this->numeratorLabel));

    this->denominatorLabel->setBufferedToImage(true);
    this->denominatorLabel->setCachedComponentImage(new CachedLabelImage(*this->denominatorLabel));

    this->setSize(32, 32);
}

TimeSignatureLargeComponent::~TimeSignatureLargeComponent() {}

void TimeSignatureLargeComponent::paint(Graphics &g)
{
    g.setColour(findDefaultColour(ColourIDs::Roll::headerSnaps));
    g.fillRect(1.f, 0.f, float(this->getWidth() - 1), 3.f);

    constexpr float dashLength = 8.f;

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark));
    for (float i = (dashLength / 2.f); i < this->getWidth(); i += (dashLength * 2.f))
    {
        g.fillRect(i + 2.f, 0.f, dashLength, 1.f);
        g.fillRect(i + 1.f, 1.f, dashLength, 1.f);
        g.fillRect(i, 2.f, dashLength, 1.f);
    }
}

void TimeSignatureLargeComponent::mouseDown(const MouseEvent &e)
{
    this->mouseDownWasTriggered = true;

    if (e.mods.isLeftButtonDown())
    {
        // don't checkpoint right here, but only before the actual change
        //this->event.getSequence()->checkpoint();

        this->dragger.startDraggingComponent(this, e);
        this->draggingHadCheckpoint = false;
        this->draggingState = true;
        this->anchor = this->event;
    }
    else
    {
        this->editor.alternateActionFor(this);
        //this->editor.showContextMenuFor(this);
    }
}

void TimeSignatureLargeComponent::mouseDrag(const MouseEvent &e)
{
    if (e.mods.isLeftButtonDown() && e.getDistanceFromDragStart() > 4)
    {
        if (this->draggingState)
        {
            this->setMouseCursor(MouseCursor::DraggingHandCursor);
            this->dragger.dragComponent(this, e, nullptr);
            const float newBeat = this->editor.getBeatByXPosition(this->getX());
            const bool beatHasChanged = (this->event.getBeat() != newBeat);

            if (beatHasChanged)
            {
                const bool firstChangeIsToCome = !this->draggingHadCheckpoint;
                auto *sequence = static_cast<TimeSignaturesSequence *>(this->event.getSequence());

                if (! this->draggingHadCheckpoint)
                {
                    sequence->checkpoint();
                    this->draggingHadCheckpoint = true;
                }

                // Drag-and-copy logic:
                if (firstChangeIsToCome && e.mods.isAnyModifierKeyDown())
                {
                    sequence->insert(this->event.copyWithNewId(), true);
                }

                sequence->change(this->event, this->event.withBeat(newBeat), true);
            }
            else
            {
                this->editor.alignTimeSignatureComponent(this);
            }
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
            this->editor.onTimeSignatureMoved(this);
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

    this->boundsOffset = { bounds.getX() - float(intBounds.getX()),
        bounds.getY(),
        bounds.getWidth() - float(intBounds.getWidth()),
        bounds.getHeight() };

    this->setBounds(intBounds);
}

void TimeSignatureLargeComponent::updateContent()
{
    if (this->numerator != this->event.getNumerator() ||
        this->denominator != this->event.getDenominator())
    {
        this->numerator = this->event.getNumerator();
        this->denominator = this->event.getDenominator();
        this->numeratorLabel->setText(String(this->numerator), dontSendNotification);
        this->denominatorLabel->setText(String(this->denominator), dontSendNotification);
    }
}

