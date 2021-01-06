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
class ProjectListener;
class SequencerLayout;
class HybridRoll;
class MidiEvent;
class ProjectPage;
class Origami;
class MidiRecorder;
class ProjectMetadata;
class ProjectTimeline;
class CommandPaletteTimelineEvents;
class UndoStack;
class Pattern;
class MidiTrack;
class Clip;

#include "TreeNode.h"
#include "DocumentOwner.h"
#include "Transport.h"
#include "TrackedItemsSource.h"
#include "HybridRollEditMode.h"
#include "MidiSequence.h"
#include "MidiTrackSource.h"
#include "CommandPaletteModel.h"

class ProjectNode final :
    public TreeNode,
    public DocumentOwner,
    public MidiTrackSource,
    public CommandPaletteModel,
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
    ProjectMetadata *getProjectInfo() const noexcept;
    ProjectTimeline *getTimeline() const noexcept;
    HybridRollEditMode &getEditMode() noexcept;
    HybridRoll *getLastFocusedRoll() const;
    
    void importMidi(InputStream &stream);
    void exportMidi(OutputStream &stream) const;

    Image getIcon() const noexcept override;

    void showPage() override;
    void recreatePage() override;
    void showPatternEditor(WeakReference<TreeNode> source);
    void showLinearEditor(WeakReference<MidiTrack> activeTrack, WeakReference<TreeNode> source);
    WeakReference<TreeNode> getLastShownTrack() const noexcept;

    void safeRename(const String &newName, bool sendNotifications) override;

    void setMidiRecordingTarget(MidiTrack *const track, const Clip *clip);
    void setEditableScope(MidiTrack *const activeTrack,
        const Clip &activeClip, bool shouldFocusToArea = false);

    //===------------------------------------------------------------------===//
    // Menu
    //===------------------------------------------------------------------===//

    bool hasMenu() const noexcept override;
    UniquePointer<Component> createMenu() override;

    //===------------------------------------------------------------------===//
    // Tree
    //===------------------------------------------------------------------===//

    void onNodeChildPostRemove(bool sendNotifications) override;

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
    Point<float> getProjectRangeInBeats() const;
    StringArray getAllTrackNames() const;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
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
    void broadcastChangeTrackBeatRange(MidiTrack *const track);

    void broadcastAddClip(const Clip &clip);
    void broadcastChangeClip(const Clip &oldClip, const Clip &newClip);
    void broadcastRemoveClip(const Clip &clip);
    void broadcastPostRemoveClip(Pattern *const pattern);

    void broadcastChangeProjectInfo(const ProjectMetadata *info);
    void broadcastChangeViewBeatRange(float firstBeat, float lastBeat);
    Point<float> broadcastChangeProjectBeatRange();

    void broadcastBeforeReloadProjectContent();
    void broadcastReloadProjectContent();

    void broadcastActivateProjectSubtree();
    void broadcastDeactivateProjectSubtree();

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
    void onBeforeResetState() override;
    void onResetState() override;

    //===------------------------------------------------------------------===//
    // Command Palette
    //===------------------------------------------------------------------===//

    Array<CommandPaletteActionsProvider *> getCommandPaletteActionProviders() const override;

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

protected:

    //===------------------------------------------------------------------===//
    // DocumentOwner
    //===------------------------------------------------------------------===//

    bool onDocumentLoad(const File &file) override;
    bool onDocumentSave(const File &file) override;
    void onDocumentImport(InputStream &stream) override;
    bool onDocumentExport(OutputStream &stream) override;

    //===------------------------------------------------------------------===//
    // MidiTrackSource
    //===------------------------------------------------------------------===//

    MidiTrack *getTrackById(const String &trackId) override;
    Pattern *getPatternByTrackId(const String &trackId) override;
    MidiSequence *getSequenceByTrackId(const String &trackId) override;

private:

    void collectTracks(Array<MidiTrack *> &resultArray, bool onlySelected = false) const;

    UniquePointer<Autosaver> autosaver;
    UniquePointer<Transport> transport;
    UniquePointer<MidiRecorder> midiRecorder;

    UniquePointer<SequencerLayout> sequencerLayout;
    HybridRollEditMode rollEditMode;

    ListenerList<ProjectListener> changeListeners;
    UniquePointer<ProjectPage> projectPage;
    ReadWriteLock tracksListLock;

    UniquePointer<ProjectMetadata> metadata;
    UniquePointer<ProjectTimeline> timeline;

    WeakReference<TreeNode> lastShownTrack;

    UniquePointer<CommandPaletteTimelineEvents> consoleTimelineEvents;

private:

    void initialize();
    SerializedData save() const;
    void load(const SerializedData &tree);

private:

    String id;

    ReadWriteLock vcsInfoLock;
    Array<const VCS::TrackedItem *> vcsItems;

    UniquePointer<UndoStack> undoStack;

    mutable float firstBeatCache = 0.f;
    mutable float lastBeatCache = Globals::Defaults::projectLength;

    mutable bool isTracksCacheOutdated = true;
    mutable FlatHashMap<String, WeakReference<MidiTrack>, StringHash> tracksRefsCache;
    void rebuildTracksRefsCacheIfNeeded() const;

};
