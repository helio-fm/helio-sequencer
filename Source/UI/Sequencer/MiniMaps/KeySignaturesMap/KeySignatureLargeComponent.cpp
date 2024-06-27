/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "KeySignaturesSequence.h"
#include "RollBase.h"
#include "ColourIDs.h"
#include "CachedLabelImage.h"
#include "KeySignatureLargeComponent.h"
#include "HelioTheme.h"

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
    this->signatureLabel->setBounds(-2, 1, 256, 24);
    this->signatureLabel->setInterceptsMouseClicks(false, false);
    this->signatureLabel->setCachedComponentImage(new CachedLabelImage(*this->signatureLabel));

    this->setMouseCursor(MouseCursor::PointingHandCursor);
    this->setSize(24, 24);
}

KeySignatureLargeComponent::~KeySignatureLargeComponent() = default;

void KeySignatureLargeComponent::paint(Graphics &g)
{
    g.setColour(findDefaultColour(ColourIDs::Roll::headerSnaps));
    g.fillRect(1.f, 0.f, float(this->getWidth() - 3), 3.f);

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark));
    HelioTheme::drawDashedHorizontalLine3(g, 1.f, 0.f, float(this->getWidth() - 1), 8.f);
}

void KeySignatureLargeComponent::mouseDown(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e))
    {
        return;
    }

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
    if (this->editor.isMultiTouchEvent(e))
    {
        return;
    }

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
                auto *sequence = static_cast<KeySignaturesSequence *>(this->event.getSequence());

                if (!this->draggingHadCheckpoint)
                {
                    sequence->checkpoint();
                    this->draggingHadCheckpoint = true;

                    // drag-and-copy:
                    if (e.mods.isShiftDown())
                    {
                        sequence->insert(this->event.withNewId(), true);
                    }
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
            !this->draggingHadCheckpoint &&
            Component::getCurrentlyModalComponent() == nullptr)
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

void KeySignatureLargeComponent::updateContent(const Temperament::Period &keyNames)
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
