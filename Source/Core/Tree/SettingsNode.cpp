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
#include "SettingsFrameWrapper.h"
#include "SettingsPage.h"
#include "MainLayout.h"
#include "Workspace.h"

class SettingsSectionsList final : public Component
{
public:

    SettingsSectionsList(int paddingLeft = 0, int paddingRight = 0,
        int paddingTop = 0, int paddingBottom = 0) :
        paddingLeft(paddingLeft),
        paddingRight(paddingRight),
        paddingTop(paddingTop),
        paddingBottom(paddingBottom)
    {
        this->setAccessible(false);
        this->setPaintingIsUnclipped(true);
    }

    void resized() override
    {
        if (this->getParentComponent() == nullptr)
        {
            return;
        }
    
        int y = this->paddingTop;

        for (int i = 0; i < this->getNumChildComponents(); ++i)
        {
            auto *item = this->getChildComponent(i);
            item->setVisible(item->isEnabled());
            if (item->isEnabled())
            {
                item->setSize(this->getWidth() - this->paddingLeft - this->paddingRight, item->getHeight());
                item->setTopLeftPosition(this->paddingLeft, y);
                y += item->getHeight();
            }
        }

        this->setSize(this->getWidth(), y + this->paddingBottom);
    }

private:

    const int paddingLeft;
    const int paddingRight;
    const int paddingTop;
    const int paddingBottom;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsSectionsList)
};

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
    this->uiSettingsWrapper = nullptr;
    this->uiSettings = nullptr;
    this->themeSettingsWrapper = nullptr;
    this->themeSettings = nullptr;
    this->audioSettingsWrapper = nullptr;
    this->audioSettings = nullptr;
    this->sectionsList = nullptr;

    this->sectionsList = make<SettingsSectionsList>(6,
        jmax(0, 6 - SettingsPage::viewportScrollBarWidth),
        2, 12);

    this->themeSettings = make<ThemeSettings>();
    this->themeSettingsWrapper = make<SettingsFrameWrapper>(this->themeSettings.get(), TRANS(I18n::Settings::ui));
    this->sectionsList->addAndMakeVisible(this->themeSettingsWrapper.get());

    this->uiSettings = make<UserInterfaceSettings>();
    this->uiSettingsWrapper = make<SettingsFrameWrapper>(this->uiSettings.get(), TRANS(I18n::Settings::uiFlags));
    this->sectionsList->addAndMakeVisible(this->uiSettingsWrapper.get());

    this->audioSettings = make<AudioSettings>(App::Workspace().getAudioCore());
    this->audioSettingsWrapper = make<SettingsFrameWrapper>(this->audioSettings.get(), TRANS(I18n::Settings::audio));
    this->sectionsList->addAndMakeVisible(this->audioSettingsWrapper.get());

    this->settingsPage = make<SettingsPage>(this->sectionsList.get());
}

bool SettingsNode::hasMenu() const noexcept
{
    return false;
}

UniquePointer<Component> SettingsNode::createMenu()
{
    return nullptr;
}
