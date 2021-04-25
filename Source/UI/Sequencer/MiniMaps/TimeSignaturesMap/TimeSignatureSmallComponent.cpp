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
#include "TimeSignaturesSequence.h"
#include "TimeSignaturesProjectMap.h"
#include "CachedLabelImage.h"
#include "TimeSignatureSmallComponent.h"

TimeSignatureSmallComponent::TimeSignatureSmallComponent(TimeSignaturesProjectMap &parent,
    const TimeSignatureEvent &targetEvent) :
    TimeSignatureComponent(parent, targetEvent)
{
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->signatureLabel = make<Label>();
    this->addAndMakeVisible(this->signatureLabel.get());
    this->signatureLabel->setFont({ Globals::UI::Fonts::XS });
    this->signatureLabel->setJustificationType(Justification::centredLeft);
    this->signatureLabel->setBounds(0, 2, 48, 16);

    this->signatureLabel->setInterceptsMouseClicks(false, false);

    this->signatureLabel->setBufferedToImage(true);
    this->signatureLabel->setCachedComponentImage(new CachedLabelImage(*this->signatureLabel));
}

TimeSignatureSmallComponent::~TimeSignatureSmallComponent() = default;

void TimeSignatureSmallComponent::paint(Graphics &g)
{
    g.setColour(this->colour);

    Path p;
    constexpr auto topPadding = 2.f;
    constexpr auto triangleSize = 5.f;
    p.addTriangle(0.f, topPadding, triangleSize, topPadding, 0.f, triangleSize + topPadding);
    g.fillPath(p);
}

void TimeSignatureSmallComponent::parentHierarchyChanged()
{
    this->setSize(this->getWidth(), this->getParentHeight());
}

void TimeSignatureSmallComponent::setRealBounds(const Rectangle<float> bounds)
{
    const auto intBounds = bounds.toType<int>();
    this->boundsOffset = {
        bounds.getX() - float(intBounds.getX()),
        bounds.getY(),
        bounds.getWidth() - float(intBounds.getWidth()),
        bounds.getHeight() };

    this->setBounds(intBounds);
}

void TimeSignatureSmallComponent::updateContent()
{
    this->signatureLabel->setText(this->event.toString(), dontSendNotification);
}
