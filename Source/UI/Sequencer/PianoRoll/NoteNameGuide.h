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

class NoteNameGuide final : public Component
{
public:

    NoteNameGuide(const String &noteName, int noteNumber);
    
    inline int getNoteNumber() const noexcept
    {
        return this->noteNumber;
    }

    inline bool isRootKey(int period) const noexcept
    {
        return this->noteNumber % period == 0;
    }

    void paint(Graphics &g) override;
    void resized() override;

private:

    const int noteNumber;

    const Colour fillColour;
    const Colour borderColour;
    const Colour shadowColour;

    UniquePointer<Label> noteNameLabel;
    Path internalPath1;
    Path internalPath2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteNameGuide)
};
