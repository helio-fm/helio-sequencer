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
#include "OrigamiHorizontal.h"
#include "ShadowDownwards.h"
#include "ShadowUpwards.h"

void OrigamiHorizontal::addFlexiblePage(Component *targetComponent)
{
    if (this->containsComponent(targetComponent))
    {
        return;
    }

    auto newPage = new Origami::Page();

    newPage->fixedSize = false;
    newPage->component = targetComponent;
    newPage->min = newPage->max = targetComponent->getHeight();

    newPage->component->setSize(this->getWidth(), this->getHeight());

    this->pages.insert(-1, newPage);

    const int commonFixedHeight = this->getCommonFixedHeight();

    // выравниваем по высоте
    for (auto page : this->pages)
    {
        if (!page->fixedSize)
        {
            Component *component = page->component;
            const int newHeight = (this->getHeight() - commonFixedHeight) / this->pages.size();
            component->setSize(component->getWidth(), newHeight);
            page->size = newHeight;
        }
        else
        {
            page->size = page->component->getHeight();
        }
    }

    this->addAndMakeVisible(newPage->component);
}


void OrigamiHorizontal::addFixedPage(Component *targetComponent)
{
    if (this->containsComponent(targetComponent))
    {
        return;
    }

    auto newPage = new Origami::Page();

    newPage->fixedSize = true;
    newPage->component = targetComponent;
    newPage->min = newPage->max = targetComponent->getHeight();

    this->pages.insert(-1, newPage);

    const int commonFixedHeight = this->getCommonFixedHeight();

    for (auto page : this->pages)
    {
        if (!page->fixedSize)
        {
            Component *component = page->component;
            const int newHeight = (this->getHeight() - commonFixedHeight) / this->pages.size();
            component->setSize(component->getWidth(), newHeight);
            page->size = newHeight;
        }
        else
        {
            page->size = page->component->getHeight();
        }
    }

    this->addAndMakeVisible(newPage->component);
}

void OrigamiHorizontal::addShadowAtTheStart()
{
    if (Page *page = this->pages.getLast())
    {
        page->shadowAtStart = new ShadowDownwards(Normal);
        page->shadowAtStart->setSize(10, 10);
        page->shadowAtStart->setInterceptsMouseClicks(false, false);
        this->addAndMakeVisible(page->shadowAtStart);
    }
}

void OrigamiHorizontal::addShadowAtTheEnd()
{
    if (Page *page = this->pages.getLast())
    {
        page->shadowAtEnd = new ShadowUpwards(Normal);
        page->shadowAtEnd->setSize(10, 10);
        page->shadowAtEnd->setInterceptsMouseClicks(false, false);
        this->addAndMakeVisible(page->shadowAtEnd);
    }
}

void OrigamiHorizontal::addResizer(int minSize, int maxSize)
{
    if (Page *page = this->pages.getLast())
    {
        page->min = minSize;
        page->max = maxSize;
        page->constrainer = new Origami::ChildConstrainer(*this);
        page->constrainer->setMinimumHeight(minSize);
        page->constrainer->setMaximumHeight(maxSize);
        page->constrainer->setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);

        page->resizer =
            new ResizableEdgeComponent(page->component,
                page->constrainer,
                ResizableEdgeComponent::bottomEdge);

        this->addAndMakeVisible(page->resizer);
        page->resizer->toFront(false);
        page->resizer->setAlwaysOnTop(true);
    }
}


void OrigamiHorizontal::resized()
{
    Rectangle<int> r(this->getLocalBounds());
    const int numPages = this->pages.size();
    
    float allPagesHeight = 0.f;

    for (int i = 0; i < numPages; ++i)
    {
        allPagesHeight += this->pages[i]->component->getHeight();
    }

    if (allPagesHeight == 0)
    {
        return;
    }

    const int commonFixedHeight = this->getCommonFixedHeight();

    for (int i = 0; i < numPages; ++i)
    {
        Page *page = this->pages[i];

        if (! page->fixedSize)
        {
            Component *component = page->component;
            const float proportionalHeight = float(component->getHeight()) / (allPagesHeight - commonFixedHeight);
            int newHeight = int(float(this->getHeight() - commonFixedHeight) * proportionalHeight);
            newHeight = jmax(newHeight, this->pages[i]->constrainer->getMinimumHeight());
            newHeight = jmin(newHeight, this->pages[i]->constrainer->getMaximumHeight());
            component->setSize(component->getWidth(), newHeight);
            this->pages[i]->size = newHeight;
        }
    }

    for (int i = 0; i < numPages; ++i)
    {
        const Page *page = this->pages[i];

        Component *component = page->component;
        Rectangle<int> newBounds(r.removeFromTop(component->getHeight()));
        newBounds.setHeight(jmax(component->getHeight(), newBounds.getHeight()));

        if (Origami::Page *nextPage = this->pages[i + 1])
        {
            this->updateLayout(page, newBounds);

            if (page->constrainer != nullptr)
            {
                const int nextSizeLimit = (nextPage->component->getHeight() - 100);
                const int newLimit = jmax(component->getHeight(), component->getHeight() + nextSizeLimit);
                page->constrainer->setMaximumHeight(newLimit);
            }
        }
        else
        {
            newBounds.setY(this->getHeight() - component->getHeight());
            this->updateLayout(page, newBounds);

            if (Origami::Page *prevPage = this->pages[i - 1])
            {
                Rectangle<int> prevBounds(prevPage->component->getBounds());
                prevBounds.setSize(prevPage->component->getWidth(),
                    this->getHeight() - prevPage->component->getY() -
                    component->getHeight());

                this->updateLayout(prevPage, prevBounds);
            }
        }
    }
}

void OrigamiHorizontal::onPageResized(Component *component)
{
    for (int i = 0; i < this->pages.size(); ++i)
    {
        Origami::Page *page = this->pages[i];

        if (component == page->component)
        {
            const int deltaHeight = (page->component->getHeight() - page->size);

            if (Origami::Page *nextPage = this->pages[i + 1])
            {
                nextPage->component->setSize(
                    nextPage->component->getWidth(),
                    nextPage->component->getHeight() - deltaHeight);
            }

            break;
        }
    }

    this->resized();
}

int OrigamiHorizontal::getCommonFixedHeight() const
{
    int commonFixedHeight = 0;

    for (auto page : this->pages)
    {
        commonFixedHeight +=
            page->fixedSize ? page->component->getHeight() : 0;
    }

    return commonFixedHeight;
}

void OrigamiHorizontal::updateLayout(const Origami::Page *page, Rectangle<int> bounds)
{
    Component *c = page->component;
    c->setBounds(bounds);

    if (Component *shadow = page->shadowAtStart)
    {
        shadow->setBounds(c->getX(), c->getY(), c->getWidth(), shadow->getHeight());
    }

    if (Component *shadow = page->shadowAtEnd)
    {
        shadow->setBounds(c->getX(), c->getBottom() - shadow->getHeight(), c->getWidth(), shadow->getHeight());
    }
    
    if (Component *resizer = page->resizer)
    {
        resizer->setBounds(0, c->getBottom() - 1, c->getWidth(), 2);
    }
}
