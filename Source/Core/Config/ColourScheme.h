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

#include "Serializable.h"

// A simple wrapper around colour map hashmap
// Used for serialization and JUCE_LIVE_CONSTANTs macros
class ColourScheme : public Serializable
{
public:

    ColourScheme() {}
    explicit ColourScheme(const String &scheme);
    ColourScheme(const ColourScheme &other);
    ~ColourScheme() override {}

    typedef HashMap<String, Colour> ColourMap;

    void randomize();

    ColourScheme &operator=(const ColourScheme &other);
    friend bool operator==(const ColourScheme &lhs, const ColourScheme &rhs);

    String getId() const;
    String getName() const;

    // Primary background gradient
    Colour getPrimaryGradientColourA() const;
    Colour getPrimaryGradientColourB() const;

    // Secondary background gradient
    Colour getSecondaryGradientColourA() const;
    Colour getSecondaryGradientColourB() const;

    // Panel and button fill
    Colour getPanelFillColour() const;
    Colour getPanelBorderColour() const;

    // Lasso and indicator
    Colour getLassoFillColour() const;
    Colour getLassoBorderColour() const;

    // Piano roll stuff
    Colour getBlackKeyColour() const;
    Colour getWhiteKeyColour() const;
    Colour getRowColour() const;
    Colour getBarColour() const;

    // Labels and buttons text
    Colour getTextColour() const;

    // Used in Icons.cpp
    Colour getIconBaseColour() const;
    Colour getIconShadowColour() const;

    //===------------------------------------------------------------------===//
    // Serialization
    //===------------------------------------------------------------------===//

    void syncWithLiveConstantEditor();

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

private:

    ColourMap colours;

    String name;

    String id;

    JUCE_LEAK_DETECTOR(ColourScheme);

};
