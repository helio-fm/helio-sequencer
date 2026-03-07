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

#include "NoteNameComponent.h"
#include "NoteNameGuidesBar.h"
#include "ColourIDs.h"

class NoteNameGuide final : public Component
{
public:

    NoteNameGuide(NoteNameGuidesBar &parentGuidesBar, int noteNumber) :
        guidesBar(parentGuidesBar),
        noteNumber(noteNumber)
    {
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);
        this->setAccessible(false);

        this->noteNameLabel = make<NoteNameComponent>();
        this->addAndMakeVisible(this->noteNameLabel.get());

        this->detailsLabel = make<Label>();
        this->addChildComponent(this->detailsLabel.get());
        this->detailsLabel->setAccessible(false);
        this->detailsLabel->setFont(this->noteNameLabel->getFont());
        this->detailsLabel->setBorderSize({ 0, 1, 0, 1 });
        this->detailsLabel->setJustificationType(Justification::centredRight);
        this->detailsLabel->setColour(Label::textColourId,
            findDefaultColour(ColourIDs::Roll::noteNameNumber));
    }
    
    inline int getNoteNumber() const noexcept
    {
        return this->noteNumber;
    }

    inline bool isRootKey(int scaleRootKey, int period) const noexcept
    {
        return (this->noteNumber - scaleRootKey) % period == 0;
    }

    int setNoteName(const String &name, int periodNumber,
        bool useFixedDo, const String &detailsText)
    {
        this->noteNameLabel->setNoteName(name, String(periodNumber), useFixedDo);
        const auto noteNameWidth = this->noteNameLabel->getRequiredWidth();

        if (detailsText.isNotEmpty())
        {
            if (this->detailsLabel->getText() != detailsText)
            {
                this->detailsLabel->setVisible(true);
                this->detailsLabel->setText(detailsText, dontSendNotification);
                this->detailsWidth = this->detailsLabel->getFont().
                    getStringWidthFloat(this->detailsLabel->getText()) +
                        this->detailsLabel->getBorderSize().getLeftAndRight();
            }
        }
        else if (this->detailsLabel->isVisible())
        {
            this->detailsLabel->setVisible(false);
            this->detailsLabel->setText({}, dontSendNotification);
            this->detailsWidth = 0.f;
        }

        const auto result = noteNameWidth + this->detailsWidth;
        return int(ceilf(float(result) / 2.f) * 2.f);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->shadowColour);
        g.fillPath(this->guidesBar.getNoteShapeOutlinePath());

        g.setColour(this->fillColour);
        g.fillPath(this->guidesBar.getNoteShapeFillPath());

        g.setColour(this->borderColour);
        g.fillRect(0.f, 0.5f, NoteNameGuidesBar::borderWidth, float(this->getHeight()) - 0.5f);
    }

    void resized() override
    {
        // even if the height is too small, the name shouldn't be cut
        constexpr auto nameHeight = int(Globals::UI::Fonts::M);
        const auto x = int(NoteNameGuidesBar::borderWidth + NoteNameGuidesBar::nameMarginLeft);
        const auto y = roundToIntAccurate(float(this->getHeight() - nameHeight) / 2.f);
        this->noteNameLabel->setBounds(x, y, this->getWidth(), nameHeight);
        this->detailsLabel->setBounds(x, y,
            this->getWidth() - x - int(NoteNameGuidesBar::nameMarginRight), nameHeight);
    }

private:

    NoteNameGuidesBar &guidesBar;

    const int noteNumber;

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::noteNameFill);
    const Colour borderColour = findDefaultColour(ColourIDs::Roll::noteNameBorder);
    const Colour shadowColour = findDefaultColour(ColourIDs::Roll::noteNameShadow);

    UniquePointer<NoteNameComponent> noteNameLabel;
    UniquePointer<Label> detailsLabel;
    float detailsWidth = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteNameGuide)
};
