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
#include "DraggingListBoxComponent.h"
#include "HelioTheme.h"
#include "ColourScheme.h"
class IconComponent;
class HelioTheme;
//[/Headers]


class ThemeSettingsItem final : public DraggingListBoxComponent
{
public:

    ThemeSettingsItem(ListBox &parentListBox);
    ~ThemeSettingsItem();

    //[UserMethods]

    void setSelected(bool shouldBeSelected) override;

    void updateDescription(bool isLastRowInList,
        bool isCurrentTheme, const ColourScheme::Ptr colours);

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]
    void applyTheme(const ColourScheme::Ptr theme);

    ColourScheme::Ptr colours;
    ScopedPointer<HelioTheme> theme;

    ListBox &listBox;
    ComponentAnimator selectionAnimator;
    ScopedPointer<Component> selectionComponent;

    Image rollImage;

    Component *createHighlighterComponent() override;

    //[/UserVariables]

    ScopedPointer<Label> schemeNameLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemeSettingsItem)
};
