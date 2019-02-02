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

#include "Common.h"
#include "ProjectTreeItem.h"

#include "TreeItemChildrenSerializer.h"
#include "TrackGroupTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"
#include "PatternEditorTreeItem.h"
#include "VersionControlTreeItem.h"
#include "VersionControl.h"

#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"

#include "Autosaver.h"
#include "Document.h"
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "BinarySerializer.h"

#include "AudioCore.h"
#include "PlayerThread.h"
#include "Pattern.h"
#include "MidiTrack.h"
#include "MidiEvent.h"
#include "TrackedItem.h"
#include "HybridRoll.h"
#include "UndoStack.h"

#include "ProjectInfo.h"
#include "ProjectTimeline.h"
#include "ProjectListener.h"
#include "ProjectPage.h"
#include "ProjectMenu.h"

#include "HelioTheme.h"
#include "SequencerLayout.h"
#include "MainLayout.h"
#include "Workspace.h"

#include "SerializationKeys.h"
#include "Config.h"
#include "Icons.h"

ProjectTreeItem::ProjectTreeItem() :
    DocumentOwner({}, "helio"),
    TreeItem({}, Serialization::Core::project)
{
    this->initialize();
}

ProjectTreeItem::ProjectTreeItem(const String &name, const String &id) :
    DocumentOwner(name, "helio"),
    TreeItem(name, Serialization::Core::project),
    id(id.isEmpty() ? Uuid().toString() : id)
{
    this->initialize();
}

ProjectTreeItem::ProjectTreeItem(const File &existingFile) :
    DocumentOwner(existingFile),
    TreeItem(existingFile.getFileNameWithoutExtension(), Serialization::Core::project),
    id(Uuid().toString())
{
    this->initialize();
}

void ProjectTreeItem::initialize()
{
    this->isTracksCacheOutdated = true;
    
    this->undoStack = new UndoStack(*this);
    
    this->autosaver = new Autosaver(*this);

    this->transport = new Transport(App::Workspace().getAudioCore());
    this->addListener(this->transport);
        
    this->info = new ProjectInfo(*this);
    this->vcsItems.add(this->info);
    
    this->timeline = new ProjectTimeline(*this, "Project Timeline");
    this->vcsItems.add(this->timeline);

    this->transport->seekToPosition(0.0);

    this->recreatePage();
}

ProjectTreeItem::~ProjectTreeItem()
{
    this->getDocument()->save();
    
    this->transport->stopPlayback();
    this->transport->stopRender();

    // remember as the recent file
    App::Workspace().getUserProfile().onProjectUnloaded(this->getId());

    this->projectPage = nullptr;

    this->removeAllListeners();
    this->sequencerLayout = nullptr;

    this->timeline = nullptr;
    this->info = nullptr;

    this->removeListener(this->transport);
    this->transport = nullptr;

    this->autosaver = nullptr;
}

String ProjectTreeItem::getId() const noexcept
{
    return this->id;
}

String ProjectTreeItem::getStats() const
{
    Array<MidiTrackTreeItem *> layerItems(this->findChildrenOfType<MidiTrackTreeItem>());
    
    int numEvents = 0;
    int numLayers = layerItems.size();

    for (int i = 0; i < numLayers; ++i)
    {
        numEvents += layerItems[i]->getSequence()->size();
    }

    return String(TRANS_PLURAL("{x} layers", numLayers) + " " + TRANS("common::and") + " " + TRANS_PLURAL("{x} events", numEvents));
}

Transport &ProjectTreeItem::getTransport() const noexcept
{
    jassert(this->transport);
    return (*this->transport);
}

ProjectInfo *ProjectTreeItem::getProjectInfo() const noexcept
{
    jassert(this->info);
    return this->info;
}

ProjectTimeline *ProjectTreeItem::getTimeline() const noexcept
{
    jassert(this->timeline);
    return this->timeline;
}

