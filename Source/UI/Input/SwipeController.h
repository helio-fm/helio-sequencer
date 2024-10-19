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

    enum class Direction : int
    {
        Undecided = 0,
        Horizontal,
        Vertical
    };

    explicit SwipeController(Listener &listener) :
        listener(listener) {}

    void mouseDrag(const MouseEvent &event) override
    {
        const auto dragOffset = event.getOffsetFromDragStart();

        if (this->direction == Direction::Undecided)
        {
            if (abs(dragOffset.x) > SwipeController::dragThreshold &&
                abs(dragOffset.x) > abs(dragOffset.y))
            {
                this->direction = Direction::Horizontal;
                this->anchor = this->listener.getHorizontalSwipeAnchor();
            }
            else if (abs(dragOffset.y) > SwipeController::dragThreshold &&
                abs(dragOffset.y) > abs(dragOffset.x))
            {
                this->direction = Direction::Vertical;
                this->anchor = this->listener.getVerticalSwipeAnchor();
            }
        }

        if (this->direction == Direction::Horizontal)
        {
            this->listener.onHorizontalSwipe(this->anchor, dragOffset.x);
        }
        else if (this->direction == Direction::Vertical)
        {
            this->listener.onVerticalSwipe(this->anchor, dragOffset.y);
        }
    }

    void mouseUp(const MouseEvent &event) override
    {
        this->direction = Direction::Undecided;
        this->anchor = 0;
    }

    Listener &listener;

    static constexpr int dragThreshold = 8;

    Direction direction = Direction::Undecided;
    int anchor = 0;

};
