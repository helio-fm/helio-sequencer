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
    return TRANS(I18n::Tree::settings);
}

void SettingsNode::showPage()
{
    if (this->settingsPage == nullptr)
    {
        this->recreatePage();
    }
    
    App::Layout().showPage(this->settingsPage.get(), this);
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
    
    this->settingsList.reset(new ComponentsList(0, 6));
    
    this->translationSettings.reset(new TranslationSettings());
    const String untranslatedLanguageCaption(CharPointer_UTF8("Language / \xe8\xaf\xad\xe8\xa8\x80 / Sprache / \xd0\xaf\xd0\xb7\xd1\x8b\xd0\xba"));
    this->translationSettingsWrapper.reset(new LabeledSettingsWrapper(this->translationSettings.get(), untranslatedLanguageCaption));
    this->settingsList->addAndMakeVisible(this->translationSettingsWrapper.get());

    this->themeSettings.reset(new ThemeSettings());
    this->themeSettingsWrapper.reset(new LabeledSettingsWrapper(this->themeSettings.get(), TRANS(I18n::Settings::ui)));
    this->settingsList->addAndMakeVisible(this->themeSettingsWrapper.get());

#if HELIO_DESKTOP
    this->uiSettings.reset(new UserInterfaceSettings());
    this->uiSettingsWrapper.reset(new SimpleSettingsWrapper(this->uiSettings.get()));
    this->settingsList->addAndMakeVisible(this->uiSettingsWrapper.get());
#endif

    this->audioSettings.reset(new AudioSettings(App::Workspace().getAudioCore()));
    this->audioSettingsWrapper.reset(new LabeledSettingsWrapper(this->audioSettings.get(), TRANS(I18n::Settings::audio)));
    this->settingsList->addAndMakeVisible(this->audioSettingsWrapper.get());

    this->syncSettings.reset(new SyncSettings());
    this->syncSettingsWrapper.reset(new LabeledSettingsWrapper(this->syncSettings.get(), TRANS(I18n::Settings::sync)));
    this->settingsList->addAndMakeVisible(this->syncSettingsWrapper.get());

    this->settingsPage.reset(new SettingsPage(this->settingsList.get()));
}

bool SettingsNode::hasMenu() const noexcept
{
    return false;
}

UniquePointer<Component> SettingsNode::createMenu()
{
    return nullptr;
}
