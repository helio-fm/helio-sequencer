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
            this->setWantsKeyboardFocus(true);
            this->addAndMakeVisible(this->content.get());
            this->content->setTopLeftPosition(padding / 2, padding / 2);
            this->syncWidthWithContent();
        }
    }

    bool keyPressed(const KeyPress &key) override
    {
        if (key.isKeyCode(KeyPress::escapeKey))
        {
            this->exitModalState(0);
            delete this;
        }

        return true;
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
        g.fillRect(1, 1, this->getWidth() - 2, this->getHeight() - 2);

        g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
        g.fillRect(1, 1, this->getWidth() - 2, 3);
        g.drawHorizontalLine(1, 1.f, float(this->getWidth() - 2));
        g.drawHorizontalLine(this->getHeight() - 2, 1.f, float(this->getWidth() - 2));
        g.drawVerticalLine(1, 1.f, float(this->getHeight() - 2));
        g.drawVerticalLine(this->getWidth() - 2, 1.f, float(this->getHeight() - 2));

        g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark));
        g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
        g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth()));
        g.drawVerticalLine(0, 0.f, float(this->getHeight()));
        g.drawVerticalLine(this->getWidth() - 1, 0.f, float(this->getHeight()));

        g.setColour(findDefaultColour(ColourIDs::BackgroundA::fill).darker(0.015f));
        constexpr float dashLength = 8.f;
        for (float i = dashLength - 2.f; i < this->getWidth(); i += (dashLength * 2.f))
        {
            g.fillRect(i + 2.f, 1.f, dashLength, 1.f);
            g.fillRect(i + 1.f, 2.f, dashLength, 1.f);
            g.fillRect(i, 3.f, dashLength, 1.f);
        }
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

void ContextMenuController::showMenu(const MouseEvent &e, int delay)
{
#if HELIO_DESKTOP
    //if (!e.mods.isRightButtonDown())
    //{
    //    return;
    //}

    // todo start an animation somewhere (breadcrumbs?)
    this->menuPosition = e.getEventRelativeTo(&App::Layout()).getPosition();
    if (!App::isUsingNativeTitleBar())
    {
        this->menuPosition.y += HEADLINE_HEIGHT;
    }

    this->startTimer(delay);
#endif
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
