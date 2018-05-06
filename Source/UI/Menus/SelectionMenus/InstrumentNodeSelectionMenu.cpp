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
#include "InstrumentNodeSelectionMenu.h"
#include "Instrument.h"
#include "CommandIDs.h"
#include "Icons.h"

InstrumentNodeSelectionMenu::InstrumentNodeSelectionMenu(Instrument &instrument,
    AudioProcessorGraph::Node::Ptr node) :
    instrument(instrument),
    node(node)
{
    this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createDefaultMenu()
{
    MenuPanel::Menu menu;

    const bool acceptsMidi = this->node->getProcessor()->acceptsMidi();
    const bool producesMidi = this->node->getProcessor()->producesMidi();
    const bool acceptsAudio = this->node->getProcessor()->getTotalNumInputChannels() > 0;
    const bool producesAudio = this->node->getProcessor()->getTotalNumOutputChannels() > 0;
    const bool hasConnections = this->instrument.hasConnectionsFor(this->node);

    menu.add(MenuItem::item(Icons::routing, TRANS("menu::selection::route::getmidi"))->
        withSubmenu()->disabledIf(!acceptsMidi)->withAction([this]()
    {
        this->updateContent(this->createMidiSourcesMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::routing, TRANS("menu::selection::route::getaudio"))->
        withSubmenu()->disabledIf(!acceptsAudio)->withAction([this]()
    {
        this->updateContent(this->createAudioSourcesMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::routing, TRANS("menu::selection::route::sendmidi"))->
        withSubmenu()->disabledIf(!producesMidi)->withAction([this]()
    {
        this->updateContent(this->createMidiDestinationsMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::routing, TRANS("menu::selection::route::sendaudio"))->
        withSubmenu()->disabledIf(!producesAudio)->withAction([this]()
    {
        this->updateContent(this->createAudioDestinationsMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::close, TRANS("menu::selection::route::disconnect"))->
        disabledIf(!hasConnections)->withAction([this]()
    {
        this->instrument.removeAllConnectionsForNode(this->node);
        this->dismiss();
    }));

    menu.add(MenuItem::item(Icons::remove, TRANS("menu::selection::route::remove"))->
        disabledIf(this->instrument.isNodeStandardIOProcessor(this->node->nodeID))->withAction([this]()
    {
        this->instrument.removeNode(this->node->nodeID);
        this->dismiss();
    }));

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createAudioSourcesMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &n : this->instrument.findAudioProducers())
    {
        const bool hasConnection = this->instrument.hasAudioConnection(n , this->node);
        menu.add(MenuItem::item(hasConnection ? Icons::apply : Icons::audioPlugin, n->getProcessor()->getName())->
            disabledIf(hasConnection || n == this->node)->withAction([this, n]()
        {
            this->instrument.addConnection(n->nodeID, 0, this->node->nodeID, 0);
            this->instrument.addConnection(n->nodeID, 1, this->node->nodeID, 1);
            this->dismiss();
        }));
    }

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createAudioDestinationsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &n : this->instrument.findAudioAcceptors())
    {
        const bool hasConnection = this->instrument.hasAudioConnection(this->node, n);
        menu.add(MenuItem::item(hasConnection ? Icons::apply : Icons::audioPlugin, n->getProcessor()->getName())->
            disabledIf(hasConnection || n == this->node)->withAction([this, n]()
        {
            this->instrument.addConnection(this->node->nodeID, 0, n->nodeID, 0);
            this->instrument.addConnection(this->node->nodeID, 1, n->nodeID, 1);
            this->dismiss();
        }));
    }

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createMidiSourcesMenu()
{    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &n : this->instrument.findMidiProducers())
    {
        const bool hasConnection = this->instrument.hasMidiConnection(n, this->node);
        menu.add(MenuItem::item(hasConnection ? Icons::apply : Icons::audioPlugin, n->getProcessor()->getName())->
            disabledIf(hasConnection || n == this->node)->withAction([this, n]()
        {
            this->instrument.addConnection(n->nodeID,
                Instrument::midiChannelNumber, this->node->nodeID, Instrument::midiChannelNumber);
            this->dismiss();
        }));
    }

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createMidiDestinationsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &n : this->instrument.findMidiAcceptors())
    {
        const bool hasConnection = this->instrument.hasMidiConnection(this->node, n);
        menu.add(MenuItem::item(hasConnection ? Icons::apply : Icons::audioPlugin, n->getProcessor()->getName())->
            disabledIf(hasConnection || n == this->node)->withAction([this, n]()
        {
            this->instrument.addConnection(this->node->nodeID,
                Instrument::midiChannelNumber, n->nodeID, Instrument::midiChannelNumber);
            this->dismiss();
        }));
    }

    return menu;
}
