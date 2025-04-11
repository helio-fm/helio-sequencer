/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "ProjectNode.h"

#include "TreeNodeSerializer.h"
#include "TrackGroupNode.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"
#include "PatternEditorNode.h"
#include "VersionControlNode.h"
#include "VersionControl.h"
#include "PianoTrackActions.h"
#include "AutomationTrackActions.h"

#include "Autosaver.h"
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "BinarySerializer.h"

#include "AudioCore.h"
#include "Pattern.h"
#include "RollBase.h"
#include "UndoStack.h"
#include "MidiRecorder.h"
#include "KeyboardMapping.h"
#include "GeneratedSequenceBuilder.h"
#include "CommandPaletteTimelineEvents.h"

#include "ProjectMetadata.h"
#include "ProjectTimeline.h"
#include "ProjectPage.h"
#include "ProjectMenu.h"

#include "SequencerLayout.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "ColourIDs.h"
#include "Config.h"

ProjectNode::ProjectNode() :
    DocumentOwner({}, "helio"),
    TreeNode({}, Serialization::Core::project)
{
    this->initialize();
}

ProjectNode::ProjectNode(const String &name, const String &id) :
    DocumentOwner(name, "helio"),
    TreeNode(name, Serialization::Core::project),
    id(id.isEmpty() ? Uuid().toString() : id)
{
    this->initialize();
}

ProjectNode::ProjectNode(const File &existingFile) :
    DocumentOwner(existingFile),
    TreeNode(existingFile.getFileNameWithoutExtension(), Serialization::Core::project),
    id(Uuid().toString())
{
    this->initialize();
}

void ProjectNode::initialize()
{
    this->undoStack = make<UndoStack>(*this);
    this->autosaver = make<Autosaver>(*this);

    this->metadata = make<ProjectMetadata>(*this);
    this->vcsItems.add(this->metadata.get());

    this->timeline = make<ProjectTimeline>(*this, "Project Timeline");
    this->vcsItems.add(this->timeline.get());

    auto &orchestra = App::Workspace().getAudioCore();

    this->transport = make<Transport>(*this, orchestra);
    this->midiRecorder = make<MidiRecorder>(*this);

    this->consoleTimelineEvents = make<CommandPaletteTimelineEvents>(*this);
    this->generatedSequenceBuilder = make<GeneratedSequenceBuilder>(*this);

    this->recreatePage();

    this->transport->seekToBeat(this->beatRange.getStart());
}

ProjectNode::~ProjectNode()
{
    this->getDocument()->save();

    this->transport->stopPlaybackAndRecording();
    this->transport->stopRender();

    // remember as a recent file
    App::Workspace().getUserProfile().onProjectUnloaded(this->getId());

    this->projectPage = nullptr;

    this->removeAllListeners();
    this->sequencerLayout = nullptr;

    this->generatedSequenceBuilder = nullptr;
    this->consoleTimelineEvents = nullptr;

    this->midiRecorder = nullptr;
    this->transport = nullptr;

    this->timeline = nullptr;
    this->metadata = nullptr;

    this->autosaver = nullptr;
    this->undoStack = nullptr;
}

String ProjectNode::getId() const noexcept
{
    return this->id;
}

String ProjectNode::getStats() const
{
    const auto tracks = this->findChildrenOfType<MidiTrackNode>();
    
    int numEvents = 0;
    int numTracks = tracks.size();
    for (int i = 0; i < numTracks; ++i)
    {
        numEvents += tracks[i]->getSequence()->size();
    }

    return String(TRANS_PLURAL("{x} tracks", numTracks) + " " + 
        TRANS(I18n::Common::conjunction) + " " +
        TRANS_PLURAL("{x} events", numEvents));
}

Transport &ProjectNode::getTransport() const noexcept
{
    jassert(this->transport);
    return (*this->transport);
}

ProjectMetadata *ProjectNode::getProjectInfo() const noexcept
{
    jassert(this->metadata);
    return this->metadata.get();
}

ProjectTimeline *ProjectNode::getTimeline() const noexcept
{
    jassert(this->timeline);
    return this->timeline.get();
}

RollEditMode &ProjectNode::getEditMode() noexcept
{
    return this->rollEditMode;
}

RollBase *ProjectNode::getLastFocusedRoll() const
{
    return this->sequencerLayout->getRoll();
}

GeneratedSequenceBuilder *ProjectNode::getGeneratedSequences() const
{
    return this->generatedSequenceBuilder.get();
}

