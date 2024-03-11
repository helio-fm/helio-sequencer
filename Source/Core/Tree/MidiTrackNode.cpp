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
#include "MidiTrackNode.h"
#include "TrackGroupNode.h"

#include "ProjectNode.h"
#include "MainLayout.h"

#include "Pattern.h"
#include "PianoRoll.h"
#include "ClipMenu.h"

#include "UndoStack.h"
#include "MidiTrackActions.h"

MidiTrackNode::MidiTrackNode(const String &name, const Identifier &type) :
    TreeNode(name, type),
    id(Uuid().toString()),
    timeSignatureOverride(this)
{
    this->lastFoundParent = this->findParentOfType<ProjectNode>();
    // do not dispatch new track events here,
    // as newly created track is not attached to any parent
}

void MidiTrackNode::safeRename(const String &newName, bool sendNotifications)
{
    String fixedName = newName.replace("\\", "/");

    while (fixedName.contains("//"))
    {
        fixedName = fixedName.replace("//", "/");
    }

    this->setTreePath(fixedName, sendNotifications);
}

void MidiTrackNode::setSelected(NotificationType shouldNotify)
{
    const auto *firstClip = this->getPattern()->getClips().getFirst();
    this->setSelected(*firstClip, shouldNotify);
}

void MidiTrackNode::setSelected(const Clip &editableScope, NotificationType shouldNotify)
{
    this->selectedClipId = editableScope;
    TreeNode::setSelected(shouldNotify);
}

const Clip &MidiTrackNode::getSelectedClip() const
{
    const auto index = this->getPattern()->indexOfSorted(&this->selectedClipId);
    jassert(index >= 0);
    return index >= 0 ?
        *this->getPattern()->getClips()[index] : this->selectedClipId;
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String MidiTrackNode::getVCSName() const
{
    return this->getTreePath();
}

SerializedData MidiTrackNode::serializeClipsDelta() const
{
    SerializedData tree(Serialization::VCS::PatternDeltas::clipsAdded);

    for (int i = 0; i < this->getPattern()->size(); ++i)
    {
        const auto clip = this->getPattern()->getUnchecked(i);
        tree.appendChild(clip->serialize());
    }

    return tree;
}

void MidiTrackNode::resetClipsDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::PatternDeltas::clipsAdded));

    this->getPattern()->reset();

    Pattern *pattern = this->getPattern();
    forEachChildWithType(state, e, Serialization::Midi::clip)
    {
        Clip c(pattern);
        c.deserialize(e);
        pattern->silentImport(c);
    }
}

Colour MidiTrackNode::getRevisionDisplayColour() const
{
    return this->getTrackColour();
}

//===----------------------------------------------------------------------===//
// MidiTrack
//===----------------------------------------------------------------------===//

const String &MidiTrackNode::getTrackId() const noexcept
{
    return this->id;
}

void MidiTrackNode::setTrackId(const String &val)
{
    this->id = val;
}

String MidiTrackNode::getTrackName() const noexcept
{
    return this->getTreePath();
}

int MidiTrackNode::getTrackChannel() const noexcept
{
    return this->channel;
}

void MidiTrackNode::setTrackChannel(int channel, bool undoable, NotificationType notificationType)
{
    const auto clampedChannel = jlimit(1, 16, channel);
    if (this->channel == clampedChannel)
    {
        return;
    }

    if (undoable)
    {
        this->getProject()->getUndoStack()->
            perform(new MidiTrackChangeChannelAction(*this->getProject(),
                this->getTrackId(), clampedChannel));
    }
    else
    {
        this->channel = clampedChannel;
        if (notificationType != dontSendNotification)
        {
            this->dispatchChangeTrackProperties();
        }
    }
}

void MidiTrackNode::setTrackName(const String &newName, bool undoable, NotificationType notificationType)
{
    if (this->getTrackName() == newName)
    {
        return;
    }

    if (undoable)
    {
        this->getProject()->getUndoStack()->
            perform(new MidiTrackRenameAction(*this->getProject(), this->getTrackId(), newName));
    }
    else
    {
        const auto sendNotifications = notificationType != dontSendNotification;
        this->safeRename(newName, sendNotifications);
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties();
            this->dispatchChangeTreeNodeViews();
        }
    }
}

