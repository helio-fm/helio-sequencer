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

#include "Clip.h"
#include "Note.h"

class ProjectMapNoteComponent final : public Component
{
public:

    ProjectMapNoteComponent(const Note &note, const Clip &clip, const Colour &baseColour) :
        note(note),
        clip(clip),
        dx(0.f),
        dw(0.f)
    {
        this->updateColour(baseColour);
        this->setInterceptsMouseClicks(false, false);
        this->setMouseClickGrabsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    inline int getKey() const noexcept           { return jlimit(0, 128, this->note.getKey() + this->clip.getKey()); }
    inline float getBeat() const noexcept        { return this->note.getBeat() + this->clip.getBeat(); }
    inline float getLength() const noexcept      { return this->note.getLength(); }
    inline float getVelocity() const noexcept    { return this->note.getVelocity() * this->clip.getVelocity(); }
    inline const Note &getNote() const noexcept  { return this->note; }

    inline void updateColour(const Colour &base)
    {
        this->colour = this->note.getTrackColour().
            interpolatedWith(base, .4f).
            withAlpha(.6f);
    }

    void setRealBounds(float x, int y, float w, int h)
    {
        this->dx = x - floorf(x);
        this->dw = ceilf(w) - w;
        this->setBounds(int(floorf(x)), y, int(ceilf(w)), h);
    }
    
    void paint(Graphics &g) override
    {
        g.setColour(this->colour);
        g.drawHorizontalLine(0, this->dx, float(this->getWidth()) - this->dw);
    }

private:

    const Note &note;
    const Clip &clip;
    
    Colour colour;

    float dx;
    float dw;

};
