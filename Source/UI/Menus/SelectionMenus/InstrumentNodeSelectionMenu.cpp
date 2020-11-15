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
#include "PluginWindow.h"
#include "Instrument.h"

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

#if PLATFORM_DESKTOP

    const bool isStdIo = this->instrument.isNodeStandardIOProcessor(this->node);

    menu.add(MenuItem::item(Icons::instrument,
        TRANS(I18n::Menu::instrumentShowWindow))->
        disabledIf(isStdIo)->closesMenu()->
        withAction([this]()
    {
        if (auto *window = PluginWindow::getWindowFor(this->instrument.getIdAndHash()))
        {
            // this callAsync trick is needed, because the menu is a modal component,
            // and after invoking this callback, it will dismiss, focusing the host window,
            // and pushing the plugin window in the background, which looks silly;
            MessageManager::callAsync([window]()
            {
                // so we have to bring it to front asynchronously:
                window->toFront(true);
            });
        }
    }));

#endif

    menu.add(MenuItem::item(Icons::routing,
        TRANS(I18n::Menu::Selection::routeGetMidi))->
        withSubmenu()->
        disabledIf(!acceptsMidi)->
        withAction([this]()
        {
            this->updateContent(this->createMidiSourcesMenu(), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::routing,
        TRANS(I18n::Menu::Selection::routeGetAudio))->
        withSubmenu()->
        disabledIf(!acceptsAudio)->
        withAction([this]()
        {
            this->updateContent(this->createAudioSourcesMenu(), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::routing,
        TRANS(I18n::Menu::Selection::routeSendmidi))->
        withSubmenu()->
        disabledIf(!producesMidi)->
        withAction([this]()
        {
            this->updateContent(this->createMidiDestinationsMenu(), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::routing,
        TRANS(I18n::Menu::Selection::routeSendaudio))->
        withSubmenu()->
        disabledIf(!producesAudio)->
        withAction([this]()
        {
            this->updateContent(this->createAudioDestinationsMenu(), MenuPanel::SlideLeft);
        }));

    menu.add(MenuItem::item(Icons::close,
        TRANS(I18n::Menu::Selection::routeDisconnect))->
        disabledIf(!hasConnections)->
        closesMenu()->
        withAction([this]()
        {
            this->instrument.removeAllConnectionsForNode(this->node);
        }));

    menu.add(MenuItem::item(Icons::remove,
        TRANS(I18n::Menu::Selection::routeRemove))->
        disabledIf(this->instrument.isNodeStandardIOProcessor(this->node->nodeID))->
        closesMenu()->
        withAction([this]()
        {
            this->instrument.removeNode(this->node->nodeID);
        }));

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createAudioSourcesMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &n : this->instrument.findAudioProducers())
    {
        const bool hasConnection = this->instrument.hasAudioConnection(n , this->node);
        menu.add(MenuItem::item(hasConnection ? Icons::apply : Icons::audioPlugin,
            n->getProcessor()->getName())->
            disabledIf(hasConnection || n == this->node)->
            closesMenu()->
            withAction([this, n]()
        {
            this->instrument.addConnection(n->nodeID, 0, this->node->nodeID, 0);
            this->instrument.addConnection(n->nodeID, 1, this->node->nodeID, 1);
        }));
    }

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createAudioDestinationsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &n : this->instrument.findAudioAcceptors())
    {
        const bool hasConnection = this->instrument.hasAudioConnection(this->node, n);
        menu.add(MenuItem::item(hasConnection ? Icons::apply : Icons::audioPlugin,
            n->getProcessor()->getName())->
            disabledIf(hasConnection || n == this->node)->
            closesMenu()->
            withAction([this, n]()
        {
            this->instrument.addConnection(this->node->nodeID, 0, n->nodeID, 0);
            this->instrument.addConnection(this->node->nodeID, 1, n->nodeID, 1);
        }));
    }

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createMidiSourcesMenu()
{    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &n : this->instrument.findMidiProducers())
    {
        const bool hasConnection = this->instrument.hasMidiConnection(n, this->node);
        menu.add(MenuItem::item(hasConnection ? Icons::apply : Icons::audioPlugin,
            n->getProcessor()->getName())->
            disabledIf(hasConnection || n == this->node)->
            closesMenu()->
            withAction([this, n]()
        {
            this->instrument.addConnection(n->nodeID,
                Instrument::midiChannelNumber, this->node->nodeID, Instrument::midiChannelNumber);
        }));
    }

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createMidiDestinationsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::back, TRANS(I18n::Menu::back))->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    for (const auto &n : this->instrument.findMidiAcceptors())
    {
        const bool hasConnection = this->instrument.hasMidiConnection(this->node, n);
        menu.add(MenuItem::item(hasConnection ? Icons::apply : Icons::audioPlugin,
            n->getProcessor()->getName())->
            disabledIf(hasConnection || n == this->node)->
            closesMenu()->
            withAction([this, n]()
        {
            this->instrument.addConnection(this->node->nodeID,
                Instrument::midiChannelNumber, n->nodeID, Instrument::midiChannelNumber);
        }));
    }

    return menu;
}
