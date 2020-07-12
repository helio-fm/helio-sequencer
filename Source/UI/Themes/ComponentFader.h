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

// Simply a shortcut wrapper for convenience
class ComponentFader final : public ComponentAnimator
{
public:

    ComponentFader() = default;

    // Always uses proxy component
    void fadeOut(Component *component, int millisecondsToTake)
    {
        if (component != nullptr)
        {
            this->animateComponent(component, component->getBounds(), 0.0f, millisecondsToTake, true, 1.f, 1.f);
            component->setVisible(false);
        }
    }

    void fadeIn(Component *component, int millisecondsToTake)
    {
        if (component != nullptr && !(component->isVisible() && component->getAlpha() == 1.0f))
        {
            component->setAlpha(0.0f);
            component->setVisible(true);
            this->animateComponent(component, component->getBounds(), 1.0f, millisecondsToTake, false, 1.f, 1.f);
        }
    }

    static float timeToDistance(float time, float startSpeed = 0.f,
        float midSpeed = 2.f, float endSpeed = 0.f) noexcept
    {
        return (time < 0.5f) ? time * (startSpeed + time * (midSpeed - startSpeed))
            : 0.5f * (startSpeed + 0.5f * (midSpeed - startSpeed))
            + (time - 0.5f) * (midSpeed + (time - 0.5f) * (endSpeed - midSpeed));
    }
};
