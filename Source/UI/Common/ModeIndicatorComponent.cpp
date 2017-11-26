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
#include "ComponentIDs.h"

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
        g.setColour(Colours::white.withAlpha(0.1f + this->brightness / 2.f));
        g.fillEllipse(0.f, 0.f, float(this->getWidth()), float(this->getHeight()));
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

ModeIndicatorComponent::ModeIndicatorComponent(int numModes) : activeMode(0)
{
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);

    this->setComponentID(ComponentIDs::modeIndicatorComponentId);

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
    const int spacing = 4;
    const int h = this->getHeight();
    const int w = this->getWidth();
    const int length = this->bars.size();
    const int cw = (h + spacing) * length;
    const int cx = w / 2 - cw / 2;

    for (int i = 0; i < length; ++i)
    {
        const int x = cx + (h + spacing) * i;
        this->bars[i]->setBounds(x, 0, h, h);
    }
}

void ModeIndicatorOwnerComponent::showModeIndicator()
{
    if (auto *i = this->findChildWithID(ComponentIDs::modeIndicatorComponentId))
    {
        this->modeIndicatorFader.fadeIn(i, 200);
    }
}

void ModeIndicatorOwnerComponent::hideModeIndicator()
{
    if (auto *i = this->findChildWithID(ComponentIDs::modeIndicatorComponentId))
    {
        this->modeIndicatorFader.fadeOut(i, 150);
    }
}

ModeIndicatorTrigger::ModeIndicatorTrigger()
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
}

void ModeIndicatorTrigger::mouseUp(const MouseEvent& event)
{
    if (ModeIndicatorOwnerComponent *owner =
        dynamic_cast<ModeIndicatorOwnerComponent *>(this->getParentComponent()))
    {
        owner->handleChangeMode();
    }
}

void ModeIndicatorTrigger::mouseEnter(const MouseEvent &event)
{
    if (ModeIndicatorOwnerComponent *owner =
        dynamic_cast<ModeIndicatorOwnerComponent *>(this->getParentComponent()))
    {
        owner->showModeIndicator();
    }
}

void ModeIndicatorTrigger::mouseExit(const MouseEvent &event)
{
    if (ModeIndicatorOwnerComponent *owner =
        dynamic_cast<ModeIndicatorOwnerComponent *>(this->getParentComponent()))
    {
        owner->hideModeIndicator();
    }
}
