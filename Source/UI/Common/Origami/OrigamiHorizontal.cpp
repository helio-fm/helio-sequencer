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
#include "LightShadowDownwards.h"
#include "LightShadowUpwards.h"

// надо как-то сохранять размеры компонентов
// и иметь возможность устанавливать размер при добавлении страницы

void OrigamiHorizontal::addPage(Component *nonOwnedComponent,
                                bool addShadowAtStart /*= false*/,
                                bool addShadowAtEnd /*= true*/,
                                bool fixedHeight /*= false*/,
                                int minSize /*= ORIGAMI_DEFAULT_MIN_SIZE*/,
                                int maxSize /*= ORIGAMI_DEFAULT_MAX_SIZE*/,
                                int insertIndex /*= -1*/)
{
    if (this->containsComponent(nonOwnedComponent))
    {
        return;
    }

    auto newPage = new Origami::Page();

    newPage->fixedSize = fixedHeight;
    newPage->component = nonOwnedComponent;

    if (!fixedHeight)
    {
        newPage->component->setSize(this->getWidth(), this->getHeight());
    }

    newPage->min = fixedHeight ? nonOwnedComponent->getHeight() : minSize;
    newPage->max = fixedHeight ? nonOwnedComponent->getHeight() : maxSize;
    
    newPage->constrainer = new Origami::ChildConstrainer(*this);
    newPage->constrainer->setMinimumHeight(newPage->min);
    newPage->constrainer->setMaximumHeight(newPage->max);
    newPage->constrainer->setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);

    newPage->resizer =
        new ResizableEdgeComponent(newPage->component,
                                   newPage->constrainer,
                                   ResizableEdgeComponent::bottomEdge);

    newPage->resizer->toFront(false);
    newPage->resizer->setAlwaysOnTop(true);

    this->pages.insert(insertIndex, newPage);

    const int commonFixedHeight = this->getCommonFixedHeight();

    // выравниваем по высоте
    for (auto page : this->pages)
    {
        if (! page->fixedSize)
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

    if (addShadowAtStart)
    {
        newPage->shadowAtStart = new LightShadowDownwards();
        newPage->shadowAtStart->setSize(10, 10);
        newPage->shadowAtStart->setInterceptsMouseClicks(false, false);
        this->addAndMakeVisible(newPage->shadowAtStart);
    }
    else
    {
        newPage->shadowAtStart = nullptr;
    }

    if (addShadowAtEnd)
    {
        newPage->shadowAtEnd = new LightShadowUpwards();
        newPage->shadowAtEnd->setSize(10, 10);
        newPage->shadowAtEnd->setInterceptsMouseClicks(false, false);
        this->addAndMakeVisible(newPage->shadowAtEnd);
    }
    else
    {
        newPage->shadowAtEnd = nullptr;
    }

    this->addAndMakeVisible(newPage->resizer);
    this->resized();
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
        
        component->setBounds(newBounds);

        if (Component *shadow = page->shadowAtStart)
        {
            shadow->setBounds(component->getX(),
                              component->getY(),
                              component->getWidth(),
                              shadow->getHeight());
        }

        if (Component *shadow = page->shadowAtEnd)
        {
            shadow->setBounds(component->getX(),
                              component->getBottom() - shadow->getHeight(),
                              component->getWidth(),
                              shadow->getHeight());
        }

        ComponentBoundsConstrainer *constrainer = page->constrainer;
        Component *resizer = page->resizer;

        // настроить констрейнеры, так, чтоб
        if (Origami::Page *nextPage = this->pages[i + 1])
        {
            resizer->setBounds(0, component->getBottom() - 2, component->getWidth(), 4);

            if (! page->fixedSize)
            {
                const int nextSizeLimit = (nextPage->component->getHeight() - nextPage->min);
                const int newLimit = jmax(component->getHeight(), component->getHeight() + nextSizeLimit);
                //Logger::writeToLog(String(newLimit));
                constrainer->setMaximumHeight(newLimit);

                if (nextPage->fixedSize)
                {
                    resizer->setInterceptsMouseClicks(false, false);
                }
            }
        }
        else
        {
            component->setTopLeftPosition(0, this->getHeight() - component->getHeight());
            resizer->setBounds(0, component->getBottom() + 2, 0, 0);

            if (Origami::Page *prevPage = this->pages[i - 1])
            {
                prevPage->component->setSize(prevPage->component->getWidth(),
                                             this->getHeight() -
                                             prevPage->component->getY() -
                                             component->getHeight());

                prevPage->resizer->setBounds(0,
                                             prevPage->component->getBottom() - 2,
                                             prevPage->component->getWidth(),
                                             4);
                
                if (Component *shadow = prevPage->shadowAtEnd)
                {
                    shadow->setBounds(prevPage->component->getX(),
                                      prevPage->component->getBottom() - shadow->getHeight(),
                                      prevPage->component->getWidth(),
                                      shadow->getHeight());
                }
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
                //nextPage->constrainer->applyBoundsToComponent(nextPage->component,
                //                                              Rectangle<int>(nextPage->component->getX(),
                //                                                             nextPage->component->getY(),
                //                                                             nextPage->component->getWidth(),
                //                                                             nextPage->component->getHeight() - deltaHeight));
                
                // must be safe
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
