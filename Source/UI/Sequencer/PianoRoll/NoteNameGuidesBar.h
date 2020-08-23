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

class PianoRoll;
class NoteNameGuide;
class Lasso;

#include "Temperament.h"

class NoteNameGuidesBar final : public Component, private ChangeListener
{
public:

    explicit NoteNameGuidesBar(PianoRoll &roll);
    ~NoteNameGuidesBar();
    
    void updatePosition();
    void updateBounds();

    void syncWithSelection(const Lasso *selection);
    void syncWithTemperament(Temperament::Ptr temperament);

private:

    PianoRoll &roll;
    OwnedArray<NoteNameGuide> guides;

    static constexpr auto defaultWidth = 36;
    static constexpr auto extendedWidth = 48;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoteNameGuidesBar)
};
