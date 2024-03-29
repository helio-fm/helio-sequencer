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

class MultiTouchListener
{
public:
    
    virtual ~MultiTouchListener() = default;

    virtual void multiTouchStartZooming() = 0;
    virtual void multiTouchContinueZooming(
        const Rectangle<float> &relativePosition,
        const Rectangle<float> &relativePositionAnchor,
        const Rectangle<float> &absolutePositionAnchor) = 0;
    virtual void multiTouchEndZooming(const MouseEvent &anchorEvent) = 0;

    virtual Point<float> getMultiTouchRelativeAnchor(const MouseEvent &e) = 0;
    virtual Point<float> getMultiTouchAbsoluteAnchor(const MouseEvent &e) = 0;
};
