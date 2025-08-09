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

#include "Config.h"
#include "UserInterfaceFlags.h"

// Simply a shortcut wrapper for convenience
class ComponentFader final : public ComponentAnimator
{
public:

    ComponentFader() = default;

    void fadeOut(Component *component,
        int millisecondsToTake, bool useProxyComponent = true)
    {
        if (component == nullptr)
        {
            jassertfalse;
            return;
        }

        if (component->isVisible() &&
            App::Config().getUiFlags()->areUiAnimationsEnabled())
        {
            ComponentAnimator::animateComponent(component,
                component->getBounds(), 0.f, millisecondsToTake, useProxyComponent, 0.0, 1.0);
        }

        component->setVisible(false);
    }

    void fadeIn(Component *component, int millisecondsToTake)
    {
        if (component == nullptr)
        {
            jassertfalse;
            return;
        }

        if (!(component->isVisible() && component->getAlpha() == 1.f))
        {
            if (App::Config().getUiFlags()->areUiAnimationsEnabled())
            {
                component->setAlpha(0.f);
                component->setVisible(true);
                ComponentAnimator::animateComponent(component,
                    component->getBounds(), 1.f, millisecondsToTake, false, 1.0, 0.0);
            }
            else
            {
                component->setAlpha(1.f);
                component->setVisible(true);
            }
        }
    }

    void animateComponent(Component *component,
        const Rectangle<int> &finalBounds,
        float finalAlpha, int millisecondsToTake,
        bool useProxyComponent, double startSpeed, double endSpeed)
    {
        if (component == nullptr)
        {
            jassertfalse;
            return;
        }

        if (App::Config().getUiFlags()->areUiAnimationsEnabled())
        {
            ComponentAnimator::animateComponent(component,
                finalBounds, finalAlpha, millisecondsToTake, useProxyComponent, startSpeed, endSpeed);
        }
        else
        {
            component->setAlpha(finalAlpha);
            component->setBounds(finalBounds);
            component->setVisible(finalAlpha > 0);
        }
    }
};
