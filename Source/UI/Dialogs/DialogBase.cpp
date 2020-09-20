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

struct DialogDragConstrainer final : public ComponentBoundsConstrainer
{
    void checkBounds(Rectangle<int> &bounds,
        const Rectangle<int> &previousBounds,
        const Rectangle<int> &limits,
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
    this->setInterceptsMouseClicks(true, true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->moveConstrainer = make<DialogDragConstrainer>();

    this->startTimer(DialogBase::focusCheckTimer, 100);
}

DialogBase::~DialogBase()
{
    if (this->isTimerRunning(DialogBase::focusCheckTimer))
    {
        this->stopTimer(DialogBase::focusCheckTimer);
    }

    if (this->background != nullptr)
    {
        this->background->postCommandMessage(CommandIDs::HideDialog);
    }
}

void DialogBase::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getBgCacheC(), {} });
    g.fillRect(1, 2, this->getWidth() - 2, this->getHeight() - 4);

    static constexpr auto lightAlpha = 2.f;

    HelioTheme::drawFrame(g, this->getWidth(), this->getHeight(), lightAlpha);

    g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight).
        withMultipliedAlpha(lightAlpha));

    g.fillRect(2, 1, this->getWidth() - 4, 5);

}

void DialogBase::parentHierarchyChanged()
{
    if (this->background == nullptr)
    {
        this->background = new DialogBackground();
        App::Layout().addAndMakeVisible(this->background);
    }
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
        animator.animateComponent(this,
            this->getBounds().reduced(20), 0.f, fadeoutTime, true, 0.0, 1.0);
    }
    else
    {
        animator.animateComponent(this,
            this->getBounds(), 0.f, fadeoutTime, true, 0.0, 1.0);
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

Rectangle<int> DialogBase::getContentBounds(float paddingAmount) const noexcept
{
    const auto actualBounds = this->getBounds()
        .reduced(DialogBase::contentMargin)
        .withTrimmedBottom(DialogBase::buttonsHeight)
        .translated(0, 1); // 1 pixel y offset because we have a header line

    return actualBounds.reduced(int(DialogBase::contentPadding * paddingAmount));
}

Rectangle<int> DialogBase::getCaptionBounds() const noexcept
{
    return this->getContentBounds().withHeight(DialogBase::captionHeight);
}

Rectangle<int> DialogBase::getButtonsBounds() const noexcept
{
    const auto actualBounds = this->getBounds().reduced(DialogBase::contentMargin);
    return actualBounds.withHeight(DialogBase::buttonsHeight).withBottomY(actualBounds.getBottom());
}

Rectangle<int> DialogBase::getRowBounds(float yInPercent, int height, int xPadding) const noexcept
{
    const auto area = this->getContentBounds().withTrimmedTop(DialogBase::captionHeight).reduced(xPadding, 0);
    const auto y = area.proportionOfHeight(yInPercent);
    return area.withHeight(height).translated(0, y - height / 2);
}

int DialogBase::getPaddingAndMarginTotal() const noexcept
{
    return DialogBase::contentPadding * 2 + DialogBase::contentMargin * 2;
}

// This is a hack to workaround some tricky focus-related issues 
void DialogBase::timerCallback(int timerId)
{
    if (timerId != DialogBase::focusCheckTimer)
    {
        return;
    }

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        auto *editor = dynamic_cast<TextEditor *>(this->getChildComponent(i));
        if (editor != nullptr && !editor->hasKeyboardFocus(false))
        {
            this->stopTimer(DialogBase::focusCheckTimer);
            editor->grabKeyboardFocus();
            editor->selectAll();
            return;
        }
    }

    // no editor found, or it already has the focus
    this->stopTimer(DialogBase::focusCheckTimer);
}
