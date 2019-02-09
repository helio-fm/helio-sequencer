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
#include "MidiTrackNode.h"
#include "TrackGroupNode.h"

#include "TreeNodeSerializer.h"
#include "ProjectNode.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Icons.h"

#include "Pattern.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "PianoRoll.h"
#include "Note.h"

#include "InstrumentNode.h"
#include "Instrument.h"

#include "MidiTrackMenu.h"

#include "UndoStack.h"
#include "MidiTrackActions.h"

MidiTrackNode::MidiTrackNode(const String &name, const Identifier &type) :
    TreeNode(name, type),
    id(Uuid().toString()),
    colour(Colours::white),
    channel(1),
    controllerNumber(0),
    mute(false),
    solo(false)
{
    this->lastFoundParent = this->findParentOfType<ProjectNode>();
    // do not dispatch new track events here,
    // as newly created track is not attached to any parent
}

MidiTrackNode::~MidiTrackNode()
{

}

void MidiTrackNode::showPage()
{
    if (ProjectNode *parentProject = this->findParentOfType<ProjectNode>())
    {
        parentProject->showLinearEditor(this, this);
    }
}

void MidiTrackNode::safeRename(const String &newName, bool sendNotifications)
{
    String fixedName = newName.replace("\\", "/");
    
    while (fixedName.contains("//"))
    {
        fixedName = fixedName.replace("//", "/");
    }
    
    this->setXPath(fixedName, sendNotifications);
}

void MidiTrackNode::importMidi(const MidiMessageSequence &sequence, short timeFormat)
{
    this->sequence->importMidi(sequence, timeFormat);
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String MidiTrackNode::getVCSName() const
{
    return this->getXPath();
}

ValueTree MidiTrackNode::serializeClipsDelta() const
{
    ValueTree tree(Serialization::VCS::PatternDeltas::clipsAdded);

    for (int i = 0; i < this->getPattern()->size(); ++i)
    {
        const auto clip = this->getPattern()->getUnchecked(i);
        tree.appendChild(clip->serialize(), nullptr);
    }

    return tree;
}

void MidiTrackNode::resetClipsDelta(const ValueTree &state)
{
    jassert(state.hasType(Serialization::VCS::PatternDeltas::clipsAdded));

    //this->reset(); // TODO test
    this->getPattern()->reset();

    Pattern *pattern = this->getPattern();
    forEachValueTreeChildWithType(state, e, Serialization::Midi::clip)
    {
        Clip c(pattern);
        c.deserialize(e);
        pattern->silentImport(c);
    }
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
    return this->getXPath();
}

int MidiTrackNode::getTrackChannel() const noexcept
{
    return this->channel;
}

void MidiTrackNode::setTrackName(const String &val, bool sendNotifications)
{
    this->safeRename(val, sendNotifications);
    if (sendNotifications)
    {
        this->dispatchChangeTrackProperties(this);
        this->dispatchChangeTreeNodeViews();
    }
}

Colour MidiTrackNode::getTrackColour() const noexcept
{
    return this->colour;
}

void MidiTrackNode::setTrackColour(const Colour &val, bool sendNotifications)
{
    if (this->colour != val)
    {
        this->colour = val;
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties(this);
            this->dispatchChangeTreeNodeViews();
        }
    }
}

String MidiTrackNode::getTrackInstrumentId() const noexcept
{
    return this->instrumentId;
}

void MidiTrackNode::setTrackInstrumentId(const String &val, bool sendNotifications)
{
    if (this->instrumentId != val)
    {
        this->instrumentId = val;
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties(this);
            // instrument id is not displayed anywhere, fix this is it does someday
            //this->dispatchChangeTreeNodeViews();
        }
    }
}

int MidiTrackNode::getTrackControllerNumber() const noexcept
{
    return this->controllerNumber;
}

void MidiTrackNode::setTrackControllerNumber(int val, bool sendNotifications)
{
    if (this->controllerNumber != val)
    {
        this->controllerNumber = val;
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties(this);
            // controller value is not displayed anywhere, fix this is it does someday
            //this->dispatchChangeTreeNodeViews();
        }
    }
}