HybridRollEditMode &ProjectTreeItem::getEditMode() noexcept
{
    return this->rollEditMode;
}

HybridRoll *ProjectTreeItem::getLastFocusedRoll() const
{
    return this->sequencerLayout->getRoll();
}

Image ProjectTreeItem::getIcon() const noexcept
{
    return Icons::findByName(Icons::project, TREE_LARGE_ICON_HEIGHT);
}

void ProjectTreeItem::showPage()
{
    this->projectPage->updateContent();
    App::Layout().showPage(this->projectPage, this);
}

void ProjectTreeItem::safeRename(const String &newName, bool sendNotifications)
{
    if (newName == this->getName())
    {
        return;
    }
    
    this->name = newName;
    
    this->getDocument()->renameFile(newName);

    if (sendNotifications)
    {
        this->broadcastChangeProjectInfo(this->info);

        App::Workspace().getUserProfile()
            .onProjectLocalInfoUpdated(this->getId(), this->getName(),
                this->getDocument()->getFullPath());

        this->dispatchChangeTreeItemView();
    }
}

void ProjectTreeItem::recreatePage()
{
    ValueTree layoutState(Serialization::UI::sequencer);
    if (this->sequencerLayout || this->projectPage)
    {
        layoutState = this->sequencerLayout->serialize();
    }
    
    this->sequencerLayout = new SequencerLayout(*this);
    this->projectPage = new ProjectPage(*this);

    this->broadcastChangeProjectBeatRange(); // let rolls update view ranges
    this->sequencerLayout->deserialize(layoutState);
}

void ProjectTreeItem::showPatternEditor(WeakReference<TreeItem> source)
{
    jassert(source != nullptr);
    this->sequencerLayout->showPatternEditor();
    App::Layout().showPage(this->sequencerLayout, source);
}

void ProjectTreeItem::showLinearEditor(WeakReference<MidiTrack> activeTrack, WeakReference<TreeItem> source)
{
    jassert(source != nullptr);
    jassert(activeTrack != nullptr);

    if (const auto *pianoTrack = dynamic_cast<PianoTrackTreeItem *>(activeTrack.get()))
    {
        this->sequencerLayout->showLinearEditor(activeTrack);
        this->lastShownTrack = source;
        App::Layout().showPage(this->sequencerLayout, source);
    }
}

WeakReference<TreeItem> ProjectTreeItem::getLastShownTrack() const noexcept
{
    return this->lastShownTrack;
}


void ProjectTreeItem::setEditableScope(MidiTrack *track, const Clip &clip, bool zoomToArea)
{
    if (auto *item = dynamic_cast<PianoTrackTreeItem *>(track))
    {
        // FIXME: as we have to switch to target tree item,
        // it will activate its 1st clip on showPage
        item->setSelected(true, true);
        // and then we have to update the scope to correct clip,
        // so that roll's scope is updated twice :(
        this->sequencerLayout->setEditableScope(track, clip, zoomToArea);
    }
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool ProjectTreeItem::hasMenu() const noexcept
{
    return true;
}

ScopedPointer<Component> ProjectTreeItem::createMenu()
{
    return new ProjectMenu(*this, MenuPanel::SlideRight);
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

bool ProjectTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    return (dragSourceDetails.description == Serialization::Core::track.toString()) ||
           (dragSourceDetails.description == Serialization::Core::trackGroup.toString());
}

//===----------------------------------------------------------------------===//
// Undos
//===----------------------------------------------------------------------===//

UndoStack *ProjectTreeItem::getUndoStack() const noexcept
{
    return this->undoStack.get();
}

void ProjectTreeItem::checkpoint()
{
    this->getUndoStack()->beginNewTransaction({});
}

void ProjectTreeItem::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint();
        this->getUndoStack()->undo();
    }
}

void ProjectTreeItem::redo()
{
    if (this->getUndoStack()->canRedo())
    {
        this->getUndoStack()->redo();
    }
}

