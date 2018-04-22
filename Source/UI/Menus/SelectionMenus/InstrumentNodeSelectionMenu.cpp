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

    menu.add(MenuItem::item(Icons::copy, TRANS("menu::selection::route::reset")));

    menu.add(MenuItem::item(Icons::cut, TRANS("menu::selection::route::getaudio"))->
        withSubmenu()->withTimer()->withAction([this]()
    {
        this->updateContent(this->createAudioSourcesMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::cut, TRANS("menu::selection::route::sendaudio"))->
        withSubmenu()->withTimer()->withAction([this]()
    {
        this->updateContent(this->createAudioDestinationsMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::cut, TRANS("menu::selection::route::getmidi"))->
        withSubmenu()->withTimer()->withAction([this]()
    {
        this->updateContent(this->createMidiSourcesMenu(), MenuPanel::SlideLeft);
    }));

    menu.add(MenuItem::item(Icons::cut, TRANS("menu::selection::route::sendmidi"))->
        withSubmenu()->withTimer()->withAction([this]()
    {
        this->updateContent(this->createMidiDestinationsMenu(), MenuPanel::SlideLeft);
    }));

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createAudioSourcesMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::left, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    // TODO

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createAudioDestinationsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::left, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    // TODO

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createMidiSourcesMenu()
{    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::left, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    // TODO

    return menu;
}

MenuPanel::Menu InstrumentNodeSelectionMenu::createMidiDestinationsMenu()
{
    MenuPanel::Menu menu;

    menu.add(MenuItem::item(Icons::left, TRANS("menu::back"))->withTimer()->withAction([this]()
    {
        this->updateContent(this->createDefaultMenu(), MenuPanel::SlideRight);
    }));

    // TODO

    return menu;
}
