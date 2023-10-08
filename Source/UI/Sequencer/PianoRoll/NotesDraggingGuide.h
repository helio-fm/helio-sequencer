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

#include "ColourIDs.h"
#include "Lasso.h"
#include "NoteComponent.h"

class NotesDraggingGuide final : public Component
{
public:

    NotesDraggingGuide()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->shadowColour);
        g.drawHorizontalLine(1, 0.f, float(this->getWidth()));
        g.drawHorizontalLine(this->getHeight() - 2, 0.f, float(this->getWidth()));
        //HelioTheme::drawDashedHorizontalLine(g, 0.f, 0.f, float(this->getWidth()), 6.f);
        //HelioTheme::drawDashedHorizontalLine(g, 0.f, float(this->getHeight() - 1), float(this->getWidth()), 6.f);

        g.setColour(this->dashColour);
        g.drawHorizontalLine(0, 0.f, float(this->getWidth()));
        g.drawHorizontalLine(this->getHeight() - 1, 0.f, float(this->getWidth()));
    }

    void resetAnchor(const Lasso &lasso)
    {
        if (lasso.getNumSelected() == 0)
        {
            jassertfalse;
            return;
        }

        this->draggingDelta = 0;
        this->lowestKey = std::numeric_limits<Note::Key>::max();
        this->highestKey = std::numeric_limits<Note::Key>::lowest();

        for (int i = 0; i < lasso.getNumSelected(); ++i)
        {
            if (const auto *noteComponent = dynamic_cast<NoteComponent *>(lasso.getSelectedItem(i)))
            {
                const auto key = noteComponent->getKey() + noteComponent->getClip().getKey();
                this->lowestKey = jmin(lowestKey, key);
                this->highestKey = jmax(highestKey, key);
            }
        }
    }

    void setKeyDelta(Note::Key keyDelta) noexcept
    {
        this->draggingDelta = keyDelta;
    }

    Range<Note::Key> getKeyRange() const noexcept
    {
        return { this->lowestKey + this->draggingDelta,
                 this->highestKey + this->draggingDelta };
    }

private:

    const Colour dashColour = findDefaultColour(ColourIDs::Roll::draggingGuide);
    const Colour shadowColour = findDefaultColour(ColourIDs::Roll::draggingGuideShadow);

    Note::Key lowestKey;
    Note::Key highestKey;
    Note::Key draggingDelta = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotesDraggingGuide)
};
