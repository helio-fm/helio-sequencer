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
            this->lastTime = Time::getMillisecondCounter();
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
    
    if (this->lastTime == 0 || this->lastTime == timeNow)
    {
        this->lastTime = timeNow;
    }
    
    const int elapsed = static_cast<int> (timeNow - this->lastTime);
    
    for (int i = tasks.size(); --i >= 0;)
    {
        if (! tasks.getUnchecked(i)->useTimeslice (elapsed))
        {
            tasks.remove (i);
            sendChangeMessage();
        }
    }
    
    this->lastTime = timeNow;
    
    if (tasks.size() == 0)
    {
        stopTimer();
    }
}

void ComponentFader::AnimationTask::reset(float finalAlpha, int millisecondsToSpendMoving, bool useProxyComponent, bool useProxyOnly)
{
    this->msElapsed = 0;
    this->msTotal = jmax(1, millisecondsToSpendMoving);
    this->lastProgress = 0;
    this->destAlpha = finalAlpha;
    this->allowedToModifyOrigin = !useProxyOnly;

    this->isChangingAlpha = (finalAlpha != component->getAlpha());

    this->left = this->component->getX();
    this->top = this->component->getY();
    this->right = this->component->getRight();
    this->bottom = this->component->getBottom();
    this->alpha = this->component->getAlpha();

    const double invTotalDistance = 4.0 / 2.0;
    this->startSpeed = 0.0;
    this->midSpeed = invTotalDistance;
    this->endSpeed = 0.0;

    if (useProxyComponent)
    {
        this->proxy.reset(new ProxyComponent(*this->component));
    }
    else
    {
        this->proxy = nullptr;
    }

    if (this->allowedToModifyOrigin)
    {
        this->component->setVisible(!useProxyComponent);
    }
    else
    {
        if (this->proxy != nullptr)
        {
            this->proxy->setAlwaysOnTop(true);
        }
    }
}

bool ComponentFader::AnimationTask::useTimeslice(const int elapsed)
{
    if (auto *c = this->proxy != nullptr ?
        static_cast<Component *>(this->proxy.get()) : static_cast<Component *>(component))
    {
        msElapsed += elapsed;
        double newProgress = msElapsed / static_cast<double>(msTotal);

        if (newProgress >= 0 && newProgress < 1.0)
        {
            newProgress = ComponentFader::timeToDistance(newProgress, startSpeed, midSpeed, endSpeed);
            const double delta = (newProgress - lastProgress) / (1.0 - lastProgress);
            jassert(newProgress >= lastProgress);
            lastProgress = newProgress;

            if (delta < 1.0)
            {
                bool stillBusy = false;

                if (isChangingAlpha)
                {
                    alpha += (destAlpha - alpha) * delta;
                    c->setAlpha(static_cast<float>(alpha));
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

void ComponentFader::AnimationTask::moveToFinalDestination()
{
    if (component != nullptr &&
        this->allowedToModifyOrigin)
    {
        component->setAlpha(static_cast<float>(destAlpha));

        if (proxy != nullptr) {
            component->setVisible(destAlpha > 0);
        }
    }
}

ComponentFader::AnimationTask::ProxyComponent::ProxyComponent(Component& c)
{
    setWantsKeyboardFocus(false);
    setBounds(c.getBounds());
    setTransform(c.getTransform());
    setAlpha(c.getAlpha());
    setInterceptsMouseClicks(false, false);

    if (Component* const parent = c.getParentComponent()) {
        parent->addAndMakeVisible(this);
    }
    else if (c.isOnDesktop() && c.getPeer() != nullptr) {
        addToDesktop(c.getPeer()->getStyleFlags() | ComponentPeer::windowIgnoresKeyPresses);
    }
    else {
        jassertfalse; // seem to be trying to animate a component that's not visible..
    }

    const float scale = static_cast<float>(Desktop::getInstance().getDisplays()
        .findDisplayForPoint(getScreenBounds().getCentre()).scale);

    image = c.createComponentSnapshot(c.getLocalBounds(), true, scale);

    setVisible(true);
    toBehind(&c);
}

void ComponentFader::AnimationTask::ProxyComponent::paint(Graphics &g)
{
    g.setOpacity(1.0f);
    g.drawImageTransformed(image, AffineTransform::scale(getWidth() / (float)image.getWidth(),
        getHeight() / (float)image.getHeight()), false);
}