Image ProjectNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::project, Globals::UI::headlineIconSize);
}

void ProjectNode::showPage()
{
    this->projectPage->updateContent();
    App::Layout().showPage(this->projectPage.get(), this);
}

void ProjectNode::safeRename(const String &newName, bool sendNotifications)
{
    if (newName == this->getName())
    {
        return;
    }
    
    this->name = newName;
    
    this->getDocument()->renameFile(newName);

    if (sendNotifications)
    {
        this->broadcastChangeProjectInfo(this->metadata.get());

        App::Workspace().getUserProfile()
            .onProjectLocalInfoUpdated(this->getId(), this->getName(),
                this->getDocument()->getFullPath());

        this->dispatchChangeTreeNodeViews();
    }
}

void ProjectNode::recreatePage()
{
    SerializedData layoutState(Serialization::UI::sequencer);
    if (this->sequencerLayout || this->projectPage)
    {
        layoutState = this->sequencerLayout->serialize();
    }
    
    this->sequencerLayout = make<SequencerLayout>(*this);
    this->projectPage = make<ProjectPage>(*this);

    // reset caches and let rolls update view ranges:
    // (fixme the below is quite a common piece of code)
    this->beatRange = { 0, Globals::Defaults::projectLength };

    // if there is anything to reload, reload:
    if (!this->findChildrenOfType<MidiTrack>().isEmpty())
    {
        this->broadcastReloadProjectContent();
        const auto range = this->broadcastChangeProjectBeatRange();
        this->broadcastChangeViewBeatRange(range.getStart(), range.getEnd());
    }

    this->sequencerLayout->deserialize(layoutState);
}

void ProjectNode::setMidiRecordingTarget(const Clip *clip)
{
    String instrumentId;
    if (clip != nullptr)
    {
        auto *track = clip->getPattern()->getTrack();
        instrumentId = this->handleChangeActiveMidiInputInstrument(track);
    }

    // if clip == nullptr, pass empty instrment id meaning "disconnect everyone"
    this->midiRecorder->setTargetScope(clip, instrumentId);
}

void ProjectNode::setEditableScope(const Clip &activeClip, bool shouldFocusToArea)
{
    auto *activeTrack = activeClip.getPattern()->getTrack();
    if (auto *item = dynamic_cast<PianoTrackNode *>(activeTrack))
    {
        this->timeline->getTimeSignaturesAggregator()->setActiveScope({ activeTrack });

        // make sure the item is selected, if it's not yet; this may end up calling
        // setSelected() -> showPage() -> showLinearEditor() -> setEditableScope() again
        item->setSelected(activeClip);

        this->changeListeners.call(&ProjectListener::onChangeViewEditableScope,
            activeTrack, activeClip, shouldFocusToArea);

        this->setMidiRecordingTarget(&activeClip);
    }
}

void ProjectNode::showPatternEditor(WeakReference<TreeNode> source)
{
    jassert(source != nullptr);
    this->sequencerLayout->showPatternEditor();
    App::Layout().showPage(this->sequencerLayout.get(), source);
}

void ProjectNode::showLinearEditor(const Clip &activeClip,
    WeakReference<TreeNode> source)
{
    jassert(source != nullptr);

    if (!activeClip.isValid())
    {
        jassertfalse;
        return;
    }

    this->sequencerLayout->showLinearEditor(activeClip);
    this->lastShownTrack = source;
    App::Layout().showPage(this->sequencerLayout.get(), source);
}

