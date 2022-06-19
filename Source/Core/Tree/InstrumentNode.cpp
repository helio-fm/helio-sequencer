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

#include "KeyboardMapping.h"
#include "KeyboardMappingPage.h"
#include "AudioPluginEditorPage.h"
#include "PluginWindow.h"

#include "SerializationKeys.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "Config.h"

InstrumentNode::InstrumentNode(Instrument *targetInstrument) :
    TreeNode({}, Serialization::Core::instrumentRoot),
    instrument(targetInstrument)
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
}

Image InstrumentNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::instrument, Globals::UI::headlineIconSize);
}

void InstrumentNode::showPage()
{
    if (this->instrument == nullptr)
    {
        jassertfalse;
        return;
    }

    App::Layout().showPage(this->instrumentEditor.get(), this);
}

void InstrumentNode::safeRename(const String &newName, bool sendNotifications)
{
    if (this->instrument == nullptr)
    { 
        jassertfalse;
        return;
    }

    if (newName == this->getName())
    {
        return;
    }

    TreeNode::safeRename(newName, sendNotifications);
    this->instrument->setName(newName);

    if (sendNotifications)
    {
        this->notifyOrchestraChanged();
    }
}

String InstrumentNode::getName() const noexcept
{
    if (this->instrument == nullptr)
    {
        jassertfalse;
        return {};
    }

    return this->instrument->getName();
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
// Serializable
//===----------------------------------------------------------------------===//

// InstrumentNode is supposed to be a temporary node, a presentation of
// Instrument model in the workspace tree, but in earlier versions it was
// serialized/deserialized independently (don't ask me why);
// now each InstrumentNode is created on the fly by OrchestraPitNode
// when it synchronizes itself with OrchestraPit, but we will leave
// this serialize() method here, so that previous versions don't break
// when loading the main config file modified by this version:

SerializedData InstrumentNode::serialize() const
{
    SerializedData tree(Serialization::Core::treeNode);
    tree.setProperty(Serialization::Core::treeNodeType, this->type);
    tree.setProperty(Serialization::Core::treeNodeName, this->getName());
    tree.setProperty(Serialization::Audio::instrumentId, this->instrument->getIdAndHash());
    // not serializing the children editor nodes: they are all temporary
    return tree;
}

// doing nothing here, it's a temporary node
// (in future versions serialize() will also be removed)
void InstrumentNode::deserialize(const SerializedData &data) {}

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

//===----------------------------------------------------------------------===//
// Audio plugin UI editor node (mobile only)
//===----------------------------------------------------------------------===//

class HelioAudioProcessorEditor final : public GenericAudioProcessorEditor
{
public:

    explicit HelioAudioProcessorEditor(AudioProcessor &p) : GenericAudioProcessorEditor(p)
    {
        this->setFocusContainerType(Component::FocusContainerType::keyboardFocusContainer);

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

    const auto node = instrument->getNodeForId(this->nodeId);

    if (node == nullptr)
    {
        delete this;
        return;
    }

    if (!this->audioPluginEditor)
    {
        if (node->getProcessor()->hasEditor())
        {
            // Some plugins (including Kontakt 3!) misbehavior messes up all the controls
            // Turns out they attach themselves to the parent window :(
            // So we cannot add them as a child component like that:
            // ui = node->getProcessor()->createEditorIfNeeded();
            // so we try to mimic that by creating a plugin window
            // while its size and position that is managed by audioPluginEditor
            if (auto *window = PluginWindow::getWindowFor(node, true))
            {
                this->audioPluginEditor = make<AudioPluginEditorPage>(window);
            }
        }
        else
        {
            auto *ui = new HelioAudioProcessorEditor(*node->getProcessor());
            auto *plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor());

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
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }

    MenuPanel::Menu createDefaultMenu()
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

        menu.add(MenuItem::item(Icons::list, TRANS(I18n::Menu::presets))->withSubmenu()->withAction([this]()
        {
            this->updateContent(this->createPresetsMenu(), MenuPanel::SlideLeft);
        }));

        return menu;
    }

    MenuPanel::Menu createPresetsMenu()
    {
        MenuPanel::Menu menu;

        menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
        {
            this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
        }));

        menu.add(MenuItem::item(Icons::commit, CommandIDs::SavePreset,
            TRANS(I18n::Menu::savePreset))->closesMenu());

        const auto allMappings = App::Config().getKeyboardMappings()->getAll();
        for (int i = 0; i < allMappings.size(); ++i)
        {
            menu.add(MenuItem::item(Icons::piano, CommandIDs::SelectPreset + i,
                allMappings.getUnchecked(i)->getName())->closesMenu());
        }

        return menu;
    }
};

KeyboardMappingNode::KeyboardMappingNode(WeakReference<Instrument> instrument) :
    TreeNode({}, Serialization::Midi::KeyboardMappings::keyboardMapping),
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
