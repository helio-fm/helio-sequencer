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

class ViewportKineticSlider final : private Timer
{
public:
    
    static ViewportKineticSlider &instance()
    {
        static ViewportKineticSlider s;
        return s;
    }
    
    void stopAnimationForViewport(Viewport *targetViewport);
    void calculateDragSpeedForViewport(Viewport *targetViewport, Point<float> absDragOffset);
    void startAnimationForViewport(Viewport *targetViewport, Point<float> force);
    
private:
    
    void timerCallback() override;
    
    struct Animator final : ReferenceCountedObject
    {
        using Ptr = ReferenceCountedObjectPtr<Animator>;
        
        Component::SafePointer<Viewport> viewport;
        
        Point<float> force;
        Point<int> anchor;
    };
    
    struct DragState final : ReferenceCountedObject
    {
        using Ptr = ReferenceCountedObjectPtr<DragState>;
        
        Component::SafePointer<Viewport> viewport;
        Point<float> force;

        Point<float> currentOffset;
        Point<float> offsetAnchor;

        double lastCheckTime = 0.0;
    };
    
    ReferenceCountedArray<Animator> animators;
    ReferenceCountedArray<DragState> dragStates;

};
