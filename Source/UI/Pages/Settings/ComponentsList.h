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

class ComponentsList final : public Component
{
public:

    ComponentsList(int paddingLeft = 0, int paddingRight = 0);

    void showChild(Component *child);
    void hideChild(Component *child);

    void resized() override;

private:

    int paddingLeft;
    int paddingRight;

    Component *findContainerOf(Component *content);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ComponentsList)
};
