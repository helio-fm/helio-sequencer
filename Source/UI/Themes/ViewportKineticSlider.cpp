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
#include "ViewportKineticSlider.h"

void ViewportKineticSlider::stopAnimationForViewport(Viewport *targetViewport)
{
    if (targetViewport == nullptr)
    {
        return;
    }
    
    for (int i = 0; i < this->animators.size(); ++i)
    {
        if (this->animators[i]->viewport == targetViewport)
        {
            this->animators.remove(i);
            break;
        }
    }
    
    for (int i = 0; i < this->dragSpeedHolders.size(); ++i)
    {
        if (this->dragSpeedHolders[i]->viewport == targetViewport)
        {
            this->dragSpeedHolders.remove(i);
            break;
        }
    }
}

void ViewportKineticSlider::calculateDragSpeedForViewport(Viewport *targetViewport, Point<float> absDragOffset)
{
    DragSpeedHolder::Ptr targetHolder;
    
    for (auto && dragSpeedHolder : this->dragSpeedHolders)
    {
        DragSpeedHolder::Ptr holder(dragSpeedHolder);
        
        if (holder->viewport == targetViewport)
        {
            targetHolder = holder;
            break;
        }
    }
    
    if (!targetHolder)
    {
        targetHolder = new DragSpeedHolder();
        targetHolder->viewport = targetViewport;
        targetHolder->force = Point<float>(0.f, 0.f);
        targetHolder->offsetAnchor = absDragOffset;
        targetHolder->lastCheckTime = Time::getMillisecondCounterHiRes();
        this->dragSpeedHolders.add(targetHolder);
    }
    
    targetHolder->currentOffset = absDragOffset;
    
    if (! this->isTimerRunning())
    {
        this->startTimerHz(60);
    }
}

#define MAX_FORCE (0.7f)
#define FORCE_COEFFICIENT (-50.f)

void ViewportKineticSlider::startAnimationForViewport(Viewport *targetViewport, Point<float> force)
{
    if (targetViewport == nullptr)
    {
        return;
    }
    
    Point<float> newForce = force;
    
    // picks up the precomputed force
    for (auto && dragSpeedHolder : this->dragSpeedHolders)
    {
        DragSpeedHolder::Ptr holder(dragSpeedHolder);
        
        if (holder->viewport == targetViewport)
        {
            newForce = holder->force;
            break;
        }
    }
    
    // cleans up
    this->stopAnimationForViewport(targetViewport);
    
    const float newLimitedForceX = jmax(-MAX_FORCE, jmin(MAX_FORCE, newForce.getX()));
    const float newLimitedForceY = jmax(-MAX_FORCE, jmin(MAX_FORCE, newForce.getY()));
    newForce = Point<float>(newLimitedForceX, newLimitedForceY);
    
    Animator::Ptr animator(new Animator());
    animator->viewport = targetViewport;
    animator->force = newForce * FORCE_COEFFICIENT;
    animator->anchor = targetViewport->getViewPosition();
    this->animators.add(animator);
    
    if (! this->isTimerRunning())
    {
        this->startTimerHz(60);
    }
}

void ViewportKineticSlider::timerCallback()
{
    if (this->animators.size() == 0 && this->dragSpeedHolders.size() == 0)
    {
        this->stopTimer();
    }
    
    // updates animators
    for (int i = 0; i < this->animators.size(); ++i)
    {
        Animator::Ptr animator(this->animators[i]);
        
        if (animator->viewport == nullptr)
        {
            this->animators.remove(i);
            break;
        }
        
        if (animator->force.getDistanceFromOrigin() < 1.0f)
        {
            this->animators.remove(i);
            break;
        }
        
        animator->force *= 0.85f;
        animator->anchor += animator->force.toInt();
        animator->viewport->setViewPosition(animator->anchor);
    }
    
    // calculates the speed
    for (auto && dragSpeedHolder : this->dragSpeedHolders)
    {
        DragSpeedHolder::Ptr holder(dragSpeedHolder);
        
        double timeDelta = Time::getMillisecondCounterHiRes() - holder->lastCheckTime;
        holder->lastCheckTime = Time::getMillisecondCounterHiRes();
        
        Point<float> dragDelta = holder->currentOffset - holder->offsetAnchor;
        holder->offsetAnchor = holder->currentOffset;
        
        Point<float> newForce = dragDelta / timeDelta;
        
        holder->force = (holder->force * 0.75f) + (newForce * 0.25f);
    }
}
