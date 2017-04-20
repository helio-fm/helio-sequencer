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

#define ORIGAMI_DEFAULT_MIN_SIZE 100
#define ORIGAMI_DEFAULT_MAX_SIZE 1000

class Origami : public Component
{
public:

    enum ColourIds
    {
        resizerLineColourId            = 0x99003001,
        resizerShadowColourId          = 0x99003002,
        resizerMovingLineColourId      = 0x99003003,
        resizerMovingShadowColourId    = 0x99003004
    };


    class ChildConstrainer : public ComponentBoundsConstrainer
    {
    public:

        explicit ChildConstrainer(Origami &origamiRef) : origami(origamiRef) { }

        void applyBoundsToComponent(Component *component, const Rectangle<int> &bounds) override;

    private:

        Origami &origami;

    };


    typedef struct
    {
        SafePointer<Component> component;
        ScopedPointer<Component> shadowAtStart;
        ScopedPointer<Component> shadowAtEnd;
        ScopedPointer<ResizableEdgeComponent> resizer;
        ScopedPointer<Origami::ChildConstrainer> constrainer;
        int size;
        int min;
        int max;
        bool fixedSize;
    } Page;


    Origami();

    ~Origami() override;


    //===------------------------------------------------------------------===//
    // Origami
    //===------------------------------------------------------------------===//

    Component *getLastFocusedComponent() const
    {
        return this->lastFocusedComponent;
    }

    virtual void addPage(Component *nonOwnedComponent,
                         bool addShadowAtStart = false,
                         bool addShadowAtEnd = true,
                         bool fixedSize = false,
                         int minSize = ORIGAMI_DEFAULT_MIN_SIZE,
                         int maxSize = ORIGAMI_DEFAULT_MAX_SIZE,
                         int insertIndex = -1) = 0;
    
    int getMinimumCommonSize() const;
    
    int getMaximumCommonSize() const;
    
    bool removePageContaining(Component *component);

    void clear();

    bool containsComponent(Component *component) const;


    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void focusGained(FocusChangeType cause) override;

    void focusOfChildComponentChanged(FocusChangeType cause) override;


protected:

    virtual void onPageResized(Component *component) = 0;

    OwnedArray<Origami::Page> pages;

private:

    SafePointer<Component> lastFocusedComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Origami)

};
