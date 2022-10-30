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

class ModeIndicatorBar final : public Component, private Timer
{
public:

    ModeIndicatorBar() = default;

    void setHighlighted(bool shouldBeHighlighted)
    {
        if (shouldBeHighlighted == this->isHighlighted)
        {
            return;
        }

        this->animationDirection = shouldBeHighlighted ? 1.f : -1.f;
        this->animationSpeed = ModeIndicatorBar::animationStartingSpeed;
        this->isHighlighted = shouldBeHighlighted;
        this->startTimerHz(60);
    }

    void paint(Graphics &g) override
    {
        const auto fgColour = findDefaultColour(Label::textColourId);
        g.setColour(fgColour.withAlpha(0.15f + this->brightness / 2.f));
        g.fillEllipse(0.f, 0.f, float(this->getWidth()), float(this->getHeight()));
    }

private:

    static constexpr auto animationStartingSpeed = 0.13f;
    static constexpr auto animationAcceleration = 0.875f;

    bool isHighlighted = false;
    float brightness = 0.f;
    float animationDirection = 1.f;
    float animationSpeed = 0.f;

    void timerCallback() override
    {
        this->brightness += this->animationDirection * this->animationSpeed;
        this->animationSpeed *= ModeIndicatorBar::animationAcceleration;

        if (this->brightness < 0.001f || this->brightness > 0.999f)
        {
            this->brightness = jlimit(0.f, 1.f, this->brightness);
            this->stopTimer();
        }

        this->repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModeIndicatorBar)
};

ModeIndicatorComponent::ModeIndicatorComponent(int numModes)
{
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);

    this->setComponentID(ComponentIDs::modeIndicatorComponentId);

    this->setNumModes(numModes);
}

ModeIndicatorComponent::~ModeIndicatorComponent()
{
    this->bars.clear(true);
}

void ModeIndicatorComponent::setNumModes(int numModes)
{
    this->bars.clear(true);

    for (int i = 0; i < numModes; ++i)
    {
        this->bars.add(new ModeIndicatorBar());
        this->bars.getLast()->setHighlighted(i == 0);
        this->addAndMakeVisible(this->bars.getLast());
    }

    // also place the bars correctly, if needed
    if (this->getParentComponent() != nullptr)
    {
        this->resized();
    }
}

ModeIndicatorComponent::Mode ModeIndicatorComponent::setCurrentMode(Mode mode)
{
    if (!this->bars.isEmpty())
    {
        this->activeMode = mode % this->bars.size();
        this->updateBarsHighlighting();
    }

    return this->activeMode;
}

ModeIndicatorComponent::Mode ModeIndicatorComponent::scrollToNextMode()
{
    if (!this->bars.isEmpty())
    {
        this->activeMode = (this->activeMode + 1) % this->bars.size();
        this->updateBarsHighlighting();
    }

    return this->activeMode;
}

void ModeIndicatorComponent::resized()
{
    constexpr auto barSize = 6;
    constexpr auto barSpacing = 4;

    const int h = this->getHeight();
    const int w = this->getWidth();
    const int length = this->bars.size();
    const int cw = (barSize + barSpacing) * length - barSpacing;
    const int cx = w / 2 - cw / 2;

    for (int i = 0; i < length; ++i)
    {
        const int x = cx + (barSize + barSpacing) * i;
        this->bars[i]->setBounds(x, h / 2 - barSize / 2, barSize, barSize);
    }
}

void ModeIndicatorComponent::updateBarsHighlighting()
{
    for (int i = 0; i < this->bars.size(); ++i)
    {
        this->bars.getUnchecked(i)->setHighlighted(i == this->activeMode);
    }
}

void ModeIndicatorOwnerComponent::showModeIndicator()
{
    if (auto *i = this->findChildWithID(ComponentIDs::modeIndicatorComponentId))
    {
        this->modeIndicatorFader.fadeIn(i, Globals::UI::fadeInLong);
    }
}

void ModeIndicatorOwnerComponent::hideModeIndicator()
{
    if (auto *i = this->findChildWithID(ComponentIDs::modeIndicatorComponentId))
    {
        this->modeIndicatorFader.fadeOut(i, Globals::UI::fadeOutLong);
    }
}

ModeIndicatorTrigger::ModeIndicatorTrigger()
{
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
}

void ModeIndicatorTrigger::mouseUp(const MouseEvent &event)
{
    if (auto *owner = dynamic_cast<ModeIndicatorOwnerComponent *>(this->getParentComponent()))
    {
        owner->handleChangeMode();
    }
}

void ModeIndicatorTrigger::mouseEnter(const MouseEvent &event)
{
    if (auto *owner = dynamic_cast<ModeIndicatorOwnerComponent *>(this->getParentComponent()))
    {
        owner->showModeIndicator();
    }
}

void ModeIndicatorTrigger::mouseExit(const MouseEvent &event)
{
    if (auto *owner = dynamic_cast<ModeIndicatorOwnerComponent *>(this->getParentComponent()))
    {
        owner->hideModeIndicator();
    }
}
