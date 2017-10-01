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
#include "ModeIndicatorComponent.h"

class ModeIndicatorBar : public Component, private Timer
{
public:

    ModeIndicatorBar() :
        isHighlighted(false),
        brightness(0.f),
        animationDirection(1.f),
        animationSpeed(0.f) {}

#define MODE_BAR_ANIMATION_SPEED 0.13f
#define MODE_BAR_ANIMATION_ACCELERATION 0.875f

    void setHighlighted(bool state)
    {
        if (state == this->isHighlighted)
        { return; }

        this->animationDirection = state ? 1.f : -1.f;
        this->animationSpeed = MODE_BAR_ANIMATION_SPEED;
        this->isHighlighted = state;
        this->startTimerHz(60);
    }

    void paint(Graphics &g) override
    {
        g.fillAll(Colours::white.withAlpha(0.04f + this->brightness / 7.f));
    }

private:

    bool isHighlighted;
    float brightness;
    float animationDirection;
    float animationSpeed;

    void timerCallback() override
    {
        this->brightness += this->animationDirection * this->animationSpeed;
        this->animationSpeed *= MODE_BAR_ANIMATION_ACCELERATION;

        if (this->brightness < 0.001f || this->brightness > 0.999f)
        {
            this->brightness = jlimit(0.f, 1.f, this->brightness);
            this->stopTimer();
        }

        this->repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeIndicatorBar)
};

String ModeIndicatorComponent::componentId = "ModeIndicatorComponentId";

ModeIndicatorComponent::ModeIndicatorComponent(int numModes) : activeMode(0)
{
    this->setInterceptsMouseClicks(false, false);
    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);

    this->setComponentID(ModeIndicatorComponent::componentId);

    // The normal default state is invisible
    this->setAlpha(0.f);

    for (int i = 0; i < numModes; ++i)
    {
        this->bars.add(new ModeIndicatorBar());
        this->bars.getLast()->setHighlighted(i == 0);
        this->addAndMakeVisible(this->bars.getLast());
    }
}

ModeIndicatorComponent::~ModeIndicatorComponent()
{
    this->bars.clear(true);
}

ModeIndicatorComponent::Mode ModeIndicatorComponent::scrollToNextMode()
{
    if (!this->bars.isEmpty())
    {
        this->activeMode = (this->activeMode + 1) % this->bars.size();
        for (int i = 0; i < this->bars.size(); ++i)
        {
            this->bars[i]->setHighlighted(i == this->activeMode);
        }

    }

    return this->activeMode;
}

void ModeIndicatorComponent::resized()
{
    const int spacing = 2;
    const int length = this->bars.size();
    const int w = ((this->getWidth() - spacing) / this->bars.size()) - spacing;

    for (int i = 0; i < this->bars.size(); ++i)
    {
        const int x = spacing + (w + spacing) * i;
        this->bars[i]->setBounds(x, spacing, w, this->getHeight() - (spacing * 2));
    }
}
