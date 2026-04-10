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

    for (int i = this->animators.size(); --i >= 0;)
    {
        if (this->animators[i]->viewport == targetViewport)
        {
            this->animators.remove(i);
        }
    }

    for (int i = this->dragStates.size(); --i >= 0;)
    {
        if (this->dragStates[i]->viewport == targetViewport)
        {
            this->dragStates.remove(i);
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

    if (!this->isTimerRunning())
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

    static constexpr auto globalSpeed = 100.f;
    force *= globalSpeed;

    auto newForce = force;

    // picks up the precomputed force
    for (auto &a : this->animators)
    {
        if (a->viewport == targetViewport)
        {
            newForce = a->force + force;
            break;
        }
    }
    for (auto &s : this->dragStates)
    {
        if (s->viewport == targetViewport)
        {
            newForce = s->force + force;
            break;
        }
    }

    this->stopAnimationForViewport(targetViewport);

    static constexpr auto maxForce = 10000.f;
    const float newLimitedForceX = jlimit(-maxForce, maxForce, newForce.getX());
    const float newLimitedForceY = jlimit(-maxForce, maxForce, newForce.getY());
    newForce = Point<float>(newLimitedForceX, newLimitedForceY);

    Animator::Ptr animator(new Animator());
    animator->viewport = targetViewport;
    animator->force = newForce;
    animator->anchor = targetViewport->getViewPosition();
    this->animators.add(animator);

    if (!this->isTimerRunning())
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

    for (int i = this->animators.size(); --i >= 0;)
    {
        auto animator = this->animators.getUnchecked(i);
        if (animator->viewport == nullptr ||
            animator->force.getDistanceSquaredFromOrigin() < 1.f)
        {
            this->animators.remove(i);
            continue;
        }

        animator->force *= 0.69f;
        animator->anchor -= animator->force.toInt();
        animator->viewport->setViewPosition(animator->anchor);
    }

    for (int i = this->dragStates.size(); --i >= 0;)
    {
        auto state = this->dragStates.getUnchecked(i);

        const auto now = Time::getMillisecondCounterHiRes();
        const auto timeDelta = now - state->lastCheckTime;
        state->lastCheckTime = now;

        const auto dragDelta = state->currentOffset - state->offsetAnchor;
        state->offsetAnchor = state->currentOffset;

        const auto newForce = dragDelta / timeDelta;
        state->force = (state->force * 0.75f) + (newForce * 0.25f);

        if (state->viewport == nullptr ||
            state->force.getDistanceSquaredFromOrigin() <= 0.01f)
        {
            this->dragStates.remove(i);
        }
    }
}
