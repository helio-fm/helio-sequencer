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

#pragma once

#include "ColourIDs.h"
#include "ShadowLeftwards.h"

class TrackStartIndicator final : public Component
{
public:

    TrackStartIndicator()
    {
        this->shadow = make<ShadowLeftwards>(ShadowType::Light);
        this->addAndMakeVisible(this->shadow.get());

        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    void updatePosition(float newStartBeat)
    {
        if (this->beatPosition == newStartBeat)
        {
            return;
        }

        this->beatPosition = newStartBeat;
        this->absPosition = double(this->beatPosition - this->viewFirstBeat) /
            double(this->viewLastBeat - this->viewFirstBeat);
    }

    void updateViewRange(float newFirstBeat, float newLastBeat)
    {
        if (this->viewFirstBeat == newFirstBeat &&
            this->viewLastBeat == newLastBeat)
        {
            jassertfalse; // please only call this when ranges change
            return;
        }

        this->viewFirstBeat = newFirstBeat;
        this->viewLastBeat = newLastBeat;
        this->absPosition = double(this->beatPosition - this->viewFirstBeat) /
            double(this->viewLastBeat - this->viewFirstBeat);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillRect(this->getLocalBounds());
    }

    void resized() override
    {
        this->shadow->setBounds(this->getWidth() - TrackStartIndicator::shadowWidth,
            0, TrackStartIndicator::shadowWidth, this->getHeight());
    }

    void updateBounds()
    {
        this->setBounds(0, 0,
            int(floor(double(this->getParentWidth()) * this->absPosition)),
            this->getParentHeight());
    }

    void updateBounds(const Rectangle<int> &viewBounds)
    {
        this->setBounds(viewBounds.getX(),
            viewBounds.getY(),
            int(floor(double(viewBounds.getWidth()) * this->absPosition)),
            viewBounds.getHeight());
    }

private:

    double absPosition = 0.0;
    float beatPosition = 0.f;

    float viewFirstBeat = 0.f;
    float viewLastBeat = Globals::Defaults::projectLength;

    const Colour fillColour =
        findDefaultColour(ColourIDs::TrackScroller::outOfRangeFill);

    static constexpr int shadowWidth = 12;

    UniquePointer<ShadowLeftwards> shadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackStartIndicator)
};
