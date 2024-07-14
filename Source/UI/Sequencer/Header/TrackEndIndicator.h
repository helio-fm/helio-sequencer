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
#include "ShadowRightwards.h"

class TrackEndIndicator final : public Component
{
public:

    TrackEndIndicator()
    {
        this->shadow = make<ShadowRightwards>(ShadowType::Light);
        this->addAndMakeVisible(this->shadow.get());

        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    void updatePosition(float newEndBeat)
    {
        if (this->beatPosition == newEndBeat)
        {
            return;
        }

        this->beatPosition = newEndBeat;
        this->absPosition = double(this->beatPosition - this->viewFirstBeat) /
            double(this->viewLastBeat - this->viewFirstBeat);
    }

    void updateViewRange(float newFirstBeat, float newLastBeat)
    {
        if (this->viewFirstBeat == newFirstBeat &&
            this->viewLastBeat == newLastBeat)
        {
            //jassertfalse; // please only call this when ranges change
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
        this->shadow->setBounds(0, 0, TrackEndIndicator::shadowWidth, this->getHeight());
    }

    void updateBounds()
    {
        this->setBounds(int(ceil(double(this->getParentWidth()) * this->absPosition)), 0,
            int(floor(double(this->getParentWidth()) * (1.0 - this->absPosition))),
            this->getParentHeight());
    }

    void updateBounds(const Rectangle<int> &viewBounds)
    {
        this->setBounds(viewBounds.getX() + int(ceil(double(viewBounds.getWidth()) * this->absPosition)),
            viewBounds.getY(),
            int(floor(double(viewBounds.getWidth()) * (1.0 - this->absPosition))),
            viewBounds.getHeight());
    }

private:

    double absPosition = 1.0;
    float beatPosition = Globals::Defaults::projectLength;

    float viewFirstBeat = 0.f;
    float viewLastBeat = Globals::Defaults::projectLength;

    const Colour fillColour =
        findDefaultColour(ColourIDs::TrackScroller::outOfRangeFill);

    static constexpr int shadowWidth = 12;

    UniquePointer<ShadowRightwards> shadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackEndIndicator)
};
