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
#include "ColourScheme.h"
#include "MobileComboBox.h"
//[/Headers]


class ThemeSettings final : public Component,
                            public ListBoxModel,
                            private ChangeListener
{
public:

    ThemeSettings();
    ~ThemeSettings();

    //[UserMethods]

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool, Component*) override;
    void paintListBoxItem(int, Graphics&, int, int, bool) override {}
    void listBoxItemClicked(int, const MouseEvent &) override {}

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

    Array<ColourScheme::Ptr> schemes;
    ColourScheme::Ptr currentScheme;

    //[/UserVariables]

    UniquePointer<MobileComboBox::Primer> fontComboPrimer;
    UniquePointer<ListBox> themesList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemeSettings)
};
