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
#include "KeySignaturesSequence.h"
#include "KeySignatureSmallComponent.h"

KeySignatureSmallComponent::KeySignatureSmallComponent(KeySignaturesProjectMap &parent,
    const KeySignatureEvent &targetEvent) :
    KeySignatureComponent(parent, targetEvent)
{
    this->signatureLabel = make<Label>();
    this->addAndMakeVisible(this->signatureLabel.get());
    this->signatureLabel->setFont({ 14.f });
    this->signatureLabel->setJustificationType(Justification::centredLeft);
    this->signatureLabel->setBounds(0, 2, 132, 16);

    this->setInterceptsMouseClicks(false, false);
    this->signatureLabel->setInterceptsMouseClicks(false, false);
}

KeySignatureSmallComponent::~KeySignatureSmallComponent() {}

void KeySignatureSmallComponent::parentHierarchyChanged()
{
    this->setSize(this->getWidth(), this->getParentHeight());
}

void KeySignatureSmallComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset =
        Rectangle<float>(bounds.getX() - float(intBounds.getX()),
            bounds.getY(),
            bounds.getWidth() - float(intBounds.getWidth()),
            bounds.getHeight());

    this->setBounds(intBounds);
}

float KeySignatureSmallComponent::getTextWidth() const
{
    return this->textWidth;
}

void KeySignatureSmallComponent::updateContent(const StringArray &keyNames)
{
    const String originalName = this->event.toString(keyNames);
    if (this->eventName != originalName)
    {
        this->eventName = originalName;
        this->textWidth = float(this->signatureLabel->getFont().getStringWidth(originalName));
        this->signatureLabel->setText(originalName, dontSendNotification);
        this->repaint();
    }
}
