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
#include "MidiLayerTreeItem.h"
#include "LayerGroupTreeItem.h"

#include "TreeItemChildrenSerializer.h"
#include "ProjectTreeItem.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Icons.h"

#include "Pattern.h"
#include "PianoLayer.h"
#include "AutomationLayer.h"
#include "PianoRoll.h"
#include "Note.h"

#include "InstrumentTreeItem.h"
#include "Instrument.h"

#include "LayerCommandPanel.h"

MidiLayerTreeItem::MidiLayerTreeItem(const String &name) :
    TreeItem(name)
{
    // есть связанный с этим открытый баг, когда это поле остается нулевым. отстой.
    this->lastFoundParent = this->findParentOfType<ProjectTreeItem>();
    // здесь мы не оповещаем родительский проект о добавлении нового слоя,
    // т.к. при создании слой еще ни к кому не приаттачен
}

MidiLayerTreeItem::~MidiLayerTreeItem()
{
    this->lastFoundParent = this->findParentOfType<ProjectTreeItem>();
    
    if (this->lastFoundParent != nullptr)
    {
        this->removeItemFromParent();
        this->lastFoundParent->hideEditor(this->layer, this);
        this->lastFoundParent->broadcastRemoveLayer(this->layer);
        this->lastFoundParent->broadcastRemovePattern(this->pattern);
        LayerGroupTreeItem::removeAllEmptyGroupsInProject(this->lastFoundParent);
    }
}


bool MidiLayerTreeItem::isMuted() const
{
    return this->getLayer()->isMuted();
}

Colour MidiLayerTreeItem::getColour() const
{
    return this->layer->getColour().interpolatedWith(Colours::white, 0.4f);
}

void MidiLayerTreeItem::showPage()
{
    if (ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>())
    {
        parentProject->showEditor(this->layer, this);
    }
}

void MidiLayerTreeItem::onRename(const String &newName)
{
//    TreeItem::onRename(newName);
//    this->setXPath(this->getXPath()); // this performs sorting layers
    
    String fixedName = newName.replace("\\", "/");
    
    while (fixedName.contains("//"))
    {
        fixedName = fixedName.replace("//", "/");
    }
    
    this->setXPath(fixedName);
    
    // a hack - tells roll to update
    // upd. and wtf do we need this?
    // upd. it seems to lose the keyboard focus on rename somehow
//    if (this->isMarkerVisible())
//    {
//        this->showPage();
//    }
    
    TreeItem::notifySubtreeMoved(this); // сделать это default логикой для всех типов нодов?
}

void MidiLayerTreeItem::importMidi(const MidiMessageSequence &sequence)
{
    this->layer->importMidi(sequence);
}



//===----------------------------------------------------------------------===//
// VCS::TrackedItem
//===----------------------------------------------------------------------===//

String MidiLayerTreeItem::getVCSName() const
{
    return this->getXPath();
}


//===----------------------------------------------------------------------===//
// ProjectEventDispatcher
//===----------------------------------------------------------------------===//

String MidiLayerTreeItem::getXPath() const
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

void MidiLayerTreeItem::setXPath(const String &path)
{
    if (path == this->getXPath())
    {
        return;
    }
    
    // рассплитить путь и переместить себя в нужное место на дереве.
    // если таких групп не существует - создать.

    StringArray parts(StringArray::fromTokens(path, TreeItem::xPathSeparator, "'"));

    TreeItem *rootItem = this->lastFoundParent;

    jassert(parts.size() >= 1);

    for (int i = 0; i < (parts.size() - 1); ++i)
    {
        bool foundSubGroup = false;
        rootItem->setOpen(true);

        for (int j = 0; j < rootItem->getNumSubItems(); ++j)
        {
            if (LayerGroupTreeItem *group = dynamic_cast<LayerGroupTreeItem *>(rootItem->getSubItem(j)))
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
            auto group = new LayerGroupTreeItem(parts[i]);
            rootItem->addChildTreeItem(group);
            group->sortByNameAmongSiblings();
            rootItem = group;
        }
    }

    const String &newName = parts[parts.size() - 1];
    this->safeRename(newName);

    this->getParentItem()->removeSubItem(this->getIndexInParent(), false);


    // пройти по всем чайлдам группы
    // и вставить в нужное место, глядя на имена.

    bool foundRightPlace = false;
    int insertIndex = 0;
    String previousChildName = "";

    for (int i = 0; i < rootItem->getNumSubItems(); ++i)
    {
        String currentChildName;

        if (LayerGroupTreeItem *layerGroupItem = dynamic_cast<LayerGroupTreeItem *>(rootItem->getSubItem(i)))
        {
            currentChildName = layerGroupItem->getName();
        }
        else if (MidiLayerTreeItem *layerItem = dynamic_cast<MidiLayerTreeItem *>(rootItem->getSubItem(i)))
        {
            currentChildName = layerItem->getName();
        }
        else
        {
            continue;
        }

        insertIndex = i;

        if ((newName.compareIgnoreCase(previousChildName) > 0) &&
            (newName.compareIgnoreCase(currentChildName) <= 0))
        {
            foundRightPlace = true;
            break;
        }

        previousChildName = currentChildName;
    }

    if (!foundRightPlace) { ++insertIndex; }

    rootItem->addChildTreeItem(this, insertIndex);
    

    // cleanup. убираем все пустые группы
    if (ProjectTreeItem *parentProject = this->findParentOfType<ProjectTreeItem>())
    {
        LayerGroupTreeItem::removeAllEmptyGroupsInProject(parentProject);
    }
}