WeakReference<TreeNode> ProjectNode::getLastShownTrack() const noexcept
{
    return this->lastShownTrack;
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool ProjectNode::hasMenu() const noexcept
{
    return true;
}

UniquePointer<Component> ProjectNode::createMenu()
{
    return make<ProjectMenu>(*this, MenuPanel::SlideRight);
}

//===----------------------------------------------------------------------===//
// Tree
//===----------------------------------------------------------------------===//

void ProjectNode::onNodeChildPostRemove(bool sendNotifications)
{
    if (sendNotifications)
    {
        // a track was removed, the project range might have changed:
        this->broadcastChangeProjectBeatRange();
    }

    TreeNode::onNodeChildPostRemove(sendNotifications);
}

//===----------------------------------------------------------------------===//
// Undos
//===----------------------------------------------------------------------===//

UndoStack *ProjectNode::getUndoStack() const noexcept
{
    return this->undoStack.get();
}

void ProjectNode::checkpoint()
{
    this->getUndoStack()->beginNewTransaction();
}

void ProjectNode::undo()
{
    this->getUndoStack()->undo();
}

void ProjectNode::redo()
{
    this->getUndoStack()->redo();
}

void ProjectNode::removeTrack(const MidiTrack &track)
{
    const String &trackId = track.getTrackId();

    if (dynamic_cast<const PianoTrackNode *>(&track))
    {
        this->getUndoStack()->perform(new PianoTrackRemoveAction(*this, this, trackId));
    }
    else if (dynamic_cast<const AutomationTrackNode *>(&track))
    {
        this->getUndoStack()->perform(new AutomationTrackRemoveAction(*this, this, trackId));
    }

    // nothing to be focused on in the piano roll, switch to patterns:
    if (this->findChildrenOfType<PianoTrackNode>().isEmpty())
    {
        this->selectFirstChildOfType<PatternEditorNode>();
    }
}

//===----------------------------------------------------------------------===//
// Project
//===----------------------------------------------------------------------===//

Array<MidiTrack *> ProjectNode::getTracks() const
{
    const ScopedReadLock lock(this->tracksListLock);
    Array<MidiTrack *> tracks;

    // now get all layers inside a tree hierarchy
    this->collectTracks(tracks);
    
    // and explicitly add the only non-tree-owned tracks
    tracks.add(this->timeline->getAnnotations());
    tracks.add(this->timeline->getKeySignatures());
    tracks.add(this->timeline->getTimeSignaturesAggregator());

    return tracks;
}

void ProjectNode::collectTracks(Array<MidiTrack *> &resultArray, bool onlySelected /*= false*/) const
{
    const auto trackNodes = this->findChildrenOfType<MidiTrackNode>();
    
    for (int i = 0; i < trackNodes.size(); ++i)
    {
        if (trackNodes.getUnchecked(i)->isSelected() || !onlySelected)
        {
            resultArray.add(trackNodes.getUnchecked(i));
        }
    }
}

Range<float> ProjectNode::getProjectBeatRange() const
{
    return this->beatRange;
}

inline void ProjectNode::applyDefaultViewMargin(Range<float> &beatRange)
{
    // set smaller margins for shorter projects:
    const auto length = jmax(float(Globals::Defaults::projectLength), beatRange.getLength());
    const auto viewMargin = jlimit(float(Globals::beatsPerBar), float(Globals::beatsPerBar * 4), length / 6.f);
    // also align the edges to nearest bars so that the grid looks nicer:
    const auto viewStartWithMargin = beatRange.getStart() - viewMargin;
    const auto viewEndWithMargin = beatRange.getEnd() + viewMargin;
    beatRange.setStart(floorf(viewStartWithMargin / Globals::beatsPerBar) * Globals::beatsPerBar);
    beatRange.setEnd(ceilf(viewEndWithMargin / Globals::beatsPerBar) * Globals::beatsPerBar);
}

Range<float> ProjectNode::calculateProjectBeatRange() const
{
    float lastBeat = -FLT_MAX;
    float firstBeat = FLT_MAX;

    this->rebuildTracksRefsCacheIfNeeded();

    for (const auto &i : this->tracksRefsCache)
    {
        const auto *track = i.second.get();
        if (track == nullptr)
        {
            jassertfalse;
            continue;
        }

        if (track->getSequence()->isEmpty())
        {
            // ignore empty tracks as they affect the project range in a misleading way
            continue;
        }

        const float sequenceFirstBeat = track->getSequence()->getFirstBeat();
        const float sequenceLastBeat = track->getSequence()->getLastBeat();
        const float patternFirstBeat = track->getPattern() ? track->getPattern()->getFirstBeat() : 0.f;
        const float patternLastBeat = track->getPattern() ? track->getPattern()->getLastBeat() : 0.f;
        firstBeat = jmin(firstBeat, sequenceFirstBeat + patternFirstBeat);
        lastBeat = jmax(lastBeat, sequenceLastBeat + patternLastBeat);
    }
    
    if (firstBeat == FLT_MAX)
    {
        firstBeat = 0;
    }
    else if (firstBeat > lastBeat)
    {
        firstBeat = lastBeat - Globals::Defaults::projectLength;
    }
    
    if ((lastBeat - firstBeat) < Globals::Defaults::projectLength)
    {
        lastBeat = firstBeat + Globals::Defaults::projectLength;
    }

    return { firstBeat, lastBeat };
}

StringArray ProjectNode::getAllTrackNames() const
{
    StringArray names;
    this->rebuildTracksRefsCacheIfNeeded();
    for (const auto &i : this->tracksRefsCache)
    {
        names.add(i.second->getTrackName());
    }
    return names;
}

// this is only used in the pattern roll to display tracks,
// and in the MIDI export; so now it's a simple getter/setter
MidiTrack::Grouping ProjectNode::getTrackGroupingMode() const noexcept
{
    return this->trackGroupingMode;
}

void ProjectNode::setTrackGroupingMode(MidiTrack::Grouping mode)
{
    this->trackGroupingMode = mode;
    this->sendChangeMessage();
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData ProjectNode::serialize() const
{
    this->getDocument()->save();

    SerializedData tree(Serialization::Core::treeNode);
    tree.setProperty(Serialization::Core::treeNodeType, this->type);
    tree.setProperty(Serialization::Core::filePath, this->getDocument()->getFullPath());
    return tree;
}

void ProjectNode::deserialize(const SerializedData &data)
{
    this->reset();

    const File fullPathFile = File(data.getProperty(Serialization::Core::filePath));

    // iOS hack: the `documents` path will change between launches
    const File relativePathFile = DocumentHelpers::getDocumentSlot(fullPathFile.getFileName());
    
    if (fullPathFile.existsAsFile())
    {
        this->getDocument()->load(fullPathFile);
        return;
    }
    else if (relativePathFile.existsAsFile())
    {
        this->getDocument()->load(relativePathFile);
        return;
    }

    jassertfalse;
    delete this;
}

void ProjectNode::reset()
{
    this->transport->seekToBeat(this->beatRange.getStart());
    this->vcsItems.clear();
    this->vcsItems.add(this->metadata.get());
    this->vcsItems.add(this->timeline.get());
    this->undoStack->clearUndoHistory();
    this->lastShownTrack = nullptr;
    TreeNode::reset();
}

SerializedData ProjectNode::save() const
{
    SerializedData tree(Serialization::Core::project);

    tree.setProperty(Serialization::Core::treeNodeName, this->name);
    tree.setProperty(Serialization::Core::projectId, this->id);
    tree.setProperty(Serialization::UI::trackGrouping, int(this->trackGroupingMode));

    tree.appendChild(this->metadata->serialize());
    tree.appendChild(this->timeline->serialize());
    tree.appendChild(this->undoStack->serialize());
    tree.appendChild(this->transport->serialize());
    tree.appendChild(this->sequencerLayout->serialize());

    TreeNodeSerializer::serializeChildren(*this, tree);

    return tree;
}

void ProjectNode::load(const SerializedData &tree)
{
    this->broadcastBeforeReloadProjectContent();
    this->reset();

    const auto root = tree.hasType(Serialization::Core::project) ?
        tree : tree.getChildWithName(Serialization::Core::project);

    if (!root.isValid()) { return; }

    this->id = root.getProperty(Serialization::Core::projectId, Uuid().toString());
    
    const auto grouping = root.getProperty(Serialization::UI::trackGrouping, int(this->trackGroupingMode));
    this->trackGroupingMode = MidiTrack::Grouping(int(grouping));

    this->metadata->deserialize(root);
    this->timeline->deserialize(root);

    // Proceed with basic properties and children
    TreeNode::deserialize(root);

    // Legacy support: if no pattern set manager found, create one
    if (nullptr == this->findChildOfType<PatternEditorNode>())
    {
        // Try to place it after 'Versions' (presumably, index 1)
        this->addChildNode(new PatternEditorNode(), 1);
    }

    this->broadcastReloadProjectContent();

    auto range = this->broadcastChangeProjectBeatRange();
    ProjectNode::applyDefaultViewMargin(range);
    this->broadcastChangeViewBeatRange(range.getStart(), range.getEnd());

    this->undoStack->deserialize(root);

    // At least, when all tracks are ready:
    this->transport->deserialize(root);
    this->sequencerLayout->deserialize(root);
}

void ProjectNode::importMidi(InputStream &stream)
{
    MidiFile tempFile;
    if (!tempFile.readFrom(stream))
    {
        DBG("Midi file appears corrupted");
        jassertfalse;
        return;
    }

    this->broadcastBeforeReloadProjectContent();
    this->timeline->reset();

    Random r;
    const auto colours = ColourIDs::getColoursList();
    const auto timeFormat = tempFile.getTimeFormat();

    for (int i = 0; i < tempFile.getNumTracks(); i++)
    {
        const auto *importedTrack = tempFile.getTrack(i);

        auto trackName = "Track " + String(i);
        const auto trackColour = colours[r.nextInt(colours.size())];

        // the MIDI standard allows to set channels and controller numbers per-message,
        // while in Helio channels and controllers are per-track for simplicity,
        // so let's group all found notes in each track by channel
        // (a different channel often means a different instrument):
        FlatHashSet<int> pianoChannels;
        
        // and group automation events by controller number, picking the last
        // found channel for it, which is not ideal but should be ok in practice:
        FlatHashMap<int, int> automationControllers;

        for (int j = 0; j < importedTrack->getNumEvents(); ++j)
        {
            const auto *event = importedTrack->getEventPointer(j);
            const auto channel = jlimit(1, Globals::numChannels, event->message.getChannel());

            if (event->message.isTrackNameEvent())
            {
                trackName = event->message.getTextFromTextMetaEvent();
            }
            else if (event->message.isController())
            {
                automationControllers.insert({ event->message.getControllerNumber(), channel });
            }
            else if (event->message.isTempoMetaEvent())
            {
                automationControllers.insert({ MidiTrack::DefaultControllers::tempoController, channel });
            }
            else if (event->message.isNoteOnOrOff())
            {
                pianoChannels.insert(channel);
            }
        }

        // split into several automation tracks, if needed
        for (const auto &trackInfo : automationControllers)
        {
            const auto trackControllerNumber = trackInfo.first;
            const auto trackChannel = trackInfo.second;
            const bool isTempoTrack = trackControllerNumber == MidiTrack::DefaultControllers::tempoController;
            const String controllerName(MidiMessage::getControllerName(trackControllerNumber));

            MidiTrackNode *trackNode = new AutomationTrackNode(isTempoTrack ?
                TRANS(I18n::Defaults::tempoTrackName) :
                (controllerName.isEmpty() ? trackName : trackName + " - " + controllerName));

            const Clip clip(trackNode->getPattern());
            trackNode->getPattern()->insert(clip, false);

            this->addChildNode(trackNode, -1, false);
            this->isTracksCacheOutdated = true;
            this->vcsItems.addIfNotAlreadyThere(trackNode);

            trackNode->setTrackControllerNumber(trackControllerNumber, dontSendNotification);
            trackNode->setTrackChannel(trackChannel, false, dontSendNotification);
            trackNode->setTrackColour(trackColour, false, dontSendNotification);

            const auto isSingleControllerTrack = automationControllers.size() == 1;
            trackNode->getSequence()->importMidi(*importedTrack, timeFormat,
                isSingleControllerTrack ? Optional<int>() : trackControllerNumber);
        }

        // split into several piano tracks, if needed
        for (const auto trackChannel : pianoChannels)
        {
            MidiTrackNode *trackNode = new PianoTrackNode(trackChannel == 1 ?
                trackName : trackName + " - " + String(trackChannel));

            const Clip clip(trackNode->getPattern());
            trackNode->getPattern()->insert(clip, false);

            this->addChildNode(trackNode, -1, false);
            this->isTracksCacheOutdated = true;
            this->vcsItems.addIfNotAlreadyThere(trackNode);

            trackNode->setTrackChannel(trackChannel, false, dontSendNotification);
            trackNode->setTrackColour(trackColour, false, dontSendNotification);

            const auto isSingleChannelTrack = pianoChannels.size() == 1;
            trackNode->getSequence()->importMidi(*importedTrack, timeFormat,
                isSingleChannelTrack ? Optional<int>() : trackChannel);
        }

        // if the track contains any key/time signatures, try importing them all,
        // skipping others (assuming that there might be cases where tracks contain
        // events of different types, e.g. mostly notes but also some meta events):
        this->timeline->getAnnotations()->getSequence()->importMidi(*importedTrack, timeFormat, {});
        this->timeline->getKeySignatures()->getSequence()->importMidi(*importedTrack, timeFormat, {});
        this->timeline->getTimeSignatures()->getSequence()->importMidi(*importedTrack, timeFormat, {});
    }
    
    this->isTracksCacheOutdated = true;
    this->broadcastReloadProjectContent();
    auto range = this->broadcastChangeProjectBeatRange();
    ProjectNode::applyDefaultViewMargin(range);
    this->broadcastChangeViewBeatRange(range.getStart(), range.getEnd());

    this->getDocument()->save();
}

//===----------------------------------------------------------------------===//
// ProjectListeners management
//===----------------------------------------------------------------------===//

void ProjectNode::addListener(ProjectListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.add(listener);
}

void ProjectNode::removeListener(ProjectListener *listener)
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.remove(listener);
}

void ProjectNode::removeAllListeners()
{
    jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
    this->changeListeners.clear();
}

//===----------------------------------------------------------------------===//
// Broadcaster
//===----------------------------------------------------------------------===//

void ProjectNode::broadcastChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    //jassert(oldEvent.isValid()); // old event is allowed to be un-owned
    jassert(newEvent.isValid());
    this->changeListeners.call(&ProjectListener::onChangeMidiEvent, oldEvent, newEvent);
    this->sendChangeMessage();
}

