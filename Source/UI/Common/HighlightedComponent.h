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

class HighlightedComponent : virtual public Component
{
public:
    
    HighlightedComponent() :
        highlighted(false),
        highlighter(nullptr),
        fadingHighlights(true)
    {
        this->setInterceptsMouseClicks(true, false);
    }
    
    void setUsesFadingHighlights(bool shouldFade)
    {
        this->fadingHighlights = shouldFade;
    }

    
    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//
    
    void mouseEnter(const MouseEvent &event) override
    {
        this->setHighlighted(true);
    }
    
    void mouseExit(const MouseEvent &event) override
    {
        this->setHighlighted(false);
        this->clearHighlighterComponentCache();
    }
    
protected:
    
    class EmptyHighlighter : public Component
    {
    public:
        
        void paint(Graphics &g) override
        {
            //g.setColour(Colours::black.withAlpha(0.15f));
            //g.drawRoundedRectangle(this->getLocalBounds().reduced(5).toFloat(), 10.f, 2.f);
        }
    };
    
    virtual Component *createHighlighterComponent()
    {
        return new EmptyHighlighter();
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
        this->highlighted = shouldBeHighlighted;
        
        if (this->highlighted)
        {
            if (this->highlighter == nullptr)
            {
                this->highlighter = this->createHighlighterComponent();
                this->highlighter->setInterceptsMouseClicks(false, false);
            }
            
            this->highlighter->setAlpha(0.f);
            this->highlighter->setBounds(this->getLocalBounds());
            this->addChildComponent(this->highlighter);
            
            if (this->fadingHighlights)
            {
                this->highlightAnimator.animateComponent(this->highlighter, this->highlighter->getBounds(), 1.f, 100, false, 0.0, 0.0);
            }
            else
            {
                this->highlighter->setVisible(true);
            }
        }
        else
        {
            if (this->highlighter != nullptr)
            {
                if (this->fadingHighlights)
                {
                    this->highlightAnimator.animateComponent(this->highlighter, this->highlighter->getBounds(), 0.f, 200, true, 0.0, 0.0);
                }
                
                this->removeChildComponent(this->highlighter);
            }
        }
    }
    
private:

    bool fadingHighlights;
    bool highlighted;
    
    ScopedPointer<Component> highlighter;
    ComponentAnimator highlightAnimator;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighlightedComponent)
    
};
