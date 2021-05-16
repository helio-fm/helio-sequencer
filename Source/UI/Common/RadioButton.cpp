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
#include "RadioButton.h"

class RadioButtonFrame final : public Component
{
public:

    explicit RadioButtonFrame(float alpha) : alpha(alpha)
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

        const Colour baseColour(findDefaultColour(Label::textColourId));

        g.setColour(baseColour.withAlpha(this->alpha * 0.75f));
        g.fillRect(x1, y1, x2 - x1 + 1, 5);

        g.setColour(baseColour.withAlpha(this->alpha * 0.25f));
        g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
        g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
        g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
        g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
    }

private:

    float alpha = 0.f;
};

RadioButton::RadioButton(const String &text, Colour c, RadioButton::Listener *listener) :
    colour(c),
    owner(listener)
{
    this->label = make<Label>();
    this->addAndMakeVisible(this->label.get());

    this->label->setFont(Globals::UI::Fonts::M);
    this->label->setText(text, dontSendNotification);
    this->label->setJustificationType(Justification::centred);
    this->label->setInterceptsMouseClicks(false, false);

    this->checkMark = make<RadioButtonFrame>(1.f);
    this->addChildComponent(this->checkMark.get());

    this->setSize(32, 32);
}

RadioButton::~RadioButton()
{
    this->checkMark = nullptr;
    this->label = nullptr;
}

void RadioButton::paint(Graphics &g)
{
    const int y1 = 2;
    const int y2 = this->getHeight() - 2;
    const int x1 = 2;
    const int x2 = this->getWidth() - 2;

    const Colour baseColour(findDefaultColour(Label::textColourId));

    // To avoid smoothed rectangles:
    g.setColour(this->colour.interpolatedWith(baseColour, 0.25f).withAlpha(0.1f));
    //g.fillRect(x1, y2 - 4, x2 - x1 + 1, 5);
    g.fillRect(x1, y1, x2 - x1 + 1, 5);

    g.setColour(this->colour.interpolatedWith(baseColour, 0.5f).withAlpha(0.1f));
    g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
    g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
    g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
    g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
}

void RadioButton::resized()
{
    this->fader.cancelAllAnimations(true);

    this->label->setBounds(0, 3, this->getWidth(), this->getHeight() - 3);
    this->checkMark->setBounds(this->getLocalBounds());

    HighlightedComponent::resized();
}

void RadioButton::mouseDown(const MouseEvent &e)
{
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

    this->owner->onRadioButtonClicked(e, this);
}

Component *RadioButton::createHighlighterComponent()
{
    return new RadioButtonFrame(0.5f);
}

void RadioButton::select()
{
    if (!this->isSelected())
    {
        this->selected = true;
        this->fader.fadeIn(this->checkMark.get(), Globals::UI::fadeInShort);
    }
}

void RadioButton::deselect()
{
    if (this->isSelected())
    {
        this->selected = false;
        this->fader.fadeOut(this->checkMark.get(), Globals::UI::fadeOutLong);
    }
}

int RadioButton::getButtonIndex() const noexcept
{
    return this->index;
}

void RadioButton::setButtonIndex(int val)
{
    this->index = val;
}

bool RadioButton::isSelected() const noexcept
{
    return this->selected;
}

Colour RadioButton::getColour() const noexcept
{
    return this->colour;
}
