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

Array<Colour> ColourIDs::getColoursList()
{
    static Array<Colour> list;

    if (list.isEmpty())
    {
        constexpr auto numColours = 20;
        constexpr auto hueStep = 1.f / (numColours + 1);
        for (float hue = 0.f; hue < (1.f - hueStep); hue += hueStep)
        {
            list.add(Colours::red.withHue(hue));
        }
    }

    return list;
}
