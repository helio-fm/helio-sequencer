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

#include "TreeNode.h"
#include "ComponentsList.h"

class SettingsNode final : public TreeNode
{
public:

    SettingsNode();
    
    String getName() const noexcept override;
    Image getIcon() const noexcept override;

    void showPage() override;
    void recreatePage() override;

    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;

private:

    UniquePointer<ComponentsList> settingsList;
    UniquePointer<Component> audioSettings;
    UniquePointer<Component> audioSettingsWrapper;
    UniquePointer<Component> uiSettings;
    UniquePointer<Component> uiSettingsWrapper;
    UniquePointer<Component> themeSettings;
    UniquePointer<Component> themeSettingsWrapper;
    UniquePointer<Component> translationSettings;
    UniquePointer<Component> translationSettingsWrapper;
    UniquePointer<Component> networkSettings;
    UniquePointer<Component> networkSettingsWrapper;
    UniquePointer<Component> syncSettings;
    UniquePointer<Component> syncSettingsWrapper;
    UniquePointer<Component> settingsPage;

};
