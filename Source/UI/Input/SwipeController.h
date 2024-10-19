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

class SwipeController final : public MouseListener
{
public:

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual int getVerticalSwipeAnchor() { return 0; }
        virtual void onVerticalSwipe(int anchor, int distance) {}
        virtual int getHorizontalSwipeAnchor() { return 0; }
        virtual void onHorizontalSwipe(int anchor, int distance) {}
    };

    explicit SwipeController(Listener &listener) :
        listener(listener) {}

    void mouseDrag(const MouseEvent &event) override
    {

    }

    Listener &listener;
};
