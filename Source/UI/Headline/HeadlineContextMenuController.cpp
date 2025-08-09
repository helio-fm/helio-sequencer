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

#include "Common.h"
#include "HeadlineContextMenuController.h"

#include "HeadlineItem.h"
#include "ColourIDs.h"
#include "MainLayout.h"
#include "HelioTheme.h"

class ContextMenuComponent final : public Component
{
public:

    ContextMenuComponent()
    {
        this->setOpaque(true);
        this->setPaintingIsUnclipped(true);
        this->setAccessible(false);
    }

    ~ContextMenuComponent() override
    {
        auto *tail = App::Layout().getMenuTail();
        jassert(tail != nullptr);

        tail->hideContextMenuMarker();
    }

    void setContent(UniquePointer<Component> component)
    {
        this->content = move(component);
        if (this->content.get())
        {
            this->addAndMakeVisible(this->content.get());
            this->content->setTopLeftPosition(padding / 2, padding / 2 + 1);
            this->syncBoundsWithContent();
        }
    }

    bool keyPressed(const KeyPress &key) override
    {
        if (key.isKeyCode(KeyPress::escapeKey))
        {
            this->dismiss();
        }

        return true;
    }

    void childBoundsChanged(Component *child) override
    {
        this->syncBoundsWithContent();
    }

    void inputAttemptWhenModal() override
    {
        this->dismiss();
    }

    void dismiss()
    {
        this->exitModalState(0);
        delete this;
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillRect(1, 1, this->getWidth() - 2, this->getHeight() - 2);
        HelioTheme::drawFrame(g, this->getWidth(), this->getHeight(), 1.25f, 1.f);

        g.setColour(this->headerColour);
        g.fillRect(1, 1, this->getWidth() - 2, 2);

        g.setColour(this->selectionMarkerColour);
        HelioTheme::drawDashedHorizontalLine2(g, 4.f, 1.f, float(this->getWidth() - 3), 8.f);
    }

private:

    static constexpr auto padding = 3;

    void syncBoundsWithContent()
    {
        if (this->getWidth() != this->content->getWidth() + padding ||
            this->getHeight() != this->content->getHeight() + padding)
        {
            const int w = this->content->getWidth() + padding;
            this->setSize(w, this->content->getHeight() + padding);

            const auto layoutBounds = App::Layout().getBoundsForPopups();
            const auto myBounds = this->getBounds();
            const auto constrained = myBounds.constrainedWithin(layoutBounds);
            this->setBounds(constrained);
        }
    }

    UniquePointer<Component> content;

    const Colour fillColour = findDefaultColour(ColourIDs::Menu::fill);
    const Colour headerColour = findDefaultColour(ColourIDs::Menu::header);
    const Colour selectionMarkerColour = findDefaultColour(ColourIDs::Menu::selectionMarker);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContextMenuComponent)
};

HeadlineContextMenuController::HeadlineContextMenuController(Component &owner) :
    owner(owner) {}

void HeadlineContextMenuController::showMenu(const MouseEvent &e, int delay)
{
#if PLATFORM_DESKTOP
    if (!e.mods.isRightButtonDown())
    {
        jassertfalse;
        return;
    }

    this->menuPosition = e.getEventRelativeTo(&App::Layout()).getPosition();

    if (!App::isUsingNativeTitleBar())
    {
        this->menuPosition.y += Globals::UI::headlineHeight;
    }

    this->startTimer(delay);
#endif
}

void HeadlineContextMenuController::cancelIfPending()
{
    this->stopTimer();
}

void HeadlineContextMenuController::timerCallback()
{
    this->stopTimer();

    if (this->onWillShowMenu != nullptr)
    {
        // give a chance to update selection, if needed
        this->onWillShowMenu();
    }

    // here we totally rely on breadcrumbs logic to get the context menu contents,
    // because at the point when user calls the context menu, the breadcrumbs item and
    // its menu data source all have been set up and ready to use, we just get
    // the tail item of breadcrumbs, and use its menu source:
    auto *tail = App::Layout().getMenuTail();
    jassert(tail != nullptr);

    if (auto menuContent = tail->getDataSource()->createMenu())
    {
        // show fancy marker at the breadcrumbs tail:
        tail->showContextMenuMarker();

        this->owner.setMouseCursor(MouseCursor::NormalCursor);
        auto container = make<ContextMenuComponent>();
        container->setTopLeftPosition(this->menuPosition);
        container->setContent(move(menuContent));
        App::showModalComponent(move(container));
    }
}
