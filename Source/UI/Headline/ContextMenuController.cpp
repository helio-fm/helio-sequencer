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
#include "ContextMenuController.h"

#include "Headline.h"
#include "ColourIDs.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "App.h"

class ContextMenuComponent final : public Component
{
public:

    static constexpr auto padding = 4;

    ContextMenuComponent(UniquePointer<Component> component) : content(move(component))
    {
        if (this->content.get())
        {
            this->addAndMakeVisible(this->content.get());
            this->syncWidthWithContent();
        }
    }

    void childBoundsChanged(Component *child)
    {
        this->syncWidthWithContent();
    }

    void inputAttemptWhenModal() override
    {
        this->exitModalState(0);
        delete this;
    }

    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::BackgroundA::fill).brighter(0.035f));
        g.fillRect(1, padding - 3, this->getWidth() - 3, this->getHeight() - padding + 3);

        // Draw a nice border around the menu:
        g.setColour(Colours::black.withAlpha(40.f / 255.f));
        g.drawHorizontalLine(this->getHeight() - 1, 1.f, float(this->getWidth() - 2));
        g.drawVerticalLine(0, padding - 1.f, float(this->getHeight() - 1));
        g.drawVerticalLine(this->getWidth() - 2, padding - 1.f, float(this->getHeight() - 1));

        g.setColour(Colours::white.withAlpha(9.f / 255.f));
        g.drawHorizontalLine(this->getHeight() - 2, 1.f, float(this->getWidth() - 2));
        g.drawVerticalLine(1, padding - 2.f, float(this->getHeight() - 1));
        g.drawVerticalLine(this->getWidth() - 3, padding - 2.f, float(this->getHeight() - 1));
    }

private:

    void syncWidthWithContent()
    {
        if (this->getWidth() != this->content->getWidth() + padding ||
            this->getHeight() != this->content->getHeight() + padding)
        {
            const int w = this->content->getWidth() + padding;
            this->setSize(w, this->content->getHeight() + padding);
        }
    }

    UniquePointer<Component> content;

};

ContextMenuController::ContextMenuController(Component &owner) :
    owner(owner) {}

void ContextMenuController::showAfter(int delay, const MouseEvent &e)
{
    // todo start an animation somewhere (breadcrumbs?)
    this->menuPosition = e.getEventRelativeTo(&App::Layout()).getPosition();
    if (!App::isUsingNativeTitleBar())
    {
        this->menuPosition.y += HEADLINE_HEIGHT;
    }

    this->startTimer(delay);
}

void ContextMenuController::cancelIfPending()
{
    this->stopTimer();
}

void ContextMenuController::timerCallback()
{
    this->stopTimer();

    // here we totally rely on breadcrumbs logic to get the context menu contents,
    // because at the point when user calls the context menu, the breadcrumbs item and
    // its menu data source all have been set up and ready to use, we just get
    // the tail item of breadcrumbs, and use its menu source:
    
    const auto menuSource = App::Layout().getTailMenu();
    auto content = menuSource->createMenu();

    if (content != nullptr)
    {
        // todo fancy flash at the breadcrumbs tail?
        this->owner.setMouseCursor(MouseCursor::NormalCursor);
        auto container = make<ContextMenuComponent>(move(content));
        container->setTopLeftPosition(this->menuPosition);
        App::showModalComponent(move(container));
    }
}
