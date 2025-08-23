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
#include "RadioButton.h"
#include "NoteNameComponent.h"

#if PLATFORM_DESKTOP
#   define RADIO_BUTTON_TRIGGERED_ON_MOUSE_UP 0
#elif PLATFORM_MOBILE
#   define RADIO_BUTTON_TRIGGERED_ON_MOUSE_UP 1
#endif

class RadioButtonFrame final : public Component
{
public:

    explicit RadioButtonFrame(float alpha) : 
        fillColour(findDefaultColour(Label::textColourId).withAlpha(alpha)),
        outlineColour(findDefaultColour(Label::textColourId).withAlpha(alpha * 0.5f))
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setWantsKeyboardFocus(false);
    }

    void paint(Graphics &g) override
    {
        const int y1 = 2;
        const int y2 = this->getHeight() - 2;
        const int x1 = 2;
        const int x2 = this->getWidth() - 2;

        g.setColour(this->fillColour);
        g.fillRect(x1, y1, x2 - x1 + 1, 4);

        g.setColour(this->outlineColour);
        g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
        g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
        g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
        g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
    }

private:

    const Colour fillColour;
    const Colour outlineColour;
};

RadioButton::RadioButton(const String &text,
    Colour colour, RadioButton::Listener *listener) :
    name(text),
    colour(colour),
    listener(listener),
    fillColour(colour.interpolatedWith(
        findDefaultColour(Label::textColourId), 0.5f).withAlpha(0.075f)),
    outlineColour(colour.interpolatedWith(
        findDefaultColour(Label::textColourId), 0.5f).withAlpha(0.1f))
{
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);

    auto label = make<Label>(String(), text);
    this->addAndMakeVisible(label.get());
    label->setFont(Globals::UI::Fonts::M);
    label->setJustificationType(Justification::centred);
    label->setInterceptsMouseClicks(false, false);
    label->setAlpha(RadioButton::contentDeselectedAlpha);

    this->minWidth = label->getFont().getStringWidth(text);

    this->content = move(label);

    this->checkMark = make<RadioButtonFrame>(0.85f);
    this->addChildComponent(this->checkMark.get());
}

RadioButton::RadioButton(const String &noteName, bool shouldUseFixedDo, Listener *listener) :
    name(noteName),
    colour(findDefaultColour(ColourIDs::ColourButton::outline)),
    listener(listener),
    fillColour(colour.interpolatedWith(
        findDefaultColour(Label::textColourId), 0.5f).withAlpha(0.075f)),
    outlineColour(colour.interpolatedWith(
        findDefaultColour(Label::textColourId), 0.5f).withAlpha(0.1f))
{
    auto noteNameComponent = make<NoteNameComponent>(true, Globals::UI::Fonts::M);
    this->addAndMakeVisible(noteNameComponent.get());
    noteNameComponent->setNoteName(noteName, {}, shouldUseFixedDo);
    noteNameComponent->setAlpha(RadioButton::contentDeselectedAlpha);

    this->minWidth = noteNameComponent->getRequiredWidth();

    this->content = move(noteNameComponent);
    
    this->checkMark = make<RadioButtonFrame>(0.85f);
    this->addChildComponent(this->checkMark.get());
}

RadioButton::~RadioButton() = default;

int RadioButton::getMinButtonWidth() const noexcept
{
    return this->minWidth;
}

void RadioButton::paint(Graphics &g)
{
    const int y1 = 2;
    const int y2 = this->getHeight() - 2;
    const int x1 = 2;
    const int x2 = this->getWidth() - 2;

    g.setColour(this->fillColour);
    g.fillRect(x1, y1, x2 - x1 + 1, 3);

    g.setColour(this->outlineColour);
    g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
    g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
    g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
    g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
}

void RadioButton::resized()
{
    this->fader.cancelAllAnimations(true);

    this->content->setBounds(0, 3, this->getWidth(), this->getHeight() - 3);
    this->checkMark->setBounds(this->getLocalBounds());

    HighlightedComponent::resized();
}

void RadioButton::handleClick(const MouseEvent &e)
{
    if (e.getDistanceFromDragStart() > 4)
    {
        return;
    }

    // let's say only left-click with no mod keys triggers the button
    // (this is kinda ugly because ScaleEditor and KeySelector depend
    // very much on this logic, but I'll leave it as it is for now)
    if (!e.mods.isRightButtonDown() && !e.mods.isAnyModifierKeyDown())
    {
        if (this->isSelected())
        {
            this->deselect();
        }
        else
        {
            this->select();
        }
    }

    this->listener->onRadioButtonClicked(e, this);
}

void RadioButton::mouseDown(const MouseEvent &e)
{
#if RADIO_BUTTON_TRIGGERED_ON_MOUSE_UP
    this->setHighlighted(true);
#else
    this->handleClick(e);
#endif
}

void RadioButton::mouseEnter(const MouseEvent &e)
{
#if RADIO_BUTTON_TRIGGERED_ON_MOUSE_UP
    this->setHighlighted(false);
#else
    HighlightedComponent::mouseEnter(e);
#endif
}

void RadioButton::mouseUp(const MouseEvent &e)
{
#if RADIO_BUTTON_TRIGGERED_ON_MOUSE_UP
    this->setHighlighted(false);
    this->handleClick(e);
#endif
}

Component *RadioButton::createHighlighterComponent()
{
    return new RadioButtonFrame(0.25f);
}

void RadioButton::select()
{
    if (!this->isSelected())
    {
        this->selected = true;
        this->fader.fadeIn(this->checkMark.get(), Globals::UI::fadeInShort);
        this->content->setAlpha(RadioButton::contentSelectedAlpha);
    }
}

void RadioButton::deselect()
{
    if (this->isSelected())
    {
        this->selected = false;
        this->fader.fadeOut(this->checkMark.get(), Globals::UI::fadeOutLong);
        this->content->setAlpha(RadioButton::contentDeselectedAlpha);
    }
}

int RadioButton::getButtonIndex() const noexcept
{
    return this->index;
}

void RadioButton::setButtonIndex(int newIndex)
{
    this->index = newIndex;
}

const String &RadioButton::getButtonName() const noexcept
{
    return this->name;
}

bool RadioButton::isSelected() const noexcept
{
    return this->selected;
}

Colour RadioButton::getColour() const noexcept
{
    return this->colour;
}