void ProjectNode::broadcastAddEvent(const MidiEvent &event)
{
    jassert(event.isValid());
    this->changeListeners.call(&ProjectListener::onAddMidiEvent, event);
    this->sendChangeMessage();
}

void ProjectNode::broadcastRemoveEvent(const MidiEvent &event)
{
    jassert(event.isValid());
    this->changeListeners.call(&ProjectListener::onRemoveMidiEvent, event);
    this->sendChangeMessage();
}

void ProjectNode::broadcastPostRemoveEvent(MidiSequence *const sequence)
{
    this->changeListeners.call(&ProjectListener::onPostRemoveMidiEvent, sequence);
    this->sendChangeMessage();
}

void ProjectNode::broadcastAddTrack(MidiTrack *const track)
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

void ProjectNode::broadcastRemoveTrack(MidiTrack *const track)
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

void ProjectNode::broadcastChangeTrackProperties(MidiTrack *const track)
{
    this->changeListeners.call(&ProjectListener::onChangeTrackProperties, track);
    this->sendChangeMessage();
}

void ProjectNode::broadcastChangeTrackBeatRange(MidiTrack *const track)
{
    this->changeListeners.call(&ProjectListener::onChangeTrackBeatRange, track);
    this->sendChangeMessage();
}

void ProjectNode::broadcastAddClip(const Clip &clip)
{
    this->changeListeners.call(&ProjectListener::onAddClip, clip);
    this->sendChangeMessage();
}

