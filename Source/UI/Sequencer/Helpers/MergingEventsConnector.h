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

class MergingEventsConnector : public Component
{
public:

    MergingEventsConnector(SafePointer<Component> sourceComponent,
        Point<float> startPosition);

    ~MergingEventsConnector();

    void setEndPosition(Point<float> position);

    Component *getSourceComponent() const noexcept;
    Component *getTargetComponent() const noexcept;
    virtual void setTargetComponent(SafePointer<Component> component) noexcept;

    virtual bool canMergeInto(SafePointer<Component> component) = 0;

    Point<float> getStartPosition() const noexcept;
    Point<float> getEndPosition() const noexcept;

    void parentSizeChanged() override;

protected:

    SafePointer<Component> sourceComponent;
    SafePointer<Component> targetComponent;

    const Point<float> startPositionAbs;
    Point<float> endPositionAbs;

    void updateBounds();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MergingEventsConnector)
};

class MergingNotesConnector final : public MergingEventsConnector
{
public:

    MergingNotesConnector(SafePointer<Component> sourceComponent,
        Point<float> startPosition);

    void paint(Graphics &g) override;

    bool canMergeInto(SafePointer<Component> component) override;
};

class MergingClipsConnector final : public MergingEventsConnector
{
public:

    MergingClipsConnector(SafePointer<Component> sourceComponent,
        Point<float> startPosition);

    void paint(Graphics &g) override;

    void setTargetComponent(SafePointer<Component> component) noexcept override;
    bool canMergeInto(SafePointer<Component> component) override;

private:

    Colour startColour = Colours::white;
    Colour endColour = Colours::white;

};
