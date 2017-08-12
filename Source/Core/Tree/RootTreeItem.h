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

#include "TreeItem.h"

class ProjectTreeItem;
class VersionControlTreeItem;
class TrackGroupTreeItem;
class MidiTrackTreeItem;
class ScriptTreeItem;
class WorkspacePage;

class RootTreeItem :
    public TreeItem
{
public:

    explicit RootTreeItem(const String &name);

    ~RootTreeItem() override;

    Colour getColour() const override;
    Image getIcon() const override;

    bool mightContainSubItems() override
    { return false; } // hide open/close button
    
    void showPage() override;
    void recreatePage() override;

    void onRename(const String &newName) override;

    void importMidi(File &file);


    //===------------------------------------------------------------------===//
    // Children
    //===------------------------------------------------------------------===//

    void checkoutProject(const String &name, const String &id, const String &key);

    ProjectTreeItem *openProject(const File &file, int insertIndex = -1);

    ProjectTreeItem *addDefaultProject(const File &projectLocation);
    ProjectTreeItem *addDefaultProject(const String &projectName);
    ProjectTreeItem *createDefaultProjectChildren(ProjectTreeItem *newProject);

    VersionControlTreeItem *addVCS(TreeItem *parent);
    TrackGroupTreeItem *addGroup(TreeItem *parent, const String &name);
    MidiTrackTreeItem *addPianoLayer(TreeItem *parent, const String &name);
    MidiTrackTreeItem *addAutoLayer(TreeItem *parent, const String &name, int controllerNumber);
    ScriptTreeItem *addScript(TreeItem *parent, const String &path);


    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    var getDragSourceDescription() override
    {
        return var::null;
    }

    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;

    //virtual void itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex) override
    //{ }

    bool isInterestedInFileDrag(const StringArray &files) override;

    void filesDropped(const StringArray &files, int insertIndex) override;


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

    ScopedPointer<WorkspacePage> introPage;

};
