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
#include "MidiTrackTreeItem.h"
#include "TrackGroupTreeItem.h"

#include "TreeItemChildrenSerializer.h"
#include "ProjectTreeItem.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Icons.h"

#include "Pattern.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "PianoRoll.h"
#include "Note.h"

#include "InstrumentTreeItem.h"
#include "Instrument.h"

#include "MidiTrackCommandPanel.h"

#include "UndoStack.h"
#include "MidiTrackActions.h"

MidiTrackTreeItem::MidiTrackTreeItem(const String &name, const Identifier &type) :
    TreeItem(name, type),
    colour(Colours::white), // TODO random color from my set
    channel(1),
    instrumentId(String::empty),
    controllerNumber(0),
    mute(false),
    solo(false)
{
    this->lastFoundParent = this->findParentOfType<ProjectTreeItem>();
    // do not dispatch new track events here,
    // as newly created track is not attached to any parent
}

MidiTrackTreeItem::~MidiTrackTreeItem()
{
    this->lastFoundParent = this->findParentOfType<ProjectTreeItem>();
    
    if (this->lastFoundParent != nullptr)
    {
        // Important: first notify
        this->lastFoundParent->hideEditor(this, this);
        this->lastFoundParent->broadcastRemoveTrack(this);
        // Then disconnect from the tree
        this->removeItemFromParent();
        TrackGroupTreeItem::removeAllEmptyGroupsInProject(this->lastFoundParent);
    }
}

Colour MidiTrackTreeItem::getColour() const noexcept
{
    return this->getTrackColour().interpolatedWith(Colours::white, 0.4f);
}

void MidiTrackTreeItem::showPage()
{
    if (ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>())
    {
        parentProject->showLinearEditor(this, this);
    }
}

void MidiTrackTreeItem::safeRename(const String &newName)
{
    String fixedName = newName.replace("\\", "/");
    
    while (fixedName.contains("//"))
    {
        fixedName = fixedName.replace("//", "/");
    }
    
    this->setXPath(fixedName);
}

void MidiTrackTreeItem::importMidi(const MidiMessageSequence &sequence)
{
    this->sequence->importMidi(sequence);
}

//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String MidiTrackTreeItem::getVCSName() const
{
    return this->getXPath();
}

ValueTree MidiTrackTreeItem::serializeClipsDelta() const
{
    ValueTree tree(Serialization::VCS::PatternDeltas::clipsAdded);

    for (int i = 0; i < this->getPattern()->size(); ++i)
    {
        const auto clip = this->getPattern()->getUnchecked(i);
        tree.appendChild(clip->serialize(), nullptr);
    }

    return tree;
}

void MidiTrackTreeItem::resetClipsDelta(const ValueTree &state)
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

Uuid MidiTrackTreeItem::getTrackId() const noexcept
{
    return this->id;
}

void MidiTrackTreeItem::setTrackId(const Uuid &val)
{
    this->id = val;
}

String MidiTrackTreeItem::getTrackName() const noexcept
{
    return this->getXPath();
}

int MidiTrackTreeItem::getTrackChannel() const noexcept
{
    return this->channel;
}

void MidiTrackTreeItem::setTrackName(const String &val, bool sendNotifications)
{
    this->safeRename(val);
    if (sendNotifications)
    {
        this->dispatchChangeTrackProperties(this);
        this->dispatchChangeTreeItemView();
    }
}

Colour MidiTrackTreeItem::getTrackColour() const noexcept
{
    return this->colour;
}

void MidiTrackTreeItem::setTrackColour(const Colour &val, bool sendNotifications)
{
    if (this->colour != val)
    {
        this->colour = val;
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties(this);
            this->dispatchChangeTreeItemView();
        }
    }
}

String MidiTrackTreeItem::getTrackInstrumentId() const noexcept
{
    return this->instrumentId;
}

void MidiTrackTreeItem::setTrackInstrumentId(const String &val, bool sendNotifications)
{
    if (this->instrumentId != val)
    {
        this->instrumentId = val;
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties(this);
            // instrument id is not displayed anywhere, fix this is it does someday
            //this->dispatchChangeTreeItemView();
        }
    }
}

