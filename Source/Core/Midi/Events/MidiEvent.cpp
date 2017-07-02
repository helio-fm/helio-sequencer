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
#include "MidiEvent.h"
#include "MidiLayer.h"


MidiEvent::MidiEvent(MidiLayer *owner, float beatVal) :
    layer(owner),
    beat(beatVal)
{
    this->id = this->createId();
}

MidiEvent::~MidiEvent()
{

}


MidiLayer *MidiEvent::getLayer() const noexcept
{
    jassert(this->layer);
    return this->layer;
}

MidiEvent::Id MidiEvent::getId() const noexcept
{
    return this->id;
}

float MidiEvent::getBeat() const noexcept
{
    return this->beat;
}

//static MidiEvent::Id recentId = 0;

MidiEvent::Id MidiEvent::createId() noexcept
{
    Uuid uuid;
    return uuid.toString().substring(16, 32);
//    return ++recentId;
}

