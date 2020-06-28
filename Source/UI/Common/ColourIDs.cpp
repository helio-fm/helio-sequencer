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

#include "Common.h"
#include "ColourIDs.h"
#include "TranslationKeys.h"

// Hardcoded for now
Array<Colour> ColourIDs::getColoursList()
{
    static Array<Colour> c;

    if (c.isEmpty())
    {
        //c.add(Colours::transparentWhite);
        //c.add(Colours::black);
        c.add(Colours::white);
        c.add(Colours::red);
        c.add(Colours::crimson);
        c.add(Colours::deeppink);
        c.add(Colours::darkviolet);
        c.add(Colours::blueviolet);
        c.add(Colours::blue);
        c.add(Colours::royalblue);
        c.add(Colours::springgreen);
        c.add(Colours::lime);
        c.add(Colours::greenyellow);
        c.add(Colours::gold);
        c.add(Colours::darkorange);
        c.add(Colours::tomato);
        c.add(Colours::orangered);
    }

    return c;
}
