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
#include "AnnotationsSequence.h"
#include "AnnotationsProjectMap.h"
#include "CachedLabelImage.h"
#include "AnnotationSmallComponent.h"

AnnotationSmallComponent::AnnotationSmallComponent(AnnotationsProjectMap &parent,
    const AnnotationEvent &targetEvent) :
    AnnotationComponent(parent, targetEvent)
{
    this->annotationLabel = make<Label>();
    this->addAndMakeVisible(this->annotationLabel.get());
    this->annotationLabel->setFont(Globals::UI::Fonts::XS);
    this->annotationLabel->setJustificationType(Justification::centredLeft);

    this->setInterceptsMouseClicks(false, false);
    this->annotationLabel->setInterceptsMouseClicks(false, false);

    this->annotationLabel->setSize(160, 16);
    this->annotationLabel->setCachedComponentImage(new CachedLabelImage(*this->annotationLabel));
}

AnnotationSmallComponent::~AnnotationSmallComponent() = default;

void AnnotationSmallComponent::paint(Graphics &g)
{
    g.setColour(this->event.getColour().interpolatedWith(this->baseColour, 0.55f).withAlpha(0.2f));
    g.fillRect(0, this->getHeight() - 2, this->getWidth() - 4, 2);
}

void AnnotationSmallComponent::resized()
{
    this->annotationLabel->setTopLeftPosition(-2, this->getHeight() - 18);
}

void AnnotationSmallComponent::parentHierarchyChanged()
{
    this->setSize(this->getWidth(), this->getParentHeight());
}

void AnnotationSmallComponent::setRealBounds(const Rectangle<float> bounds)
{
    Rectangle<int> intBounds(bounds.toType<int>());
    this->boundsOffset = {
        bounds.getX() - float(intBounds.getX()),
        bounds.getY(),
        bounds.getWidth() - float(intBounds.getWidth()),
        bounds.getHeight() };

    this->setBounds(intBounds);
}

void AnnotationSmallComponent::updateContent()
{
    if (this->lastColour != this->event.getColour())
    {
        this->lastColour = this->event.getColour();
        const auto fgColour = findDefaultColour(Label::textColourId);
        this->annotationLabel->setColour(Label::textColourId, this->lastColour.interpolatedWith(fgColour, 0.55f));
        auto *cachedImage = static_cast<CachedLabelImage<Label> *>(this->annotationLabel->getCachedComponentImage());
        jassert(cachedImage != nullptr);
        cachedImage->forceInvalidate();
    }

    if (this->annotationLabel->getText() != this->event.getDescription())
    {
        this->annotationLabel->setText(this->event.getDescription(), dontSendNotification);
        this->textWidth = float(this->annotationLabel->getFont().getStringWidth(this->event.getDescription()));
    }

    this->repaint();
}

float AnnotationSmallComponent::getTextWidth() const
{
    return this->textWidth;
}
