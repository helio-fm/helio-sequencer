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
#include "OrchestraPitTreeItem.h"

#include "TreeItemChildrenSerializer.h"
#include "InstrumentTreeItem.h"
#include "Instrument.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Icons.h"
#include "PluginScanner.h"
#include "OrchestraPitMenu.h"
#include "SerializationKeys.h"
#include "Workspace.h"

OrchestraPitTreeItem::OrchestraPitTreeItem() :
    TreeItem("Instruments", Serialization::Core::instrumentsList)
{
    this->recreatePage();
}

Image OrchestraPitTreeItem::getIcon() const noexcept
{
    return Icons::findByName(Icons::orchestraPit, TREE_LARGE_ICON_HEIGHT);
}

String OrchestraPitTreeItem::getName() const noexcept
{
    return TRANS("tree::instruments");
}

void OrchestraPitTreeItem::showPage()
{
    App::Layout().showPage(this->instrumentsPage, this);
}

void OrchestraPitTreeItem::recreatePage()
{
    this->instrumentsPage =
        new OrchestraPitPage(App::Workspace().getPluginManager(), *this);
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool OrchestraPitTreeItem::hasMenu() const noexcept
{
    return true;
}

ScopedPointer<Component> OrchestraPitTreeItem::createMenu()
{
    return new OrchestraPitMenu(*this);
}

//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

bool OrchestraPitTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    bool isInterested = (dragSourceDetails.description == Serialization::Core::instrumentRoot.toString());
    isInterested |= (nullptr != dynamic_cast<PluginDescriptionDragnDropWrapper *>(dragSourceDetails.description.getObject()));

    if (isInterested)
    { this->setOpen(true); }

    return isInterested;
}

void OrchestraPitTreeItem::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
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

    TreeItem::itemDropped(dragSourceDetails, insertIndex);
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

InstrumentTreeItem *OrchestraPitTreeItem::addInstrumentTreeItem(Instrument *instrument, int insertIndex)
{
    jassert(MessageManager::getInstance()->isThisTheMessageThread());
    auto newInstrument = new InstrumentTreeItem(instrument);
    this->addChildTreeItem(newInstrument, insertIndex);
    this->sendChangeMessage();
    return newInstrument;
}
