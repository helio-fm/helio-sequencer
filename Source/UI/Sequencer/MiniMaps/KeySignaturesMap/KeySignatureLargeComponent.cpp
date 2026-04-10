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
#include "KeySignatureLargeComponent.h"
#include "KeySignaturesSequence.h"
#include "RollBase.h"
#include "CachedLabelImage.h"
#include "NoteNameComponent.h"
#include "HelioTheme.h"
#include "ColourIDs.h"

KeySignatureLargeComponent::KeySignatureLargeComponent(KeySignaturesProjectMap &parent,
    const KeySignatureEvent &targetEvent) noexcept :
    KeySignatureComponent(parent, targetEvent),
    anchor(targetEvent)
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setAccessible(false);

    this->nameComponent = make<NoteNameComponent>();
    this->addAndMakeVisible(this->nameComponent.get());
    this->nameComponent->setBounds(KeySignatureLargeComponent::labelX, 1,
        KeySignatureLargeComponent::labelWidth,
        KeySignatureLargeComponent::keySignatureHeight - 1);

    this->setMouseCursor(MouseCursor::PointingHandCursor);
}

KeySignatureLargeComponent::~KeySignatureLargeComponent() = default;

//===----------------------------------------------------------------------===//
// SelectableComponent
//===----------------------------------------------------------------------===//

void KeySignatureLargeComponent::setSelected(bool selected)
{
    if (this->selectedState != selected)
    {
        this->selectedState = selected;
        this->repaint();
    }
}

bool KeySignatureLargeComponent::isSelected() const noexcept
{
    return this->selectedState;
}

const String &KeySignatureLargeComponent::getSelectionGroupId() const noexcept
{
    return this->event.getSequence()->getTrackId();
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

void KeySignatureLargeComponent::startDragging()
{
    this->draggingHadCheckpoint = false;
    this->draggingState = true;
    this->anchor = this->event;
}

bool KeySignatureLargeComponent::getDraggingDelta(const MouseEvent &e, float &outDelta)
{
    this->dragger.dragComponent(this, e, nullptr);
    const auto newBeat = this->editor.getBeatByXPosition(this->getX());
    outDelta = newBeat - this->anchor.getBeat();
    const bool beatChanged = newBeat != this->getBeat();
    return beatChanged;
}

KeySignatureEvent KeySignatureLargeComponent::continueDragging(float deltaBeat) const noexcept
{
    jassert(this->draggingState);
    const float newBeat = this->anchor.getBeat() + deltaBeat;
    return this->event.withBeat(newBeat);
}

void KeySignatureLargeComponent::endDragging()
{
    this->draggingState = false;
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void KeySignatureLargeComponent::paint(Graphics &g)
{
    g.setColour(this->fillColour.withMultipliedAlpha(this->fillAlpha));
    g.fillRect(1, 0, this->getWidth() - 1, this->getHeight() - 1);
    g.fillRect(1.5f, float(this->getHeight() - 1), float(this->getWidth() - 2), 1.f);

    g.setColour(this->borderColour.withMultipliedAlpha(this->borderAlpha));
    g.fillRect(1, 0, this->getWidth() - 1, 1);
    g.fillRect(1.5f, 1.f, float(this->getWidth() - 2), 1.f);

    if (this->selectedState)
    {
        HelioTheme::drawDashedHorizontalLine(g, 1.f,
            float(this->getHeight() - 1), float(this->getWidth() - 2));
    }
}

// "if ... static_cast" is here only so the macro can use the if's scope
#define forEachSelectedKeySignature(lasso, child) \
    for (int _i = 0; _i < lasso.getNumSelected(); _i++) \
        if (auto *child = static_cast<KeySignatureLargeComponent *>(lasso.getSelectedItem(_i)))

void KeySignatureLargeComponent::mouseDown(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
    {
        return;
    }

    if (e.mods.isLeftButtonDown())
    {
        this->dragger.startDraggingComponent(this, e);

        const auto &selection = this->editor.getLassoSelection();
        if (!selection.isSelected(this))
        {
            this->startDragging();
        }
        else
        {
            forEachSelectedKeySignature(selection, selectedComponent)
            {
                selectedComponent->startDragging();
            }
        }
    }
    else
    {
        this->editor.onKeySignatureAction(this, e.mods);
    }
}

void KeySignatureLargeComponent::mouseDrag(const MouseEvent &e)
{
    if (this->editor.isMultiTouchEvent(e) ||
        e.mods.isBackButtonDown() || e.mods.isForwardButtonDown())
    {
        return;
    }

    if (this->draggingState &&
        e.mods.isLeftButtonDown() && e.getDistanceFromDragStart() > 4)
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);

        float deltaBeat = 0.f;
        const bool eventChanged = this->getDraggingDelta(e, deltaBeat);
        if (!eventChanged)
        {
            this->editor.alignKeySignatureComponent(this);
            return;
        }

        const auto &selection = this->editor.getLassoSelection();
        const auto dragThisOnly = !selection.isSelected(this);

        auto *sequence = static_cast<KeySignaturesSequence *>(this->event.getSequence());

        if (!this->draggingHadCheckpoint)
        {
            sequence->checkpoint();
            this->draggingHadCheckpoint = true;

            // drag-and-copy:
            if (e.mods.isShiftDown())
            {
                if (dragThisOnly)
                {
                    sequence->insert(this->getEvent().withNewId(), true);
                    this->toFront(false);
                }
                else
                {
                    forEachSelectedKeySignature(selection, component)
                    {
                        sequence->insert(component->getEvent().withNewId(), true);
                    }

                    forEachSelectedKeySignature(selection, component)
                    {
                        component->toFront(false);
                    }
                }
            }
        }

        if (dragThisOnly)
        {
            sequence->change(this->getEvent(), this->continueDragging(deltaBeat), true);
        }
        else
        {
            Array<KeySignatureEvent> groupBefore, groupAfter;
            forEachSelectedKeySignature(selection, selectedComponent)
            {
                groupBefore.add(selectedComponent->getEvent());
                groupAfter.add(selectedComponent->continueDragging(deltaBeat));
            }
            sequence->changeGroup(groupBefore, groupAfter, true);
        }
    }
}

