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
#include "ScaleEditor.h"

ScaleEditor::~ScaleEditor() = default;

void ScaleEditor::resized()
{
    int x = 0;
    const auto w = this->buttonWidth;
    for (const auto &button : this->buttons)
    {
        button->setBounds(x, 0, w, this->getHeight());
        x += w;
    }
}

void ScaleEditor::onRadioButtonClicked(const MouseEvent &e, RadioButton *clickedButton)
{
    if (clickedButton->getButtonIndex() == 0)
    {
        // no empty scales please, and no scales without the root key
        clickedButton->select();
    }

    if (e.mods.isRightButtonDown() || e.mods.isAnyModifierKeyDown())
    {
        if (this->onScaleNotePreview != nullptr)
        {
            this->onScaleNotePreview(clickedButton->getButtonIndex());
        }

        return; // rmb click is note preview
    }

    jassert(this->scale != nullptr);

    Array<int> keys;
    for (const auto b : this->buttons)
    {
        if (b->isSelected())
        {
            keys.add(b->getButtonIndex());
        }
    }

    this->scale = this->scale->withKeys(keys);
    this->updateButtonsState();

    if (this->onScaleChanged != nullptr)
    {
        this->onScaleChanged(this->scale);
    }
}

void ScaleEditor::setScale(const Scale::Ptr scale)
{
    if (this->scale == nullptr ||
        !this->scale->isEquivalentTo(scale))
    {
        this->scale = scale;
        this->rebuildButtons();
    }
    else
    {
        this->scale = scale;
    }
}

void ScaleEditor::rebuildButtons()
{
    jassert(this->scale != nullptr);

    this->buttons.clearQuick(true);

    for (int i = 0; i < this->scale->getBasePeriod(); ++i)
    {
        auto button = make<RadioButton>(String(i), this->buttonColour, this);
        button->setButtonIndex(i);
        this->addAndMakeVisible(button.get());
        this->buttons.add(button.release());
    }

    this->setSize(this->buttonWidth * this->buttons.size(), ScaleEditor::rowHeight);
    this->resized(); // force repositioning the buttons
    this->updateButtonsState();
}

void ScaleEditor::updateButtonsState()
{
    jassert(this->scale != nullptr);
    for (int i = 0; i < this->buttons.size(); ++i)
    {
        if (this->scale->hasKey(i))
        {
            buttons.getUnchecked(i)->select();
        }
        else
        {
            buttons.getUnchecked(i)->deselect();
        }
    }
}

void ScaleEditor::setButtonWidth(int newWidth)
{
    this->buttonWidth = newWidth;
}
