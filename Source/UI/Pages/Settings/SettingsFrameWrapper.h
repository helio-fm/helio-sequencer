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

#include "FramePanel.h"

class SettingsFrameWrapper final : public Component
{
public:

    SettingsFrameWrapper(Component *targetComponent, const String &title = {});
    ~SettingsFrameWrapper() override;

    void showNonOwned(Component *targetComponent, const String &title);
    void resized() override;

private:

    SafePointer<Component> target;

    UniquePointer<FramePanel> panel;
    UniquePointer<Label> titleLabel;

    static constexpr auto titleMargin = 8;
    static constexpr auto titleHeight = 24;
    static constexpr auto panelMargin = 6;

    static constexpr auto contentMarginX = 4;
    static constexpr auto contentMarginY = 6;

    static constexpr auto labeledTotalMargins =
        contentMarginY * 2 + titleHeight + titleMargin * 2 + panelMargin;

    static constexpr auto simpleTotalMargins =
        contentMarginY * 2 + panelMargin * 2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsFrameWrapper)
};
