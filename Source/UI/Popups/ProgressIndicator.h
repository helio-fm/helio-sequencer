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

#include "Common.h"

#include "Icons.h"
#define PROGRESS_INDICATOR_UPDATE_TIMS_MS 17

class ProgressIndicator final : public Component, private Timer
{
public:

    ProgressIndicator() : indicatorDegree(0)
    {
        this->setInterceptsMouseClicks(false, false);
        this->indicatorShape = Icons::getDrawableByName(Icons::progressIndicator);
    }

    ~ProgressIndicator() override
    {
        this->stopAnimating();
    }
    
    void startAnimating()
    {
        this->startTimer(PROGRESS_INDICATOR_UPDATE_TIMS_MS);
    }
    
    void stopAnimating()
    {
        this->stopTimer();
    }
    
    void paint (Graphics& g) override
    {
        // We need to go deeper.
        if (DrawableComposite *group =
                dynamic_cast<DrawableComposite *>(this->indicatorShape->getChildComponent(0)->getChildComponent(0)))
        {
            Rectangle<float> allArea(group->getContentArea());
            AffineTransform fitTransform = RectanglePlacement(RectanglePlacement::onlyReduceInSize)
                    .getTransformToFit(allArea, this->getLocalBounds().toFloat());

            const float radius = 10;
            const float indicatorRadian = float(360.f - this->indicatorDegree) * (MathConstants<float>::pi / 180.f);
            const Point<float> indicatorPosition(radius * cosf(indicatorRadian), radius * sinf(indicatorRadian));

            const float numSegments = float(group->getNumChildComponents());

            for (int i = 0; i < numSegments; ++i)
            {
                Component *child = group->getChildComponent(i);

                if (DrawableComposite *dc = dynamic_cast<DrawableComposite *>(child))
                {
                    const float oneSegment = (360.f / numSegments);
                    const float currentPartRadian = (i * oneSegment) * (MathConstants<float>::pi / 180.f);
                    const Point<float> currentPartPosition(radius * cosf(currentPartRadian), radius * sinf(currentPartRadian));

                    const float distance = currentPartPosition.getDistanceFrom(indicatorPosition);
                    const float partAlpha = jmax(0.1f, 1.f - (distance / (radius * 2)));

                    const Rectangle<float> drawableBounds(dc->getDrawableBounds());
                    const Rectangle<float> subArea(dc->getContentArea());

                    if (DrawablePath *dp = dynamic_cast<DrawablePath *>(dc->getChildComponent(0)))
                    {
                        Path p(dp->getPath());
                        AffineTransform a(RectanglePlacement(RectanglePlacement::onlyReduceInSize).
                                          getTransformToFit(drawableBounds, subArea.transformedBy(fitTransform)));
                        g.setColour(Colours::black.withAlpha(0.15f));
                        g.strokePath(p, PathStrokeType(1.f), a);
                        g.setColour(Colours::white.withAlpha(partAlpha));
                        g.fillPath(p, a);
                    }
                }
            }
        }
    }

private:

    void timerCallback() override
    {
        this->indicatorDegree = (this->indicatorDegree + 7) % 360;
        this->repaint();
    }

    int indicatorDegree;

    ScopedPointer<Drawable> indicatorShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgressIndicator)
};
