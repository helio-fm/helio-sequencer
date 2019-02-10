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
#include "OrchestraPitNode.h"

#include "TreeNodeSerializer.h"
#include "InstrumentNode.h"
#include "Instrument.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Icons.h"
#include "PluginScanner.h"
#include "OrchestraPitMenu.h"
#include "SerializationKeys.h"
#include "Workspace.h"

OrchestraPitNode::OrchestraPitNode() :
    TreeNode("Instruments", Serialization::Core::instrumentsList)
{
    this->recreatePage();
}

Image OrchestraPitNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::orchestraPit, TREE_LARGE_ICON_HEIGHT);
}

String OrchestraPitNode::getName() const noexcept
{
    return TRANS("tree::instruments");
}

void OrchestraPitNode::showPage()
{
    App::Layout().showPage(this->instrumentsPage, this);
}

void OrchestraPitNode::recreatePage()
{
    this->instrumentsPage =
        new OrchestraPitPage(App::Workspace().getPluginManager(), *this);
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool OrchestraPitNode::hasMenu() const noexcept
{
    return true;
}

ScopedPointer<Component> OrchestraPitNode::createMenu()
{
    return new OrchestraPitMenu(*this);
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

bool OrchestraPitNode::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    bool isInterested = (dragSourceDetails.description == Serialization::Core::instrumentRoot.toString());
    isInterested |= (nullptr != dynamic_cast<PluginDescriptionDragnDropWrapper *>(dragSourceDetails.description.getObject()));

    if (isInterested)
    { this->setOpen(true); }

    return isInterested;
}

void OrchestraPitNode::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
    if (auto list = dynamic_cast<ListBox *>(dragSourceDetails.sourceComponent.get()))
    {
        if (auto pd = dynamic_cast<PluginDescriptionDragnDropWrapper *>(dragSourceDetails.description.getObject()))
        {
            PluginDescription pluginDescription(pd->pluginDescription);
            App::Workspace().getAudioCore().addInstrument(pluginDescription, pluginDescription.descriptiveName,
                [this, insertIndex](Instrument *instrument)
                {
                    jassert(instrument);
                    this->addInstrumentTreeItem(instrument, insertIndex);
                    DBG("Loaded " + instrument->getName());
                });
        }
    }

    TreeNode::itemDropped(dragSourceDetails, insertIndex);
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

InstrumentNode *OrchestraPitNode::addInstrumentTreeItem(Instrument *instrument, int insertIndex)
{
    jassert(MessageManager::getInstance()->isThisTheMessageThread());
    auto newInstrument = new InstrumentNode(instrument);
    this->addChildTreeItem(newInstrument, insertIndex);
    this->sendChangeMessage();
    return newInstrument;
}
