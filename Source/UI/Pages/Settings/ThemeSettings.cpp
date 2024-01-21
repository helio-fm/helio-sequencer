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
#include "ThemeSettings.h"
#include "ThemeSettingsItem.h"
#include "Config.h"

ThemeSettings::ThemeSettings()
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->themesList = make<ListBox>();
    this->addAndMakeVisible(this->themesList.get());

    this->schemes = App::Config().getColourSchemes()->getAll();
    this->currentScheme = App::Config().getColourSchemes()->getCurrent();

    this->setSize(600, ThemeSettings::verticalContentMargin * 2 +
        this->schemes.size() * ThemeSettings::rowHeight);

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
    this->themesList->setBounds(this->getLocalBounds()
        .reduced(ThemeSettings::horizontalContentMargin,
            ThemeSettings::verticalContentMargin));
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

    const auto colourScheme = this->schemes[rowNumber];
    const bool isLastRow = rowNumber == this->schemes.size() - 1;
    const bool isSelected = this->currentScheme->isEquivalentTo(colourScheme);

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<ThemeSettingsItem *>(existingComponentToUpdate))
        {
            row->updateDescription(isLastRow, isSelected, colourScheme);
        }
    }
    else
    {
        auto *row = new ThemeSettingsItem(*this->themesList);
        row->updateDescription(isLastRow, isSelected, colourScheme);
        return row;
    }

    return existingComponentToUpdate;
}

int ThemeSettings::getNumRows()
{
    return this->schemes.size();
}
