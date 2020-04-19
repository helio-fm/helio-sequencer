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
StringPairArray ColourIDs::getColoursList()
{
    StringPairArray c;
    //c.set(TRANS(I18n::Colours::none),           Colours::transparentWhite.toString());
    //c.set(TRANS(I18n::Colours::black),          Colours::black.toString());
    c.set(TRANS(I18n::Colours::white),            Colours::white.toString());
    c.set(TRANS(I18n::Colours::red),              Colours::red.toString());
    c.set(TRANS(I18n::Colours::crimson),          Colours::crimson.toString());
    c.set(TRANS(I18n::Colours::deepPink),         Colours::deeppink.toString());
    c.set(TRANS(I18n::Colours::darkViolet),       Colours::darkviolet.toString());
    c.set(TRANS(I18n::Colours::blueViolet),       Colours::blueviolet.toString());
    c.set(TRANS(I18n::Colours::blue),             Colours::blue.toString());
    c.set(TRANS(I18n::Colours::royalBlue),        Colours::royalblue.toString());
    c.set(TRANS(I18n::Colours::springGreen),      Colours::springgreen.toString());
    c.set(TRANS(I18n::Colours::lime),             Colours::lime.toString());
    c.set(TRANS(I18n::Colours::greenYellow),      Colours::greenyellow.toString());
    c.set(TRANS(I18n::Colours::gold),             Colours::gold.toString());
    c.set(TRANS(I18n::Colours::darkOrange),       Colours::darkorange.toString());
    c.set(TRANS(I18n::Colours::tomato),           Colours::tomato.toString());
    c.set(TRANS(I18n::Colours::orangeRed),        Colours::orangered.toString());
    return c;
}