void KeySignatureLargeComponent::mouseUp(const MouseEvent &e)
{
    if (this->draggingState)
    {
        this->setMouseCursor(MouseCursor::PointingHandCursor);
        this->endDragging();
    }

    if (e.getDistanceFromDragStart() < 10 &&
        !this->draggingHadCheckpoint &&
        Component::getCurrentlyModalComponent() == nullptr)
    {
        this->editor.onKeySignatureAction(this, e.mods);
    }
}

void KeySignatureLargeComponent::mouseEnter(const MouseEvent &e)
{
    this->fillAlpha = KeySignatureLargeComponent::fillFocusedAlpha;
    this->borderAlpha = KeySignatureLargeComponent::borderFocusedAlpha;
    this->repaint();
}

void KeySignatureLargeComponent::mouseExit(const MouseEvent &e)
{
    this->fillAlpha = KeySignatureLargeComponent::fillUnfocusedAlpha;
    this->borderAlpha = KeySignatureLargeComponent::borderUnfocusedAlpha;
    this->repaint();
}

void KeySignatureLargeComponent::setRealBounds(const Rectangle<float> bounds)
{
    const auto intBounds = bounds.toType<int>();
    if (this->getBounds() == intBounds)
    {
        return;
    }

    // if the component is too small for the note name label,
    // cut the label size in a way that only the key name is displayed;
    // truncated parts of scale names often looks like a visual noise:
    const auto keyNameWidth =
        this->textWidth - this->nameComponent->getDetailsWidthFloat();
    const auto newWidth = bounds.getWidth() <=
        (KeySignatureLargeComponent::labelX + (ceilf(keyNameWidth / 8.f) * 16.f)) ?
            int(keyNameWidth) : KeySignatureLargeComponent::labelWidth;

    if (this->nameComponent->getWidth() != newWidth)
    {
        this->nameComponent->setSize(newWidth, this->nameComponent->getHeight());
        this->nameComponent->forceInvalidateCacheImage();
    }

    this->setBounds(intBounds);
}

float KeySignatureLargeComponent::getTextWidth() const
{
    return this->textWidth;
}

void KeySignatureLargeComponent::updateContent(const Temperament::Period &keyNames, bool useFixedDo)
{
    const auto newNoteName = this->event.getRootKeyNameOfDefault(keyNames);
    const auto newDetailsText = this->event.getScale()->getLocalizedName();

    this->nameComponent->setNoteName(newNoteName, newDetailsText, useFixedDo);
    this->textWidth = this->nameComponent->getContentWidthFloat();

    this->repaint();
}