void ProjectTreeItem::clearUndoHistory()
{
    this->getUndoStack()->clearUndoHistory();
}

void ProjectTreeItem::removeTrack(const MidiTrack &track)
{
    const String &trackId = track.getTrackId();

    if (dynamic_cast<const PianoTrackTreeItem *>(&track))
    {
        this->getUndoStack()->perform(new PianoTrackRemoveAction(*this, this, trackId));
    }
    else if (dynamic_cast<const AutomationTrackTreeItem *>(&track))
    {
        this->getUndoStack()->perform(new AutomationTrackRemoveAction(*this, this, trackId));
    }
}

//===----------------------------------------------------------------------===//
// Project
//===----------------------------------------------------------------------===//

Array<MidiTrack *> ProjectTreeItem::getTracks() const
{
    const ScopedReadLock lock(this->tracksListLock);
    Array<MidiTrack *> tracks;

    // now get all layers inside a tree hierarchy
    this->collectTracks(tracks);
    
    // and explicitly add the only non-tree-owned layers
    tracks.add(this->timeline->getAnnotations());
    tracks.add(this->timeline->getKeySignatures());
    tracks.add(this->timeline->getTimeSignatures());

    return tracks;
}

Array<MidiTrack *> ProjectTreeItem::getSelectedTracks() const
{
    const ScopedReadLock lock(this->tracksListLock);
    Array<MidiTrack *> tracks;
    this->collectTracks(tracks, true);
    return tracks;
}

void ProjectTreeItem::collectTracks(Array<MidiTrack *> &resultArray, bool onlySelected /*= false*/) const
{
    const Array<MidiTrackTreeItem *> treeItems =
        this->findChildrenOfType<MidiTrackTreeItem>();
    
    for (int i = 0; i < treeItems.size(); ++i)
    {
        if (treeItems.getUnchecked(i)->isSelected() || !onlySelected)
        {
            resultArray.add(treeItems.getUnchecked(i));
        }
    }
}

Point<float> ProjectTreeItem::getProjectRangeInBeats() const
{
    float lastBeat = -FLT_MAX;
    float firstBeat = FLT_MAX;

    this->rebuildTracksRefsCacheIfNeeded();

    for (const auto &i : this->tracksRefsCache)
    {
        const auto *track = i.second.get();
        const float sequenceFirstBeat = track->getSequence()->getFirstBeat();
        const float sequenceLastBeat = track->getSequence()->getLastBeat();
        const float patternFirstBeat = track->getPattern() ? track->getPattern()->getFirstBeat() : 0.f;
        const float patternLastBeat = track->getPattern() ? track->getPattern()->getLastBeat() : 0.f;
        firstBeat = jmin(firstBeat, sequenceFirstBeat + patternFirstBeat);
        lastBeat = jmax(lastBeat, sequenceLastBeat + patternLastBeat);
    }
    
    const float defaultNumBeats = DEFAULT_NUM_BARS * BEATS_PER_BAR;

    if (firstBeat == FLT_MAX)
    {
        firstBeat = 0;
    }
    else if (firstBeat > lastBeat)
    {
        firstBeat = lastBeat - defaultNumBeats;
    }
    
    if ((lastBeat - firstBeat) < defaultNumBeats)
    {
        lastBeat = firstBeat + defaultNumBeats;
    }

    return { firstBeat, lastBeat };
}

