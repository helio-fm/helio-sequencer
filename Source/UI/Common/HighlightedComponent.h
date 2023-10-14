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

class HighlightedComponent : virtual public Component
{
public:
    
    HighlightedComponent()
    {
        this->setInterceptsMouseClicks(true, false);
        this->setMouseClickGrabsKeyboardFocus(false);
    }
    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override
    {
        if (this->highlighter != nullptr)
        {
            this->highlighter->setBounds(this->getLocalBounds());
        }
    }

    void mouseEnter(const MouseEvent &) override
    {
        if (this->isEnabled())
        {
            this->setHighlighted(true);
        }
    }
    
    void mouseExit(const MouseEvent &) override
    {
        this->setHighlighted(false);
        this->clearHighlighterComponentCache();
    }
    
protected:
    
    virtual Component *createHighlighterComponent()
    {
        return new Component();
    }
    
    void clearHighlighterComponentCache()
    {
        this->highlighter = nullptr;
    }
    
    void clearHighlighterAndStopAnimations()
    {
        this->highlightAnimator.cancelAllAnimations(true);
        this->highlighter = nullptr;
    }
    
    virtual void setHighlighted(bool shouldBeHighlighted)
    {
        if (this->highlighted == shouldBeHighlighted)
        {
            return;
        }

        this->highlighted = shouldBeHighlighted;
        
        if (this->highlighted)
        {
            if (this->highlighter == nullptr)
            {
                this->highlighter.reset(this->createHighlighterComponent());
                if (this->highlighter == nullptr)
                {
                    return;
                }

                this->highlighter->setWantsKeyboardFocus(false);
                this->highlighter->setInterceptsMouseClicks(false, false);
            }

            this->highlighter->setBounds(this->getLocalBounds());
            this->addAndMakeVisible(this->highlighter.get());

            if (this->fadingHighlights)
            {
                this->highlighter->setAlpha(0.f);
                this->highlightAnimator.animateComponent(this->highlighter.get(),
                    this->highlighter->getBounds(), 1.f, Globals::UI::fadeInShort, false, 1.0, 0.0);
            }
        }
        else
        {
            if (this->highlighter != nullptr)
            {
                this->highlightAnimator.animateComponent(this->highlighter.get(),
                    this->highlighter->getBounds(), 0.f, Globals::UI::fadeOutShort, true, 1.0, 0.0);

                this->removeChildComponent(this->highlighter.get());
            }
        }
    }
    
private:

    bool fadingHighlights = false;
    bool highlighted = false;
    
    UniquePointer<Component> highlighter;
    ComponentAnimator highlightAnimator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighlightedComponent)
};
