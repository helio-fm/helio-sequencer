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

#pragma once

#include "CachedLabelImage.h"
#include "ColourIDs.h"

class NoteNameGuide final : public Component
{
public:

    NoteNameGuide(const String &noteName, int noteNumber) : noteNumber(noteNumber)
    {
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);

        this->noteNameLabel = make<Label>();
        this->addAndMakeVisible(this->noteNameLabel.get());
        this->noteNameLabel->setFont(Globals::UI::Fonts::S);
        this->noteNameLabel->setJustificationType(Justification::centredLeft);

        this->noteNameLabel->setCachedComponentImage(new CachedLabelImage(*this->noteNameLabel));
        this->noteNameLabel->setText(noteName, dontSendNotification);
    }
    
    inline int getNoteNumber() const noexcept
    {
        return this->noteNumber;
    }

    inline bool isRootKey(int period) const noexcept
    {
        return this->noteNumber % period == 0;
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->shadowColour);
        g.fillPath(this->internalPath1);

        g.setColour(this->fillColour);
        g.fillPath(this->internalPath2);

        g.setColour(this->borderColour);
        g.fillRect(0, 1, 3, this->getHeight() - 1);
    }

    void resized() override
    {
        this->noteNameLabel->setBounds(1, (this->getHeight() / 2) - 10, 45, 21);

        this->internalPath1.clear();
        this->internalPath1.startNewSubPath (3.f, 1.f);
        this->internalPath1.lineTo(30.f, 1.f);
        this->internalPath1.lineTo(34.f, static_cast<float> (this->getHeight()));
        this->internalPath1.lineTo(3.f, static_cast<float> (this->getHeight()));
        this->internalPath1.closeSubPath();

        this->internalPath2.clear();
        this->internalPath2.startNewSubPath(0.f, 1.f);
        this->internalPath2.lineTo(29.f, 1.f);
        this->internalPath2.lineTo(33.f, static_cast<float>(this->getHeight()));
        this->internalPath2.lineTo(0.f, static_cast<float>(this->getHeight()));
        this->internalPath2.closeSubPath();
    }

private:

    const int noteNumber;

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::noteNameFill);
    const Colour borderColour = findDefaultColour(ColourIDs::Roll::noteNameBorder);
    const Colour shadowColour = findDefaultColour(ColourIDs::Roll::noteNameShadow);

    UniquePointer<Label> noteNameLabel;
    Path internalPath1;
    Path internalPath2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteNameGuide)
};
