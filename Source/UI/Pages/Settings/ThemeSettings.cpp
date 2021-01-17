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
#include "ThemeSettings.h"
#include "ThemeSettingsItem.h"
#include "Config.h"

ThemeSettings::ThemeSettings()
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->fontComboPrimer = make<MobileComboBox::Primer>();
    this->addAndMakeVisible(this->fontComboPrimer.get());

    this->themesList = make<ListBox>();
    this->addAndMakeVisible(this->themesList.get());

    this->schemes = App::Config().getColourSchemes()->getAll();
    this->currentScheme = App::Config().getColourSchemes()->getCurrent();

    const int numSchemes = this->schemes.size();
    this->setSize(600, 16 + numSchemes * ThemeSettings::rowHeight);

    this->themesList->setModel(this);
    this->themesList->setRowHeight(ThemeSettings::rowHeight);
    this->themesList->getViewport()->setScrollBarsShown(true, false);

    App::Config().getColourSchemes()->addChangeListener(this);
}

ThemeSettings::~ThemeSettings()
{
    App::Config().getColourSchemes()->removeChangeListener(this);
}

void ThemeSettings::resized()
{
    this->fontComboPrimer->setBounds(4, 4, this->getWidth() - 8, this->getHeight() - 8);
    this->themesList->setBounds(8, 8, this->getWidth() - 24, this->getHeight() - 16);
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void ThemeSettings::changeListenerCallback(ChangeBroadcaster *source)
{
    this->schemes = App::Config().getColourSchemes()->getAll();
    this->currentScheme = App::Config().getColourSchemes()->getCurrent();
    this->themesList->updateContent();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *ThemeSettings::refreshComponentForRow(int rowNumber, bool isRowSelected,
    Component *existingComponentToUpdate)
{
    if (rowNumber >= this->schemes.size())
    {
        return existingComponentToUpdate;
    }

    const bool isCurrentScheme =
        (this->currentScheme->getResourceId() ==
            this->schemes[rowNumber]->getResourceId());

    const bool isLastRow = (rowNumber == this->schemes.size() - 1);

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<ThemeSettingsItem *>(existingComponentToUpdate))
        {
            row->updateDescription(isLastRow, isCurrentScheme, this->schemes[rowNumber]);
        }
    }
    else
    {
        auto *row = new ThemeSettingsItem(*this->themesList);
        row->updateDescription(isLastRow, isCurrentScheme, this->schemes[rowNumber]);
        return row;
    }

    return existingComponentToUpdate;
}

int ThemeSettings::getNumRows()
{
    return this->schemes.size();
}
