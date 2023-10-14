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

#if !NO_NETWORK

#include "ConfigurationResource.h"

class SyncSettings final : public Component,
    public ListBoxModel, private ChangeListener
{
public:

    SyncSettings();
    ~SyncSettings();

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int, bool, Component *) override;
    void paintListBoxItem(int, Graphics &, int, int, bool) override {}

    void resized() override;

private:

#if PLATFORM_DESKTOP
    static constexpr auto rowHeight = 32;
#elif PLATFORM_MOBILE
    static constexpr auto rowHeight = 48;
#endif

    void changeListenerCallback(ChangeBroadcaster *source) override;

    Array<bool> syncFlags;
    ReferenceCountedArray<ConfigurationResource> resources;

    void reloadConfigsList();
    void reloadSyncFlags();

    UniquePointer<ListBox> resourcesList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncSettings)
};

class NetworkSettings final : public Component
{
public:

    NetworkSettings();

    void resized() override;

private:

    UniquePointer<ToggleButton> checkForUpdatesButton;

};


#endif
