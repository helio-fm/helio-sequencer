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
#include "HighlightedComponent.h"
#include "IconButton.h"
#include "TreeNode.h"

class IconComponent;
class HeadlineDropdown;
//[/Headers]

#include "HeadlineItemArrow.h"

class HeadlineNavigationPanel final : public Component
{
public:

    HeadlineNavigationPanel();
    ~HeadlineNavigationPanel();

    //[UserMethods]
    void updateState(bool canGoPrevious, bool canGoNext);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]
    ScopedPointer<HeadlineDropdown> dropdown;
    Colour bgColour;
    //[/UserVariables]

    UniquePointer<IconButton> navigatePrevious;
    UniquePointer<IconButton> navigateNext;
    UniquePointer<HeadlineItemArrow> component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeadlineNavigationPanel)
};
