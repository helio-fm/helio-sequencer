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
#include "NoteNameComponent.h"
#include "IconComponent.h"

NoteNameComponent::NoteNameComponent(bool isCentered, float fontSize) :
    isCentered(isCentered),
    iconSize(int(fontSize) - 5)
{
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, false);

    this->nameLabel = make<Label>();
    this->addAndMakeVisible(this->nameLabel.get());
    this->nameLabel->setFont(fontSize);
    this->nameLabel->setBorderSize({});
    this->nameLabel->setJustificationType(Justification::centred);

    this->detailsLabel = make<Label>();
    this->addAndMakeVisible(this->detailsLabel.get());
    this->detailsLabel->setFont(fontSize);
    this->detailsLabel->setBorderSize({});
    this->detailsLabel->setJustificationType(Justification::centred);
    this->detailsLabel->setColour(Label::textColourId,
        findDefaultColour(Label::textColourId).withMultipliedAlpha(0.9f));
}

NoteNameComponent::~NoteNameComponent() = default;

void NoteNameComponent::resized()
{
    auto localBounds = this->getLocalBounds();

    if (this->isCentered)
    {
        const auto contentWidth = this->getRequiredWidthFloat();
        localBounds.removeFromLeft(roundToIntAccurate((float(this->getWidth()) - contentWidth) / 2.f));
    }

    if (this->prefix != nullptr)
    {
        localBounds.removeFromLeft(int(ceilf(this->prefixBounds.getWidth())));
        this->prefix->setBounds(localBounds.
            withWidth(this->iconSize).
            translated(-this->iconSize - NoteNameComponent::iconMargin, 0));
    }

    this->nameLabel->setBounds(localBounds.removeFromLeft(int(ceilf(this->textWidth))));

    if (this->suffix != nullptr)
    {
        this->suffix->setBounds(localBounds.
            withWidth(this->iconSize).
            translated(NoteNameComponent::iconMargin, 0));
        localBounds.removeFromLeft(NoteNameComponent::iconMargin +
            int(ceilf(this->suffixBounds.getWidth())));
    }

    this->detailsLabel->setBounds(localBounds.
        withWidth(int(ceilf(this->detailsWidth))).
        translated(NoteNameComponent::detailsMargin, 0));
}

int NoteNameComponent::getRequiredWidth() const noexcept
{
    return int(ceilf(this->getRequiredWidthFloat()));
}

float NoteNameComponent::getRequiredWidthFloat() const noexcept
{
    float result = this->textWidth;

    if (this->prefix != nullptr)
    {
        result += this->prefixBounds.getWidth() + NoteNameComponent::iconMargin;
    }

    if (this->suffix != nullptr)
    {
        result += this->suffixBounds.getWidth() + NoteNameComponent::iconMargin;
    }

    if (this->detailsText.hasValue())
    {
         result += this->detailsWidth + NoteNameComponent::detailsMargin;
    }

    return result;
}

