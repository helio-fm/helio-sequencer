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
#include "LightShadowRightwards.h"
#include "LightShadowLeftwards.h"

void OrigamiVertical::addPage(Component *nonOwnedComponent,
                              bool addShadowAtStart /*= false*/,
                              bool addShadowAtEnd /*= true*/,
                              bool fixedWidth /*= false*/,
                              int minSize /*= ORIGAMI_DEFAULT_MIN_SIZE*/,
                              int maxSize /*= ORIGAMI_DEFAULT_MAX_SIZE*/,
                              int insertIndex /*= -1*/)
{
    if (this->containsComponent(nonOwnedComponent))
    {
        return;
    }

    auto newPage = new Origami::Page();

    newPage->fixedSize = fixedWidth;
    newPage->component = nonOwnedComponent;

    if (!fixedWidth)
    {
        newPage->component->setSize(this->getWidth(), this->getHeight());
    }

    newPage->min = fixedWidth ? nonOwnedComponent->getWidth() : minSize;
    newPage->max = fixedWidth ? nonOwnedComponent->getWidth() : maxSize;
    
    newPage->constrainer = new Origami::ChildConstrainer(*this);
    newPage->constrainer->setMinimumWidth(newPage->min);
    newPage->constrainer->setMaximumWidth(newPage->max);
    newPage->constrainer->setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);

    newPage->resizer =
        new ResizableEdgeComponent(newPage->component,
                                   newPage->constrainer,
                                   ResizableEdgeComponent::rightEdge);

    newPage->resizer->toFront(false);
    newPage->resizer->setAlwaysOnTop(true);

    this->pages.insert(insertIndex, newPage);

    const int commonFixedWidth = this->getCommonFixedWidth();

    // выравниваем по ширине
    for (auto page : this->pages)
    {
        if (! page->fixedSize)
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

    if (addShadowAtStart)
    {
        newPage->shadowAtStart = new LightShadowRightwards();
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
        newPage->shadowAtEnd = new LightShadowLeftwards();
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


void OrigamiVertical::resized()
{
    Rectangle<int> r(getLocalBounds());
    const int numPages = this->pages.size();

    // считем сумму длин компонентов,
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
            // по ней считаем коэффициенты ширины каждого,
            const float proportionalWidth = float(component->getWidth()) / (allPagesWidth - commonFixedWidth);
            // и применяем их к новой ширине:
            int newWidth = roundFloatToInt(float(this->getWidth() - commonFixedWidth) * proportionalWidth);
            newWidth = jmax(newWidth, page->constrainer->getMinimumWidth());
            newWidth = jmin(newWidth, page->constrainer->getMaximumWidth());
            component->setSize(newWidth, component->getHeight());
            // запомним эту ширину:
            page->size = newWidth;
        }
    }

    // здесь тупо строим сетку по готовым размерам
    for (int i = 0; i < numPages; ++i)
    {
        const Page *page = this->pages[i];
        Component *component = page->component;
        Rectangle<int> newBounds(r.removeFromLeft(component->getWidth()));
        newBounds.setWidth(jmax(component->getWidth(), newBounds.getWidth()));
        component->setBounds(newBounds);

        if (Component *shadow = page->shadowAtStart)
        {
            shadow->setBounds(component->getX(),
                              component->getY(),
                              shadow->getWidth(),
                              component->getHeight());
        }

        if (Component *shadow = page->shadowAtEnd)
        {
            shadow->setBounds(component->getRight() - shadow->getWidth(),
                              component->getY(),
                              shadow->getWidth(),
                              component->getHeight());
        }

        if (Origami::Page *nextPage = this->pages[i + 1])
        {
            page->resizer->setBounds(component->getRight() - 1,
                                     0,
                                     3,
                                     component->getHeight());

            if (! page->fixedSize)
            {
                const int nextSizeLimit = (nextPage->component->getWidth() - 100);
                const int newLimit = jmax(component->getWidth(), component->getWidth() + nextSizeLimit);
                page->constrainer->setMaximumWidth(newLimit);

                if (nextPage->fixedSize)
                {
                    page->resizer->setInterceptsMouseClicks(false, false);
                }
            }
        }
        else
        {
            component->setTopLeftPosition(this->getWidth() - component->getWidth(), 0);
            page->resizer->setBounds(component->getRight() + 2, 0, 0, 0);

            if (Origami::Page *prevPage = this->pages[i - 1])
            {
                prevPage->component->setSize(this->getWidth() -
                                             prevPage->component->getX() -
                                             component->getWidth(),
                                             prevPage->component->getHeight());

                prevPage->resizer->setBounds(prevPage->component->getRight() - 1,
                                             0,
                                             3,
                                             prevPage->component->getHeight());
                
                if (Component *shadow = prevPage->shadowAtEnd)
                {
                    shadow->setBounds(prevPage->component->getRight() - shadow->getWidth(),
                                      prevPage->component->getY(),
                                      shadow->getWidth(),
                                      prevPage->component->getHeight());
                }
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
