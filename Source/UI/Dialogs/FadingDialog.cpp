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
#include "FadingDialog.h"
#include "DialogBackground.h"
#include "CommandIDs.h"
#include "MainLayout.h"

#define DIALOG_HAS_BACKGROUND 1
//#define DIALOG_HAS_BACKGROUND 0

FadingDialog::FadingDialog()
{
    this->setAlpha(0.f);
    this->setAlwaysOnTop(true);
    this->toFront(true);
}

FadingDialog::~FadingDialog()
{
#if DIALOG_HAS_BACKGROUND
    if (this->background != nullptr)
    {
        this->background->postCommandMessage(CommandIDs::HideDialog);
    }
#endif
}

void FadingDialog::parentHierarchyChanged()
{
#if DIALOG_HAS_BACKGROUND
    if (this->background == nullptr)
    {
        this->background = new DialogBackground();
        App::Layout().addAndMakeVisible(this->background);
    }
#endif
}

void FadingDialog::dismiss()
{
    this->fadeOut();
    delete this;
}

void FadingDialog::fadeOut()
{
    const int fadeoutTime = 200;
    auto &animator = Desktop::getInstance().getAnimator();
    if (App::isOpenGLRendererEnabled())
    {
        animator.animateComponent(this, this->getBounds().reduced(20), 0.f, fadeoutTime, true, 0.0, 0.0);
    }
    else
    {
        animator.animateComponent(this, this->getBounds(), 0.f, fadeoutTime, true, 0.0, 0.0);
    }
}

void FadingDialog::updatePosition()
{
#if HELIO_DESKTOP
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
#elif HELIO_MOBILE
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2.5f);
#endif
}