Colour MidiTrackNode::getTrackColour() const noexcept
{
    return this->colour;
}

void MidiTrackNode::setTrackColour(const Colour &val, bool undoable, NotificationType notificationType)
{
    if (this->colour == val)
    {
        return;
    }

    if (undoable)
    {
        this->getProject()->getUndoStack()->
            perform(new MidiTrackChangeColourAction(*this->getProject(), this->getTrackId(), val));
    }
    else
    {
        this->colour = val;

        if (notificationType != dontSendNotification)
        {
            this->dispatchChangeTrackProperties();
            this->dispatchChangeTreeNodeViews();
        }
    }
}

// contains both instrument id and hash concatenated
String MidiTrackNode::getTrackInstrumentId() const noexcept
{
    return this->instrumentId;
}

void MidiTrackNode::setTrackInstrumentId(const String &val, bool undoable, NotificationType notificationType)
{
    if (this->instrumentId == val)
    {
        return;
    }

    if (undoable)
    {
        this->getProject()->getUndoStack()->
            perform(new MidiTrackChangeInstrumentAction(*this->getProject(), this->getTrackId(), val));
    }
    else
    {
        this->instrumentId = val;

        if (notificationType != dontSendNotification)
        {
            this->dispatchChangeTrackProperties();

            if (this->isSelected())
            {
                this->dispatchChangeActiveMidiInputInstrument();
            }
        }
    }
}

int MidiTrackNode::getTrackControllerNumber() const noexcept
{
    return this->controllerNumber;
}

void MidiTrackNode::setTrackControllerNumber(int val, NotificationType notificationType)
{
    if (this->controllerNumber == val)
    {
        return;
    }
    
    // not undoable because it is only set once when the track is created
    this->controllerNumber = val;

    if (notificationType != dontSendNotification)
    {
        this->dispatchChangeTrackProperties();
    }
}

MidiSequence *MidiTrackNode::getSequence() const noexcept
{
    return this->sequence.get();
}

Pattern *MidiTrackNode::getPattern() const noexcept
{
    return this->pattern.get();
}

// Time signature override

bool MidiTrackNode::hasTimeSignatureOverride() const noexcept
{
    return this->timeSignatureOverride.isValid();
}

const TimeSignatureEvent *MidiTrackNode::getTimeSignatureOverride() const noexcept
{
    return &this->timeSignatureOverride;
}

void MidiTrackNode::setTimeSignatureOverride(const TimeSignatureEvent &ts, bool undoable, NotificationType notificationType)
{
    if (undoable)
    {
        this->getProject()->getUndoStack()->perform(new MidiTrackChangeTimeSignatureAction(*this->getProject(), this->getTrackId(), ts));
    }
    else
    {
        // a time signature can only be dragged within the sequence range minus 1 bar,
        // that might be useful, e.g. if the track starts with off-beat notes:
        const auto maxBeat = jmax(0.f, this->getSequence()->getLengthInBeats() - ts.getBarLengthInBeats());
        // it shouldn't be possible to pass the out-of-range time signature here,
        jassert(ts.getBeat() >= 0.f && ts.getBeat() <= maxBeat);
        // but let's constrain it anyway just to be safe:
        const auto constrainedBeat = jlimit(0.f, maxBeat, ts.getBeat());
        this->timeSignatureOverride.applyChanges(ts.withBeat(constrainedBeat));

        if (notificationType != dontSendNotification)
        {
            this->getProject()->broadcastChangeTrackProperties(this);
        }
    }
}

//===----------------------------------------------------------------------===//
// ProjectEventDispatcher
//===----------------------------------------------------------------------===//

String MidiTrackNode::getTreePath() const noexcept
{
    const TreeNodeBase *rootItem = this;
    String xpath = this->getName();

    while (auto *item = rootItem->getParent())
    {
        rootItem = item;

        if (auto *parentProject = dynamic_cast<ProjectNode *>(item))
        {
            return xpath;
        }

        if (auto *treeItem = dynamic_cast<TreeNode *>(item))
        {
            xpath = treeItem->getName() + TreeNode::xPathSeparator + xpath;
        }
    }

    return xpath;
}

