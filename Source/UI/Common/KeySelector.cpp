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
#include "KeySelector.h"
#include "ColourIDs.h"

KeySelector::KeySelector(const Temperament::Period &period)
{
    this->setInterceptsMouseClicks(false, true);

    const auto colour = findDefaultColour(ColourIDs::ColourButton::outline);

    // arrange the enharmonics so that the first one in the list
    // is considered the main one andis placed in the center;
    // if any others are present, sharps are placed in
    // the top row andflats are placed in the bottom row:
    for (int i = 0; i < period.size(); ++i)
    {
        const auto &enharmonics = period.getReference(i);
        jassert(!enharmonics.isEmpty());

        KeyEnharmonics keyButtons;
        keyButtons.mainKey = make<RadioButton>(enharmonics[0], colour, this);
        keyButtons.mainKey->setButtonIndex(i);
        this->addAndMakeVisible(keyButtons.mainKey.get());

        if (enharmonics.size() > 1)
        {
            auto keyButton = make<RadioButton>(enharmonics[1], colour, this);
            keyButton->setButtonIndex(i);
            this->addAndMakeVisible(keyButton.get());

            if (enharmonics[1].endsWithChar('b'))
            {
                this->hasFlatsRow = true;
                keyButtons.flatKey = move(keyButton);
            }
            else
            {
                this->hasSharpsRow = true;
                keyButtons.sharpKey = move(keyButton);
            }
        }

        if (enharmonics.size() > 2)
        {
            auto keyButton = make<RadioButton>(enharmonics[2], colour, this);
            keyButton->setButtonIndex(i);
            this->addAndMakeVisible(keyButton.get());

            if (enharmonics[2].endsWithChar('b'))
            {
                this->hasFlatsRow = true;
                keyButtons.flatKey = move(keyButton);
            }
            else
            {
                this->hasSharpsRow = true;
                keyButtons.sharpKey = move(keyButton);
            }
        }

        this->buttons.add(move(keyButtons));
    }

    this->setSize(KeySelector::buttonWidth * period.size(),
        KeySelector::mainRowHeight +
            (this->hasFlatsRow ? KeySelector::altRowHeight : 0) +
            (this->hasSharpsRow ? KeySelector::altRowHeight : 0));
}

void KeySelector::resized()
{
    int x = 0;
    constexpr auto w = KeySelector::buttonWidth;
    for (const auto &buttonKeys : this->buttons)
    {
        if (buttonKeys.sharpKey != nullptr)
        {
            buttonKeys.sharpKey->setBounds(x, 0, w, KeySelector::altRowHeight);
        }

        buttonKeys.mainKey->setBounds(x,
            this->hasSharpsRow ? KeySelector::altRowHeight : 0,
            w, KeySelector::mainRowHeight);

        if (buttonKeys.flatKey != nullptr)
        {
            buttonKeys.flatKey->setBounds(x,
                this->getHeight() - KeySelector::altRowHeight, w, KeySelector::altRowHeight);
        }

        x += w;
    }
}

void KeySelector::onRadioButtonClicked(const MouseEvent &e, RadioButton *button)
{
    if (e.mods.isRightButtonDown() || e.mods.isAnyModifierKeyDown())
    {
        if (this->onKeyPreview != nullptr)
        {
            this->onKeyPreview(button->getButtonIndex());
        }

        return; // rmb click is note preview
    }

    this->setSelectedKey(button->getButtonIndex(), button->getButtonName());

    if (this->onKeyChanged != nullptr)
    {
        // in the absence of enharmonic equivalents,
        // pass an empty string indicating the default key name
        const auto keyName = (this->hasFlatsRow || this->hasSharpsRow) ?
            button->getButtonName() : String();

        this->onKeyChanged(button->getButtonIndex(), keyName);
    }
}

void KeySelector::setSelectedKey(int key, const String &keyName)
{
    for (auto *child : this->getChildren())
    {
        if (auto *button = dynamic_cast<RadioButton *>(child))
        {
            if (button->getButtonIndex() == key &&
                button->getButtonName() == keyName)
            {
                button->select();
            }
            else
            {
                button->deselect();
            }
        }
    }
}
