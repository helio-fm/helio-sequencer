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
#include "Origami.h"


Origami::Origami()
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);
    this->setSize(256, 256); // not 0
}

Origami::~Origami()
{
}

//===----------------------------------------------------------------------===//
// Origami
//===----------------------------------------------------------------------===//

void Origami::clear()
{
    this->pages.clear();
    this->removeAllChildren();
}

bool Origami::removePageContaining(Component *component)
{
    for (int i = 0; i < this->pages.size(); ++i)
    {
        if (component == this->pages[i]->component)
        {
            this->pages.remove(i);
            this->resized();
            return true;
        }
    }
    
    return false;
}

bool Origami::containsComponent(Component *component) const
{
    for (auto page : this->pages)
    {
        if (component == page->component)
        {
            return true;
        }
    }

    return false;
}

int Origami::getMinimumCommonSize() const
{
    int minSize = 0;
    
    for (auto page : this->pages)
    {
        minSize += page->min;
    }
    
    return minSize;
}

int Origami::getMaximumCommonSize() const
{
    int maxSize = 0;
    
    for (auto page : this->pages)
    {
        maxSize += page->max;
    }
    
    return maxSize;
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void Origami::ChildConstrainer::applyBoundsToComponent(Component &component, Rectangle<int> bounds)
{
    ComponentBoundsConstrainer::applyBoundsToComponent(component, bounds);
    this->origami.onPageResized(&component);
}
