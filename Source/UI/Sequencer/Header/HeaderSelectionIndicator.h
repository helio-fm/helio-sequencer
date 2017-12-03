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


class HeaderSelectionIndicator  : public Component,
                                  private Timer
{
public:

    HeaderSelectionIndicator ();

    ~HeaderSelectionIndicator();

    //[UserMethods]

    inline void setStartAnchor(double absX)
    {
        this->startAbsPosition = absX;
        this->endAbsPosition = absX;
        this->updateBounds();
    }

    inline void setEndAnchor(double absX)
    {
        this->endAbsPosition = absX;
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

    double startAbsPosition;
    double endAbsPosition;

    void updateBounds()
    {
        const double start = jmin(this->startAbsPosition, this->endAbsPosition);
        const double end = jmax(this->startAbsPosition, this->endAbsPosition);

        const int startX = int(double(this->getParentWidth()) * start);
        const int endX = int(double(this->getParentWidth()) * end);

        this->setBounds(startX, this->getY(), (endX - startX), this->getHeight());
    }

    //[/UserVariables]


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderSelectionIndicator)
};
