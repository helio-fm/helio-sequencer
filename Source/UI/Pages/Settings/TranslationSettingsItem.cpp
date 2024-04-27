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
#include "TranslationSettingsItem.h"
#include "Config.h"
#include "ColourIDs.h"
#include "IconComponent.h"

class TranslationSettingsItemHighlighter final : public Component
{
public:

    TranslationSettingsItemHighlighter()
    {
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
        g.fillRoundedRectangle(36.f, 2.f,
            float(this->getWidth() - 41), float(this->getHeight() - 6), 2.f);
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TranslationSettingsItemHighlighter)
};

class TranslationSettingsItemSelection final : public Component
{
public:

    TranslationSettingsItemSelection()
    {
        this->setPaintingIsUnclipped(true);

        this->iconComponent = make<IconComponent>(Icons::apply, 1.f, int(iconSize));
        this->addAndMakeVisible(this->iconComponent.get());
    }

    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
        g.fillRoundedRectangle(36.f, 2.f,
            float(this->getWidth() - 41), float(this->getHeight() - 6), 2.f);
    }

    void resized() override
    {
        this->iconComponent->setBounds(12,
            (this->getHeight() / 2) - (iconSize / 2), iconSize, iconSize);
    }

private:

    static constexpr auto iconSize = 16;
    UniquePointer<IconComponent> iconComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TranslationSettingsItemSelection)
};

TranslationSettingsItem::TranslationSettingsItem(ListBox &parentListBox) :
    DraggingListBoxComponent(parentListBox.getViewport())
{
    this->localeLabel = make<Label>();
    this->addAndMakeVisible(this->localeLabel.get());
    this->localeLabel->setJustificationType(Justification::centredLeft);
    this->localeLabel->setFont(Globals::UI::Fonts::M);

    this->idLabel = make<Label>();
    this->addAndMakeVisible(this->idLabel.get());
    this->idLabel->setJustificationType(Justification::centredRight);
    this->idLabel->setFont(Globals::UI::Fonts::M);

    this->separator = make<SeparatorHorizontal>();
    this->addAndMakeVisible(this->separator.get());

    this->selectionComponent = make<TranslationSettingsItemSelection>();
    this->addChildComponent(this->selectionComponent.get());
}

TranslationSettingsItem::~TranslationSettingsItem() = default;

void TranslationSettingsItem::resized()
{
    constexpr auto leftMargin = 38;
    constexpr auto rightMargin = 10;
    constexpr auto idLabelSize = 100;

    this->localeLabel->setBounds(leftMargin, 0, this->getWidth() - leftMargin, this->getHeight() - 3);
    this->idLabel->setBounds(this->getWidth() - (idLabelSize + rightMargin), 0, idLabelSize, this->getHeight() - 3);
    this->separator->setBounds(leftMargin, this->getHeight() - 2, this->getWidth() - leftMargin - 8, 2);

    this->selectionComponent->setBounds(this->getLocalBounds());
}

void TranslationSettingsItem::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected)
    {
        App::Config().getTranslations()->loadLocaleWithId(this->idLabel->getText());
        App::recreateLayout();
    }
}

void TranslationSettingsItem::updateDescription(bool isLastRowInList,
    bool isCurrentLocale, const String &localeName, const String &localeId)
{
    this->separator->setVisible(!isLastRowInList);
    this->localeLabel->setText(localeName, dontSendNotification);
    this->idLabel->setText(localeId, dontSendNotification);

    if (isCurrentLocale)
    {
        this->selectionAnimator.fadeIn(this->selectionComponent.get(), Globals::UI::fadeInShort);
    }
    else
    {
        this->selectionAnimator.fadeOut(this->selectionComponent.get(), Globals::UI::fadeOutShort);
    }
}

Component *TranslationSettingsItem::createHighlighterComponent()
{
    return new TranslationSettingsItemHighlighter();
}
