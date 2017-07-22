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

//#define PROJECT_HAS_MAP_RENDERER 1
#define PROJECT_HAS_MAP_RENDERER 0

class Autosaver;
class Document;
class Project;
class ProjectListener;
class SequencerLayout;
class HybridRoll;
class MidiEvent;
class TrackMapRenderer;
class ProjectPage;
class Origami;
class TrackMap;
class Transport;
class ProjectInfo;
class ProjectTimeline;
class HybridRollCommandPanel;
class UndoStack;
class RecentFilesList;
class Pattern;

#include "TreeItem.h"
#include "DocumentOwner.h"
#include "Transport.h"
#include "TrackedItemsSource.h"
#include "ProjectSequencesWrapper.h"
#include "HybridRollEditMode.h"
#include "MidiLayer.h"

// todo depends on AudioCore
class ProjectTreeItem :
    public TreeItem,
    public DocumentOwner,
    public VCS::TrackedItemsSource,  // vcs stuff
    public ChangeListener // subscribed to VersionControl
{
public:

    explicit ProjectTreeItem(const String &name);
    explicit ProjectTreeItem(const File &existingFile);
    ~ProjectTreeItem() override;
    
    void deletePermanently();
    
    String getId() const;
    String getStats() const;

    Transport &getTransport() const noexcept;
	ProjectInfo *getProjectInfo() const noexcept;
	ProjectTimeline *getTimeline() const noexcept;
	HybridRollEditMode getEditMode() const noexcept;
	HybridRoll *getLastFocusedRoll() const;

	void repaintEditor();
    
    void importMidi(File &file);
    void exportMidi(File &file) const;

	Colour getColour() const override;
    Image getIcon() const override;

    void showPage() override;
    void recreatePage() override;
    void savePageState() const;
    void loadPageState();

    void onRename(const String &newName) override;

    void showEditor(MidiLayer *activeLayer, TreeItem *source);
    void showEditorsGroup(Array<MidiLayer *> layersGroup, TreeItem *source);
    void hideEditor(MidiLayer *activeLayer, TreeItem *source);

    void updateActiveGroupEditors();
	void activateLayer(MidiLayer* layer, bool selectOthers, bool deselectOthers);


    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    Component *createItemMenu() override;


    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    void onItemMoved() override;

    var getDragSourceDescription() override
    { return var::null; }

    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;

    //virtual void itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex) override
    //{ }


    //===------------------------------------------------------------------===//
    // Undos
    //===------------------------------------------------------------------===//

    UndoStack *getUndoStack() const noexcept;
    void checkpoint();
    void undo();
    void redo();
    void clearUndoHistory();

	Pattern *getPatternWithId(const String &uuid);

    template<typename T>
    T *getLayerWithId(const String &uuid)
    {
        this->rebuildLayersHashIfNeeded();
        return dynamic_cast<T *>(this->layersHash[uuid].get());
    }

    template<typename T>
    T *findChildByLayerId(const String &uuid) const
    {
        Array<T *> allChildren = this->findChildrenOfType<T>();
        
        for (int i = 0; i < allChildren.size(); ++i)
        {
            if (allChildren.getUnchecked(i)->getLayer()->getLayerId().toString() == uuid)
            {
                return allChildren.getUnchecked(i);
            }
        }
        
        return nullptr;
    }


    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    Array<MidiLayer *> getLayersList() const;

    Array<MidiLayer *> getSelectedLayersList() const;

    Point<float> getTrackRangeInBeats() const;


    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;

    void reset() override;


    //===------------------------------------------------------------------===//
    // Project listeners
    //===------------------------------------------------------------------===//

    void addListener(ProjectListener *listener);

    void removeListener(ProjectListener *listener);

    void removeAllListeners();


    //===------------------------------------------------------------------===//
    // Broadcaster
    //===------------------------------------------------------------------===//

    void broadcastAddEvent(const MidiEvent &event);
    void broadcastChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent);
    void broadcastRemoveEvent(const MidiEvent &event);
	void broadcastPostRemoveEvent(MidiLayer *const layer);

    void broadcastAddTrack(MidiLayer *const layer, Pattern *const pattern = nullptr);
    void broadcastChangeTrack(MidiLayer *const layer, Pattern *const pattern = nullptr);
    void broadcastRemoveTrack(MidiLayer *const layer, Pattern *const pattern = nullptr);

    void broadcastAddClip(const Clip &clip);
    void broadcastChangeClip(const Clip &oldClip, const Clip &newClip);
    void broadcastRemoveClip(const Clip &clip);
    void broadcastPostRemoveClip(Pattern *const pattern);

    void broadcastChangeProjectInfo(const ProjectInfo *info);
    void broadcastChangeProjectBeatRange();


    //===------------------------------------------------------------------===//
    // VCS::TrackedItemsSource
    //===------------------------------------------------------------------===//

    String getVCSName() const override
    {
        return this->getName();
    }

    int getNumTrackedItems() override;

    VCS::TrackedItem *getTrackedItem(int index) override;

    VCS::TrackedItem *initTrackedItem(const String &type, const Uuid &id) override;

    bool deleteTrackedItem(VCS::TrackedItem *item) override;


    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

protected:

    //===------------------------------------------------------------------===//
    // DocumentOwner
    //===------------------------------------------------------------------===//

    bool onDocumentLoad(File &file) override;

    void onDocumentDidLoad(File &file) override;

    bool onDocumentSave(File &file) override;

    void onDocumentImport(File &file) override;

    bool onDocumentExport(File &file) override;

private:

    void collectLayers(Array<MidiLayer *> &resultArray, bool onlySelectedLayers = false) const;

    ScopedPointer<Autosaver> autosaver;
    ScopedPointer<Transport> transport;
    WeakReference<RecentFilesList> recentFilesList;

#if PROJECT_HAS_MAP_RENDERER
    ScopedPointer<TrackMapRenderer> renderer;

    ScopedPointer<Component> trackMap;
#endif

    ScopedPointer<SequencerLayout> editor;

    HybridRollEditMode rollEditMode;

    ListenerList<ProjectListener> changeListeners;

    ScopedPointer<ProjectPage> projectSettings;

    ReadWriteLock layersListLock;

    ScopedPointer<ProjectInfo> info;

    ScopedPointer<ProjectTimeline> timeline;

private:

    void initialize();
    XmlElement *save() const;
    void load(const XmlElement &xml);

private:

    void registerVcsItem(const MidiLayer *layer);
    void registerVcsItem(const Pattern *pattern);

    void unregisterVcsItem(const MidiLayer *layer);
    void unregisterVcsItem(const Pattern *pattern);

    ReadWriteLock vcsInfoLock;
    Array<const VCS::TrackedItem *> vcsItems;

private:

    ScopedPointer<UndoStack> undoStack;

    bool isLayersHashOutdated;
    HashMap<String, WeakReference<MidiLayer> > layersHash;

    void rebuildLayersHashIfNeeded();

};
