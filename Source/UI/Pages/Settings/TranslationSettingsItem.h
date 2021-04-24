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

#include "DraggingListBoxComponent.h"
#include "SeparatorHorizontal.h"

class TranslationSettingsItem final : public DraggingListBoxComponent
{
public:

    explicit TranslationSettingsItem(ListBox &parentListBox);
    ~TranslationSettingsItem();

    void setSelected(bool shouldBeSelected) override;
    void updateDescription(bool isLastRowInList, bool isCurrentLocale,
        const String &localeName, const String &localeId);

    void resized() override;

private:

    ComponentAnimator selectionAnimator;
    UniquePointer<Component> selectionComponent;

    Component *createHighlighterComponent() override;

    friend class TranslationSettings;

    UniquePointer<Label> localeLabel;
    UniquePointer<Label> idLabel;
    UniquePointer<SeparatorHorizontal> separator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TranslationSettingsItem)
};
