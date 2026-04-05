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

#include "ConfigurationResource.h"

// A simple wrapper around colour map hashmap
// Used for serialization and JUCE_LIVE_CONSTANTs macros
class ColourScheme final : public ConfigurationResource
{
public:

    using Ptr = ReferenceCountedObjectPtr<ColourScheme>;
    using ColourMap = FlatHashMap<Identifier, Colour, IdentifierHash>;

    ColourScheme() = default;
    ColourScheme(const ColourScheme &other) = default;

    ColourScheme &operator=(const ColourScheme &other) = default;
    friend bool operator==(const ColourScheme &lhs, const ColourScheme &rhs);

    String getName() const noexcept;

    Colour getPageFillColour() const;
    Colour getHeadlineFillColour() const;
    Colour getSidebarFillColour() const;
    Colour getDialogFillColour() const;
    Colour getFrameBorderColour() const;

    Colour getLassoFillColour() const;
    Colour getLassoBorderColour() const;

    Colour getTimelineColour() const;
    Colour getBlackKeyColour() const;
    Colour getWhiteKeyColour() const;
    Colour getRowColour() const;
    Colour getBarColour() const;

    Colour getTextColour() const;

    Colour getIconBaseColour() const;
    Colour getIconShadowColour() const;

    Colour getScriptBackgroundColour() const;
    Colour getScriptTextColour() const;
    Colour getScriptKeywordColour() const;
    Colour getScriptFunctionColour() const;
    Colour getScriptLiteralColour() const;
    Colour getScriptBracketColour() const;
    Colour getScriptErrorColour() const;
    Colour getScriptCommentColour() const;

    void syncWithLiveConstantEditor();

    bool isEquivalentTo(const ColourScheme::Ptr other) const;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const noexcept override;
    void deserialize(const SerializedData &data) noexcept override;
    void reset() noexcept override;

    //===------------------------------------------------------------------===//
    // ConfigurationResource
    //===------------------------------------------------------------------===//

    String getResourceId() const noexcept override;

private:

    ColourMap colours;
    String name;

    JUCE_LEAK_DETECTOR(ColourScheme)
};
