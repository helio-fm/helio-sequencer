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

class PageBackgroundB;

class SettingsPage final : public Component
{
public:

    explicit SettingsPage(Component *settingsList);
    ~SettingsPage() override;

    void resized() override;

#if PLATFORM_DESKTOP
    static constexpr auto viewportScrollBarWidth = 2;
#elif PLATFORM_MOBILE
    static constexpr auto viewportScrollBarWidth = 32;
#endif

private:

    UniquePointer<PageBackgroundB> background;
    UniquePointer<Viewport> viewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPage)
};
