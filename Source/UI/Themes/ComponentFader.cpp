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
#include "ComponentFader.h"

class ComponentFader::AnimationTask
{
public:
    explicit AnimationTask (Component* c) noexcept  : component (c) {}
    
    void reset (float finalAlpha,
                int millisecondsToSpendMoving,
                bool useProxyComponent,
                bool useProxyOnly)
    {
        this->msElapsed = 0;
        this->msTotal = jmax (1, millisecondsToSpendMoving);
        this->lastProgress = 0;
        this->destAlpha = finalAlpha;
        this->allowedToModifyOrigin = !useProxyOnly;
        
        this->isChangingAlpha = (finalAlpha != component->getAlpha());
        
        this->left    = this->component->getX();
        this->top     = this->component->getY();
        this->right   = this->component->getRight();
        this->bottom  = this->component->getBottom();
        this->alpha   = this->component->getAlpha();
        
        const double invTotalDistance = 4.0 / 2.0;
        this->startSpeed = 0.0;
        this->midSpeed = invTotalDistance;
        this->endSpeed = 0.0;
        
        if (useProxyComponent)
        {
            this->proxy = new ProxyComponent(*this->component);
        }
        else
        {
            this->proxy = nullptr;
        }
        
        if (this->allowedToModifyOrigin)
        {
            this->component->setVisible (! useProxyComponent);
        }
        else
        {
            if (this->proxy != nullptr)
            {
                this->proxy->setAlwaysOnTop(true);
            }
        }
    }
    
    bool useTimeslice (const int elapsed)
    {
        if (Component* const c = proxy != nullptr ? static_cast<Component*> (proxy)
            : static_cast<Component*> (component))
        {
            msElapsed += elapsed;
            double newProgress = msElapsed / static_cast<double>( msTotal);
            
            if (newProgress >= 0 && newProgress < 1.0)
            {
                newProgress = timeToDistance (newProgress);
                const double delta = (newProgress - lastProgress) / (1.0 - lastProgress);
                jassert (newProgress >= lastProgress);
                lastProgress = newProgress;
                
                if (delta < 1.0)
                {
                    bool stillBusy = false;
                    
                    if (isChangingAlpha)
                    {
                        alpha += (destAlpha - alpha) * delta;
                        c->setAlpha (static_cast<float>( alpha));
                        stillBusy = true;
                    }
                    
                    if (stillBusy) {
                        return true;
                    }
                }
            }
        }
        
        moveToFinalDestination();
        return false;
    }
    
    void moveToFinalDestination()
    {
        if (component != nullptr &&
            this->allowedToModifyOrigin)
        {
            component->setAlpha (static_cast<float>( destAlpha));
            
            if (proxy != nullptr) {
                component->setVisible (destAlpha > 0);
            }
        }
    }
    
    class ProxyComponent final : public Component
    {
    public:
        explicit ProxyComponent (Component& c)
        {
            setWantsKeyboardFocus (false);
            setBounds (c.getBounds());
            setTransform (c.getTransform());
            setAlpha (c.getAlpha());
            setInterceptsMouseClicks (false, false);
            
            if (Component* const parent = c.getParentComponent()) {
                parent->addAndMakeVisible (this);
            } else if (c.isOnDesktop() && c.getPeer() != nullptr) {
                addToDesktop (c.getPeer()->getStyleFlags() | ComponentPeer::windowIgnoresKeyPresses);
            } else {
                jassertfalse; // seem to be trying to animate a component that's not visible..
            }
            
            const float scale = static_cast<float>(Desktop::getInstance().getDisplays()
                .findDisplayForPoint(getScreenBounds().getCentre()).scale);
            
            image = c.createComponentSnapshot (c.getLocalBounds(), true, scale);
            
            setVisible (true);
            toBehind (&c);
        }
        
        void paint (Graphics& g) override
        {
            g.setOpacity (1.0f);
            g.drawImageTransformed(image, AffineTransform::scale(getWidth()  / (float)image.getWidth(),
                                                                 getHeight() / (float)image.getHeight()), false);
        }
        
    private:
        
        Image image;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProxyComponent)
    };
    
    WeakReference<Component> component;
    ScopedPointer<Component> proxy;
    
    double destAlpha;
    
    int msElapsed, msTotal;
    double startSpeed, midSpeed, endSpeed, lastProgress;
    double left, top, right, bottom, alpha;
    bool isChangingAlpha;
    bool allowedToModifyOrigin;
    
