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

#include "TreeNode.h"
#include "HighlightedComponent.h"

class HeadlineItemHighlighter;
class HeadlineItemDataSource;

class HeadlineDropdown final : public Component, private Timer
{
public:

    HeadlineDropdown(WeakReference<HeadlineItemDataSource> targetItem, const Point<int> &position);
    ~HeadlineDropdown();

    void childBoundsChanged(Component *) override;

    void paint(Graphics &g) override;
    void resized() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
    void inputAttemptWhenModal() override;

private:

    static constexpr auto padding = 15;

    WeakReference<HeadlineItemDataSource> item;

    void timerCallback() override;
    void syncWidthWithContent();

    Path backgroundShape;

    UniquePointer<Component> content;
    UniquePointer<HeadlineItemHighlighter> header;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineDropdown)
};
