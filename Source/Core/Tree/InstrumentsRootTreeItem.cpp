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
#include "InstrumentsRootTreeItem.h"

#include "TreeItemChildrenSerializer.h"
#include "InstrumentTreeItem.h"
#include "Instrument.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "Icons.h"
#include "PluginManager.h"
#include "InstrumentsPage.h"
#include "InstrumentsCommandPanel.h"
#include "SerializationKeys.h"
#include "App.h"
#include "Workspace.h"

InstrumentsRootTreeItem::InstrumentsRootTreeItem() :
    TreeItem("Instruments", Serialization::Core::instrumentsList)
{
    this->recreatePage();
}

Colour InstrumentsRootTreeItem::getColour() const
{
    return Colour(0xffff80f3);
}

Image InstrumentsRootTreeItem::getIcon() const
{
    return Icons::findByName(Icons::saxophone, TREE_LARGE_ICON_HEIGHT);
}

String InstrumentsRootTreeItem::getName() const
{
    return TRANS("tree::instruments");
}

void InstrumentsRootTreeItem::showPage()
{
    App::Layout().showPage(this->instrumentsPage, this);
}

void InstrumentsRootTreeItem::recreatePage()
{
    this->instrumentsPage = new InstrumentsPage(App::Workspace().getPluginManager(), *this);
}


//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

ScopedPointer<Component> InstrumentsRootTreeItem::createItemMenu()
{
    return new InstrumentsCommandPanel(*this);
}


//===----------------------------------------------------------------------===//
// Dragging
//===----------------------------------------------------------------------===//

bool InstrumentsRootTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails &dragSourceDetails)
{
    bool isInterested = (dragSourceDetails.description == Serialization::Core::instrumentRoot.toString());

    isInterested |= (nullptr != dynamic_cast<PluginDescriptionWrapper *>(dragSourceDetails.description.getObject()));

    if (isInterested)
    { this->setOpen(true); }

    return isInterested;
}

void InstrumentsRootTreeItem::itemDropped(const DragAndDropTarget::SourceDetails &dragSourceDetails, int insertIndex)
{
    if (ListBox *list = dynamic_cast<ListBox *>(dragSourceDetails.sourceComponent.get()))
    {
        if (PluginDescriptionWrapper *pd = dynamic_cast<PluginDescriptionWrapper *>(dragSourceDetails.description.getObject()))
        {
            PluginDescription pluginDescription(pd->pluginDescription);
            App::Workspace().getAudioCore().addInstrument(pluginDescription, pluginDescription.descriptiveName,
                [this, insertIndex](Instrument *instrument)
                {
                    jassert(instrument);
                    this->addInstrumentTreeItem(instrument, insertIndex);
                    Logger::writeToLog("Loaded " + instrument->getName());
                });
        }
    }

    TreeItem::itemDropped(dragSourceDetails, insertIndex);
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

InstrumentTreeItem *InstrumentsRootTreeItem::addInstrumentTreeItem(Instrument *instrument, int insertIndex)
{
    this->setOpen(true);
    auto newInstrument = new InstrumentTreeItem(instrument);
    this->addChildTreeItem(newInstrument, insertIndex);
    return newInstrument;
}
