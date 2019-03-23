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

#pragma once

class Instrument;

#include "MenuPanel.h"

class InstrumentNodeSelectionMenu final : public MenuPanel
{
public:
    
    InstrumentNodeSelectionMenu(Instrument &instrument,
        AudioProcessorGraph::Node::Ptr node);

private:

    Instrument &instrument;
    AudioProcessorGraph::Node::Ptr node;

    MenuPanel::Menu createDefaultMenu();
    MenuPanel::Menu createAudioSourcesMenu();
    MenuPanel::Menu createAudioDestinationsMenu();
    MenuPanel::Menu createMidiSourcesMenu();
    MenuPanel::Menu createMidiDestinationsMenu();

};