int MidiTrackTreeItem::getTrackControllerNumber() const noexcept
{
    return this->controllerNumber;
}

void MidiTrackTreeItem::setTrackControllerNumber(int val, bool sendNotifications)
{
    if (this->controllerNumber != val)
    {
        this->controllerNumber = val;
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties(this);
            // controller value is not displayed anywhere, fix this is it does someday
            //this->dispatchChangeTreeItemView();
        }
    }
}

bool MidiTrackTreeItem::isTrackMuted() const noexcept
{
    return this->mute;
}

void MidiTrackTreeItem::setTrackMuted(bool shouldBeMuted, bool sendNotifications)
{
    if (this->mute != shouldBeMuted)
    {
        this->mute = shouldBeMuted;
        if (sendNotifications)
        {
            this->dispatchChangeTrackProperties(this);
            this->dispatchChangeTreeItemView();
        }
    }
}

MidiSequence *MidiTrackTreeItem::getSequence() const noexcept
{
    return this->sequence;
}

Pattern *MidiTrackTreeItem::getPattern() const noexcept
{
    return this->pattern;
}

//===----------------------------------------------------------------------===//
// ProjectEventDispatcher
//===----------------------------------------------------------------------===//

String MidiTrackTreeItem::getXPath() const noexcept
{
    const TreeViewItem *rootItem = this;
    String xpath = this->getName();

    while (TreeViewItem *item = rootItem->getParentItem())
    {
        rootItem = item;

        if (ProjectTreeItem *parentProject = dynamic_cast<ProjectTreeItem *>(item))
        { return xpath; }

        if (TreeItem *treeItem = dynamic_cast<TreeItem *>(item))
        { xpath = treeItem->getName() + TreeItem::xPathSeparator + xpath; }
    }

    return xpath;
}

void MidiTrackTreeItem::setXPath(const String &path)
{
    if (path == this->getXPath())
    {
        return;
    }
    
    // Split path and move the item into a target place in a tree
    // If no matching groups found, create them

    StringArray parts(StringArray::fromTokens(path, TreeItem::xPathSeparator, "'"));

    TreeItem *rootItem = this->lastFoundParent;

    jassert(parts.size() >= 1);

    for (int i = 0; i < (parts.size() - 1); ++i)
    {
        bool foundSubGroup = false;
        rootItem->setOpen(true);

        for (int j = 0; j < rootItem->getNumSubItems(); ++j)
        {
            if (TrackGroupTreeItem *group = dynamic_cast<TrackGroupTreeItem *>(rootItem->getSubItem(j)))
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
            auto group = new TrackGroupTreeItem(parts[i]);
            rootItem->addChildTreeItem(group);
            group->sortByNameAmongSiblings();
            rootItem = group;
        }
    }

    this->name = TreeItem::createSafeName(parts[parts.size() - 1]);

    this->getParentItem()->removeSubItem(this->getIndexInParent(), false);

    // and insert into the right place depending on path
    bool foundRightPlace = false;
    int insertIndex = 0;
    String previousChildName = "";

    for (int i = 0; i < rootItem->getNumSubItems(); ++i)
    {
        String currentChildName;

        if (TrackGroupTreeItem *layerGroupItem = dynamic_cast<TrackGroupTreeItem *>(rootItem->getSubItem(i)))
        {
            currentChildName = layerGroupItem->getName();
        }
        else if (MidiTrackTreeItem *layerItem = dynamic_cast<MidiTrackTreeItem *>(rootItem->getSubItem(i)))
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
    rootItem->addChildTreeItem(this, insertIndex);
    
    // Cleanup all empty groups
    if (ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>())
    {
        TrackGroupTreeItem::removeAllEmptyGroupsInProject(parentProject);
    }
}

void MidiTrackTreeItem::dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeEvent(oldEvent, newEvent);
    }
}

void MidiTrackTreeItem::dispatchAddEvent(const MidiEvent &event)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastAddEvent(event);
    }
}

void MidiTrackTreeItem::dispatchRemoveEvent(const MidiEvent &event)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastRemoveEvent(event);
    }
}

void MidiTrackTreeItem::dispatchPostRemoveEvent(MidiSequence *const layer)
{
    jassert(layer == this->sequence);
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastPostRemoveEvent(layer);
    }
}

