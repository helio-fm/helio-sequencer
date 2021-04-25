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
#include "FineTuningValueIndicator.h"

FineTuningValueIndicator::FineTuningValueIndicator(float initialValue, String suffix) :
    value(initialValue),
    suffix(suffix)
{
    this->valueLabel = make<Label>();
    this->addAndMakeVisible(this->valueLabel.get());
    this->valueLabel->setFont({ Globals::UI::Fonts::XS });
    this->valueLabel->setJustificationType(Justification::centred);

    this->setSize(64, 64);
}

FineTuningValueIndicator::~FineTuningValueIndicator() = default;

void FineTuningValueIndicator::paint(Graphics &g)
{
    constexpr auto startAngleRadians = MathConstants<float>::pi * 1.5f;
    constexpr auto endAngleRadians = MathConstants<float>::pi * 2.5f;

    LookAndFeel::getDefaultLookAndFeel().drawRotarySlider(g, 0, 0,
        this->getWidth(), this->getHeight(), this->value,
        startAngleRadians, endAngleRadians, this->dummySlider);
}

void FineTuningValueIndicator::resized()
{
    static constexpr auto labelHeight = 24;
    this->valueLabel->setBounds(0, this->getHeight() - labelHeight - 2, this->getWidth(), labelHeight);
}

void FineTuningValueIndicator::setValue(float newValue)
{
    this->setValue(newValue, newValue);
}

void FineTuningValueIndicator::setValue(float newValue, float valueView)
{
    if (this->value == newValue)
    {
        return;
    }

    this->value = newValue;

    static constexpr auto numDecimalPlaces = 3;
    this->valueLabel->setText(String(valueView, numDecimalPlaces) + this->suffix, dontSendNotification);

    this->repaint();
}

void FineTuningValueIndicator::setValue(float newValue, int valueView)
{
    if (this->value == newValue)
    {
        return;
    }

    this->value = newValue;
    this->valueLabel->setText(String(valueView) + this->suffix, dontSendNotification);

    this->repaint();
}

void FineTuningValueIndicator::repositionToTargetAt(Component *component, Point<int> offset)
{
    this->setTopLeftPosition(offset + component->getBounds().getCentre() - this->getLocalBounds().getCentre());
}

void FineTuningValueIndicator::setDisplayValue(bool shouldDisplay)
{
    this->valueLabel->setVisible(shouldDisplay);
}