void ProjectNode::broadcastChangeClip(const Clip &oldClip, const Clip &newClip)
{
    this->changeListeners.call(&ProjectListener::onChangeClip, oldClip, newClip);
    this->sendChangeMessage();
}

void ProjectNode::broadcastRemoveClip(const Clip &clip)
{
    this->changeListeners.call(&ProjectListener::onRemoveClip, clip);
    this->sendChangeMessage();
}

void ProjectNode::broadcastPostRemoveClip(Pattern *const pattern)
{
    this->changeListeners.call(&ProjectListener::onPostRemoveClip, pattern);
    this->sendChangeMessage();
}

void ProjectNode::broadcastReloadGeneratedSequence(const Clip &clip,
    MidiSequence *const generatedSequence)
{
    this->changeListeners.call(&ProjectListener::onReloadGeneratedSequence, clip, generatedSequence);
    this->sendChangeMessage();
}

void ProjectNode::broadcastChangeProjectInfo(const ProjectMetadata *info)
{
    this->changeListeners.call(&ProjectListener::onChangeProjectInfo, info);
    this->sendChangeMessage();
}

Range<float> ProjectNode::broadcastChangeProjectBeatRange()
{
    const auto newBeatRange = this->calculateProjectBeatRange();
    
    if (this->beatRange != newBeatRange)
    {
        this->beatRange = newBeatRange;

        // changeListeners.call iterates listeners list in REVERSE order
        // so that transport updates playhead position later then others,
        // so that resizing roll will make playhead glitch;
        // as a hack, just force transport to update its playhead position before all others
        this->transport->onChangeProjectBeatRange(this->beatRange.getStart(), this->beatRange.getEnd());
        this->changeListeners.callExcluding(this->transport.get(),
            &ProjectListener::onChangeProjectBeatRange, this->beatRange.getStart(), this->beatRange.getEnd());

        this->sendChangeMessage();
    }

    return this->beatRange;
}

