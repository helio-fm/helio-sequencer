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
#include "SettingsTreeItem.h"
#include "TreeItemChildrenSerializer.h"
#include "MainLayout.h"
#include "Icons.h"
#include "SerializationKeys.h"
#include "AuthSettings.h"
#include "AudioSettings.h"
#include "UserInterfaceSettings.h"
#include "TranslationSettings.h"
#include "ComponentsList.h"
#include "LabeledSettingsWrapper.h"
#include "SettingsPage.h"
#include "App.h"
#include "Workspace.h"

SettingsTreeItem::SettingsTreeItem() :
    TreeItem("Settings", Serialization::Core::settings)
{
    // Too much garbage in the main tree,
    // let's make it accessible from the button on the title page
    //this->setVisible(false);
}

Colour SettingsTreeItem::getColour() const noexcept
{
    return Colour(0xffffbe92).interpolatedWith(Colour(0xffff80f3), 0.5f);
}

Image SettingsTreeItem::getIcon() const noexcept
{
    return Icons::findByName(Icons::settings, TREE_LARGE_ICON_HEIGHT);
}

String SettingsTreeItem::getName() const noexcept
{
    return TRANS("tree::settings");
}

void SettingsTreeItem::showPage()
{
    if (this->settingsPage == nullptr)
    {
        this->recreatePage();
    }
    
    App::Layout().showPage(this->settingsPage, this);
}

void SettingsTreeItem::recreatePage()
{
    this->settingsPage = nullptr;
    this->authSettingsWrapper = nullptr;
    this->authSettings = nullptr;
    this->translationSettingsWrapper = nullptr;
    this->translationSettings = nullptr;
    this->uiSettingsWrapper = nullptr;
    this->uiSettings = nullptr;
    this->audioSettingsWrapper = nullptr;
    this->audioSettings = nullptr;
    this->settingsList = nullptr;
    
    this->settingsList = new ComponentsList(0, 6);
    
    this->translationSettings = new TranslationSettings();
    const String untranslatedLanguageCaption(CharPointer_UTF8("Language / Sprache / Langue / Idioma / Lingua / \xd0\xaf\xd0\xb7\xd1\x8b\xd0\xba"));
    this->translationSettingsWrapper = new LabeledSettingsWrapper(this->translationSettings, untranslatedLanguageCaption);
    this->settingsList->addAndMakeVisible(this->translationSettingsWrapper);
    
    //this->authSettings = new AuthorizationSettings();
    //this->authSettingsWrapper = new LabeledSettingsWrapper(this->authSettings, TRANS("settings::auth"));
    //this->settingsList->addAndMakeVisible(this->authSettingsWrapper);

    this->uiSettings = new UserInterfaceSettings();
    this->uiSettingsWrapper = new LabeledSettingsWrapper(this->uiSettings, TRANS("settings::ui"));
    this->settingsList->addAndMakeVisible(this->uiSettingsWrapper);

    this->audioSettings = new AudioSettings(App::Workspace().getAudioCore());
    this->audioSettingsWrapper = new LabeledSettingsWrapper(this->audioSettings, TRANS("settings::audio"));
    this->settingsList->addAndMakeVisible(this->audioSettingsWrapper);

    this->settingsPage = new SettingsPage(this->settingsList);
}

bool SettingsTreeItem::hasMenu() const noexcept
{
    return false;
}

ScopedPointer<Component> SettingsTreeItem::createMenu()
{
    return nullptr;
}
