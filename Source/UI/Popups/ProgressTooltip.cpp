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
#include "ProgressTooltip.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"

ProgressTooltip::ProgressTooltip(bool cancellable) : isCancellable(cancellable)
{
    this->progressIndicator = make<ProgressIndicator>();
    this->addAndMakeVisible(this->progressIndicator.get());

    this->setComponentID(ComponentIDs::progressTooltipId);
    this->progressIndicator->startAnimating();

    this->setSize(ProgressTooltip::tooltipSize, ProgressTooltip::tooltipSize);
}

void ProgressTooltip::paint(Graphics& g)
{
    g.setColour(Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(this->getLocalBounds().toFloat(), 15.000f);
}

void ProgressTooltip::resized()
{
    Rectangle<int> imageBounds(0, 0, ProgressTooltip::imageSize, ProgressTooltip::imageSize);
    this->progressIndicator->setBounds(imageBounds.withCentre(this->getLocalBounds().getCentre()));
}

void ProgressTooltip::parentHierarchyChanged()
{
    this->setCentrePosition(this->getParentWidth() / 2, this->getParentHeight() / 2);
}

void ProgressTooltip::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::DismissModalComponentAsync)
    {
        this->cancel();
    }
}

bool ProgressTooltip::keyPressed(const KeyPress& key)
{
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancel();
        return true;
    }

    return false;
}

void ProgressTooltip::inputAttemptWhenModal()
{
    this->postCommandMessage(CommandIDs::DismissModalComponentAsync);
}

void ProgressTooltip::cancel()
{
    if (!this->isCancellable)
    {
        return;
    }

    if (this->onCancel != nullptr)
    {
        this->onCancel();
    }

    this->dismiss();
}
