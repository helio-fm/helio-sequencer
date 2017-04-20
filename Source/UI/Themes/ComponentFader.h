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

class ComponentFader : public ChangeBroadcaster, private Timer
{
public:

    ComponentFader();
    
    ~ComponentFader() override;
    
    void animateComponent(Component *component,
                          float finalAlpha,
                          int animationDurationMilliseconds,
                          bool useProxyComponent,
                          bool useProxyOnly = false);
    
    // Always uses proxy component
    void fadeOut(Component *component, int millisecondsToTake);
    void fadeIn(Component *component, int millisecondsToTake);

    // Always uses proxy component, doesn't change the original one
    void fadeOutSnapshot(Component *component, int millisecondsToTake);

    void softFadeOut(float targetAlpha, Component *component, int millisecondsToTake);
    void softFadeIn(Component *component, int millisecondsToTake);

    void cancelAnimation(Component *component, const bool setFinalAlpha);
    void cancelAllAnimations(bool setFinalAlpha);
    
    bool isAnimating(Component *component) const noexcept;
    bool isAnimating() const noexcept;
    
private:

    class AnimationTask;
    OwnedArray<AnimationTask> tasks;
    uint32 lastTime;
    
    AnimationTask *findTaskFor(Component *) const noexcept;
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentFader)
};
