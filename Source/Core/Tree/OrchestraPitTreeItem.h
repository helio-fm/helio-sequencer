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
class InstrumentTreeItem;
class OrchestraPitPage;

#include "TreeItem.h"

class OrchestraPitTreeItem final : public TreeItem
{
public:

    OrchestraPitTreeItem();

    String getName() const noexcept override;
    Colour getColour() const noexcept override;
    Image getIcon() const noexcept override;

    void showPage() override;
    void recreatePage() override;

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    var getDragSourceDescription() override { return {}; }
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex) override;

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    ScopedPointer<Component> createMenu() override;

private:
    
    friend class OrchestraPitPage;
    friend class OrchestraPitMenu;
    friend class AudioPluginSelectionMenu;
    
    InstrumentTreeItem *addInstrumentTreeItem(Instrument *instrument, int insertIndex = -1);
    ScopedPointer<OrchestraPitPage> instrumentsPage;

};
