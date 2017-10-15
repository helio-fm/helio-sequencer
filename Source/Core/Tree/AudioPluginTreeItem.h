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

class AudioPlugin;
class AudioPluginEditor;

#include "TreeItem.h"

class AudioPluginTreeItem : public TreeItem
{
public:

    AudioPluginTreeItem(uint32 pluginID, const String &name);
    ~AudioPluginTreeItem() override;

    Colour getColour() const override;
    Image getIcon() const override;
    uint32 getNodeId() const noexcept;

    ScopedPointer<Component> createItemMenu() override;
    void showPage() override;


    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    var getDragSourceDescription() override;
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex) override;


    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;

private:

    ScopedPointer<Component> audioPluginEditor;
    const uint32 filterID;

};
