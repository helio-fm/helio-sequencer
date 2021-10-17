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
#include "KeySignaturesSequence.h"
#include "RollBase.h"
#include "ColourIDs.h"
#include "CachedLabelImage.h"
#include "KeySignatureLargeComponent.h"

KeySignatureLargeComponent::KeySignatureLargeComponent(KeySignaturesProjectMap &parent,
    const KeySignatureEvent &targetEvent) :
    KeySignatureComponent(parent, targetEvent),
    anchor(targetEvent)
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->signatureLabel = make<Label>();
    this->addAndMakeVisible(this->signatureLabel.get());
    this->signatureLabel->setFont(Globals::UI::Fonts::S);
    this->signatureLabel->setJustificationType(Justification::centredLeft);
    this->signatureLabel->setBounds(-2, 1, 192, 24);
    this->signatureLabel->setInterceptsMouseClicks(false, false);
    this->signatureLabel->setCachedComponentImage(new CachedLabelImage(*this->signatureLabel));

    this->setMouseCursor(MouseCursor::PointingHandCursor);
    this->setSize(24, 24);
}

KeySignatureLargeComponent::~KeySignatureLargeComponent() = default;

void KeySignatureLargeComponent::paint(Graphics &g)
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

void KeySignatureLargeComponent::mouseDown(const MouseEvent &e)
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
        this->editor.onKeySignatureAltAction(this);
    }
}

void KeySignatureLargeComponent::mouseDrag(const MouseEvent &e)
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
                auto *sequence = static_cast<KeySignaturesSequence *>(this->event.getSequence());

                if (! this->draggingHadCheckpoint)
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
                this->editor.alignKeySignatureComponent(this);
            }
        }
    }
}

void KeySignatureLargeComponent::mouseUp(const MouseEvent &e)
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
            if (e.mods.isAnyModifierKeyDown())
            {
                this->editor.onKeySignatureAltAction(this);
            }
            else
            {
                this->editor.onKeySignatureMainAction(this);
            }
        }
    }

    this->mouseDownWasTriggered = false;
}

void KeySignatureLargeComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = Rectangle<float>(bounds.getX() - float(intBounds.getX()),
        bounds.getY(),
        bounds.getWidth() - float(intBounds.getWidth()),
        bounds.getHeight());

    this->setBounds(intBounds);
}

float KeySignatureLargeComponent::getTextWidth() const
{
    return this->textWidth;
}

void KeySignatureLargeComponent::updateContent(const StringArray &keyNames)
{
    const String originalName = this->event.toString(keyNames);
    if (this->eventName != originalName)
    {
        this->eventName = originalName;
        this->textWidth = float(this->signatureLabel->getFont().getStringWidth(originalName));
        this->signatureLabel->setText(originalName, dontSendNotification);
        this->repaint();
    }
}
