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
#include "ScaleEditor.h"
#include "KeySelector.h"
#include "ColourIDs.h"

ScaleEditor::ScaleEditor() : scale(Scale::getNaturalMajorScale())
{
    const Colour base(findDefaultColour(ColourIDs::ColourButton::outline));

    for (int i = 0; i < 12; ++i)
    {
        ScopedPointer<RadioButton> button(new RadioButton(String(i), base, this));
        button->setButtonIndex(i);
        this->addAndMakeVisible(button);
        this->buttons.add(button.release());
    }

    this->updateButtons();
}

ScaleEditor::~ScaleEditor()
{
}

void ScaleEditor::resized()
{
    int x = 0;
    for (const auto &button : this->buttons)
    {
        const int w = this->getWidth() / this->buttons.size();
        button->setBounds(x, 0, w, this->getHeight());
        x += w;
    }
}

void ScaleEditor::onRadioButtonClicked(RadioButton *clickedButton)
{
    if (clickedButton->getButtonIndex() == 0)
    {
        // Root key cannot be missed
        clickedButton->select();
    }

    Array<int> keys;
    for (const auto b : this->buttons)
    {
        if (b->isSelected())
        {
            keys.add(b->getButtonIndex());
        }
    }

    this->scale = this->scale->withKeys(keys);
    this->updateButtons();

    if (auto *parentListener = dynamic_cast<ScaleEditor::Listener *>(this->getParentComponent()))
    {
        parentListener->onScaleChanged(this->scale);
    }
}

void ScaleEditor::setScale(const Scale::Ptr scale)
{
    if (!this->scale->isEquivalentTo(scale))
    {
        this->scale = scale;
        this->updateButtons();
    }
    else
    {
        this->scale = scale;
    }
}

void ScaleEditor::updateButtons()
{
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
