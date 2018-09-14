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
#include "AudioPluginTreeItem.h"
#include "TreeItemChildrenSerializer.h"
#include "MainLayout.h"
#include "Icons.h"

#include "InstrumentTreeItem.h"
#include "Instrument.h"
#include "AudioPluginEditorPage.h"
#include "SerializationKeys.h"

#include "App.h"
#include "PluginWindow.h"

class HelioAudioProcessorEditor : public GenericAudioProcessorEditor
{
public:

    explicit HelioAudioProcessorEditor(AudioProcessor *const p) :
        GenericAudioProcessorEditor(p)
    {
        this->setFocusContainer(true);

        // установим ему масимальную высоту
        for (int i = 0; i < this->getNumChildComponents(); ++i)
        {
            if (PropertyPanel *panel = dynamic_cast<PropertyPanel *>(this->getChildComponent(i)))
            {
                this->setSize(this->getWidth(), panel->getTotalContentHeight());
            }
        }
    }

    void paint(Graphics &g) override
    { }
};

AudioPluginTreeItem::AudioPluginTreeItem(AudioProcessorGraph::NodeID pluginID, const String &name) :
    TreeItem(name, Serialization::Audio::audioPlugin),
    audioPluginEditor(nullptr),
    nodeId(pluginID)
{
    this->setVisible(false);
}

bool AudioPluginTreeItem::hasMenu() const noexcept
{
    return false;
}

ScopedPointer<Component> AudioPluginTreeItem::createMenu()
{
    return nullptr;
}

Colour AudioPluginTreeItem::getColour() const noexcept
{
    return Colour(0xffd151ff);
}

Image AudioPluginTreeItem::getIcon() const noexcept
{
    return Icons::findByName(Icons::audioPlugin, HEADLINE_ICON_SIZE);
}

AudioProcessorGraph::NodeID AudioPluginTreeItem::getNodeId() const noexcept
{
    return this->nodeId;
}

void AudioPluginTreeItem::showPage()
{
    const auto instrument =
        this->findParentOfType<InstrumentTreeItem>()->getInstrument();

    if (instrument == nullptr)
    {
        delete this;
        return;
    }

    const AudioProcessorGraph::NodeID nodeId(nodeId);
    const AudioProcessorGraph::Node::Ptr f(instrument->getNodeForId(nodeId));

    if (f == nullptr)
    {
        delete this;
        return;
    }

    if (!this->audioPluginEditor)
    {
        const bool forceGenericEditor = false;

        if (!forceGenericEditor && f->getProcessor()->hasEditor())
        {
            // Some plugins (including Kontakt 3!) misbehavior messes up all the controls
            // Turns out they attach themselves to the parent window :(
            // So we cannot add them as a child component like that:
            // ui = f->getProcessor()->createEditorIfNeeded();
            // so we try to mimic that by creating a plugin window
            // while its size and position that is managed by audioPluginEditor
            if (PluginWindow *const window = PluginWindow::getWindowFor(f, false, true))
            {
                this->audioPluginEditor = new AudioPluginEditorPage(window);
            }
        }
        else
        {
            AudioProcessorEditor *const ui =
                new HelioAudioProcessorEditor(f->getProcessor());

            AudioPluginInstance *const plugin =
                dynamic_cast<AudioPluginInstance *>(f->getProcessor());

            if (plugin != nullptr)
            {
                ui->setName(plugin->getName());
            }

            this->audioPluginEditor = new AudioPluginEditorPage(ui);
        }
    }

    // Something went wrong
    if (!this->audioPluginEditor)
    {
        delete this;
        return;
    }

    App::Layout().showPage(this->audioPluginEditor, this);
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

var AudioPluginTreeItem::getDragSourceDescription() { return {}; }
bool AudioPluginTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails) { return false; }
void AudioPluginTreeItem::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex) {}
