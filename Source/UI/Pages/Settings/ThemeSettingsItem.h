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

#include "DraggingListBoxComponent.h"
#include "HelioTheme.h"
#include "ColourScheme.h"

class IconComponent;
class HelioTheme;

class ThemeSettingsItem final : public DraggingListBoxComponent
{
public:

    explicit ThemeSettingsItem(ListBox &parentListBox);
    ~ThemeSettingsItem();

    void setSelected(bool shouldBeSelected) override;

    void updateDescription(bool isLastRowInList,
        bool isCurrentTheme, const ColourScheme::Ptr colours);

    void paint(Graphics &g) override;
    void resized() override;

private:

    void applyTheme(const ColourScheme::Ptr theme);

    ColourScheme::Ptr colours;
    UniquePointer<HelioTheme> theme;

    ListBox &listBox;
    ComponentAnimator selectionAnimator;
    UniquePointer<Component> selectionComponent;

    Image rollImage;
    Image icon1;
    Image icon2;

    Component *createHighlighterComponent() override;

    UniquePointer<Label> schemeNameLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemeSettingsItem)
};
