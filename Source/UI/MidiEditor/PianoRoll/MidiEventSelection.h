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

#include "MidiEventComponent.h"
#include "MidiLayer.h"

class SelectionProxyArray : public Array<MidiEventComponent *>, public ReferenceCountedObject
{
public:
    SelectionProxyArray() {}
    typedef ReferenceCountedObjectPtr<SelectionProxyArray> Ptr;
};


class MidiEventSelection :
    public SelectedItemSet<MidiEventComponent *>
{
public:

    MidiEventSelection() : SelectedItemSet() {}
    
    explicit MidiEventSelection(const ItemArray &items) : SelectedItemSet (items)
    {
        this->invalidateCache();
    }
    
    void itemSelected(MidiEventComponent *item) override
    {
        this->invalidateCache();
        item->setSelected(true);
    }

    void itemDeselected(MidiEventComponent *item) override
    {
        this->invalidateCache();
        item->setSelected(false);
    }

    void needsToCalculateSelectionBounds()
    {
        this->bounds = Rectangle<int>();

        for (int i = 0; i < this->getNumSelected(); ++i)
        {
            this->bounds = this->bounds.getUnion(this->getSelectedItem(i)->getBounds());
        }
    }

    Rectangle<int> getSelectionBounds() const
    {
        return this->bounds;
    }
    
    
    typedef HashMap< String, SelectionProxyArray::Ptr > MultiLayerMap;

    void invalidateCache()
    {
        this->selectionsCache.clear();
    }

    const MultiLayerMap &getMultiLayerSelections() const
    {
        if (this->selectionsCache.size() == 0)
        {
            this->rebuildCache();
        }
        
        return this->selectionsCache;
    }

    bool shouldDisplayGhostNotes() const
    {
        return (this->getNumSelected() <= 32); // just a sane limit
    }

private:

    Rectangle<int> bounds;
    
    mutable MultiLayerMap selectionsCache;

    void rebuildCache() const
    {
        //Logger::writeToLog("rebuildCache");

        for (int i = 0; i < this->getNumSelected(); ++i)
        {
            MidiEventComponent *item = this->getSelectedItem(i);
            const String &layerId(item->getEvent().getLayer()->getLayerIdAsString());
            
            SelectionProxyArray::Ptr targetArray;
            
            if (this->selectionsCache.contains(layerId))
            {
                targetArray = this->selectionsCache[layerId];
            }
            else
            {
                targetArray = new SelectionProxyArray();
            }
            
            targetArray->add(item);
            this->selectionsCache.set(layerId, targetArray);
        }
    }

};
