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
#include "OverlayButton.h"
#include "PanelBackgroundB.h"
#include "ModalDialogInput.h"
#include "DocumentHelpers.h"
#include "IconButton.h"
#include "ComponentIDs.h"
#include "Config.h"

KeyboardMappingPage::KeyboardMappingPage(WeakReference<Instrument> instrument) :
    instrument(instrument)
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->setComponentID(ComponentIDs::keyboardMapping);

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

    this->background = make<PanelBackgroundB>();
    this->addAndMakeVisible(this->background.get());

    this->rangeLabel = make<Label>();
    this->rangeLabel->setFont(Globals::UI::Fonts::XL);
    this->rangeLabel->setJustificationType(Justification::centred);
    this->addAndMakeVisible(this->rangeLabel.get());

    this->leftArrow = make<IconButton>(Icons::stretchLeft, CommandIDs::KeyMapPreviousPage);
    this->addAndMakeVisible(this->leftArrow.get());

    this->rightArrow = make<IconButton>(Icons::stretchRight, CommandIDs::KeyMapNextPage);
    this->addAndMakeVisible(this->rightArrow.get());

    for (int i = 0; i < Globals::twelveToneKeyboardSize; ++i)
    {
        auto keyLabel = make<Label>();
        keyLabel->setFont(Globals::UI::Fonts::M);
        keyLabel->setJustificationType(Justification::centredRight);
        keyLabel->setInterceptsMouseClicks(false, false);
        this->addAndMakeVisible(keyLabel.get());
        this->keyLabels.add(keyLabel.release());

        auto keyButton = make<OverlayButton>();
        keyButton->onStateChange = [this, i]()
        {
            // when released, just stops all sound
            this->stopAllSound();

            if (this->keyButtons[i]->getState() == Button::buttonDown)
            {
                this->onKeyPreview(i);
            }
        };

        this->addAndMakeVisible(keyButton.get());
        this->keyButtons.add(keyButton.release());

        auto mappingLabel = make<Label>();
        mappingLabel->setFont(Globals::UI::Fonts::M);
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

        const auto keyBounds = rowBounds.removeFromLeft(48);
        this->keyLabels.getUnchecked(i)->setBounds(keyBounds);
        this->keyButtons.getUnchecked(i)->setBounds(keyBounds);

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

    const auto canShowPreviousPave = this->canShowPreviousPage();
    const auto canShowNextPage = this->canShowNextPage();

    this->leftArrow->setInterceptsMouseClicks(canShowPreviousPave, false);
    this->leftArrow->setIconAlphaMultiplier(canShowPreviousPave ? 1.f : 0.25f);

    this->rightArrow->setInterceptsMouseClicks(canShowNextPage, false);
    this->rightArrow->setIconAlphaMultiplier(canShowNextPage ? 1.f : 0.25f);

    for (int i = base; i < base + Globals::twelveToneKeyboardSize; ++i)
    {
        const auto mapped = keyMap->map(i);
        const int buttonIndex = i % Globals::twelveToneKeyboardSize;

        this->keyLabels.getUnchecked(buttonIndex)->
            setText(String(i) + ":", dontSendNotification);

        this->mappingLabels.getUnchecked(buttonIndex)->
            setText(mapped.toString(), dontSendNotification);
    }
}

void KeyboardMappingPage::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::KeyMapPreviousPage:
        if (this->canShowPreviousPage())
        {
            this->syncWithRange(this->currentPageBase - Globals::twelveToneKeyboardSize);
        }
        return;
    case CommandIDs::KeyMapNextPage:
        if (this->canShowNextPage())
        {
            this->syncWithRange(this->currentPageBase + Globals::twelveToneKeyboardSize);
        }
        return;
    case CommandIDs::KeyMapLoadScala:
        this->loadScalaMappings();
        return;
    case CommandIDs::KeyMapReset:
        this->instrument->getKeyboardMapping()->reset();
        return;
    case CommandIDs::KeyMapCopyToClipboard:
        SystemClipboard::copyTextToClipboard(
            this->instrument->getKeyboardMapping()->toString());
        return;
    case CommandIDs::KeyMapPasteFromClipboard:
        this->instrument->getKeyboardMapping()->
            loadMapFromString(SystemClipboard::getTextFromClipboard());
        return;
    case CommandIDs::SavePreset:
        this->savePreset();
        return;
    default:
        break;
    }

    const auto allMappings = App::Config().getKeyboardMappings()->getAll();
    if (commandId >= CommandIDs::SelectPreset &&
        commandId <= (CommandIDs::SelectPreset + allMappings.size()))
    {
        const auto presetIndex = commandId - CommandIDs::SelectPreset;
        const auto mapping = allMappings.getUnchecked(presetIndex);
        this->instrument->getKeyboardMapping()->loadMapFromPreset(mapping.get());
    }
}

