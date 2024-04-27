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
#include "DialogBase.h"
#include "DialogBackground.h"
#include "HelioTheme.h"
#include "MainLayout.h"
#include "ColourIDs.h"
#include "App.h"

struct DialogDragConstrainer final : public ComponentBoundsConstrainer
{
    void checkBounds(Rectangle<int> &bounds,
        const Rectangle<int> &previousBounds,
        const Rectangle<int> &limits,
        bool, bool, bool, bool) override
    {
#if PLATFORM_DESKTOP
        const auto constrains = App::Layout().getBoundsForPopups()
            .translated(0, Globals::UI::headlineHeight).reduced(2);
#elif PLATFORM_MOBILE
        const auto constrains = App::Layout().getLocalBounds();
#endif

        bounds = bounds.constrainedWithin(constrains);
    }
};

DialogBase::DialogBase()
{
    this->toFront(true);
    this->setAlwaysOnTop(true);
    this->setInterceptsMouseClicks(true, true);

#if PLATFORM_DESKTOP
    this->setMouseClickGrabsKeyboardFocus(false);
#elif PLATFORM_MOBILE
    this->setWantsKeyboardFocus(true);
    this->setMouseClickGrabsKeyboardFocus(true);
#endif

    this->moveConstrainer = make<DialogDragConstrainer>();
}

DialogBase::~DialogBase()
{
    if (this->background != nullptr)
    {
        this->background->postCommandMessage(CommandIDs::DismissDialog);
    }
}

void DialogBase::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getDialogBackground(), {} });
    g.fillRect(1, 2, this->getWidth() - 2, this->getHeight() - 4);

    static constexpr auto lightAlpha = 1.5f;
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

void DialogBase::visibilityChanged()
{
    if (this->isShowing())
    {
        this->resetKeyboardFocus();
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

void DialogBase::inputAttemptWhenModal()
{
#if PLATFORM_MOBILE
    // try to detect if the virtual keyboard is shown:
    if (nullptr != dynamic_cast<TextEditor *>(Component::getCurrentlyFocusedComponent()))
    {
        this->resetKeyboardFocus(); // will hopefully hide it
        return;
    }
#endif

    this->postCommandMessage(CommandIDs::DismissDialog);
}

void DialogBase::dismiss()
{
    this->fadeOut();
    delete this;
}

void DialogBase::fadeOut()
{
    auto &animator = Desktop::getInstance().getAnimator();
    if (App::isOpenGLRendererEnabled())
    {
        animator.animateComponent(this, this->getBounds().reduced(20),
            0.f, Globals::UI::fadeOutLong, true, 0.0, 1.0);
    }
    else
    {
        animator.animateComponent(this, this->getBounds(),
            0.f, Globals::UI::fadeOutLong, true, 0.0, 1.0);
    }
}

void DialogBase::updatePosition()
{
#if PLATFORM_DESKTOP
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
#elif PLATFORM_MOBILE
    const auto isPhoneLayout = App::isRunningOnPhone();
    const auto minY = this->getHeight() / 2; // so the top is at 0
    const auto centreY = isPhoneLayout ? minY : jmax(minY, this->getParentHeight() / 4);
    this->setCentrePosition(this->getParentWidth() / 2, centreY);
#endif
}

Rectangle<int> DialogBase::getContentBounds(bool noPadding) const noexcept
{
    const auto isPhoneLayout = App::isRunningOnPhone();

    constexpr auto headerSize = 4;
    const auto actualBounds = this->getLocalBounds()
        .reduced(DialogBase::Defaults::contentMargin)
        .withTrimmedBottom(isPhoneLayout ? headerSize : headerSize + DialogBase::Defaults::DesktopAndTablet::buttonsHeight)
        .withTrimmedRight(isPhoneLayout ? DialogBase::Defaults::Phone::buttonsWidth : 0)
        .translated(0, headerSize);

    return actualBounds.reduced(noPadding ? 1 : DialogBase::Defaults::contentPaddingHorizontal,
        noPadding ? 1 : DialogBase::Defaults::contentPaddingVertical);
}

Rectangle<int> DialogBase::getCaptionBounds() const noexcept
{
    const auto isPhoneLayout = App::isRunningOnPhone();
    return this->getContentBounds().withHeight(isPhoneLayout ?
        DialogBase::Defaults::Phone::captionHeight :
        DialogBase::Defaults::DesktopAndTablet::captionHeight);
}

Rectangle<int> DialogBase::getButtonsBounds() const noexcept
{
    const auto isPhoneLayout = App::isRunningOnPhone();
    const auto actualBounds = this->getLocalBounds()
        .reduced(DialogBase::Defaults::contentMargin);
    return isPhoneLayout ?
        actualBounds
            .withWidth(DialogBase::Defaults::Phone::buttonsWidth)
            .withRightX(actualBounds.getRight()) :
        actualBounds
            .withHeight(DialogBase::Defaults::DesktopAndTablet::buttonsHeight)
            .withBottomY(actualBounds.getBottom()) ;
}

Rectangle<int> DialogBase::getButton1Bounds() const noexcept
{
    const auto isPhoneLayout = App::isRunningOnPhone();
    const auto buttonsBounds = this->getButtonsBounds();
    return isPhoneLayout ?
        buttonsBounds.withTrimmedBottom(buttonsBounds.getHeight() / 2) :
        buttonsBounds.withTrimmedLeft(buttonsBounds.getWidth() / 2);
}

Rectangle<int> DialogBase::getButton2Bounds() const noexcept
{
    const auto isPhoneLayout = App::isRunningOnPhone();
    const auto buttonsBounds = this->getButtonsBounds();
    return isPhoneLayout ?
        buttonsBounds.withTrimmedTop(buttonsBounds.getHeight() / 2 + 1) :
        buttonsBounds.withTrimmedRight(buttonsBounds.getWidth() / 2 + 1);
}

Rectangle<int> DialogBase::getContentWithoutCaptionBounds() const noexcept
{
    const auto isPhoneLayout = App::isRunningOnPhone();
    return this->getContentBounds().withTrimmedTop(isPhoneLayout ?
        DialogBase::Defaults::Phone::captionHeight :
        DialogBase::Defaults::DesktopAndTablet::captionHeight);
}

Rectangle<int> DialogBase::getRowBounds(float proportionOfHeight, int height, int xPadding) const noexcept
{
    const auto area = this->getContentWithoutCaptionBounds().reduced(xPadding, 0);
    const auto y = area.proportionOfHeight(proportionOfHeight);
    return area.withHeight(height).translated(0, y - height / 2);
}

int DialogBase::getHorizontalSpacingExceptContent() const noexcept
{
    const auto isPhoneLayout = App::isRunningOnPhone();
    const auto buttons = isPhoneLayout ? DialogBase::Defaults::Phone::buttonsWidth : 0;
    return DialogBase::Defaults::contentPaddingHorizontal * 2 + DialogBase::Defaults::contentMargin * 2 + buttons;
}
