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
#include "InstrumentNode.h"
#include "TreeNodeSerializer.h"
#include "MainLayout.h"
#include "Icons.h"
#include "SerializationKeys.h"

#include "Instrument.h"
#include "InstrumentEditor.h"
#include "InstrumentMenu.h"
#include "OrchestraPit.h"
#include "OrchestraPitNode.h"
#include "AudioPluginNode.h"
#include "Workspace.h"
#include "AudioCore.h"


InstrumentNode::InstrumentNode(Instrument *targetInstrument) :
    TreeNode({}, Serialization::Core::instrumentRoot),
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

InstrumentNode::~InstrumentNode()
{
    // cleanup UI before unplugging an instrument
    this->deleteAllChildren();
    this->removeInstrumentEditor();

    if (!this->instrument.wasObjectDeleted())
    {
        this->audioCore->removeInstrument(this->instrument);
    }
}

Image InstrumentNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::instrument, HEADLINE_ICON_SIZE);
}

void InstrumentNode::showPage()
{
    if (this->instrument.wasObjectDeleted())
    {
        delete this;
        return;
    }

    App::Layout().showPage(this->instrumentEditor, this);
}

void InstrumentNode::safeRename(const String &newName, bool sendNotifications)
{
    if (this->instrument.wasObjectDeleted())
    { 
        delete this;
        return;
    }

    TreeNode::safeRename(newName, sendNotifications);
    this->instrument->setName(newName);

    if (sendNotifications)
    {
        this->notifyOrchestraChanged();
    }
}


//===----------------------------------------------------------------------===//
// Instrument
//===----------------------------------------------------------------------===//

WeakReference<Instrument> InstrumentNode::getInstrument() const noexcept
{
    return this->instrument;
}

String InstrumentNode::getInstrumentIdAndHash() const
{
    return this->instrument->getIdAndHash();
}

Array<uint32> InstrumentNode::getInstrumentNodeIds() const
{
    Array<uint32> result;

    for (int i = 0; i < this->instrument->getNumNodes(); ++i)
    {
        result.add(this->instrument->getNode(i)->nodeID.uid);
    }

    return result;
}

bool InstrumentNode::hasInstrumentWithNodeId(uint32 targetNodeId) const
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

TreeNode *InstrumentNode::findAudioPluginEditorForNodeId(AudioProcessorGraph::NodeID nodeId) const
{
    for (const auto audioPluginTreeItem : this->findChildrenOfType<AudioPluginNode>())
    {
        if (audioPluginTreeItem->getNodeId() == nodeId)
        {
            return audioPluginTreeItem;
        }
    }

    return nullptr;
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool InstrumentNode::hasMenu() const noexcept
{
    return true;
}

ScopedPointer<Component> InstrumentNode::createMenu()
{
    return new InstrumentMenu(*this, App::Workspace().getPluginManager());
}

void InstrumentNode::updateChildrenEditors()
{
    this->deleteAllChildren();

    for (int i = 0; i < this->instrument->getNumNodes(); ++i)
    {
        const AudioProcessorGraph::Node::Ptr node = this->instrument->getNode(i);

        if (AudioProcessorGraph::AudioGraphIOProcessor *io =
                    dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor *>(node->getProcessor()))
        {
            continue; // для девайсов ввода-вывода показывать нечего, пропускаем
        }

        // неюзабельно, используем только на мобилках
        auto *ap = new AudioPluginNode(node->nodeID, node->getProcessor()->getName());
        this->addChildTreeItem(ap);
    }
}

//===----------------------------------------------------------------------===//
// Callbacks
//===----------------------------------------------------------------------===//

Function<void(const String &text)> InstrumentNode::getRenameCallback()
{
    return [this](const String &text)
    {
        if (text != this->getName())
        {
            this->safeRename(text, true);
        }
    };
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree InstrumentNode::serialize() const
{
    ValueTree tree(Serialization::Core::treeNode);
    tree.setProperty(Serialization::Core::treeNodeType, this->type, nullptr);
    tree.setProperty(Serialization::Core::treeNodeName, this->name, nullptr);
    tree.setProperty(Serialization::Audio::instrumentId, this->instrument->getIdAndHash(), nullptr);
    return tree;
}

void InstrumentNode::deserialize(const ValueTree &tree)
{
    this->reset();

    const String id = tree.getProperty(Serialization::Audio::instrumentId);
    this->instrument = this->audioCore->findInstrumentById(id);

    if (this->instrument == nullptr ||
        !this->instrument->isValid())
    {
        delete this;
        return;
    }

    this->initInstrumentEditor();

    // Proceed with basic properties and children
    TreeNode::deserialize(tree);

    this->updateChildrenEditors();
}

void InstrumentNode::initInstrumentEditor()
{
    if (this->instrumentEditor == nullptr)
    {
        this->instrumentEditor = new InstrumentEditor(this->instrument, this->audioCore);
        this->instrumentEditor->updateComponents();
    }
}

void InstrumentNode::removeInstrumentEditor()
{
    if (this->instrumentEditor != nullptr)
    {
        this->instrumentEditor = nullptr;
    }
}

void InstrumentNode::notifyOrchestraChanged()
{
    if (auto *orchestra = dynamic_cast<OrchestraPitNode *>(this->getParent()))
    {
        orchestra->sendChangeMessage();
    }
}