void ProjectNode::broadcastBeforeReloadProjectContent()
{
    this->changeListeners.call(&ProjectListener::onBeforeReloadProjectContent);
}

void ProjectNode::broadcastReloadProjectContent()
{
    this->isTracksCacheOutdated = true;

    this->changeListeners.call(&ProjectListener::onReloadProjectContent,
        this->getTracks(), this->metadata.get());

    this->sendChangeMessage();
}

void ProjectNode::broadcastActivateProjectSubtree()
{
    this->changeListeners.call(&ProjectListener::onActivateProjectSubtree, this->metadata.get());
}

void ProjectNode::broadcastDeactivateProjectSubtree()
{
    this->changeListeners.call(&ProjectListener::onDeactivateProjectSubtree, this->metadata.get());
}

void ProjectNode::broadcastChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->changeListeners.call(&ProjectListener::onChangeViewBeatRange, firstBeat, lastBeat);
    // this->sendChangeMessage(); the project itself didn't change, so dont call this
}

String ProjectNode::handleChangeActiveMidiInputInstrument(MidiTrack *const track)
{
    auto &audioCore = App::Workspace().getAudioCore();

    auto instrumentId = track->getTrackInstrumentId();

    // if the track has no instrument assigned, fallback to the default one
    if (instrumentId.isEmpty())
    {
        instrumentId = audioCore.getDefaultInstrument()->getIdAndHash();
    }

    const auto temperament = this->getProjectInfo()->getTemperament();
    audioCore.setActiveMidiPlayer(instrumentId,
        temperament->getPeriodSize(), temperament->getChromaticMap(), false);

    return instrumentId;
}

