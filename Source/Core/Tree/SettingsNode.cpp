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

#include "Common.h"
#include "SettingsNode.h"
#include "TreeNodeSerializer.h"
#include "SerializationKeys.h"

#include "SyncSettings.h"
#include "AudioSettings.h"
#include "ThemeSettings.h"
#include "UserInterfaceSettings.h"
#include "TranslationSettings.h"

#include "LabeledSettingsWrapper.h"
#include "SimpleSettingsWrapper.h"

#include "ComponentsList.h"
#include "SettingsPage.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "Icons.h"

SettingsNode::SettingsNode() :
    TreeNode("Settings", Serialization::Core::settings)
{
    // Too much garbage in the main tree,
    // let's make it accessible from the button on the title page
    //this->setVisible(false);
}

Image SettingsNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::settings, TREE_NODE_ICON_HEIGHT);
}

String SettingsNode::getName() const noexcept
{
    return TRANS("tree::settings");
}

void SettingsNode::showPage()
{
    if (this->settingsPage == nullptr)
    {
        this->recreatePage();
    }
    
    App::Layout().showPage(this->settingsPage, this);
}

void SettingsNode::recreatePage()
{
    this->settingsPage = nullptr;
    this->syncSettingsWrapper = nullptr;
    this->syncSettings = nullptr;
    this->translationSettingsWrapper = nullptr;
    this->translationSettings = nullptr;
    this->uiSettingsWrapper = nullptr;
    this->uiSettings = nullptr;
    this->themeSettingsWrapper = nullptr;
    this->themeSettings = nullptr;
    this->audioSettingsWrapper = nullptr;
    this->audioSettings = nullptr;
    this->settingsList = nullptr;
    
    this->settingsList = new ComponentsList(0, 6);
    
    this->translationSettings = new TranslationSettings();
    const String untranslatedLanguageCaption(CharPointer_UTF8("Language / Sprache / Langue / \xe8\xaf\xad\xe8\xa8\x80 / \xd0\xaf\xd0\xb7\xd1\x8b\xd0\xba"));
    this->translationSettingsWrapper = new LabeledSettingsWrapper(this->translationSettings, untranslatedLanguageCaption);
    this->settingsList->addAndMakeVisible(this->translationSettingsWrapper);

    this->themeSettings = new ThemeSettings();
    this->themeSettingsWrapper = new LabeledSettingsWrapper(this->themeSettings, TRANS("settings::ui"));
    this->settingsList->addAndMakeVisible(this->themeSettingsWrapper);

#if HELIO_DESKTOP
    this->uiSettings = new UserInterfaceSettings();
    this->uiSettingsWrapper = new SimpleSettingsWrapper(this->uiSettings);
    this->settingsList->addAndMakeVisible(this->uiSettingsWrapper);
#endif

    this->audioSettings = new AudioSettings(App::Workspace().getAudioCore());
    this->audioSettingsWrapper = new LabeledSettingsWrapper(this->audioSettings, TRANS("settings::audio"));
    this->settingsList->addAndMakeVisible(this->audioSettingsWrapper);

    this->syncSettings = new SyncSettings();
    this->syncSettingsWrapper = new LabeledSettingsWrapper(this->syncSettings, TRANS("settings::sync"));
    this->settingsList->addAndMakeVisible(this->syncSettingsWrapper);

    this->settingsPage = new SettingsPage(this->settingsList);
}

bool SettingsNode::hasMenu() const noexcept
{
    return false;
}

ScopedPointer<Component> SettingsNode::createMenu()
{
    return nullptr;
}
