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

#pragma once

#include "ColourIDs.h"

class TimeDistanceIndicator final : public Component, private Timer
{
public:

    TimeDistanceIndicator() :
        colour(findDefaultColour(ColourIDs::RollHeader::timeDistance))
    {
        this->timeLabel = make<Label>();
        this->addAndMakeVisible(this->timeLabel.get());
        this->timeLabel->setFont(Globals::UI::Fonts::L);
        this->timeLabel->setJustificationType(Justification::centred);

        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setAlwaysOnTop(true);

        this->setSize(32, 32);

        this->setAlpha(0.f);
        this->startTimerHz(60);
    }

    ~TimeDistanceIndicator() override
    {
        Desktop::getInstance().getAnimator().animateComponent(this,
            this->getBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 0.0);
    }

    Label *getTimeLabel() const noexcept
    {
        return this->timeLabel.get();
    }

    inline void setAnchoredBetween(double absX1, double absX2)
    {
        this->startAbsPosition = jmin(absX1, absX2);
        this->endAbsPosition = jmax(absX1, absX2);
        this->updateBounds();
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->colour);

        constexpr auto dashLength = 8;
        for (int i = dashLength; i < this->getWidth() - dashLength; i += dashLength * 2)
        {
            g.fillRect(i + 1, 0, dashLength, 1); // looks fancier)
            g.fillRect(i, 1, dashLength, 2);
            //g.fillRect(i, 0, dashLength, 3);
        }
    }

    void resized() override
    {
        this->timeLabel->setBounds(0, 4, this->getWidth(), 20);
    }

    void parentHierarchyChanged() override
    {
        this->updateBounds();
    }

    void parentSizeChanged() override
    {
        this->updateBounds();
    }

private:

    void timerCallback() override
    {
        this->setAlpha(this->getAlpha() + 0.1f);

        if (this->getAlpha() >= 1.f)
        {
            this->stopTimer();
        }
    }

    const Colour colour;

    double startAbsPosition = 0.0;
    double endAbsPosition = 0.0;

    void updateBounds()
    {
        const int startX = int(double(this->getParentWidth()) * this->startAbsPosition);
        const int endX = int(double(this->getParentWidth()) * this->endAbsPosition);
        this->setBounds(startX, this->getY(), (endX - startX), this->getHeight());
    }

    UniquePointer<Label> timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeDistanceIndicator)
};
