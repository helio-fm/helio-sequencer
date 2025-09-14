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

#include "CachedLabelImage.h"
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

        this->noteName = make<NoteNameComponent>();
        this->addAndMakeVisible(this->noteName.get());
    }
    
    inline int getNoteNumber() const noexcept
    {
        return this->noteNumber;
    }

    inline bool isRootKey(int scaleRootKey, int period) const noexcept
    {
        return (this->noteNumber - scaleRootKey) % period == 0;
    }

    int setNoteName(const String &name, int periodNumber, bool useFixedDo)
    {
        this->noteName->setNoteName(name, String(periodNumber), useFixedDo);
        return this->noteName->getRequiredWidth();
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
        constexpr auto minHeight = int(Globals::UI::Fonts::M);
        const auto nameHeight = jmax(this->getHeight(), minHeight);
        this->noteName->setBounds(int(NoteNameGuidesBar::borderWidth + NoteNameGuidesBar::nameMarginLeft),
            roundToIntAccurate(float(this->getHeight() - nameHeight) / 2.f),
            this->getWidth(),
            nameHeight);
    }

private:

    NoteNameGuidesBar &guidesBar;

    const int noteNumber;

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::noteNameFill);
    const Colour borderColour = findDefaultColour(ColourIDs::Roll::noteNameBorder);
    const Colour shadowColour = findDefaultColour(ColourIDs::Roll::noteNameShadow);

    UniquePointer<NoteNameComponent> noteName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteNameGuide)
};
