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
#include "SizeSwitcherComponent.h"
#include "Icons.h"

#define DEFAULT_ALPHA 0.2f
#define HIGHLIGHT_ALPHA 0.35f
#define THIS_MARGIN 8

//#if HELIO_DESKTOP
#   define SIZESWITCHER_HAS_ANIMATIONS 1
//#else if defined HELIO_MOBILE
//#   define SIZESWITCHER_HAS_ANIMATIONS 0
//#endif


SizeSwitcherComponent::SizeSwitcherComponent(Component *target, int min, int mid, int max) :
    targetComponent(target),
    minWidth(min),
    midWidth(mid),
    maxWidth(max),
    alpha(DEFAULT_ALPHA)
{
    this->setSize(32, 32);
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(true, false);
    this->setAlpha(DEFAULT_ALPHA);

    this->addAndMakeVisible(this->minButton);
    this->addAndMakeVisible(this->maxButton);
}


//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void SizeSwitcherComponent::paint(Graphics &g)
{
//    const Colour hideToggleColour(this->findColour(SizeSwitcherComponent::borderColourId));
//    g.setColour(hideToggleColour);
//    g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
}

void SizeSwitcherComponent::parentHierarchyChanged()
{
    this->updateButtonsImages();
}

void SizeSwitcherComponent::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->minButton.setTopLeftPosition(THIS_MARGIN,
                                       (this->getHeight() / 2) - (this->minButton.getHeight() / 2));
    
    this->maxButton.setTopLeftPosition((this->getParentWidth() - this->maxButton.getWidth() - THIS_MARGIN),
                                       (this->getHeight() / 2) - (this->maxButton.getHeight() / 2));
    
    if (this->shouldTriggerMinimization())
    {
        this->animator.fadeIn(&this->minButton, 500);
        this->animator.fadeOut(&this->maxButton, 1);
    }
    else
    {
        this->animator.fadeIn(&this->maxButton, 500);
        this->animator.fadeOut(&this->minButton, 200);
    }
    //[/UserCode_parentSizeChanged]
}

void SizeSwitcherComponent::resized()
{
    this->updateButtonsImages();
}

void SizeSwitcherComponent::mouseEnter(const MouseEvent &e)
{
    this->setAlpha(HIGHLIGHT_ALPHA);
}

void SizeSwitcherComponent::mouseExit(const MouseEvent &e)
{
    this->setAlpha(DEFAULT_ALPHA);
}

void SizeSwitcherComponent::mouseUp(const MouseEvent &e)
{
    this->trigger();
}

inline int iconHeight(int baseHeight)
{
    return int(float(baseHeight) * 1.25);
}

void SizeSwitcherComponent::updateButtonsImages()
{
    Image minButtonImage(Icons::findByName(Icons::backward, iconHeight(this->getHeight())));
    Image maxButtonImage(Icons::findByName(Icons::forward, iconHeight(this->getHeight())));
    
    this->minButton.setImage(minButtonImage);
    this->maxButton.setImage(maxButtonImage);
}

void SizeSwitcherComponent::trigger()
{
    if (this->shouldTriggerMinimization())
    {
#if SIZESWITCHER_HAS_ANIMATIONS
        Rectangle<int> parentBounds(this->getParentComponent()->getBounds());
        parentBounds.setSize(this->minWidth, this->getParentHeight());
        this->animator.animateComponent(this->getParentComponent(), parentBounds, 1.f, 150, false, 0.f, 0.f);
#else
        this->getParentComponent()->setSize(this->minWidth, this->getParentHeight());
#endif
    }
    else
    {
        if (this->getParentWidth() < this->midWidth)
        {
#if SIZESWITCHER_HAS_ANIMATIONS
            Rectangle<int> parentBounds(this->getParentComponent()->getBounds());
            parentBounds.setSize(this->midWidth, this->getParentHeight());
            this->animator.animateComponent(this->getParentComponent(), parentBounds, 1.f, 150, false, 0.f, 0.f);
#else
            this->getParentComponent()->setSize(this->midWidth, this->getParentHeight());
#endif
        }
        else
        {
#if SIZESWITCHER_HAS_ANIMATIONS
            Rectangle<int> parentBounds(this->getParentComponent()->getBounds());
            parentBounds.setSize(this->maxWidth, this->getParentHeight());
            this->animator.animateComponent(this->getParentComponent(), parentBounds, 1.f, 150, false, 0.f, 0.f);
#else
            this->getParentComponent()->setSize(this->maxWidth, this->getParentHeight());
#endif
        }
    }
}

bool SizeSwitcherComponent::shouldTriggerMinimization() const
{
    const int parentWidth = this->getParentWidth();
    return parentWidth > ((this->minWidth + this->maxWidth) / 2);
}
