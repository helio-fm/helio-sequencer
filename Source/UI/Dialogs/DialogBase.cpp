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
#include "DialogBase.h"
#include "DialogBackground.h"
#include "HelioTheme.h"
#include "MainLayout.h"
#include "CommandIDs.h"
#include "ColourIDs.h"

#define DIALOG_HAS_BACKGROUND 1
//#define DIALOG_HAS_BACKGROUND 0

struct DialogDragConstrainer final : public ComponentBoundsConstrainer
{
    void checkBounds(Rectangle<int>& bounds,
        const Rectangle<int>& previousBounds,
        const Rectangle<int>& limits,
        bool, bool, bool, bool) override
    {
        const auto constrain = App::Layout().getBoundsForPopups()
            .translated(0, Globals::UI::headlineHeight).reduced(2);

        bounds = bounds.constrainedWithin(constrain);
    }
};

DialogBase::DialogBase()
{
    this->toFront(true);
    this->setAlwaysOnTop(true);

    this->moveConstrainer = make<DialogDragConstrainer>();
}

DialogBase::~DialogBase()
{
#if DIALOG_HAS_BACKGROUND
    if (this->background != nullptr)
    {
        this->background->postCommandMessage(CommandIDs::HideDialog);
    }
#endif
}

void DialogBase::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getBgCacheC(), {} });
    g.fillRect(1, 2, this->getWidth() - 2, this->getHeight() - 4);

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark));
    g.fillRect(1, 0, this->getWidth() - 2, 2);
    g.fillRect(1, this->getHeight() - 2, this->getWidth() - 2, 2);
    g.fillRect(0, 1, 2, this->getHeight() - 2);
    g.fillRect(this->getWidth() - 2, 1, 2, this->getHeight() - 2);

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight).withMultipliedAlpha(2.f));
    g.fillRect(2, 1, this->getWidth() - 4, 5);
    g.fillRect(2, 1, this->getWidth() - 4, 1);
    g.fillRect(2, this->getHeight() - 2, this->getWidth() - 4, 1);
    g.fillRect(1, 2, 1, this->getHeight() - 4);
    g.fillRect(this->getWidth() - 2, 2, 1, this->getHeight() - 4);
}

void DialogBase::parentHierarchyChanged()
{
#if DIALOG_HAS_BACKGROUND
    if (this->background == nullptr)
    {
        this->background = new DialogBackground();
        App::Layout().addAndMakeVisible(this->background);
    }
#endif
}

void DialogBase::mouseDown(const MouseEvent &e)
{
    this->dragger.startDraggingComponent(this, e);
}

void DialogBase::mouseDrag(const MouseEvent &e)
{
    this->dragger.dragComponent(this, e, this->moveConstrainer.get());
}

void DialogBase::dismiss()
{
    this->fadeOut();
    delete this;
}

void DialogBase::fadeOut()
{
    static constexpr auto fadeoutTime = 150;
    auto &animator = Desktop::getInstance().getAnimator();
    if (App::isOpenGLRendererEnabled())
    {
        animator.animateComponent(this, this->getBounds().reduced(20), 0.f, fadeoutTime, true, 0.0, 1.0);
    }
    else
    {
        animator.animateComponent(this, this->getBounds(), 0.f, fadeoutTime, true, 0.0, 1.0);
    }
}

void DialogBase::updatePosition()
{
#if HELIO_DESKTOP
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
#elif HELIO_MOBILE
    // Place the dialog slightly above the center, so that screen keyboard doesn't mess with it:
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 3.5f);
#endif
}
