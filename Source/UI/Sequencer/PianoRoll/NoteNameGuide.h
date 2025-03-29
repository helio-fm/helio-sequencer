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

    explicit NoteNameGuide(int noteNumber) : noteNumber(noteNumber)
    {
        this->setPaintingIsUnclipped(true);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);

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

    int setNoteName(const String &name, int periodNumber)
    {
        this->noteName->setNoteName(name, String(periodNumber));
        return this->noteName->getRequiredWidth();
    }

    void setWidth(int newWidth)
    {
        if (this->nameWidth != newWidth)
        {
            this->nameWidth = newWidth;
            this->resized();
        }
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->shadowColour);
        g.fillPath(this->internalPath1);

        g.setColour(this->fillColour);
        g.fillPath(this->internalPath2);

        g.setColour(this->borderColour);
        g.fillRect(0.f, 0.5f, NoteNameGuidesBar::borderWidth, float(this->getHeight()) - 0.5f);
    }

    void resized() override
    {
        // even if the height is too small, the name shouldn't be cut
        constexpr auto minHeight = int(Globals::UI::Fonts::M);
        const auto nameHeight = jmax(this->getHeight(), minHeight);
        this->noteName->setBounds(int(NoteNameGuidesBar::borderWidth + NoteNameGuidesBar::nameMargin),
            (this->getHeight() - nameHeight) / 2, this->nameWidth, nameHeight);

        const auto w = float(this->getWidth());
        const auto h = float(this->getHeight());

        this->internalPath1.clear();
        this->internalPath1.startNewSubPath(NoteNameGuidesBar::borderWidth + 1, 1.f);
        this->internalPath1.lineTo(w - NoteNameGuidesBar::arrowWidth, 1.f);
        this->internalPath1.lineTo(w, (h / 2.f) + 0.5f);
        this->internalPath1.lineTo(w - NoteNameGuidesBar::arrowWidth, h);
        this->internalPath1.lineTo(NoteNameGuidesBar::borderWidth + 1.f, h);
        this->internalPath1.closeSubPath();

        this->internalPath2.clear();
        this->internalPath2.startNewSubPath(0.f, 1.f);
        this->internalPath2.lineTo(w - NoteNameGuidesBar::arrowWidth - 1.f, 1.f);
        this->internalPath2.lineTo(w - 1.f, (h / 2.f) + 0.5f);
        this->internalPath2.lineTo(w - NoteNameGuidesBar::arrowWidth - 1.f, h);
        this->internalPath2.lineTo(0.f, h);
        this->internalPath2.closeSubPath();
    }

private:

    const int noteNumber;

    int nameWidth = 32;

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::noteNameFill);
    const Colour borderColour = findDefaultColour(ColourIDs::Roll::noteNameBorder);
    const Colour shadowColour = findDefaultColour(ColourIDs::Roll::noteNameShadow);

    UniquePointer<NoteNameComponent> noteName;

    Path internalPath1;
    Path internalPath2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteNameGuide)
};
