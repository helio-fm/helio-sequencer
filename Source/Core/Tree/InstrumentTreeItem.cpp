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
#include "InstrumentTreeItem.h"
#include "TreeItemChildrenSerializer.h"
#include "MainLayout.h"
#include "Icons.h"
#include "SerializationKeys.h"

#include "Instrument.h"
#include "InstrumentEditor.h"
#include "InstrumentCommandPanel.h"
#include "InstrumentsPage.h"
#include "AudioPluginTreeItem.h"

#include "App.h"
#include "Workspace.h"
#include "AudioCore.h"


InstrumentTreeItem::InstrumentTreeItem(Instrument *targetInstrument) :
    TreeItem({}, Serialization::Core::instrument),
    instrument(targetInstrument),
    instrumentEditor(nullptr)
{
    this->audioCore = &App::Workspace().getAudioCore();
    
    if (this->instrument != nullptr)
    {
        this->name = this->instrument->getName();
        this->initInstrumentEditor();
    }
}

InstrumentTreeItem::~InstrumentTreeItem()
{
    if (! this->instrument.wasObjectDeleted())
    {
        this->audioCore->removeInstrument(this->instrument);
    }

    this->deleteAllSubItems(); // перед отключением инструмента уберем все редакторы
    this->removeInstrumentEditor();
}

Colour InstrumentTreeItem::getColour() const
{
    return Colour(0xffff80f3).interpolatedWith(Colour(0xffa489ff), 0.5f);
    //return Colour(0xffd151ff);
}

Image InstrumentTreeItem::getIcon() const
{
    return Icons::findByName(Icons::instrumentGraph, TREE_ICON_HEIGHT);
}

void InstrumentTreeItem::showPage()
{
    if (this->instrument.wasObjectDeleted())
    { 
        delete this;
        return;
    }

    App::Layout().showPage(this->instrumentEditor, this);
}

void InstrumentTreeItem::safeRename(const String &newName)
{
    if (this->instrument.wasObjectDeleted())
    { 
        delete this;
        return;
    }

    TreeItem::safeRename(newName);
    this->instrument->setName(newName);
    this->dispatchChangeTreeItemView();
}


//===----------------------------------------------------------------------===//
// Instrument
//===----------------------------------------------------------------------===//

Instrument *InstrumentTreeItem::getInstrument() const
{
    return this->instrument;
}

String InstrumentTreeItem::getInstrumentIdAndHash() const
{
    return this->instrument->getIdAndHash();
}

Array<uint32> InstrumentTreeItem::getInstrumentNodeIds() const
{
    Array<uint32> result;

    for (int i = 0; i < this->instrument->getNumNodes(); ++i)
    {
        result.add(this->instrument->getNode(i)->nodeId);
    }

    return result;
}

bool InstrumentTreeItem::hasInstrumentWithNodeId(uint32 targetNodeId) const
{
    for (const uint32 existingNodeId : this->getInstrumentNodeIds())
    {
        if (existingNodeId == targetNodeId)
        {
            return true;
        }
    }

    return false;
}

TreeItem *InstrumentTreeItem::findAudioPluginEditorForNodeId(uint32 nodeId) const
{
    for (const auto audioPluginTreeItem : this->findChildrenOfType<AudioPluginTreeItem>())
    {
        if (audioPluginTreeItem->getNodeId() == nodeId)
        {
            return audioPluginTreeItem;
        }
    }

    return nullptr;
}


//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

var InstrumentTreeItem::getDragSourceDescription()
{
    return Serialization::Core::instrument;
}

bool InstrumentTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    const bool isInterested =
        (nullptr != dynamic_cast<PluginDescriptionWrapper *>(dragSourceDetails.description.getObject()));

    if (isInterested)
    { this->setOpen(true); }

    return isInterested;
}

void InstrumentTreeItem::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
    if (ListBox *list = dynamic_cast<ListBox *>(dragSourceDetails.sourceComponent.get()))
    {
        if (PluginDescriptionWrapper *pd = dynamic_cast<PluginDescriptionWrapper *>(dragSourceDetails.description.getObject()))
        {
            const PluginDescription pluginDescription(pd->pluginDescription);
            this->instrument->addNodeToFreeSpace(pluginDescription);
            this->updateChildrenEditors();
            //Console::setStatus("added " + pluginDescription->descriptiveName);
        }
    }

    TreeItem::itemDropped(dragSourceDetails, insertIndex);
}


//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

ScopedPointer<Component> InstrumentTreeItem::createItemMenu()
{
    return new InstrumentCommandPanel(*this);
}

void InstrumentTreeItem::updateChildrenEditors()
{
    this->deleteAllSubItems();

    for (int i = 0; i < this->instrument->getNumNodes(); ++i)
    {
        const AudioProcessorGraph::Node::Ptr node = this->instrument->getNode(i);

        if (AudioProcessorGraph::AudioGraphIOProcessor *io =
                    dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor *>(node->getProcessor()))
        {
            continue; // для девайсов ввода-вывода показывать нечего, пропускаем
        }

        // неюзабельно, используем только на мобилках
        AudioPluginTreeItem *ap = new AudioPluginTreeItem(node->nodeId, node->getProcessor()->getName());
        this->addChildTreeItem(ap);
    }
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *InstrumentTreeItem::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::treeItem);
    xml->setAttribute(Serialization::Core::treeItemType, this->type);
    xml->setAttribute(Serialization::Core::treeItemName, this->name);
    xml->setAttribute("id", this->instrument->getIdAndHash());
    return xml;
}

void InstrumentTreeItem::deserialize(const XmlElement &xml)
{
    this->reset();

    const String id = xml.getStringAttribute("id");
    this->instrument = this->audioCore->findInstrumentById(id);

    // если в аудиоядре инструмент исчез:
    if (this->instrument == nullptr)
    {
        delete this;
        return;
    }

    this->initInstrumentEditor();

    // Proceed with basic properties and children
    TreeItem::deserialize(xml);

    this->updateChildrenEditors();
}

void InstrumentTreeItem::initInstrumentEditor()
{
    if (this->instrumentEditor == nullptr)
    {
        this->instrumentEditor = new InstrumentEditor(*this->instrument, this->audioCore);
        this->instrumentEditor->updateComponents();
    }
}

void InstrumentTreeItem::removeInstrumentEditor()
{
    if (this->instrumentEditor != nullptr)
    {
        this->instrumentEditor = nullptr;
    }
}