//===----------------------------------------------------------------------===//
// DocumentOwner
//===----------------------------------------------------------------------===//

bool ProjectNode::onDocumentLoad(const File &file)
{
    const auto tree = DocumentHelpers::load(file);

    if (tree.isValid())
    {
        this->load(tree);

        App::Workspace().getUserProfile()
            .onProjectLocalInfoUpdated(this->getId(), this->getName(),
                this->getDocument()->getFullPath());

        return true;
    }

    return false;
}

bool ProjectNode::onDocumentSave(const File &file)
{
    const auto projectNode = this->save();
#if DEBUG
    DocumentHelpers::save<XmlSerializer>(file.withFileExtension("xml"), projectNode);
#endif
    return DocumentHelpers::save<BinarySerializer>(file, projectNode);
}

void ProjectNode::onDocumentImport(InputStream &stream)
{
    // assumes MIDI import, todo checks
    this->importMidi(stream);
}

bool ProjectNode::onDocumentExport(OutputStream &stream)
{
    // assumes MIDI export, todo checks
    return this->exportMidi(stream);
}

bool ProjectNode::exportMidi(OutputStream &stream) const
{
    MidiFile tempFile;
    static const double midiClock = 960.0;
    tempFile.setTicksPerQuarterNote(int(midiClock));

    static Clip noTransform;
    static KeyboardMapping simpleMapping;

    // Solo flags won't be taken into account
    // in MIDI export, as I believe they shouldn't:
    const bool soloFlag = false;

    // Metronome track flag is only needed for playback:
    const bool metronomeFlag = false;

    const auto grouping = this->getTrackGroupingMode();
    FlatHashMap<String, MidiMessageSequence, StringHash> sequences;

    for (const auto *track : this->getTracks())
    {
        const auto groupKey = track->getTrackGroupKey(grouping);
        if (!sequences.contains(groupKey))
        {
            sequences.insert({ groupKey, {} });
        }

        auto &sequence = sequences[groupKey];

        // todo add more meta events like track name
        if (track->getPattern() != nullptr)
        {
            for (const auto *clip : track->getPattern()->getClips())
            {
                track->getSequence()->exportMidi(sequence, *clip,
                    simpleMapping, *this->generatedSequenceBuilder,
                    soloFlag, metronomeFlag,
                    this->beatRange.getStart(), this->beatRange.getEnd(),
                    midiClock);
            }
        }
        else
        {
            track->getSequence()->exportMidi(sequence, noTransform,
                simpleMapping, *this->generatedSequenceBuilder,
                soloFlag, metronomeFlag,
                this->beatRange.getStart(), this->beatRange.getEnd(),
                midiClock);
        }
    }

    for (const auto &i : sequences)
    {
        // the project will not necessarily start from 0 timestamp;
        // normally we don't care (not caring about that also makes the code simpler),
        // but when exporting to MIDI file, let's make sure the start is at zero:
        auto &sequence = sequences[i.first];
        sequence.addTimeToMessages(-this->beatRange.getStart() * midiClock);
        tempFile.addTrack(sequence);
    }

    return tempFile.writeTo(stream);
}

//===----------------------------------------------------------------------===//
// MidiTrackSource
//===----------------------------------------------------------------------===//

