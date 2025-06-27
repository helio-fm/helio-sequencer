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

#include "Common.h"
#include "SettingsNode.h"
#include "SerializationKeys.h"
#include "AudioSettings.h"
#include "ThemeSettings.h"
#include "UserInterfaceSettings.h"
#include "TranslationSettings.h"
#include "SettingsFrameWrapper.h"
#include "ComponentsList.h"
#include "SettingsPage.h"
#include "MainLayout.h"
#include "Workspace.h"

SettingsNode::SettingsNode() :
    TreeNode("Settings", Serialization::Core::settings) {}

Image SettingsNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::settings, Globals::UI::headlineIconSize);
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
    this->translationSettingsWrapper = nullptr;
    this->translationSettings = nullptr;
    this->uiSettingsWrapper = nullptr;
    this->uiSettings = nullptr;
    this->themeSettingsWrapper = nullptr;
    this->themeSettings = nullptr;
    this->audioSettingsWrapper = nullptr;
    this->audioSettings = nullptr;
    this->settingsList = nullptr;

    constexpr auto listPadding = 10;
    this->settingsList = make<ComponentsList>(listPadding,
        jmax(0, listPadding - SettingsPage::viewportScrollBarWidth));

    this->translationSettings = make<TranslationSettings>();
    const String untranslatedLanguageCaption(CharPointer_UTF8("Language / \xe8\xaf\xad\xe8\xa8\x80 / Sprache / \xd0\xaf\xd0\xb7\xd1\x8b\xd0\xba"));
    this->translationSettingsWrapper = make<SettingsFrameWrapper>(this->translationSettings.get(), untranslatedLanguageCaption);
    this->settingsList->addAndMakeVisible(this->translationSettingsWrapper.get());

    this->themeSettings = make<ThemeSettings>();
    this->themeSettingsWrapper = make<SettingsFrameWrapper>(this->themeSettings.get(), TRANS(I18n::Settings::ui));
    this->settingsList->addAndMakeVisible(this->themeSettingsWrapper.get());

    this->uiSettings = make<UserInterfaceSettings>();
    this->uiSettingsWrapper = make<SettingsFrameWrapper>(this->uiSettings.get(), TRANS(I18n::Settings::uiFlags));
    this->settingsList->addAndMakeVisible(this->uiSettingsWrapper.get());

    this->audioSettings = make<AudioSettings>(App::Workspace().getAudioCore());
    this->audioSettingsWrapper = make<SettingsFrameWrapper>(this->audioSettings.get(), TRANS(I18n::Settings::audio));
    this->settingsList->addAndMakeVisible(this->audioSettingsWrapper.get());

    this->settingsPage = make<SettingsPage>(this->settingsList.get());
}

bool SettingsNode::hasMenu() const noexcept
{
    return false;
}

UniquePointer<Component> SettingsNode::createMenu()
{
    return nullptr;
}
