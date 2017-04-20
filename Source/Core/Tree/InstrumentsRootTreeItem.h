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

class Instrument;
class InstrumentsPage;
class InstrumentTreeItem;

#include "TreeItem.h"

// todo fix pluginmanager dependency
class InstrumentsRootTreeItem :
    public TreeItem
{
public:

    InstrumentsRootTreeItem();
    ~InstrumentsRootTreeItem() override;

    Colour getColour() const override;
    Image getIcon() const override;
    String getCaption() const override;

    void showPage() override;
    void recreatePage() override;

    
    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    var getDragSourceDescription() override
    {
        return var::null;
    }

    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;

    void itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex) override;


    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    Component *createItemMenu() override;


    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;

private:
    
    friend class InstrumentsPage;

    friend class InstrumentsCommandPanel;
    
    InstrumentTreeItem *addInstrumentTreeItem(Instrument *instrument, int insertIndex = -1);

    ScopedPointer<InstrumentsPage> instrumentsPage;

};
