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
        this->setOpaque(false);
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


AudioPluginTreeItem::AudioPluginTreeItem(uint32 pluginID, const String &name) :
    TreeItem(name),
    audioPluginEditor(nullptr),
    filterID(pluginID)
{
    this->setVisible(false);
}

AudioPluginTreeItem::~AudioPluginTreeItem()
{
}


Colour AudioPluginTreeItem::getColour() const
{
    return Colour(0xffd151ff);
}

Image AudioPluginTreeItem::getIcon() const
{
    return Icons::findByName(Icons::instrumentSettings, TREE_ICON_HEIGHT);
}

uint32 AudioPluginTreeItem::getNodeId() const noexcept
{
    return this->filterID;
}

void AudioPluginTreeItem::showPage()
{
    const Instrument *instrument =
        this->findParentOfType<InstrumentTreeItem>()->getInstrument();

    if (instrument == nullptr)
    {
        delete this;
        return;
    }

    const AudioProcessorGraph::Node::Ptr f(instrument->getNodeForId(filterID));

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

var AudioPluginTreeItem::getDragSourceDescription()
{
    return var::null;
}

bool AudioPluginTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    return false;
}

void AudioPluginTreeItem::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *AudioPluginTreeItem::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::treeItem);
    xml->setAttribute("type", Serialization::Core::audioPlugin);
    xml->setAttribute("name", this->name);
    return xml;
}

void AudioPluginTreeItem::deserialize(const XmlElement &xml)
{
    this->reset();

    const String& type = xml.getStringAttribute("type");

    if (type != Serialization::Core::audioPlugin) { return; }

    this->setName(xml.getStringAttribute("name"));

    TreeItemChildrenSerializer::deserializeChildren(*this, xml);
}
