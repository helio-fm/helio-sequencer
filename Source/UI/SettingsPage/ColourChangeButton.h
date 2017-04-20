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

class ColourChangeButton :
    public TextButton,
    public ChangeListener
{
public:
    ColourChangeButton(const String &name)
        : TextButton(name)
    {
        this->setSize(10, 24);
        this->changeWidthToFitText();
    }

    virtual void clicked() override
    {
        ColourSelector *colourSelector = new ColourSelector();
        colourSelector->setName("background");
        colourSelector->setCurrentColour(this->findColour(TextButton::buttonColourId));
        colourSelector->addChangeListener(this);
        colourSelector->setColour(ColourSelector::backgroundColourId, Colours::transparentBlack);
        colourSelector->setSize(300, 400);

        CallOutBox::launchAsynchronously(colourSelector, getScreenBounds(), nullptr);
    }

    virtual void changeListenerCallback(ChangeBroadcaster *source) override
    {
        if (ColourSelector *cs = dynamic_cast <ColourSelector *>(source))
        {
            this->setColour(TextButton::buttonColourId, cs->getCurrentColour());
        }
    }
};
