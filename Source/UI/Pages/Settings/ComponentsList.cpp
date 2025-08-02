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
#include "ComponentsList.h"

ComponentsList::ComponentsList(int paddingLeft,
    int paddingRight, int paddingTop, int paddingBottom) :
    paddingLeft(paddingLeft),
    paddingRight(paddingRight),
    paddingTop(paddingTop),
    paddingBottom(paddingBottom)
{
    this->setAccessible(false);
    this->setPaintingIsUnclipped(true);
}

void ComponentsList::resized()
{
    if (this->getParentComponent() == nullptr)
    {
        return;
    }
    
    int y = this->paddingTop;

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        auto *item = this->getChildComponent(i);
        item->setVisible(item->isEnabled());
        if (item->isEnabled())
        {
            item->setSize(this->getWidth() - this->paddingLeft - this->paddingRight, item->getHeight());
            item->setTopLeftPosition(this->paddingLeft, y);
            y += item->getHeight();
        }
    }

    this->setSize(this->getWidth(), y + this->paddingBottom);
}
