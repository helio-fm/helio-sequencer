/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "OrchestraPitNode.h"

#include "Instrument.h"
#include "MainLayout.h"
#include "Icons.h"
#include "PluginScanner.h"
#include "OrchestraPitMenu.h"
#include "SerializationKeys.h"
#include "Workspace.h"
#include "AudioCore.h"

OrchestraPitNode::OrchestraPitNode() :
    TreeNode("Instruments", Serialization::Core::instrumentsList),
    orchestra(App::Workspace().getAudioCore())
{
    this->syncAllInstruments();
    this->recreatePage();
    this->orchestra.addOrchestraListener(this);
}

OrchestraPitNode::~OrchestraPitNode()
{
    this->orchestra.removeOrchestraListener(this);
}

Image OrchestraPitNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::orchestraPit, Globals::UI::headlineIconSize);
}

String OrchestraPitNode::getName() const noexcept
{
    return TRANS(I18n::Tree::instruments);
}

void OrchestraPitNode::showPage()
{
    App::Layout().showPage(this->instrumentsPage.get(), this);
}

void OrchestraPitNode::recreatePage()
{
    this->instrumentsPage = make<OrchestraPitPage>(App::Workspace().getPluginManager(), *this);
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool OrchestraPitNode::hasMenu() const noexcept
{
    return true;
}

UniquePointer<Component> OrchestraPitNode::createMenu()
{
    return make<OrchestraPitMenu>(*this);
}

//===----------------------------------------------------------------------===//
// OrchestraListener
//===----------------------------------------------------------------------===//

void OrchestraPitNode::onAddInstrument(Instrument *instrument)
{
    for (auto *instrumentNode : this->findChildrenOfType<InstrumentNode>())
    {
        if (instrumentNode->getInstrument() == instrument)
        {
            // this assertion shouldn't be hit normally,
            // but it will, if some future version of this app
            // adds more built-in instruments, like metronome:
            jassertfalse;
            return;
        }
    }

    jassert(MessageManager::getInstance()->isThisTheMessageThread());
    auto *newInstrument = new InstrumentNode(instrument);
    this->addChildNode(newInstrument);
    this->sendChangeMessage();
}

void OrchestraPitNode::onRemoveInstrument(Instrument *instrument)
{
    for (auto *instrumentNode : this->findChildrenOfType<InstrumentNode>())
    {
        if (instrumentNode->getInstrument() == instrument)
        {
            TreeNode::deleteNode(instrumentNode, true);
            this->sendChangeMessage();
            return;
        }
    }

    jassertfalse;
}

//===----------------------------------------------------------------------===//
// Serialization
//===----------------------------------------------------------------------===//

SerializedData OrchestraPitNode::serialize() const
{
    // in future we shouldn't serialize children here, but for now we will,
    // just to maintain compatibility of the main config file with previous versions
    return TreeNode::serialize();
}

// we override deserialization to do nothing:
// all instrument nodes will be created on the fly,
// since this is all the presentation of OrchestraPit model;
// see the comment for InstrumentNode::serialize
void OrchestraPitNode::deserialize(const SerializedData &data) {}

void OrchestraPitNode::syncAllInstruments()
{
    this->deleteAllChildren();

    for (auto *instrument : this->orchestra.getInstruments())
    {
        jassert(MessageManager::getInstance()->isThisTheMessageThread());
        auto *newInstrument = new InstrumentNode(instrument);
        this->addChildNode(newInstrument);
    }

    this->sendChangeMessage();
}
