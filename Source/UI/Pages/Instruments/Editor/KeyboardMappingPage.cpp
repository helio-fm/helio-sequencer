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
#include "KeyboardMappingPage.h"
#include "KeyboardMapping.h"
#include "HeadlineContextMenuController.h"
#include "PanelBackgroundB.h"
#include "IconButton.h"
#include "FramePanel.h"

KeyboardMappingPage::KeyboardMappingPage(WeakReference<Instrument> instrument) :
    instrument(instrument)
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

    this->background = make<PanelBackgroundB>();
    this->addAndMakeVisible(this->background.get());

    this->rangeLabel = make<Label>();
    this->rangeLabel->setFont({ 28.f });
    this->rangeLabel->setJustificationType(Justification::centred);
    this->addAndMakeVisible(this->rangeLabel.get());

    this->leftArrow = make<IconButton>(Icons::stretchLeft, CommandIDs::ShowPreviousPage);
    this->addAndMakeVisible(this->leftArrow.get());

    this->rightArrow = make<IconButton>(Icons::stretchRight, CommandIDs::ShowNextPage);
    this->addAndMakeVisible(this->rightArrow.get());

    for (int i = 0; i < Globals::twelveToneKeyboardSize; ++i)
    {
        // todo key preview buttons

        auto keyButton = make<Label>();
        keyButton->setFont({ 18.f });
        keyButton->setJustificationType(Justification::centredRight);
        this->addAndMakeVisible(keyButton.get());
        this->keyButtons.add(keyButton.release());

        auto mappingLabel = make<Label>();
        mappingLabel->setFont({ 18.f });
        mappingLabel->setEditable(true);
        mappingLabel->setJustificationType(Justification::centredLeft);
        mappingLabel->onTextChange = [this, i]()
        {
            this->onKeyMappingUpdated(i);
        };

        this->addAndMakeVisible(mappingLabel.get());
        this->mappingLabels.add(mappingLabel.release());
    }

    this->syncWithRange(0);

    this->instrument->getKeyboardMapping()->addChangeListener(this);
}

KeyboardMappingPage::~KeyboardMappingPage()
{
    if (this->instrument != nullptr)
    {
        this->instrument->getKeyboardMapping()->removeChangeListener(this);
    }
}

void KeyboardMappingPage::resized()
{
    this->background->setBounds(this->getLocalBounds());

    const auto centre = this->getLocalBounds().getCentre();

    auto rangeControlsBounds =
        Rectangle<int>(0, 0, 256, 40).withCentre(centre.withY(96));

    const auto mappingsBounds =
        this->getLocalBounds().reduced(96, 48).withTrimmedTop(128);

    this->leftArrow->setBounds(rangeControlsBounds.removeFromLeft(64));
    this->rightArrow->setBounds(rangeControlsBounds.removeFromRight(64));
    this->rangeLabel->setBounds(rangeControlsBounds);
    
    static constexpr auto numRows = 16;
    static constexpr auto numColumns = Globals::twelveToneKeyboardSize / numRows;
    static constexpr auto editorHeight = 22;

    for (int i = 0; i < Globals::twelveToneKeyboardSize; ++i)
    {
        const int row = i % numRows;
        const int column = i / numRows;

        const auto rowWidth = mappingsBounds.getWidth() / numColumns;
        const auto rowHeight = mappingsBounds.getHeight() / numRows;

        auto rowBounds =
            Rectangle<int>(mappingsBounds.getX() + rowWidth * column,
                mappingsBounds.getY() + rowHeight * row, rowWidth, rowHeight);

        this->keyButtons.getUnchecked(i)->setBounds(rowBounds.removeFromLeft(48));
        this->mappingLabels.getUnchecked(i)->setBounds(rowBounds
            .withHeight(editorHeight).withCentre(rowBounds.getCentre()));
    }
}

void KeyboardMappingPage::syncWithRange(int base)
{
    this->currentPageBase = base;

    const auto *keyMap = this->instrument->getKeyboardMapping();

    this->rangeLabel->setText(String(base) + " - " + 
        String(base + Globals::twelveToneKeyboardSize - 1), dontSendNotification);

    const auto canShowPreiousPave = base > 0;
    const auto canShowNextPage = base +
        Globals::twelveToneKeyboardSize < KeyboardMapping::maxMappedKeys;

    this->leftArrow->setInterceptsMouseClicks(canShowPreiousPave, false);
    this->leftArrow->setAlpha(canShowPreiousPave ? 1.f : 0.25f);

    this->rightArrow->setInterceptsMouseClicks(canShowNextPage, false);
    this->rightArrow->setAlpha(canShowNextPage ? 1.f : 0.25f);

    for (int i = base; i < base + Globals::twelveToneKeyboardSize; ++i)
    {
        const auto mapped = keyMap->map(i);
        const int buttonIndex = i % Globals::twelveToneKeyboardSize;

        this->keyButtons.getUnchecked(buttonIndex)->
            setText(String(i) + ":", dontSendNotification);

        this->mappingLabels.getUnchecked(buttonIndex)->
            setText(mapped.toString(), dontSendNotification);
    }
}

void KeyboardMappingPage::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::ShowPreviousPage:
        this->syncWithRange(this->currentPageBase - Globals::twelveToneKeyboardSize);
        break;
    case CommandIDs::ShowNextPage:
        this->syncWithRange(this->currentPageBase + Globals::twelveToneKeyboardSize);
        break;
    default:
        break;
    }
}

void KeyboardMappingPage::mouseDown(const MouseEvent &e)
{
    if (e.mods.isRightButtonDown())
    {
        this->contextMenuController->showMenu(e);
    }
}

void KeyboardMappingPage::changeListenerCallback(ChangeBroadcaster *source)
{
    this->syncWithRange(this->currentPageBase);
}

void KeyboardMappingPage::onKeyMappingUpdated(int i)
{
    const int key = this->currentPageBase + i;
    auto *editor = this->mappingLabels.getUnchecked(i);
    auto *keyMap = this->instrument->getKeyboardMapping();
    
    const auto mapped = KeyboardMapping::KeyChannel::fromString(editor->getText());

    if (mapped.isValid())
    {
        keyMap->updateKey(key, mapped);
    }
    else
    {
        editor->setText(keyMap->map(key).toString(), dontSendNotification);
    }
}