private:
    
    double timeToDistance (const double time) const noexcept
    {
        return (time < 0.5) ? time * (startSpeed + time * (midSpeed - startSpeed))
        : 0.5 * (startSpeed + 0.5 * (midSpeed - startSpeed))
        + (time - 0.5) * (midSpeed + (time - 0.5) * (endSpeed - midSpeed));
    }
};


ComponentFader::ComponentFader() : lastTime (0) {}

ComponentFader::~ComponentFader() {}

ComponentFader::AnimationTask *ComponentFader::findTaskFor(Component *const component) const noexcept
{
    for (int i = tasks.size(); --i >= 0;) {
        if (component == tasks.getUnchecked(i)->component.get()) {
            return tasks.getUnchecked(i);
        }
    }
    
    return nullptr;
}

void ComponentFader::animateComponent(Component *component,
                                      float finalAlpha,
                                      int millisecondsToSpendMoving,
                                      bool useProxyComponent,
                                      bool useProxyOnly)
{
    if (component != nullptr)
    {
        AnimationTask* at = findTaskFor (component);
        
        if (at == nullptr)
        {
            at = new AnimationTask (component);
            tasks.add (at);
            sendChangeMessage();
        }
        
        at->reset (finalAlpha, millisecondsToSpendMoving, useProxyComponent, useProxyOnly);
        
        if (! isTimerRunning())
        {
            lastTime = Time::getMillisecondCounter();
            startTimer (1000 / 50);
        }
    }
}

void ComponentFader::fadeOut (Component* component, int millisecondsToTake)
{
    if (component != nullptr)
    {
        if (component->isShowing() && millisecondsToTake > 0) {
            animateComponent (component, 0.0f, millisecondsToTake, true);
        }
        
        component->setVisible (false);
    }
}

void ComponentFader::fadeIn (Component* component, int millisecondsToTake)
{
    if (component != nullptr && ! (component->isVisible() && component->getAlpha() == 1.0f))
    {
        component->setAlpha (0.0f);
        component->setVisible (true);
        animateComponent (component, 1.0f, millisecondsToTake, false);
    }
}

void ComponentFader::fadeOutSnapshot(Component *component, int millisecondsToTake)
{
    if (component != nullptr)
    {
        if (component->isShowing() && millisecondsToTake > 0) {
            animateComponent (component, 0.0f, millisecondsToTake, true, true);
        }
    }
}

void ComponentFader::softFadeOut(float targetAlpha, Component *component, int millisecondsToTake)
{
    if (component != nullptr)
    {
        if (component->isShowing() && millisecondsToTake > 0) {
            animateComponent (component, targetAlpha, millisecondsToTake, false);
        }
    }
}

void ComponentFader::softFadeIn(Component *component, int millisecondsToTake)
{
    if (component != nullptr && ! (component->isVisible() && component->getAlpha() == 1.0f))
    {
        component->setVisible (true);
        animateComponent (component, 1.0f, millisecondsToTake, false);
    }
}

void ComponentFader::cancelAllAnimations (const bool setFinalAlpha)
{
    if (tasks.size() > 0)
    {
        if (setFinalAlpha) {
            for (int i = tasks.size(); --i >= 0;) {
                tasks.getUnchecked(i)->moveToFinalDestination();
            }
        }
        
        tasks.clear();
        sendChangeMessage();
    }
}

void ComponentFader::cancelAnimation (Component* const component,
                                      const bool setFinalAlpha)
{
    if (AnimationTask* const at = findTaskFor (component))
    {
        if (setFinalAlpha) {
            at->moveToFinalDestination();
        }
        
        tasks.removeObject (at);
        sendChangeMessage();
    }
}

bool ComponentFader::isAnimating (Component* component) const noexcept
{
    return findTaskFor (component) != nullptr;
}

bool ComponentFader::isAnimating() const noexcept
{
    return tasks.size() != 0;
}

void ComponentFader::timerCallback()
{
    const uint32 timeNow = Time::getMillisecondCounter();
    
    if (lastTime == 0 || lastTime == timeNow) {
        lastTime = timeNow;
    }
    
    const int elapsed = static_cast<int> (timeNow - lastTime);
    
    for (int i = tasks.size(); --i >= 0;)
    {
        if (! tasks.getUnchecked(i)->useTimeslice (elapsed))
        {
            tasks.remove (i);
            sendChangeMessage();
        }
    }
    
    lastTime = timeNow;
    
    if (tasks.size() == 0) {
        stopTimer();
    }
}
