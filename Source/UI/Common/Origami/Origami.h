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
    class ChildConstrainer : public ComponentBoundsConstrainer
    {
    public:
        explicit ChildConstrainer(Origami &origamiRef) : origami(origamiRef) { }
        void applyBoundsToComponent(Component &component, Rectangle<int> bounds) override;

    private:
        Origami &origami;
    };

    typedef struct
    {
        SafePointer<Component> component;
        ScopedPointer<Component> shadowAtStart;
        ScopedPointer<Component> shadowAtEnd;
        ScopedPointer<Component> resizer;
        ScopedPointer<Origami::ChildConstrainer> constrainer;
        int size;
        int min = ORIGAMI_DEFAULT_MIN_SIZE;
        int max = ORIGAMI_DEFAULT_MAX_SIZE;
        bool fixedSize;
    } Page;

    Origami();
    ~Origami() override;

    //===------------------------------------------------------------------===//
    // Origami
    //===------------------------------------------------------------------===//

    virtual void addFlexiblePage(Component *component) = 0;
    virtual void addFixedPage(Component *component) = 0;
    virtual void addShadowAtTheStart() = 0;
    virtual void addShadowAtTheEnd() = 0;
    virtual void addResizer(int minSize, int maxSize) = 0;

    int getMinimumCommonSize() const;
    int getMaximumCommonSize() const;
    bool removePageContaining(Component *component);
    void clear();
    bool containsComponent(Component *component) const;

protected:

    virtual void onPageResized(Component *component) = 0;

    OwnedArray<Origami::Page> pages;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Origami)

};