void MidiTrackNode::setTreePath(const String &path, bool sendNotifications)
{
    if (path == this->getTreePath() || path.isEmpty())
    {
        return;
    }

    // Split path and move the item into a target place in a tree
    // If no matching groups found, create them

    StringArray parts(StringArray::fromTokens(path, TreeNode::xPathSeparator, "'"));

    TreeNode *rootItem = this->lastFoundParent;

    jassert(rootItem != nullptr);
    jassert(parts.size() >= 1);

    for (int i = 0; i < (parts.size() - 1); ++i)
    {
        bool foundSubGroup = false;

        for (int j = 0; j < rootItem->getNumChildren(); ++j)
        {
            if (auto *group = dynamic_cast<TrackGroupNode *>(rootItem->getChild(j)))
            {
                if (group->getName() == parts[i])
                {
                    foundSubGroup = true;
                    rootItem = group;
                    break;
                }
            }
        }

        if (!foundSubGroup)
        {
            auto group = new TrackGroupNode(parts[i]);
            rootItem->addChildNode(group);
            group->sortByNameAmongSiblings();
            rootItem = group;
        }
    }

    this->name = TreeNode::createSafeName(parts[parts.size() - 1]);

    this->getParent()->removeChild(this->getIndexInParent(), false);

    // and insert into the right place depending on path
    bool foundRightPlace = false;
    int insertIndex = 0;
    String previousChildName = "";

    for (int i = 0; i < rootItem->getNumChildren(); ++i)
    {
        String currentChildName;

        if (auto *layerGroupItem = dynamic_cast<TrackGroupNode *>(rootItem->getChild(i)))
        {
            currentChildName = layerGroupItem->getName();
        }
        else if (auto *layerItem = dynamic_cast<MidiTrackNode *>(rootItem->getChild(i)))
        {
            currentChildName = layerItem->getName();
        }
        else
        {
            continue;
        }

        insertIndex = i;

        if ((this->name.compareIgnoreCase(previousChildName) > 0) &&
            (this->name.compareIgnoreCase(currentChildName) <= 0))
        {
            foundRightPlace = true;
            break;
        }

        previousChildName = currentChildName;
    }

    if (!foundRightPlace)
    {
        ++insertIndex;
    }

    // This will also send changed-parent notifications
    rootItem->addChildNode(this, insertIndex, sendNotifications);

    // Cleanup all empty groups
    if (auto *parentProject = this->findParentOfType<ProjectNode>())
    {
        TrackGroupNode::removeAllEmptyGroupsInProject(parentProject);
    }
}

void MidiTrackNode::dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeEvent(oldEvent, newEvent);
    }
}

void MidiTrackNode::dispatchAddEvent(const MidiEvent &event)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastAddEvent(event);
    }
}

void MidiTrackNode::dispatchRemoveEvent(const MidiEvent &event)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastRemoveEvent(event);
    }
}

void MidiTrackNode::dispatchPostRemoveEvent(MidiSequence *const layer)
{
    jassert(layer == this->sequence.get());
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastPostRemoveEvent(layer);
    }
}

void MidiTrackNode::dispatchChangeTrackProperties()
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeTrackProperties(this);
    }
}

void MidiTrackNode::dispatchChangeActiveMidiInputInstrument()
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->handleChangeActiveMidiInputInstrument(this);
    }
}

void MidiTrackNode::dispatchChangeTrackBeatRange()
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeTrackBeatRange(this);
    }
}

void MidiTrackNode::dispatchChangeProjectBeatRange()
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeProjectBeatRange();
    }
}

void MidiTrackNode::dispatchAddClip(const Clip &clip)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastAddClip(clip);
    }
}

void MidiTrackNode::dispatchChangeClip(const Clip &oldClip, const Clip &newClip)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeClip(oldClip, newClip);
    }
}

void MidiTrackNode::dispatchRemoveClip(const Clip &clip)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastRemoveClip(clip);
    }
}

void MidiTrackNode::dispatchPostRemoveClip(Pattern *const pattern)
{
    jassert(pattern == this->pattern.get());
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastPostRemoveClip(pattern);
    }
}