void MidiLayerTreeItem::dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeEvent(oldEvent, newEvent);
    }
}

void MidiLayerTreeItem::dispatchAddEvent(const MidiEvent &event)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastAddEvent(event);
    }
}

void MidiLayerTreeItem::dispatchRemoveEvent(const MidiEvent &event)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastRemoveEvent(event);
    }
}

void MidiLayerTreeItem::dispatchPostRemoveEvent(const MidiLayer *layer)
{
    if (this->lastFoundParent != nullptr)
    {
		this->lastFoundParent->broadcastPostRemoveEvent(layer);
    }
}

void MidiLayerTreeItem::dispatchReloadLayer(const MidiLayer *layer)
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeLayer(layer);
        this->repaintItem(); // if colour changed
    }
}

void MidiLayerTreeItem::dispatchChangeLayerBeatRange()
{
    if (this->lastFoundParent != nullptr)
    {
        this->lastFoundParent->broadcastChangeProjectBeatRange();
    }
}


void MidiLayerTreeItem::dispatchAddClip(const Clip &clip)
{
	// TODO
}


void MidiLayerTreeItem::dispatchChangeClip(const Clip &oldClip, const Clip &newClip)
{
	// TODO
}


void MidiLayerTreeItem::dispatchRemoveClip(const Clip &clip)
{
	// TODO
}


void MidiLayerTreeItem::dispatchPostRemoveClip(const Pattern *pattern)
{
	// TODO
}


void MidiLayerTreeItem::dispatchReloadPattern(const Pattern *pattern)
{
	// TODO
}


void MidiLayerTreeItem::dispatchChangePatternBeatRange()
{
	// TODO
}

ProjectTreeItem *MidiLayerTreeItem::getProject() const
{
    return this->lastFoundParent;
}


//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

var MidiLayerTreeItem::getDragSourceDescription()
{
    if (this->isCompactMode())
    { return var::null; }

    return Serialization::Core::layer;
}

void MidiLayerTreeItem::onItemMoved()
{
    if (this->lastFoundParent)
    {
        this->lastFoundParent->updateActiveGroupEditors();
        this->lastFoundParent->sendChangeMessage();
    }

    ProjectTreeItem *newParent = this->findParentOfType<ProjectTreeItem>();

    const bool parentProjectChanged = (this->lastFoundParent != newParent);
    const bool needsToRepaintEditor = (this->isMarkerVisible() && (this->lastFoundParent != nullptr) && parentProjectChanged);

    if (parentProjectChanged)
    {
        if (this->lastFoundParent)
        {
            this->lastFoundParent->broadcastRemoveLayer(this->layer);
            this->lastFoundParent->broadcastRemovePattern(this->pattern);
        }

        if (newParent)
        {
            newParent->broadcastAddLayer(this->layer);
            newParent->broadcastAddPattern(this->pattern);
            newParent->updateActiveGroupEditors();
        }

        this->lastFoundParent = newParent;
    }

    if (needsToRepaintEditor)
    {
        this->showPage();
    }
}

bool MidiLayerTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    bool isInterested = (dragSourceDetails.description == Serialization::Core::instrument);

    if (isInterested)
    { this->setOpen(true); }

    return isInterested;
}

void MidiLayerTreeItem::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
    if (TreeView *treeView = dynamic_cast<TreeView *>(dragSourceDetails.sourceComponent.get()))
    {
        TreeItem *selected = TreeItem::getSelectedItem(treeView);

        if (InstrumentTreeItem *iti = dynamic_cast<InstrumentTreeItem *>(selected))
        {
            this->layer->setInstrumentId(iti->getInstrumentIdAndHash());
        }
    }

    //TreeItem::itemDropped(dragSourceDetails, insertIndex);
}


//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

Component *MidiLayerTreeItem::createItemMenu()
{
    return new LayerCommandPanel(*this);
}
