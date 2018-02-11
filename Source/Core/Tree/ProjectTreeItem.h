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
class ToolsSidebar;
class UndoStack;
class RecentFilesList;
class Pattern;

#include "TreeItem.h"
#include "DocumentOwner.h"
#include "Transport.h"
#include "TrackedItemsSource.h"
#include "ProjectSequencesWrapper.h"
#include "HybridRollEditMode.h"
#include "MidiSequence.h"
#include "MidiTrackSource.h"

class ProjectTreeItem :
    public TreeItem,
    public DocumentOwner,
    public MidiTrackSource,
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
    HybridRollEditMode &getEditMode() noexcept;
    HybridRoll *getLastFocusedRoll() const;
    
    void importMidi(File &file);
    void exportMidi(File &file) const;

    Colour getColour() const override;
    Image getIcon() const override;

    void showPage() override;
    void recreatePage() override;
    void savePageState() const;
    void loadPageState();

    void safeRename(const String &newName) override;

    void showPatternEditor(TreeItem *source);
    void showLinearEditor(MidiSequence *activeLayer, TreeItem *source);
    void hideEditor(MidiSequence *activeLayer, TreeItem *source);
    WeakReference<TreeItem> getLastShownTrack() const;

    void updateActiveGroupEditors();
    void activateLayer(MidiSequence* layer, bool selectOthers, bool deselectOthers);

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    ScopedPointer<Component> createItemMenu() override;

    //===------------------------------------------------------------------===//
    // Dragging
    //===------------------------------------------------------------------===//

    var getDragSourceDescription() override { return var::null; }
    bool isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;

    //===------------------------------------------------------------------===//
    // Undos
    //===------------------------------------------------------------------===//

    UndoStack *getUndoStack() const noexcept;
    void checkpoint();
    void undo();
    void redo();
    void clearUndoHistory();

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    Array<MidiTrack *> getTracks() const;
    Array<MidiTrack *> getSelectedTracks() const;
    Point<float> getProjectRangeInBeats() const;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
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
    void broadcastPostRemoveEvent(MidiSequence *const layer);

    void broadcastAddTrack(MidiTrack *const track);
    void broadcastRemoveTrack(MidiTrack *const track);
    void broadcastChangeTrackProperties(MidiTrack *const track);

    void broadcastAddClip(const Clip &clip);
    void broadcastChangeClip(const Clip &oldClip, const Clip &newClip);
    void broadcastRemoveClip(const Clip &clip);
    void broadcastPostRemoveClip(Pattern *const pattern);

    void broadcastChangeProjectInfo(const ProjectInfo *info);
    void broadcastChangeViewBeatRange(float firstBeat, float lastBeat);
    void broadcastReloadProjectContent();
    Point<float> broadcastChangeProjectBeatRange();

    //===------------------------------------------------------------------===//
    // VCS::TrackedItemsSource
    //===------------------------------------------------------------------===//

    String getVCSName() const override;
    int getNumTrackedItems() override;
    VCS::TrackedItem *getTrackedItem(int index) override;
    VCS::TrackedItem *initTrackedItem(const Identifier &type, const Uuid &id) override;
    bool deleteTrackedItem(VCS::TrackedItem *item) override;
    void onResetState() override;

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

    //===------------------------------------------------------------------===//
    // MidiTrackSource
    //===------------------------------------------------------------------===//

    MidiTrack *getTrackById(const String &trackId) override;
    Pattern *getPatternByTrackId(const String &trackId) override;
    MidiSequence *getSequenceByTrackId(const String &trackId) override;

private:

    void collectTracks(Array<MidiTrack *> &resultArray, bool onlySelected = false) const;

    ScopedPointer<Autosaver> autosaver;
    ScopedPointer<Transport> transport;
    WeakReference<RecentFilesList> recentFilesList;

#if PROJECT_HAS_MAP_RENDERER
    ScopedPointer<TrackMapRenderer> renderer;

    ScopedPointer<Component> trackMap;
#endif

    ScopedPointer<SequencerLayout> sequencerLayout;
    HybridRollEditMode rollEditMode;
    ListenerList<ProjectListener> changeListeners;
    ScopedPointer<ProjectPage> projectSettings;
    ReadWriteLock tracksListLock;
    ScopedPointer<ProjectInfo> info;
    ScopedPointer<ProjectTimeline> timeline;

    WeakReference<TreeItem> lastShownTrack;

private:

    void initialize();
    ValueTree save() const;
    void load(const ValueTree &tree);

private:

    ReadWriteLock vcsInfoLock;
    Array<const VCS::TrackedItem *> vcsItems;

    ScopedPointer<UndoStack> undoStack;

    bool isLayersHashOutdated;
    SparseHashMap<String, WeakReference<MidiSequence>, StringHash> sequencesHash;

    void rebuildSequencesHashIfNeeded();

};
