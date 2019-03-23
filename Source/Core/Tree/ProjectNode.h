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

class Autosaver;
class Document;
class Project;
class ProjectListener;
class SequencerLayout;
class HybridRoll;
class MidiEvent;
class ProjectPage;
class Origami;
class TrackMap;
class Transport;
class ProjectInfo;
class ProjectTimeline;
class UndoStack;
class Pattern;
class MidiTrack;
class Clip;

#include "TreeNode.h"
#include "DocumentOwner.h"
#include "Transport.h"
#include "TrackedItemsSource.h"
#include "ProjectSequencesWrapper.h"
#include "HybridRollEditMode.h"
#include "MidiSequence.h"
#include "MidiTrackSource.h"

class ProjectNode final :
    public TreeNode,
    public DocumentOwner,
    public MidiTrackSource,
    public VCS::TrackedItemsSource,  // vcs stuff
    public ChangeListener // subscribed to VersionControl
{
public:

    ProjectNode();
    explicit ProjectNode(const String &name, const String &id = {});
    explicit ProjectNode(const File &existingFile);
    ~ProjectNode() override;
    
    String getId() const noexcept;
    String getStats() const;

    Transport &getTransport() const noexcept;
    ProjectInfo *getProjectInfo() const noexcept;
    ProjectTimeline *getTimeline() const noexcept;
    HybridRollEditMode &getEditMode() noexcept;
    HybridRoll *getLastFocusedRoll() const;
    
    void importMidi(const File &file);
    void exportMidi(File &file) const;

    Image getIcon() const noexcept override;

    void showPage() override;
    void recreatePage() override;

    void safeRename(const String &newName, bool sendNotifications) override;

    void showPatternEditor(WeakReference<TreeNode> source);
    void showLinearEditor(WeakReference<MidiTrack> activeTrack, WeakReference<TreeNode> source);
    WeakReference<TreeNode> getLastShownTrack() const noexcept;

    void setEditableScope(MidiTrack *track, const Clip &clip, bool zoomToArea = false);

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    ScopedPointer<Component> createMenu() override;
    
    //===------------------------------------------------------------------===//
    // Undos
    //===------------------------------------------------------------------===//

    void checkpoint();
    void undo();
    void redo();
    void clearUndoHistory();
    UndoStack *getUndoStack() const noexcept;
    void removeTrack(const MidiTrack &track);

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    Array<MidiTrack *> getTracks() const;
    Array<MidiTrack *> getSelectedTracks() const;
    Point<float> getProjectRangeInBeats() const;
    StringArray getAllTrackNames() const;

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

    String getVCSId() const override;
    String getVCSName() const override;
    int getNumTrackedItems() override;
    VCS::TrackedItem *getTrackedItem(int index) override;
    VCS::TrackedItem *initTrackedItem(const Identifier &type,
        const Uuid &id, const VCS::TrackedItem &newState) override;
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

    ScopedPointer<SequencerLayout> sequencerLayout;
    HybridRollEditMode rollEditMode;
    ListenerList<ProjectListener> changeListeners;
    ScopedPointer<ProjectPage> projectPage;
    ReadWriteLock tracksListLock;
    ScopedPointer<ProjectInfo> info;
    ScopedPointer<ProjectTimeline> timeline;

    WeakReference<TreeNode> lastShownTrack;

private:

    void initialize();
    ValueTree save() const;
    void load(const ValueTree &tree);

private:

    String id;

    ReadWriteLock vcsInfoLock;
    Array<const VCS::TrackedItem *> vcsItems;

    ScopedPointer<UndoStack> undoStack;

    mutable bool isTracksCacheOutdated;
    mutable FlatHashMap<String, WeakReference<MidiTrack>, StringHash> tracksRefsCache;
    void rebuildTracksRefsCacheIfNeeded() const;

};
