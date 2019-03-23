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
#include "ComponentsList.h"

ComponentsList::ComponentsList(int paddingLeft, int paddingRight) :
    paddingLeft(paddingLeft),
    paddingRight(paddingRight)
{
    this->setSize(800, 600);
    this->setPaintingIsUnclipped(true);
}

void ComponentsList::resized()
{
    if (nullptr == this->getParentComponent())
    {
        return;
    }
    
    int h = 0;

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        auto *item = this->getChildComponent(i);
        item->setVisible(item->isEnabled());
        if (item->isEnabled())
        {
            item->setSize(this->getWidth() - this->paddingLeft - this->paddingRight, item->getHeight());
            item->setTopLeftPosition(this->paddingLeft, h);
            h += item->getHeight();
        }
    }

    this->setSize(this->getWidth(), h);
}

void ComponentsList::showChild(Component *child)
{
    auto *container = this->findContainerOf(child);
    if (container != nullptr && !container->isEnabled())
    {
        container->setEnabled(true);
        this->resized();
    }
}

void ComponentsList::hideChild(Component *child)
{
    auto *container = this->findContainerOf(child);
    if (container != nullptr && container->isEnabled())
    {
        container->setEnabled(false);
        this->resized();
    }
}

Component *ComponentsList::findContainerOf(Component *content)
{
    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        auto *container = this->getChildComponent(i);
        if (container == content)
        {
            return container;
        }

        for (int j = 0; j < container->getNumChildComponents(); ++j)
        {
            auto *child = container->getChildComponent(j);
            if (child == content)
            {
                return container;
            }
        }
    }

    return nullptr;
}
