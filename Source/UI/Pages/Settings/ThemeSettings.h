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

#include "ColourScheme.h"

class ThemeSettings final : public Component,
    public ListBoxModel,
    private ChangeListener
{
public:

    ThemeSettings();
    ~ThemeSettings();

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool, Component*) override;
    void paintListBoxItem(int, Graphics&, int, int, bool) override {}
    void listBoxItemClicked(int, const MouseEvent &) override {}

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;

private:

#if PLATFORM_DESKTOP
    static constexpr auto rowHeight = 46;
#elif PLATFORM_MOBILE
    static constexpr auto rowHeight = 52;
#endif

    static constexpr auto horizontalContentMargin = 16;
    static constexpr auto verticalContentMargin = 8;

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

    Array<ColourScheme::Ptr> schemes;
    ColourScheme::Ptr currentScheme;

    UniquePointer<ListBox> themesList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThemeSettings)
};
