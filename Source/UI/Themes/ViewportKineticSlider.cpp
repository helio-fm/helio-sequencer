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
    
    for (int i = 0; i < this->dragStates.size(); ++i)
    {
        if (this->dragStates[i]->viewport == targetViewport)
        {
            this->dragStates.remove(i);
            break;
        }
    }
}

void ViewportKineticSlider::calculateDragSpeedForViewport(Viewport *targetViewport, Point<float> absDragOffset)
{
    DragState::Ptr targetState;
    
    for (auto &s : this->dragStates)
    {
        if (s->viewport == targetViewport)
        {
            targetState = s;
            break;
        }
    }
    
    if (!targetState)
    {
        targetState = new DragState();
        targetState->viewport = targetViewport;
        targetState->force = Point<float>(0.f, 0.f);
        targetState->offsetAnchor = absDragOffset;
        targetState->lastCheckTime = Time::getMillisecondCounterHiRes();
        this->dragStates.add(targetState);
    }
    
    targetState->currentOffset = absDragOffset;
    
    if (! this->isTimerRunning())
    {
        this->startTimerHz(60);
    }
}

void ViewportKineticSlider::startAnimationForViewport(Viewport *targetViewport, Point<float> force)
{
    if (targetViewport == nullptr)
    {
        return;
    }
    
    auto newForce = force;
    
    // picks up the precomputed force
    for (auto &s : this->dragStates)
    {
        if (s->viewport == targetViewport)
        {
            newForce = s->force;
            break;
        }
    }
    
    // cleans up
    this->stopAnimationForViewport(targetViewport);

    static constexpr auto maxForce = 0.75f;
    static constexpr auto startingForceMultiplier = -50.f;

    const float newLimitedForceX = jmax(-maxForce, jmin(maxForce, newForce.getX()));
    const float newLimitedForceY = jmax(-maxForce, jmin(maxForce, newForce.getY()));
    newForce = Point<float>(newLimitedForceX, newLimitedForceY);
    
    Animator::Ptr animator(new Animator());
    animator->viewport = targetViewport;
    animator->force = newForce * startingForceMultiplier;
    animator->anchor = targetViewport->getViewPosition();
    this->animators.add(animator);
    
    if (! this->isTimerRunning())
    {
        this->startTimerHz(60);
    }
}

void ViewportKineticSlider::timerCallback()
{
    if (this->animators.isEmpty() && this->dragStates.isEmpty())
    {
        this->stopTimer();
    }
    
    // updates animators
    for (int i = 0; i < this->animators.size(); ++i)
    {
        auto animator = this->animators.getUnchecked(i);
        
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
    for (auto &state : this->dragStates)
    {
        const auto timeDelta = Time::getMillisecondCounterHiRes() - state->lastCheckTime;
        state->lastCheckTime = Time::getMillisecondCounterHiRes();
        
        const auto dragDelta = state->currentOffset - state->offsetAnchor;
        state->offsetAnchor = state->currentOffset;
        
        const auto newForce = dragDelta / timeDelta;
        state->force = (state->force * 0.75f) + (newForce * 0.25f);
    }
}
