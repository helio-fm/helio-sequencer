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
#include "ColourButton.h"
#include "IconComponent.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

class ColourButtonFrame final : public Component
{
public:

    ColourButtonFrame()
    {
        this->setInterceptsMouseClicks(false, false);
        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        const int y1 = 2;
        const int y2 = this->getHeight() - 2;
        const int x1 = 2;
        const int x2 = this->getWidth() - 2;

        g.setColour(findDefaultColour(ColourIDs::ColourButton::outline).withAlpha(0.5f));
        g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
        g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
        g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
        g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
    }
};

ColourButton::ColourButton(Colour c, ColourButton::Listener *listener) :
    colour(c),
    owner(listener)
{
    this->setPaintingIsUnclipped(true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->checkMark = make<IconComponent>(Icons::apply);
    this->addChildComponent(this->checkMark.get());

    this->setSize(32, 32);
}

ColourButton::~ColourButton()
{
    this->checkMark = nullptr;
}

void ColourButton::paint(Graphics &g)
{
    const int y1 = 2;
    const int y2 = this->getHeight() - 2;
    const int x1 = 2;
    const int x2 = this->getWidth() - 2;

    const Colour base(findDefaultColour(ColourIDs::ColourButton::outline));

    // To avoid smoothed rectangles:
    g.setColour(this->colour.interpolatedWith(base, 0.25f).withAlpha(0.9f));
    //g.fillRect(x1, y2 - 4, x2 - x1 + 1, 5);
    g.fillRect(x1, y1, x2 - x1 + 1, 5);

    g.setColour(this->colour.interpolatedWith(base, 0.5f).withAlpha(0.1f));
    g.drawVerticalLine(x1 - 1, float(y1), float(y2 + 1));
    g.drawVerticalLine(x2 + 1, float(y1), float(y2 + 1));
    g.drawHorizontalLine(y1 - 1, float(x1), float(x2 + 1));
    g.drawHorizontalLine(y2 + 1, float(x1), float(x2 + 1));
}

void ColourButton::resized()
{
    this->fader.cancelAllAnimations(true);

    const int s = jmin(this->getHeight(), this->getWidth()) - 8;
    this->checkMark->setSize(s, s);

    jassert(s < 30);

    const auto c = this->getLocalBounds().getCentre();
    this->checkMark->setCentrePosition(c.x, c.y + 3);

    HighlightedComponent::resized();
}

void ColourButton::mouseDown(const MouseEvent &e)
{
    if (!this->isSelected())
    {
        this->select();
        this->owner->onColourButtonClicked(this);
    }
}

Component *ColourButton::createHighlighterComponent()
{
    return new ColourButtonFrame();
}

void ColourButton::select()
{
    if (!this->isSelected())
    {
        this->selected = true;
        this->fader.fadeIn(this->checkMark.get(), Globals::UI::fadeInShort);
    }
}

void ColourButton::deselect()
{
    if (this->isSelected())
    {
        this->selected = false;
        this->fader.fadeOut(this->checkMark.get(), Globals::UI::fadeOutLong);
    }
}

int ColourButton::getButtonIndex() const noexcept
{
    return this->index;
}

void ColourButton::setButtonIndex(int val)
{
    this->index = val;
}

bool ColourButton::isSelected() const noexcept
{
    return this->selected;
}

Colour ColourButton::getColour() const noexcept
{
    return this->colour;
}
