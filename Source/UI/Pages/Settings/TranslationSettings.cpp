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
#include "TranslationSettings.h"
#include "TranslationSettingsItem.h"
#include "Config.h"

TranslationSettings::TranslationSettings()
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->translationsList = make<ListBox>();
    this->addAndMakeVisible(this->translationsList.get());

    this->availableTranslations = App::Config().getTranslations()->getAll();
    this->currentTranslation = App::Config().getTranslations()->getCurrent();

    this->setSize(600, TranslationSettings::verticalContentMargin * 2 +
        this->availableTranslations.size() * TranslationSettings::rowHeight);

    this->translationsList->setModel(this);
    this->translationsList->setRowHeight(TranslationSettings::rowHeight);
    this->translationsList->getViewport()->setScrollBarsShown(true, false);

    this->scrollToSelectedLocale();

    App::Config().getTranslations()->addChangeListener(this);
}

TranslationSettings::~TranslationSettings()
{
    App::Config().getTranslations()->removeChangeListener(this);
}

void TranslationSettings::resized()
{
    this->translationsList->setBounds(this->getLocalBounds()
        .reduced(TranslationSettings::horizontalContentMargin,
            TranslationSettings::verticalContentMargin));
}

void TranslationSettings::scrollToSelectedLocale()
{
    int selectedRow = 0;
    for (int i = 0; i < this->availableTranslations.size(); ++i)
    {
        if (this->availableTranslations[i] == this->currentTranslation)
        {
            selectedRow = i;
            break;
        }
    }

    this->translationsList->scrollToEnsureRowIsOnscreen(selectedRow);
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void TranslationSettings::changeListenerCallback(ChangeBroadcaster *source)
{
    this->availableTranslations = App::Config().getTranslations()->getAll();
    this->currentTranslation = App::Config().getTranslations()->getCurrent();
    this->translationsList->updateContent();
    this->scrollToSelectedLocale();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *TranslationSettings::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber >= this->availableTranslations.size())
    {
        return existingComponentToUpdate;
    }

    const bool isLastRow = (rowNumber == this->availableTranslations.size() - 1);
    const String localeName(this->availableTranslations[rowNumber]->getName());
    const String localeId(this->availableTranslations[rowNumber]->getId());
    const bool isCurrentLocale = this->availableTranslations[rowNumber] == this->currentTranslation;

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<TranslationSettingsItem *>(existingComponentToUpdate))
        {
            row->updateDescription(isLastRow, isCurrentLocale, localeName, localeId);
        }
    }
    else
    {
        auto *row = new TranslationSettingsItem(*this->translationsList);
        row->updateDescription(isLastRow, isCurrentLocale, localeName, localeId);
        return row;
    }

    return existingComponentToUpdate;
}

int TranslationSettings::getNumRows()
{
    return this->availableTranslations.size();
}