StringArray ProjectTreeItem::getAllTrackNames() const
{
    StringArray names;
    this->rebuildTracksRefsCacheIfNeeded();
    for (const auto &i : this->tracksRefsCache)
    {
        names.add(i.second->getTrackName());
    }
    return names;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ProjectTreeItem::serialize() const
{
    this->getDocument()->save();

    ValueTree tree(Serialization::Core::treeItem);
    tree.setProperty(Serialization::Core::treeItemType, this->type, nullptr);
    tree.setProperty(Serialization::Core::filePath, this->getDocument()->getFullPath(), nullptr);
    return tree;
}

void ProjectTreeItem::deserialize(const ValueTree &tree)
{
    this->reset();

    const File fullPathFile = File(tree.getProperty(Serialization::Core::filePath));
    const File relativePathFile = DocumentHelpers::getDocumentSlot(fullPathFile.getFileName());
    
    if (!fullPathFile.existsAsFile() && !relativePathFile.existsAsFile())
    {
        delete this;
        return;
    }
    
    this->getDocument()->load(fullPathFile, relativePathFile);
}

void ProjectTreeItem::reset()
{
    this->transport->seekToPosition(0.f);
    this->vcsItems.clear();
    this->vcsItems.add(this->info);
    this->vcsItems.add(this->timeline);
    this->undoStack->clearUndoHistory();
    TreeItem::reset();
    // TODO test if we need that really (I suggest we don't):
    //this->broadcastReloadProjectContent();
    //this->broadcastChangeProjectBeatRange();
}

ValueTree ProjectTreeItem::save() const
{
    ValueTree tree(Serialization::Core::project);

    tree.setProperty(Serialization::Core::treeItemName, this->name, nullptr);
    tree.setProperty(Serialization::Core::projectId, this->id, nullptr);

    tree.appendChild(this->info->serialize(), nullptr);
    tree.appendChild(this->timeline->serialize(), nullptr);
    tree.appendChild(this->undoStack->serialize(), nullptr);
    tree.appendChild(this->transport->serialize(), nullptr);
    tree.appendChild(this->sequencerLayout->serialize(), nullptr);

    TreeItemChildrenSerializer::serializeChildren(*this, tree);

    return tree;
}

void ProjectTreeItem::load(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::Core::project) ?
        tree : tree.getChildWithName(Serialization::Core::project);

    if (!root.isValid()) { return; }

    this->id = root.getProperty(Serialization::Core::projectId, Uuid().toString());

    this->info->deserialize(root);
    this->timeline->deserialize(root);

    // Proceed with basic properties and children
    TreeItem::deserialize(root);

    // Legacy support: if no pattern set manager found, create one
    if (nullptr == this->findChildOfType<PatternEditorTreeItem>())
    {
        // Try to place it after 'Versions' (presumably, index 1)
        this->addChildTreeItem(new PatternEditorTreeItem(), 1);
    }

    this->broadcastReloadProjectContent();
    const auto range = this->broadcastChangeProjectBeatRange();

    // a hack to add some margin to project beat range,
    // then to round beats to nearest bars
    // because rolls' view ranges are rounded to bars
    const float r = float(BEATS_PER_BAR);
    const float viewStartWithMargin = range.getX() - r;
    const float viewEndWithMargin = range.getY() + r;
    const float viewFirstBeat = floorf(viewStartWithMargin / r) * r;
    const float viewLastBeat = ceilf(viewEndWithMargin / r) * r;
    this->broadcastChangeViewBeatRange(viewFirstBeat, viewLastBeat);

    this->undoStack->deserialize(root);

    // At least, when all tracks are ready:
    this->transport->deserialize(root);
    this->sequencerLayout->deserialize(root);
}

void ProjectTreeItem::importMidi(const File &file)
{
    MidiFile tempFile;
    FileInputStream in(file);
    const bool readOk = tempFile.readFrom(in);

    if (!readOk)
    {
        DBG("Midi file appears corrupted");
        return;
    }

    Random r;
    const auto colours = MenuPanel::getColoursList().getAllValues();

    for (int i = 0; i < tempFile.getNumTracks(); i++)
    {
        const MidiMessageSequence *currentTrack = tempFile.getTrack(i);
        const String trackName = "Track " + String(i);
        MidiTrackTreeItem *track = new PianoTrackTreeItem(trackName);

        const Clip clip(track->getPattern());
        track->getPattern()->insert(clip, false);

        this->addChildTreeItem(track, -1, false);

        // Set some colour
        const int ci = r.nextInt(colours.size());
        track->setTrackColour(Colour::fromString(colours[ci]), dontSendNotification);

        track->importMidi(*currentTrack, tempFile.getTimeFormat());
    }
    
    this->broadcastReloadProjectContent();
    this->broadcastChangeProjectBeatRange();
    this->getDocument()->save();
}