bool MidiTrackNode::isTrackMuted() const noexcept
{
    return this->mute;
}

void MidiTrackNode::setTrackMuted(bool shouldBeMuted, bool sendNotifications)
{
    if (this->mute != shouldBeMuted)
    {
        this->mute = shouldBeMuted;
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties(this);
            this->dispatchChangeTreeNodeViews();
        }
    }
}

MidiSequence *MidiTrackNode::getSequence() const noexcept
{
    return this->sequence;
}

Pattern *MidiTrackNode::getPattern() const noexcept
{
    return this->pattern;
}

//===----------------------------------------------------------------------===//
// ProjectEventDispatcher
//===----------------------------------------------------------------------===//

String MidiTrackNode::getXPath() const noexcept
{
    const TreeNodeBase *rootItem = this;
    String xpath = this->getName();

    while (TreeNodeBase *item = rootItem->getParent())
    {
        rootItem = item;

        if (ProjectNode *parentProject = dynamic_cast<ProjectNode *>(item))
        {
            return xpath;
        }

        if (TreeNode *treeItem = dynamic_cast<TreeNode *>(item))
        {
            xpath = treeItem->getName() + TreeNode::xPathSeparator + xpath;
        }
    }

    return xpath;
}

void MidiTrackNode::setXPath(const String &path, bool sendNotifications)
{
    if (path == this->getXPath())
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
            if (TrackGroupNode *group = dynamic_cast<TrackGroupNode *>(rootItem->getChild(j)))
            {
                if (group->getName() == parts[i])
                {
                    foundSubGroup = true;
                    rootItem = group;
                    break;
                }
            }
        }

        if (! foundSubGroup)
        {
            auto group = new TrackGroupNode(parts[i]);
            rootItem->addChildTreeItem(group);
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

        if (TrackGroupNode *layerGroupItem = dynamic_cast<TrackGroupNode *>(rootItem->getChild(i)))
        {
            currentChildName = layerGroupItem->getName();
        }
        else if (MidiTrackNode *layerItem = dynamic_cast<MidiTrackNode *>(rootItem->getChild(i)))
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

    if (!foundRightPlace) { ++insertIndex; }

    // This will also send changed-parent notifications
    rootItem->addChildTreeItem(this, insertIndex, sendNotifications);
    
    // Cleanup all empty groups
    if (ProjectNode *parentProject = this->findParentOfType<ProjectNode>())
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
    jassert(layer == this->sequence);
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastPostRemoveEvent(layer);
    }
}

void MidiTrackNode::dispatchChangeTrackProperties(MidiTrack *const track)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeTrackProperties(this);
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
    jassert(pattern == this->pattern);
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

void MidiTrackNode::onItemAddedToTree(bool sendNotifications)
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

void MidiTrackNode::onItemDeletedFromTree(bool sendNotifications)
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

ScopedPointer<Component> MidiTrackNode::createMenu()
{
    return new MidiTrackMenu(*this);
}

//===----------------------------------------------------------------------===//
// Callbacks
//===----------------------------------------------------------------------===//

Function<void(const String &text)> MidiTrackNode::getRenameCallback()
{
    return [this](const String &text)
    {
        if (text != this->getXPath())
        {
            auto project = this->getProject();
            const auto &trackId = this->getTrackId();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackRenameAction(*project, trackId, text));
        }
    };
}

Function<void(const String &text)> MidiTrackNode::getChangeColourCallback()
{
    return [this](const String &text)
    {
        const Colour colour(Colour::fromString(text));
        if (colour != this->getTrackColour())
        {
            auto project = this->getProject();
            const auto &trackId = this->getTrackId();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackChangeColourAction(*project, trackId, colour));
        }
    };
}

Function<void(const String &instrumentId)> MidiTrackNode::getChangeInstrumentCallback()
{
    return [this](const String &instrumentId)
    {
        if (instrumentId != this->getTrackInstrumentId())
        {
            auto project = this->getProject();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackChangeInstrumentAction(*project, this->getTrackId(), instrumentId));
        }
    };
}
