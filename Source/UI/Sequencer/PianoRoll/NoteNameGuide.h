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

//[Headers]
//[/Headers]


class NoteNameGuide final : public Component
{
public:

    NoteNameGuide(const String &noteName, int noteNumber);
    ~NoteNameGuide();

    //[UserMethods]
    inline int getNoteNumber() const noexcept
    {
        return this->noteNumber;
    }

    inline bool isRootKey(int period) const noexcept
    {
        return this->noteNumber % period == 0;
    }
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]
    const int noteNumber;

    const Colour fillColour;
    const Colour borderColour;
    const Colour shadowColour;
    //[/UserVariables]

    UniquePointer<Label> noteNameLabel;
    Path internalPath1;
    Path internalPath2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteNameGuide)
};