//===----------------------------------------------------------------------===//
// ProjectListeners management
//===----------------------------------------------------------------------===//

void ProjectTreeItem::addListener(ProjectListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.add(listener);
}

void ProjectTreeItem::removeListener(ProjectListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.remove(listener);
}

void ProjectTreeItem::removeAllListeners()
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.clear();
}


//===----------------------------------------------------------------------===//
// Broadcaster
//===----------------------------------------------------------------------===//

void ProjectTreeItem::broadcastChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    //jassert(oldEvent.isValid()); // old event is allowed to be un-owned
    jassert(newEvent.isValid());
    this->changeListeners.call(&ProjectListener::onChangeMidiEvent, oldEvent, newEvent);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastAddEvent(const MidiEvent &event)
{
    jassert(event.isValid());
    this->changeListeners.call(&ProjectListener::onAddMidiEvent, event);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastRemoveEvent(const MidiEvent &event)
{
    jassert(event.isValid());
    this->changeListeners.call(&ProjectListener::onRemoveMidiEvent, event);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastPostRemoveEvent(MidiSequence *const layer)
{
    this->changeListeners.call(&ProjectListener::onPostRemoveMidiEvent, layer);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastAddTrack(MidiTrack *const track)
{
    this->isTracksCacheOutdated = true;

    if (auto *tracked = dynamic_cast<VCS::TrackedItem *>(track))
    {
        const ScopedWriteLock lock(this->vcsInfoLock);
        this->vcsItems.addIfNotAlreadyThere(tracked);
    }

    this->changeListeners.call(&ProjectListener::onAddTrack, track);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastRemoveTrack(MidiTrack *const track)
{
    this->isTracksCacheOutdated = true;

    if (auto *tracked = dynamic_cast<VCS::TrackedItem *>(track))
    {
        const ScopedWriteLock lock(this->vcsInfoLock);
        this->vcsItems.removeAllInstancesOf(tracked);
    }

    this->changeListeners.call(&ProjectListener::onRemoveTrack, track);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastChangeTrackProperties(MidiTrack *const track)
{
    this->changeListeners.call(&ProjectListener::onChangeTrackProperties, track);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastAddClip(const Clip &clip)
{
    this->changeListeners.call(&ProjectListener::onAddClip, clip);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastChangeClip(const Clip &oldClip, const Clip &newClip)
{
    this->changeListeners.call(&ProjectListener::onChangeClip, oldClip, newClip);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastRemoveClip(const Clip &clip)
{
    this->changeListeners.call(&ProjectListener::onRemoveClip, clip);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastPostRemoveClip(Pattern *const pattern)
{
    this->changeListeners.call(&ProjectListener::onPostRemoveClip, pattern);
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastChangeProjectInfo(const ProjectInfo *info)
{
    this->changeListeners.call(&ProjectListener::onChangeProjectInfo, info);
    this->sendChangeMessage();
}

Point<float> ProjectTreeItem::broadcastChangeProjectBeatRange()
{
    const auto beatRange = this->getProjectRangeInBeats();
    const float firstBeat = beatRange.getX();
    const float lastBeat = beatRange.getY();
    
    // FIXME
    // changeListeners.call iterates listeners list in it's order
    // so that transport updates playhead position later then others,
    // so that resizing roll will make playhead glitch;
    // for now - just force transport to update its playhead position before all others
    // (doing the same work twice though)
    this->transport->onChangeProjectBeatRange(firstBeat, lastBeat);
    
    this->changeListeners.call(&ProjectListener::onChangeProjectBeatRange, firstBeat, lastBeat);
    this->sendChangeMessage();

    return beatRange;
}

void ProjectTreeItem::broadcastReloadProjectContent()
{
    this->changeListeners.call(&ProjectListener::onReloadProjectContent, this->getTracks());
    this->sendChangeMessage();
}

void ProjectTreeItem::broadcastChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->changeListeners.call(&ProjectListener::onChangeViewBeatRange, firstBeat, lastBeat);
    // this->sendChangeMessage(); the project itself didn't change, so dont call this
}


//===----------------------------------------------------------------------===//
// DocumentOwner
//===----------------------------------------------------------------------===//

bool ProjectTreeItem::onDocumentLoad(File &file)
{
    if (file.existsAsFile())
    {
        const ValueTree tree(DocumentHelpers::load(file));
        if (tree.isValid())
        {
            this->load(tree);
            return true;
        }
    }

    return false;
}

void ProjectTreeItem::onDocumentDidLoad(File &file)
{
    App::Workspace().getUserProfile()
        .onProjectLocalInfoUpdated(this->getId(), this->getName(),
            this->getDocument()->getFullPath());
}

bool ProjectTreeItem::onDocumentSave(File &file)
{
    const auto projectNode(this->save());
#if DEBUG
    DocumentHelpers::save<XmlSerializer>(file.withFileExtension("xml"), projectNode);
#endif
    return DocumentHelpers::save<BinarySerializer>(file, projectNode);
}

void ProjectTreeItem::onDocumentImport(File &file)
{
    if (file.hasFileExtension("mid") || file.hasFileExtension("midi"))
    {
        this->importMidi(file);
    }
}

bool ProjectTreeItem::onDocumentExport(File &file)
{
    if (file.hasFileExtension("mid") || file.hasFileExtension("midi"))
    {
        this->exportMidi(file);
        return true;
    }

    return false;
}

void ProjectTreeItem::exportMidi(File &file) const
{
    MidiFile tempFile;
    static const double midiClock = 960.0;
    tempFile.setTicksPerQuarterNote(int(midiClock));
    static Clip noTransform;

    const auto &tracks = this->getTracks();
    for (const auto *track : tracks)
    {
        MidiMessageSequence sequence;

        if (track->getPattern() != nullptr)
        {
            for (const auto *clip : track->getPattern()->getClips())
            {
                track->getSequence()->exportMidi(sequence, *clip, 0.0, midiClock);
            }
        }
        else
        {
            track->getSequence()->exportMidi(sequence, noTransform, 0.0, midiClock);
        }

        tempFile.addTrack(sequence);
    }
    
    if (file.exists())
    {
        file.deleteFile();
    }

    ScopedPointer<OutputStream> out(new FileOutputStream(file));
    tempFile.writeTo(*out);
}

//===----------------------------------------------------------------------===//
// MidiTrackSource
//===----------------------------------------------------------------------===//

MidiTrack *ProjectTreeItem::getTrackById(const String &trackId)
{
    this->rebuildTracksRefsCacheIfNeeded();
    return this->tracksRefsCache[trackId].get();
}

Pattern *ProjectTreeItem::getPatternByTrackId(const String &trackId)
{
    this->rebuildTracksRefsCacheIfNeeded();
    if (auto track = this->tracksRefsCache[trackId].get())
    {
        return track->getPattern();
    }

    return nullptr;
}

MidiSequence *ProjectTreeItem::getSequenceByTrackId(const String &trackId)
{
    this->rebuildTracksRefsCacheIfNeeded();
    if (auto track = this->tracksRefsCache[trackId].get())
    {
        return track->getSequence();
    }

    return nullptr;
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItemsSource
//===----------------------------------------------------------------------===//

String ProjectTreeItem::getVCSId() const
{
    return this->getId();
}

String ProjectTreeItem::getVCSName() const
{
    return this->getName();
}

int ProjectTreeItem::getNumTrackedItems()
{
    const ScopedReadLock lock(this->vcsInfoLock);
    return this->vcsItems.size();
}

VCS::TrackedItem *ProjectTreeItem::getTrackedItem(int index)
{
    const ScopedReadLock lock(this->vcsInfoLock);
    return const_cast<VCS::TrackedItem *>(this->vcsItems[index]);
}

VCS::TrackedItem *ProjectTreeItem::initTrackedItem(const Identifier &type,
    const Uuid &id, const VCS::TrackedItem &newState)
{
    if (type == Serialization::Core::pianoTrack)
    {
        auto *track = new PianoTrackTreeItem("");
        track->setVCSUuid(id);
        this->addChildTreeItem(track, -1, false);
        // add explicitly, since we aren't going to receive a notification:
        this->isTracksCacheOutdated = true;
        this->vcsItems.addIfNotAlreadyThere(track);
        track->resetStateTo(newState);
        return track;
    }
    if (type == Serialization::Core::automationTrack)
    {
        auto *track = new AutomationTrackTreeItem("");
        track->setVCSUuid(id);
        this->addChildTreeItem(track, -1, false);
        this->isTracksCacheOutdated = true;
        this->vcsItems.addIfNotAlreadyThere(track);
        track->resetStateTo(newState);
        return track;
    }
    else if (type == Serialization::Core::projectInfo)
    {
        this->info->setVCSUuid(id);
        this->info->resetStateTo(newState);
        return this->info;
    }
    else if (type == Serialization::Core::projectTimeline)
    {
        this->timeline->setVCSUuid(id);
        this->timeline->resetStateTo(newState);
        return this->timeline;
    }
    
    return nullptr;
}

bool ProjectTreeItem::deleteTrackedItem(VCS::TrackedItem *item)
{
    if (auto *treeItem = dynamic_cast<MidiTrackTreeItem *>(item))
    {
        TreeItem::deleteItem(treeItem, false); // don't broadcastRemoveTrack
        this->vcsItems.removeAllInstancesOf(item);
        this->isTracksCacheOutdated = true;
        return true;
    }

    return false;
}

void ProjectTreeItem::onResetState()
{
    this->broadcastReloadProjectContent();
    this->broadcastChangeProjectBeatRange();
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void ProjectTreeItem::changeListenerCallback(ChangeBroadcaster *source)
{
    if (VersionControl *vcs = dynamic_cast<VersionControl *>(source))
    {
        DocumentOwner::sendChangeMessage();
        //this->getDocument()->save();
        
        // FIXME! a bug reproduced in iOS when callin forceSave() here:
        // VCS start rebuilding diff after a commit in a separate thread,
        // at the same time main thread saves the project and flushes the VCS pack,
        // and eventually the pack cannot get delta data.
        //this->getDocument()->forceSave();
    }
}

void ProjectTreeItem::rebuildTracksRefsCacheIfNeeded() const
{
    if (this->isTracksCacheOutdated)
    {
        this->tracksRefsCache.clear();

        this->tracksRefsCache[this->timeline->getAnnotations()->getTrackId()] =
            this->timeline->getAnnotations();

        this->tracksRefsCache[this->timeline->getKeySignatures()->getTrackId()] =
            this->timeline->getKeySignatures();

        this->tracksRefsCache[this->timeline->getTimeSignatures()->getTrackId()] =
            this->timeline->getTimeSignatures();
        
        const Array<MidiTrack *> children = this->findChildrenOfType<MidiTrack>();
        
        for (int i = 0; i < children.size(); ++i)
        {
            MidiTrack *const track = children.getUnchecked(i);
            this->tracksRefsCache[track->getTrackId()] = track;
        }
        
        this->isTracksCacheOutdated = false;
    }
}
