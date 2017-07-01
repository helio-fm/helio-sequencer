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

//[Headers]
class IconComponent;
//[/Headers]


class TimeDistanceIndicator  : public Component,
                               private Timer
{
public:

    TimeDistanceIndicator ();

    ~TimeDistanceIndicator();

    //[UserMethods]

    Label *getTimeLabel() const
    {
        return this->timeLabel;
    }

    inline void setAnchoredBetween(float absX1, float absX2)
    {
        this->startAbsPosition = jmin(absX1, absX2);
        this->endAbsPosition = jmax(absX1, absX2);
        this->updateBounds();
    }

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;


private:

    //[UserVariables]

    void timerCallback() override
    {
        this->setAlpha(this->getAlpha() + 0.1f);

        if (this->getAlpha() >= 1.f)
        {
            this->stopTimer();
        }
    }

    float startAbsPosition;
    float endAbsPosition;

    void updateBounds()
    {
        const int startX = int(float(this->getParentWidth()) * this->startAbsPosition);
        const int endX = int(float(this->getParentWidth()) * this->endAbsPosition);

        this->setBounds(startX,
                        this->getY(),
                        (endX - startX),
                        this->getHeight());
    }

    //[/UserVariables]

    ScopedPointer<Label> timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeDistanceIndicator)
};
