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

#pragma once

class FineTuningValueIndicator final : public Component
{
public:

    FineTuningValueIndicator(float initialValue, const String &suffix) :
        value(initialValue),
        suffix(suffix)
    {
        this->valueLabel = make<Label>();
        this->addAndMakeVisible(this->valueLabel.get());
        this->valueLabel->setFont(Globals::UI::Fonts::XS);
        this->valueLabel->setJustificationType(Justification::centred);

        this->setSize(64, 64);
    }
 
    ~FineTuningValueIndicator() = default;

    void setValue(float newValue)
    {
        this->setValue(newValue, newValue);
    }

    void setValue(float newValue, float valueView)
    {
        if (this->value == newValue)
        {
            return;
        }

        static constexpr auto numDecimalPlaces = 3;
        this->setValue(newValue, String(valueView, numDecimalPlaces));
    }

    void setValue(float newValue, const String &valueView)
    {
        if (this->value == newValue)
        {
            return;
        }

        this->value = newValue;
        this->valueLabel->setText(valueView + this->suffix, dontSendNotification);

        this->repaint();
    }

    void repositionAtTargetTop(Component *component)
    {
        const auto topRelativeToMyParent = this->getParentComponent()->
            getLocalPoint(component, Point<int>(component->getWidth() / 2, 0));

        this->setTopLeftPosition(topRelativeToMyParent -
            Point<int>(this->getWidth() / 2, this->getHeight() - 8));

        this->valueLabel->setBounds(0,
            this->getHeight() / 2 - FineTuningValueIndicator::labelHeight / 2,
            this->getWidth(), FineTuningValueIndicator::labelHeight);
    }

    void repositionAtTargetCenter(Component *component)
    {
        const auto centreRelativeToMyParent = this->getParentComponent()->
            getLocalPoint(component, component->getLocalBounds().getCentre());

        this->setTopLeftPosition(centreRelativeToMyParent - this->getLocalBounds().getCentre());

        this->valueLabel->setBounds(0,
            this->getHeight() - FineTuningValueIndicator::labelHeight - 2,
            this->getWidth(), FineTuningValueIndicator::labelHeight);
    }

    void setShouldDisplayValue(bool shouldDisplay)
    {
        this->valueLabel->setVisible(shouldDisplay);
    }

    void paint(Graphics &g) override
    {
        constexpr auto startAngleRadians = MathConstants<float>::pi * 1.5f;
        constexpr auto endAngleRadians = MathConstants<float>::pi * 2.5f;

        LookAndFeel::getDefaultLookAndFeel().drawRotarySlider(g, 0, 0,
            this->getWidth(), this->getHeight(), this->value,
            startAngleRadians, endAngleRadians, this->dummySlider);
    }

private:

    float value = 0.f;
    const String suffix;
    Slider dummySlider;

    UniquePointer<Label> valueLabel;

    static constexpr auto labelHeight = 24;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FineTuningValueIndicator)
};
