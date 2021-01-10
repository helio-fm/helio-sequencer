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

#include "Instrument.h"
#include "InstrumentEditor.h"
#include "InstrumentMenu.h"

#include "OrchestraPit.h"
#include "OrchestraPitNode.h"

#include "KeyboardMappingPage.h"
#include "AudioPluginEditorPage.h"
#include "PluginWindow.h"

#include "MainLayout.h"
#include "Workspace.h"
#include "AudioCore.h"

InstrumentNode::InstrumentNode(Instrument *targetInstrument) :
    TreeNode({}, Serialization::Core::instrumentRoot),
    instrument(targetInstrument),
    instrumentEditor(nullptr)
{
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
        App::Workspace().getAudioCore().removeInstrument(this->instrument);
    }
}

Image InstrumentNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::instrument, Globals::UI::headlineIconSize);
}

void InstrumentNode::showPage()
{
    if (this->instrument.wasObjectDeleted())
    {
        this->removeFromOrchestraAndDelete();
        return;
    }

    App::Layout().showPage(this->instrumentEditor.get(), this);
}

void InstrumentNode::safeRename(const String &newName, bool sendNotifications)
{
    if (this->instrument.wasObjectDeleted())
    { 
        this->removeFromOrchestraAndDelete();
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

UniquePointer<Component> InstrumentNode::createMenu()
{
    return make<InstrumentMenu>(*this,
        App::Workspace().getPluginManager(),
        App::Workspace().getAudioCore());
}

void InstrumentNode::recreateChildrenEditors()
{
    this->deleteAllChildren();

    for (int i = 0; i < this->instrument->getNumNodes(); ++i)
    {
        const auto node = this->instrument->getNode(i);

        if (auto *io = dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor *>(node->getProcessor()))
        {
            continue; // no editors available for the standard io nodes
        }

        // a node for the plugin UI page (only used on mobiles):
        this->addChildNode(new AudioPluginNode(node->nodeID, node->getProcessor()->getName()));
    }

    // a node for the keyboard mapping editor page
    this->addChildNode(new KeyboardMappingNode(this->instrument));
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

SerializedData InstrumentNode::serialize() const
{
    SerializedData tree(Serialization::Core::treeNode);
    tree.setProperty(Serialization::Core::treeNodeType, this->type);
    tree.setProperty(Serialization::Core::treeNodeName, this->name);
    tree.setProperty(Serialization::Audio::instrumentId, this->instrument->getIdAndHash());
    // not serializing the children editor nodes: they are all temporary
    return tree;
}

void InstrumentNode::deserialize(const SerializedData &data)
{
    this->reset();

    const String id = data.getProperty(Serialization::Audio::instrumentId);

    this->instrument = App::Workspace().getAudioCore().findInstrumentById(id);

    if (this->instrument == nullptr)
    {
        this->removeFromOrchestraAndDelete();
        return;
    }

    // Proceed with basic properties and children
    TreeNode::deserialize(data);

    if (this->instrument != nullptr)
    {
        this->name = this->instrument->getName();
        this->initInstrumentEditor();
    }

    this->recreateChildrenEditors();
}

void InstrumentNode::initInstrumentEditor()
{
    if (this->instrumentEditor == nullptr)
    {
        auto *audioCore = &App::Workspace().getAudioCore();
        this->instrumentEditor = make<InstrumentEditor>(this->instrument, audioCore);
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

void InstrumentNode::removeFromOrchestraAndDelete()
{
    if (auto *parent = dynamic_cast<OrchestraPitNode *>(this->getParent()))
    {
        parent->removeInstrumentNode(this);
    }
}


//===----------------------------------------------------------------------===//
// Audio plugin UI editor node
//===----------------------------------------------------------------------===//

class HelioAudioProcessorEditor final : public GenericAudioProcessorEditor
{
public:

    explicit HelioAudioProcessorEditor(AudioProcessor &p) :
        GenericAudioProcessorEditor(p)
    {
        this->setFocusContainer(true);

        // установим ему масимальную высоту
        for (int i = 0; i < this->getNumChildComponents(); ++i)
        {
            if (auto *panel = dynamic_cast<PropertyPanel *>(this->getChildComponent(i)))
            {
                this->setSize(this->getWidth(), panel->getTotalContentHeight());
            }
        }
    }
};

AudioPluginNode::AudioPluginNode(AudioProcessorGraph::NodeID pluginID, const String &name) :
    TreeNode(name, Serialization::Audio::audioPlugin),
    audioPluginEditor(nullptr),
    nodeId(pluginID) {}

bool AudioPluginNode::hasMenu() const noexcept
{
    return false;
}

UniquePointer<Component> AudioPluginNode::createMenu()
{
    return nullptr;
}

Image AudioPluginNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::audioPlugin, Globals::UI::headlineIconSize);
}

AudioProcessorGraph::NodeID AudioPluginNode::getNodeId() const noexcept
{
    return this->nodeId;
}

void AudioPluginNode::showPage()
{
    const auto instrument =
        this->findParentOfType<InstrumentNode>()->getInstrument();

    if (instrument == nullptr)
    {
        delete this;
        return;
    }

    const AudioProcessorGraph::Node::Ptr f(instrument->getNodeForId(this->nodeId));

    if (f == nullptr)
    {
        delete this;
        return;
    }

    if (!this->audioPluginEditor)
    {
        if (f->getProcessor()->hasEditor())
        {
            // Some plugins (including Kontakt 3!) misbehavior messes up all the controls
            // Turns out they attach themselves to the parent window :(
            // So we cannot add them as a child component like that:
            // ui = f->getProcessor()->createEditorIfNeeded();
            // so we try to mimic that by creating a plugin window
            // while its size and position that is managed by audioPluginEditor
            if (auto *window = PluginWindow::getWindowFor(f, true))
            {
                this->audioPluginEditor = make<AudioPluginEditorPage>(window);
            }
        }
        else
        {
            auto *ui = new HelioAudioProcessorEditor(*f->getProcessor());
            auto *plugin = dynamic_cast<AudioPluginInstance *>(f->getProcessor());

            if (plugin != nullptr)
            {
                ui->setName(plugin->getName());
            }

            this->audioPluginEditor = make<AudioPluginEditorPage>(ui);
        }

        // Something went wrong
        if (!this->audioPluginEditor)
        {
            delete this;
            return;
        }
    }

    App::Layout().showPage(this->audioPluginEditor.get(), this);
}

//===----------------------------------------------------------------------===//
// Keyboard mapping menu & editor node
//===----------------------------------------------------------------------===//

class KeyboardMappingMenu final : public MenuPanel
{
public:

    KeyboardMappingMenu()
    {
        MenuPanel::Menu menu;

        menu.add(MenuItem::item(Icons::browse, CommandIDs::KeyMapLoadScala,
            TRANS(I18n::Menu::keyboardMappingLoadScala))->closesMenu());

        menu.add(MenuItem::item(Icons::reset, CommandIDs::KeyMapReset,
            TRANS(I18n::Menu::keyboardMappingReset))->closesMenu());

        menu.add(MenuItem::item(Icons::copy, CommandIDs::KeyMapCopyToClipboard,
            TRANS(I18n::Menu::copy))->closesMenu());

        menu.add(MenuItem::item(Icons::paste, CommandIDs::KeyMapPasteFromClipboard,
            TRANS(I18n::Menu::paste))->closesMenu());

        this->updateContent(menu, MenuPanel::SlideRight);
    }
};

KeyboardMappingNode::KeyboardMappingNode(WeakReference<Instrument> instrument) :
    TreeNode({}, Serialization::Midi::keyboardMapping),
    instrument(instrument) {}

String KeyboardMappingNode::getName() const noexcept
{
    return TRANS(I18n::Tree::keyboardMapping);
}

Image KeyboardMappingNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::piano, Globals::UI::headlineIconSize);
}

bool KeyboardMappingNode::hasMenu() const noexcept
{
    return true;
}

UniquePointer<Component> KeyboardMappingNode::createMenu()
{
    return make<KeyboardMappingMenu>();
}

void KeyboardMappingNode::showPage()
{
    if (this->instrument.wasObjectDeleted())
    {
        delete this;
        return;
    }

    if (this->keyboardMappingPage == nullptr)
    {
        this->keyboardMappingPage = make<KeyboardMappingPage>(this->instrument);
    }

    App::Layout().showPage(this->keyboardMappingPage.get(), this);
}