void NoteNameComponent::setNoteName(const String &newNoteName,
    Optional<String> newDetailsText, bool useFixedDoNotation)
{
    this->noteName = newNoteName;
    this->detailsText = newDetailsText;

    if (newNoteName.isEmpty())
    {
        jassertfalse;
        return;
    }

    this->prefix = nullptr;
    this->suffix = nullptr;
    this->prefixBounds = {};
    this->suffixBounds = {};
    this->nameLabel->setText({}, dontSendNotification);

    juce_wchar c;
    int numFlats = 0;
    int numSharps = 0;
    int numUps = 0;
    int numDowns = 0;
    auto ptr = newNoteName.getCharPointer();
    do
    {
        c = ptr.getAndAdvance();
        switch (c)
        {
        case '^':
            numUps++;
            break;
        case 'v':
            numDowns++;
            break;
        case 'b':
            numFlats++;
            break;
        case '#':
            numSharps++;
            break;
        case 'x':
            numSharps += 2;
            break;
        case 'A':
            this->nameLabel->setText(useFixedDoNotation ? TRANS(I18n::Solfege::la) : "A", dontSendNotification);
            break;
        case 'B':
        case 'H':
            this->nameLabel->setText(useFixedDoNotation ? TRANS(I18n::Solfege::si) : "B", dontSendNotification);
            break;
        case 'C':
            this->nameLabel->setText(useFixedDoNotation ? TRANS(I18n::Solfege::ut) : "C", dontSendNotification);
            break;
        case 'D':
            this->nameLabel->setText(useFixedDoNotation ? TRANS(I18n::Solfege::re) : "D", dontSendNotification);
            break;
        case 'E':
            this->nameLabel->setText(useFixedDoNotation ? TRANS(I18n::Solfege::mi) : "E", dontSendNotification);
            break;
        case 'F':
            this->nameLabel->setText(useFixedDoNotation ? TRANS(I18n::Solfege::fa) : "F", dontSendNotification);
            break;
        case 'G':
            this->nameLabel->setText(useFixedDoNotation ? TRANS(I18n::Solfege::sol) : "G", dontSendNotification);
            break;
        default:
            break;
        }
    } while (c != 0);

    jassert(numSharps <= 2);
    jassert(numFlats <= 2);
    jassert(numUps <= 2);
    jassert(numDowns <= 2);

    constexpr auto suffixAlignment = RectanglePlacement::xLeft | RectanglePlacement::yMid;
    constexpr auto prefixAlignment = RectanglePlacement::xRight | RectanglePlacement::yMid;

    const auto iconColour = findDefaultColour(Label::textColourId).withMultipliedAlpha(0.65f);

    if (numSharps == 1)
    {
        this->suffix = make<IconComponent>(Icons::findByName(Icons::sharp, this->iconSize,
            suffixAlignment, iconColour, Colours::transparentBlack, this->suffixBounds));
    }
    else if (numSharps > 1)
    {
        this->suffix = make<IconComponent>(Icons::findByName(Icons::doubleSharp, this->iconSize,
            suffixAlignment, iconColour, Colours::transparentBlack, this->suffixBounds));
    }

    if (numFlats == 1)
    {
        this->suffix = make<IconComponent>(Icons::findByName(Icons::flat, this->iconSize,
            suffixAlignment, iconColour, Colours::transparentBlack, this->suffixBounds));
    }
    else if (numFlats > 1)
    {
        this->suffix = make<IconComponent>(Icons::findByName(Icons::doubleFlat, this->iconSize,
            suffixAlignment, iconColour, Colours::transparentBlack, this->suffixBounds));
    }

    if (numUps == 1)
    {
        this->prefix = make<IconComponent>(Icons::findByName(Icons::microtoneUp, this->iconSize,
            prefixAlignment, iconColour, Colours::transparentBlack, this->prefixBounds));
    }
    else if (numUps > 1)
    {
        this->prefix = make<IconComponent>(Icons::findByName(Icons::microtoneUp2, this->iconSize,
            prefixAlignment, iconColour, Colours::transparentBlack, this->prefixBounds));
    }

    if (numDowns == 1)
    {
        this->prefix = make<IconComponent>(Icons::findByName(Icons::microtoneDown, this->iconSize,
            prefixAlignment, iconColour, Colours::transparentBlack, this->prefixBounds));
    }
    else if (numDowns > 1)
    {
        this->prefix = make<IconComponent>(Icons::findByName(Icons::microtoneDown2, this->iconSize,
            prefixAlignment, iconColour, Colours::transparentBlack, this->prefixBounds));
    }

    if (this->nameLabel->getText().isEmpty())
    {
        jassertfalse;
        this->nameLabel->setText(this->noteName, dontSendNotification);
    }

    this->detailsLabel->setText(this->detailsText.orFallback(String()), dontSendNotification);
    this->detailsLabel->setVisible(this->detailsLabel->getText().isNotEmpty());

    if (this->prefix != nullptr)
    {
        this->addAndMakeVisible(this->prefix.get());
    }

    if (this->suffix != nullptr)
    {
        this->addAndMakeVisible(this->suffix.get());
    }

    this->textWidth = this->nameLabel->getFont().getStringWidthFloat(this->nameLabel->getText());
    this->detailsWidth = this->detailsLabel->getFont().getStringWidthFloat(this->detailsLabel->getText());

    if (this->getParentComponent() != nullptr)
    {
        this->resized();
    }
}
