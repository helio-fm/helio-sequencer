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

class ComponentFader final : public ChangeBroadcaster, private Timer
{
public:

    ComponentFader() = default;
    
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
    
    static float timeToDistance(float time, float startSpeed = 0.f,
        float midSpeed = 2.f, float endSpeed = 0.f) noexcept
    {
        return (time < 0.5f) ? time * (startSpeed + time * (midSpeed - startSpeed))
            : 0.5f * (startSpeed + 0.5f * (midSpeed - startSpeed))
            + (time - 0.5f) * (midSpeed + (time - 0.5f) * (endSpeed - midSpeed));
    }

    static double timeToDistance(double time, double startSpeed = 0.0,
        double midSpeed = 2.0, double endSpeed = 0.0) noexcept
    {
        return (time < 0.5) ? time * (startSpeed + time * (midSpeed - startSpeed))
            : 0.5 * (startSpeed + 0.5 * (midSpeed - startSpeed))
            + (time - 0.5) * (midSpeed + (time - 0.5) * (endSpeed - midSpeed));
    }

private:

    class AnimationTask final
    {
    public:

        explicit AnimationTask(Component *c) noexcept : component(c) {}

        void reset(float finalAlpha,
            int millisecondsToSpendMoving,
            bool useProxyComponent,
            bool useProxyOnly);
        bool useTimeslice(const int elapsed);
        void moveToFinalDestination();

        class ProxyComponent final : public Component
        {
        public:
            explicit ProxyComponent(Component& c);
            void paint(Graphics& g) override;
            Image image;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProxyComponent)
        };

        WeakReference<Component> component;
        UniquePointer<Component> proxy;

        double destAlpha;
        int msElapsed, msTotal;
        double startSpeed, midSpeed, endSpeed, lastProgress;
        double left, top, right, bottom, alpha;
        bool isChangingAlpha;
        bool allowedToModifyOrigin;
    };

    OwnedArray<AnimationTask> tasks;
    uint32 lastTime = 0;
    
    AnimationTask *findTaskFor(Component *) const noexcept;
    void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentFader)
};
