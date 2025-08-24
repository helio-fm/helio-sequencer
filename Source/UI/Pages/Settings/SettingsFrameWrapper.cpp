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
#include "SettingsFrameWrapper.h"

SettingsFrameWrapper::SettingsFrameWrapper(Component *targetComponent, const String &title)
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setFocusContainerType(Component::FocusContainerType::none);

    jassert(targetComponent != nullptr);

    this->panel = make<FramePanel>();
    this->addAndMakeVisible(this->panel.get());

    this->titleLabel = make<Label>();
    this->titleLabel->setFont(Globals::UI::Fonts::L);
    this->titleLabel->setJustificationType(Justification::centredLeft);
    this->addAndMakeVisible(this->titleLabel.get());

    this->setEnabled(targetComponent->isEnabled() && targetComponent->getHeight() > 0);
    this->showNonOwned(targetComponent, title);
}

SettingsFrameWrapper::~SettingsFrameWrapper() = default;

void SettingsFrameWrapper::resized()
{
    this->titleLabel->setBounds(titleMargin, titleMargin,
        this->getWidth() - titleMargin * 2, titleHeight);

    const auto panelY = this->titleLabel->isVisible() ?
        titleHeight + titleMargin * 2 : panelMargin;

    this->panel->setBounds(panelMargin, panelY,
        this->getWidth() - panelMargin * 2,
        this->getHeight() - panelY - panelMargin);

    if (this->target != nullptr)
    {
        this->target->setBounds(this->panel->getBounds().
            reduced(contentMarginX, contentMarginY));
    }
}

void SettingsFrameWrapper::showNonOwned(Component *targetComponent, const String &title)
{
    if (this->target != nullptr)
    {
        this->removeChildComponent(this->target);
    }

    this->target = targetComponent;
    this->addAndMakeVisible(this->target);
    this->titleLabel->setText(title, dontSendNotification);
    this->titleLabel->setVisible(title.isNotEmpty());

    const auto totalMargins = this->titleLabel->isVisible() ?
        labeledTotalMargins : simpleTotalMargins;

    this->setSize(this->getWidth(),
        this->target->getHeight() + totalMargins);
}