MidiTrack *ProjectNode::getTrackById(const String &trackId)
{
    this->rebuildTracksRefsCacheIfNeeded();
    return this->tracksRefsCache[trackId].get();
}

Pattern *ProjectNode::getPatternByTrackId(const String &trackId)
{
    this->rebuildTracksRefsCacheIfNeeded();
    if (auto track = this->tracksRefsCache[trackId].get())
    {
        return track->getPattern();
    }

    return nullptr;
}

MidiSequence *ProjectNode::getSequenceByTrackId(const String &trackId)
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

String ProjectNode::getVCSId() const
{
    return this->getId();
}

String ProjectNode::getVCSName() const
{
    return this->getName();
}

int ProjectNode::getNumTrackedItems()
{
    const ScopedReadLock lock(this->vcsInfoLock);
    return this->vcsItems.size();
}

VCS::TrackedItem *ProjectNode::getTrackedItem(int index)
{
    const ScopedReadLock lock(this->vcsInfoLock);
    return const_cast<VCS::TrackedItem *>(this->vcsItems[index]);
}

VCS::TrackedItem *ProjectNode::initTrackedItem(const Identifier &type,
    const Uuid &id, const VCS::TrackedItem &newState)
{
    if (type == Serialization::Core::pianoTrack)
    {
        auto *track = new PianoTrackNode("");
        track->setVCSUuid(id);
        this->addChildNode(track, -1, false);
        // add explicitly, since we aren't going to receive a notification:
        this->isTracksCacheOutdated = true;
        this->vcsItems.addIfNotAlreadyThere(track);
        track->resetStateTo(newState);
        return track;
    }
    if (type == Serialization::Core::automationTrack)
    {
        auto *track = new AutomationTrackNode("");
        track->setVCSUuid(id);
        this->addChildNode(track, -1, false);
        this->isTracksCacheOutdated = true;
        this->vcsItems.addIfNotAlreadyThere(track);
        track->resetStateTo(newState);
        return track;
    }
    else if (type == Serialization::Core::projectInfo)
    {
        this->metadata->setVCSUuid(id);
        this->metadata->resetStateTo(newState);
        return this->metadata.get();
    }
    else if (type == Serialization::Core::projectTimeline)
    {
        this->timeline->setVCSUuid(id);
        this->timeline->resetStateTo(newState);
        return this->timeline.get();
    }
    
    return nullptr;
}

bool ProjectNode::deleteTrackedItem(VCS::TrackedItem *item)
{
    if (auto *treeItem = dynamic_cast<MidiTrackNode *>(item))
    {
        TreeNode::deleteNode(treeItem, false); // don't broadcastRemoveTrack
        this->vcsItems.removeAllInstancesOf(item);
        this->isTracksCacheOutdated = true;
        return true;
    }

    return false;
}

void ProjectNode::onBeforeResetState()
{
    this->broadcastBeforeReloadProjectContent();
}

void ProjectNode::onResetState()
{
    this->lastShownTrack = nullptr;
    this->undoStack->clearUndoHistory();
    this->broadcastReloadProjectContent();

    auto range = this->broadcastChangeProjectBeatRange();
    ProjectNode::applyDefaultViewMargin(range);
    this->broadcastChangeViewBeatRange(range.getStart(), range.getEnd());

    // during vcs operations, notifications are not sent, including tree selection changes,
    // which happen, when something is deleted, so we need to do it afterwards:
    this->getRootNode()->findActiveNode()->sendSelectionNotification();
}

//===----------------------------------------------------------------------===//
// Command Palette
//===----------------------------------------------------------------------===//

Array<CommandPaletteActionsProvider *> ProjectNode::getCommandPaletteActionProviders()
{
    if (this->getLastFocusedRoll()->isShowing())
    {
        return { this->consoleTimelineEvents.get() };
    }

    return {};
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void ProjectNode::changeListenerCallback(ChangeBroadcaster *source)
{
    if (auto *vcs = dynamic_cast<VersionControl *>(source))
    {
        DocumentOwner::sendChangeMessage();
    }
}

void ProjectNode::rebuildTracksRefsCacheIfNeeded() const
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
        
        const auto children = this->findChildrenOfType<MidiTrack>();
        
        for (int i = 0; i < children.size(); ++i)
        {
            MidiTrack *const track = children.getUnchecked(i);
            this->tracksRefsCache[track->getTrackId()] = track;
        }
        
        this->isTracksCacheOutdated = false;
    }
}