ProjectNode *MidiTrackNode::getProject() const noexcept
{
    jassert(this->lastFoundParent != nullptr);
    return this->lastFoundParent;
}

//===----------------------------------------------------------------------===//
// Add to tree and remove from tree callbacks
//===----------------------------------------------------------------------===//

void MidiTrackNode::onNodeAddToTree(bool sendNotifications)
{
    auto *newParent = this->findParentOfType<ProjectNode>();
    jassert(newParent != nullptr);

    const bool parentHasChanged = (this->lastFoundParent != newParent);
    this->lastFoundParent = newParent;

    if (parentHasChanged &&
        sendNotifications &&
        this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastAddTrack(this);
    }
}

void MidiTrackNode::onNodeRemoveFromTree(bool sendNotifications)
{
    this->lastFoundParent = this->findParentOfType<ProjectNode>();
    if (this->lastFoundParent != nullptr)
    {
        if (sendNotifications)
        {
            // Important: first notify
            this->lastFoundParent->broadcastRemoveTrack(this);
        }

        // Then disconnect from the tree
        this->removeNodeFromParent();
        TrackGroupNode::removeAllEmptyGroupsInProject(this->lastFoundParent);
    }
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool MidiTrackNode::hasMenu() const noexcept
{
    return true;
}

UniquePointer<Component> MidiTrackNode::createMenu()
{
    return make<ClipMenu>(this->getSelectedClip(), this->getProject()->getUndoStack());
}

//===----------------------------------------------------------------------===//
// VCS helpers
//===----------------------------------------------------------------------===//

SerializedData MidiTrackNode::serializePathDelta() const
{
    using namespace Serialization::VCS;
    SerializedData tree(MidiTrackDeltas::trackPath);
    tree.setProperty(delta, this->getTrackName());
    return tree;
}

SerializedData MidiTrackNode::serializeColourDelta() const
{
    using namespace Serialization::VCS;
    SerializedData tree(MidiTrackDeltas::trackColour);
    tree.setProperty(delta, this->getTrackColour().toString());
    return tree;
}

SerializedData MidiTrackNode::serializeChannelDelta() const
{
    using namespace Serialization::VCS;
    SerializedData tree(MidiTrackDeltas::trackChannel);
    tree.setProperty(delta, this->getTrackChannel());
    return tree;
}

SerializedData MidiTrackNode::serializeInstrumentDelta() const
{
    using namespace Serialization::VCS;
    SerializedData tree(MidiTrackDeltas::trackInstrument);
    tree.setProperty(delta, this->getTrackInstrumentId());
    return tree;
}

SerializedData MidiTrackNode::serializeTimeSignatureDelta() const
{
    using namespace Serialization::VCS;
    SerializedData tree(TimeSignatureDeltas::timeSignaturesChanged);
    if (this->hasTimeSignatureOverride())
    {
        tree.appendChild(this->timeSignatureOverride.serialize());
    }
    return tree;
}

void MidiTrackNode::resetPathDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackPath));
    const String newName = state.getProperty(Serialization::VCS::delta);
    this->setTrackName(newName, false, dontSendNotification);
}

void MidiTrackNode::resetColourDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackColour));
    const String colourString = state.getProperty(Serialization::VCS::delta);
    const auto colour = Colour::fromString(colourString);
    this->setTrackColour(colour, false, dontSendNotification);
}

void MidiTrackNode::resetChannelDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackChannel));
    const int channel = state.getProperty(Serialization::VCS::delta, 1);
    this->setTrackChannel(channel, false, dontSendNotification);
}

void MidiTrackNode::resetInstrumentDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::MidiTrackDeltas::trackInstrument));
    const String instrumentId = state.getProperty(Serialization::VCS::delta);
    this->setTrackInstrumentId(instrumentId, false, dontSendNotification);
}

void MidiTrackNode::resetTimeSignatureDelta(const SerializedData &state)
{
    jassert(state.hasType(Serialization::VCS::TimeSignatureDeltas::timeSignaturesChanged));

    this->timeSignatureOverride.reset();
    forEachChildWithType(state, e, Serialization::Midi::timeSignature)
    {
        this->timeSignatureOverride.deserialize(e);
    }
}
