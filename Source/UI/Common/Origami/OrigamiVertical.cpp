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
#include "OrigamiVertical.h"
#include "ShadowRightwards.h"
#include "ShadowLeftwards.h"

void OrigamiVertical::addFlexiblePage(Component *targetComponent)
{
    if (this->containsComponent(targetComponent))
    {
        return;
    }

    auto newPage = new Origami::Page();
    newPage->fixedSize = false;
    newPage->component = targetComponent;
    newPage->component->setSize(this->getWidth(), this->getHeight());
    newPage->min = newPage->max = targetComponent->getWidth();

    this->pages.insert(-1, newPage);
    const int commonFixedWidth = this->getCommonFixedWidth();

    // width justification
    for (auto page : this->pages)
    {
        if (!page->fixedSize)
        {
            Component *component = page->component;
            const int newWidth = (this->getWidth() - commonFixedWidth) / this->pages.size();
            component->setSize(newWidth, component->getHeight());
            page->size = newWidth;
        }
        else
        {
            page->size = page->component->getWidth();
        }
    }

    this->addAndMakeVisible(newPage->component);
}

void OrigamiVertical::addFixedPage(Component *targetComponent)
{
    if (this->containsComponent(targetComponent))
    {
        return;
    }

    auto newPage = new Origami::Page();
    newPage->fixedSize = true;
    newPage->component = targetComponent;
    newPage->min = newPage->max = targetComponent->getWidth();
    this->pages.insert(-1, newPage);

    const int commonFixedWidth = this->getCommonFixedWidth();

    // width justification
    for (auto page : this->pages)
    {
        if (!page->fixedSize)
        {
            Component *component = page->component;
            const int newWidth = (this->getWidth() - commonFixedWidth) / this->pages.size();
            component->setSize(newWidth, component->getHeight());
            page->size = newWidth;
        }
        else
        {
            page->size = page->component->getWidth();
        }
    }

    this->addAndMakeVisible(newPage->component);
}

void OrigamiVertical::addShadowAtTheStart()
{
    if (Page *page = this->pages.getLast())
    {
        page->shadowAtStart = new ShadowRightwards(Normal);
        page->shadowAtStart->setSize(10, 10);
        this->addAndMakeVisible(page->shadowAtStart);
    }
}

void OrigamiVertical::addShadowAtTheEnd()
{
    if (Page *page = this->pages.getLast())
    {
        page->shadowAtEnd = new ShadowLeftwards(Normal);
        page->shadowAtEnd->setSize(10, 10);
        this->addAndMakeVisible(page->shadowAtEnd);
    }
}

void OrigamiVertical::addResizer(int minSize, int maxSize)
{
    if (Page *page = this->pages.getLast())
    {
        page->min = minSize;
        page->max = maxSize;
        page->constrainer = new Origami::ChildConstrainer(*this);
        page->constrainer->setMinimumWidth(minSize);
        page->constrainer->setMaximumWidth(maxSize);
        page->constrainer->setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);

        page->resizer =
            new ResizableEdgeComponent(page->component,
                page->constrainer,
                ResizableEdgeComponent::rightEdge);

        this->addAndMakeVisible(page->resizer);
        page->resizer->toFront(false);
        page->resizer->setAlwaysOnTop(true);
    }
}

void OrigamiVertical::resized()
{
    Rectangle<int> r(this->getLocalBounds());
    const int numPages = this->pages.size();

    float allPagesWidth = 0.f;
    for (int i = 0; i < numPages; ++i)
    {
        allPagesWidth += this->pages[i]->component->getWidth();
    }

    if (allPagesWidth == 0)
    {
        return;
    }

    const int commonFixedWidth = this->getCommonFixedWidth();

    for (int i = 0; i < numPages; ++i)
    {
        Page *page = this->pages[i];

        if (! page->fixedSize)
        {
            Component *component = page->component;
            const float proportionalWidth = float(component->getWidth()) / (allPagesWidth - commonFixedWidth);
            int newWidth = roundToInt(float(this->getWidth() - commonFixedWidth) * proportionalWidth);
            if (page->constrainer != nullptr)
            {
                newWidth = jmax(newWidth, page->constrainer->getMinimumWidth());
            }
            component->setSize(newWidth, component->getHeight());
            page->size = newWidth;
        }
    }

    // Update the grid
    for (int i = 0; i < numPages; ++i)
    {
        const Page *page = this->pages[i];
        Component *component = page->component;
        Rectangle<int> newBounds(r.removeFromLeft(component->getWidth()));
        newBounds.setWidth(jmax(component->getWidth(), newBounds.getWidth()));

        if (Origami::Page *nextPage = this->pages[i + 1])
        {
            this->updateLayout(page, newBounds);

            if (page->constrainer != nullptr)
            {
                const int nextSizeLimit = (nextPage->component->getWidth() - 100);
                const int newLimit = jmax(component->getWidth(), component->getWidth() + nextSizeLimit);
                page->constrainer->setMaximumWidth(newLimit);
            }
        }
        else
        {
            // Align the last components to the right:
            newBounds.setX(this->getWidth() - component->getWidth());
            this->updateLayout(page, newBounds);

            if (Origami::Page *prevPage = this->pages[i - 1])
            {
                Rectangle<int> prevBounds(prevPage->component->getBounds());
                prevBounds.setSize(this->getWidth() -
                    prevPage->component->getX() - component->getWidth(),
                    prevPage->component->getHeight());

                this->updateLayout(prevPage, prevBounds);
            }
        }
    }
}

void OrigamiVertical::onPageResized(Component *component)
{
    // ищем дельту ширины
    for (int i = 0; i < this->pages.size(); ++i)
    {
        Origami::Page *page = this->pages[i];

        if (component == page->component)
        {
            const int deltaWidth = (page->component->getWidth() - page->size);

            // теперь надо ее же вычесть из ширины следующего, если он есть
            if (Origami::Page *nextPage = this->pages[i + 1])
            {
                // OwnedArray вернет nullptr, если мы за границами массива
                nextPage->component->setSize(
                    nextPage->component->getWidth() - deltaWidth,
                    nextPage->component->getHeight());
            }

            break;
        }
    }

    this->resized();
}

int OrigamiVertical::getCommonFixedWidth() const
{
    int commonFixedWidth = 0;

    for (auto page : this->pages)
    {
        commonFixedWidth +=
            page->fixedSize ? page->component->getWidth() : 0;
    }

    return commonFixedWidth;
}

void OrigamiVertical::updateLayout(const Origami::Page *page, Rectangle<int> bounds)
{
    Component *c = page->component;
    c->setBounds(bounds);

    if (Component *shadow = page->shadowAtStart)
    {
        shadow->setBounds(c->getX(), c->getY(), shadow->getWidth(), c->getHeight());
    }

    if (Component *shadow = page->shadowAtEnd)
    {
        shadow->setBounds(c->getRight() - shadow->getWidth(), c->getY(), shadow->getWidth(), c->getHeight());
    }
    
    if (Component *resizer = page->resizer)
    {
        resizer->setBounds(c->getRight() - 1, 0, 2, c->getHeight());
    }
}
