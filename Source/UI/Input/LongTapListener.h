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

#pragma once

class LongTapListener
{
public:
    virtual ~LongTapListener() = default;
    
    // When controller is subscribed as addMouseListener(this->longTapController, true)),
    // listener may want to check for (target == this) before any action
    // as the event may come from one of it's children:
    virtual void longTapEvent(const Point<float> &position,
        const WeakReference<Component> &target) = 0;

};