void MidiTrackTreeItem::dispatchChangeTrackProperties(MidiTrack *const track)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeTrackProperties(this);
    }
}

void MidiTrackTreeItem::dispatchChangeProjectBeatRange()
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeProjectBeatRange();
    }
}

void MidiTrackTreeItem::dispatchAddClip(const Clip &clip)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastAddClip(clip);
    }
}

void MidiTrackTreeItem::dispatchChangeClip(const Clip &oldClip, const Clip &newClip)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeClip(oldClip, newClip);
    }
}

void MidiTrackTreeItem::dispatchRemoveClip(const Clip &clip)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastRemoveClip(clip);
    }
}

void MidiTrackTreeItem::dispatchPostRemoveClip(Pattern *const pattern)
{
    jassert(pattern == this->pattern);
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastPostRemoveClip(pattern);
    }
}

ProjectTreeItem *MidiTrackTreeItem::getProject() const
{
    return this->lastFoundParent;
}


//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

var MidiTrackTreeItem::getDragSourceDescription()
{
    return Serialization::Core::track.toString();
}

void MidiTrackTreeItem::onItemParentChanged()
{
    if (this->lastFoundParent)
    {
        this->lastFoundParent->updateActiveGroupEditors();
        this->lastFoundParent->sendChangeMessage();
    }

    ProjectTreeItem *newParent = this->findParentOfType<ProjectTreeItem>();

    const bool parentProjectChanged = (this->lastFoundParent != newParent);
    const bool needsToRepaintEditor = (this->isMarkerVisible() &&
        (this->lastFoundParent != nullptr) && parentProjectChanged);

    if (parentProjectChanged)
    {
        if (this->lastFoundParent)
        {
            this->lastFoundParent->broadcastRemoveTrack(this);
        }

        if (newParent)
        {
            newParent->broadcastAddTrack(this);
            newParent->updateActiveGroupEditors();
        }

        this->lastFoundParent = newParent;
    }

    if (needsToRepaintEditor)
    {
        this->showPage();
    }
}

bool MidiTrackTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    bool isInterested = (Serialization::Core::instrumentRoot.toString() == dragSourceDetails.description.toString());

    if (isInterested)
    { this->setOpen(true); }

    return isInterested;
}

void MidiTrackTreeItem::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
    if (TreeView *treeView = dynamic_cast<TreeView *>(dragSourceDetails.sourceComponent.get()))
    {
        TreeItem *selected = TreeItem::getSelectedItem(treeView);

        if (InstrumentTreeItem *iti = dynamic_cast<InstrumentTreeItem *>(selected))
        {
            this->setTrackInstrumentId(iti->getInstrumentIdAndHash(), true);
        }
    }
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool MidiTrackTreeItem::hasMenu() const noexcept
{
    return true;
}

ScopedPointer<Component> MidiTrackTreeItem::createMenu()
{
    return new MidiTrackCommandPanel(*this);
}

//===----------------------------------------------------------------------===//
// Callbacks
//===----------------------------------------------------------------------===//

Function<void(const String &text)> MidiTrackTreeItem::getRenameCallback()
{
    return[this](const String &text)
    {
        if (text != this->getXPath())
        {
            auto project = this->getProject();
            const auto trackId = this->getTrackId().toString();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackRenameAction(*project, trackId, text));
        }
    };
}

Function<void(const String &text)> MidiTrackTreeItem::getChangeColourCallback()
{
    return[this](const String &text)
    {
        const Colour colour(Colour::fromString(text));
        if (colour != this->getColour())
        {
            auto project = this->getProject();
            const auto trackId = this->getTrackId().toString();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackChangeColourAction(*project, trackId, colour));
        }
    };
}

Function<void(const String &instrumentId)> MidiTrackTreeItem::getChangeInstrumentCallback()
{
    return[this](const String &instrumentId)
    {
        if (instrumentId != this->getTrackInstrumentId())
        {
            auto project = this->getProject();
            const auto trackId = this->getTrackId().toString();
            project->getUndoStack()->beginNewTransaction();
            project->getUndoStack()->perform(new MidiTrackChangeInstrumentAction(*project, trackId, instrumentId));
        }
    };
}
