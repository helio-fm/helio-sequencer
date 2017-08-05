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
#include "ProjectEventDispatcher.h"

#include "TrackedItem.h"
#include "Delta.h"

class Pattern;
class MidiLayer;
class ProjectTreeItem;
class InstrumentDescription;

class MidiLayerTreeItem :
    public TreeItem,
    public ProjectEventDispatcher,
    public VCS::TrackedItem
{
public:

    explicit MidiLayerTreeItem(const String &name);

    ~MidiLayerTreeItem() override;

	String getXPath() const;
	void setXPath(const String &path);

    bool isMuted() const;
    Colour getColour() const override;

    void showPage() override;
    void onRename(const String &newName) override;

    inline MidiLayer *getLayer() const noexcept 
    { return this->layer; }

    inline Pattern *getPattern() const noexcept 
    { return this->pattern; }

    void importMidi(const MidiMessageSequence &sequence);

    //===------------------------------------------------------------------===//
    // VCS::TrackedItem
    //===------------------------------------------------------------------===//

    String getVCSName() const override;
    XmlElement *serializeClipsDelta() const;
    void resetClipsDelta(const XmlElement *state);

    //===------------------------------------------------------------------===//
    // ProjectEventDispatcher
    //===------------------------------------------------------------------===//

    void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void dispatchAddEvent(const MidiEvent &event) override;
    void dispatchRemoveEvent(const MidiEvent &event) override;
	void dispatchPostRemoveEvent(MidiLayer *const layer) override;

    void dispatchReloadLayer(MidiLayer *const layer) override;
    void dispatchChangeLayerBeatRange() override;
    
	void dispatchAddClip(const Clip &clip) override;
	void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) override;
	void dispatchRemoveClip(const Clip &clip) override;
	void dispatchPostRemoveClip(Pattern *const pattern) override;

	void dispatchReloadPattern(Pattern *const pattern) override;
	void dispatchChangePatternBeatRange() override;

    ProjectTreeItem *getProject() const override;

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    void onItemMoved() override;
    var getDragSourceDescription() override;
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex) override;

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    Component *createItemMenu() override;


protected:

    ProjectTreeItem *lastFoundParent;

    ScopedPointer<MidiLayer> layer;

    ScopedPointer<Pattern> pattern;
    
private:

    String getNameForRenamingCallback() const override
    {
        return this->getXPath(); // allows edit full layer path
    }
};