void KeyboardMappingPage::savePreset() const
{
    auto dialog = ModalDialogInput::Presets::savePreset();
    dialog->onOk = [this](const String &mapName)
    {
        if (mapName.isEmpty())
        {
            return;
        }

        KeyboardMapping::Ptr map(new KeyboardMapping());
        map->loadMapFromPreset(this->instrument->getKeyboardMapping());
        map->setName(mapName);

        App::Config().getKeyboardMappings()->updateUserResource(map);
    };

    App::showModalComponent(move(dialog));
}

void KeyboardMappingPage::visibilityChanged()
{
    if (!this->isVisible())
    {
        this->stopAllSound();
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

void KeyboardMappingPage::stopAllSound()
{
    auto &midiCollector = this->instrument->getProcessorPlayer().getMidiMessageCollector();
    for (int i = 1; i < Globals::numChannels; ++i)
    {
        auto notesOff = MidiMessage::allNotesOff(i);
        notesOff.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
        midiCollector.addMessageToQueue(notesOff);
    }
}

void KeyboardMappingPage::onKeyPreview(int i)
{
    const int key = this->currentPageBase + i;
    auto *keyMap = this->instrument->getKeyboardMapping();
    const auto mapped = keyMap->map(key);

    const auto time = Time::getMillisecondCounterHiRes() * 0.001;
    auto &midiCollector = this->instrument->getProcessorPlayer().getMidiMessageCollector();

    auto message = MidiMessage::noteOn(mapped.channel, mapped.key, 0.5f);
    message.setTimeStamp(time + 0.01);
    midiCollector.addMessageToQueue(message);
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

bool KeyboardMappingPage::canShowPreviousPage() const noexcept
{
    return this->currentPageBase > 0;
}

bool KeyboardMappingPage::canShowNextPage() const noexcept
{
    return this->currentPageBase +
        Globals::twelveToneKeyboardSize < KeyboardMapping::numMappedKeys;
}

void KeyboardMappingPage::loadScalaMappings()
{
#if JUCE_ANDROID
    const auto filter = "*/*";
#else
    const auto filter = "*.kbm";
#endif

    this->importFileChooser = make<FileChooser>(TRANS(I18n::Dialog::workspaceCreateProjectCaption),
        File::getSpecialLocation(File::userDocumentsDirectory), filter, true);
    
    DocumentHelpers::showFileChooser(this->importFileChooser,
        Globals::UI::FileChooser::forFileToOpen,
        [this](URL &url)
    {
        if (!url.isLocalFile())
        {
            return;
        }

        const auto file = url.getLocalFile();
        const auto nameWithoutExtension = file.getFileNameWithoutExtension();
        
        Array<File> allFilesToImport;

        StringArray nameComponents;
        nameComponents.addTokens(nameWithoutExtension, "_", "");
        if (nameComponents.size() > 1 &&
            nameComponents[nameComponents.size() - 1].getIntValue() > 0)
        {
            const auto nameTemplate = 
                nameComponents.joinIntoString("_", 0, nameComponents.size() - 1);

            // search for "same_name_*.kbm" files for other channels' mappings
            allFilesToImport = file.getParentDirectory()
                .findChildFiles(File::TypesOfFileToFind::findFiles, false,
                    nameTemplate + "_*.kbm");
        }
        else
        {
            // import a single file
            allFilesToImport.add(file);
        }

        auto *keyMap = this->instrument->getKeyboardMapping();
        keyMap->reset();

        for (const auto &i : allFilesToImport)
        {
            const auto readStream = i.createInputStream();
            keyMap->loadScalaKbmFile(*readStream.get(),
                i.getFileNameWithoutExtension());
        }
    });
}
