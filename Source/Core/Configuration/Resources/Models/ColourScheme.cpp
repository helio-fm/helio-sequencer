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

#include "Common.h"
#include "ColourScheme.h"
#include "SerializationKeys.h"

bool operator==(const ColourScheme &lhs, const ColourScheme &rhs)
{
    for (const auto &i : lhs.colours)
    {
        if (!rhs.colours.contains(i.first) ||
            rhs.colours.at(i.first) != i.second)
        {
            return false;
        }
    }

    return lhs.name == rhs.name;
}

String ColourScheme::getName() const noexcept
{
    return this->name;
}

Colour ColourScheme::getPageFillColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::pageFill);
}

Colour ColourScheme::getTimelineColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::timeline);
}

Colour ColourScheme::getHeadlineFillColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::headlineFill);
}

Colour ColourScheme::getSidebarFillColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::sidebarFill);
}

Colour ColourScheme::getDialogFillColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::dialogFill);
}

Colour ColourScheme::getFrameBorderColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::frameBorder);
}

Colour ColourScheme::getLassoFillColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::lassoFill);
}

Colour ColourScheme::getLassoBorderColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::lassoBorder);
}

Colour ColourScheme::getBlackKeyColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::blackKey);
}

Colour ColourScheme::getWhiteKeyColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::whiteKey);
}

Colour ColourScheme::getRowColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::row);
}

Colour ColourScheme::getBarColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::bar);
}

Colour ColourScheme::getTextColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::text);
}

Colour ColourScheme::getIconBaseColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::iconBase);
}

Colour ColourScheme::getIconShadowColour() const
{
    using namespace Serialization;
    return this->colours.at(UI::Colours::iconShadow);
}

Colour ColourScheme::getScriptBackgroundColour() const
{
    using namespace Serialization;
    return this->colours.contains(UI::Colours::scriptBackground) ?
        this->colours.at(UI::Colours::scriptBackground) :
        this->getPageFillColour();
}

Colour ColourScheme::getScriptTextColour() const
{
    using namespace Serialization;
    return this->colours.contains(UI::Colours::scriptText) ?
        this->colours.at(UI::Colours::scriptText) :
        this->getTextColour();
}

Colour ColourScheme::getScriptKeywordColour() const
{
    using namespace Serialization;
    return this->colours.contains(UI::Colours::scriptKeyword) ?
        this->colours.at(UI::Colours::scriptKeyword) :
        this->getScriptTextColour();
}

Colour ColourScheme::getScriptFunctionColour() const
{
    using namespace Serialization;
    return this->colours.contains(UI::Colours::scriptFunction) ?
        this->colours.at(UI::Colours::scriptFunction) :
        this->getScriptTextColour();
}

Colour ColourScheme::getScriptLiteralColour() const
{
    using namespace Serialization;
    return this->colours.contains(UI::Colours::scriptLiteral) ?
        this->colours.at(UI::Colours::scriptLiteral) :
        this->getScriptTextColour();
}

Colour ColourScheme::getScriptBracketColour() const
{
    using namespace Serialization;
    return this->colours.contains(UI::Colours::scriptBracket) ?
        this->colours.at(UI::Colours::scriptBracket) :
        this->getScriptTextColour();
}

Colour ColourScheme::getScriptErrorColour() const
{
    using namespace Serialization;
    return this->colours.contains(UI::Colours::scriptError) ?
        this->colours.at(UI::Colours::scriptError) :
        this->getScriptTextColour();
}

Colour ColourScheme::getScriptCommentColour() const
{
    using namespace Serialization;
    return this->colours.contains(UI::Colours::scriptComment) ?
        this->colours.at(UI::Colours::scriptComment) :
        this->getScriptTextColour();
}

//===----------------------------------------------------------------------===//
// Serialization
//===----------------------------------------------------------------------===//

void ColourScheme::syncWithLiveConstantEditor()
{
    using namespace Serialization;

    this->reset();

    this->colours[UI::Colours::pageFill] = this->getPageFillColour();
    this->colours[UI::Colours::timeline] = this->getTimelineColour();
    this->colours[UI::Colours::sidebarFill] = this->getSidebarFillColour();
    this->colours[UI::Colours::headlineFill] = this->getHeadlineFillColour();
    this->colours[UI::Colours::dialogFill] = this->getDialogFillColour();
    this->colours[UI::Colours::frameBorder] = this->getFrameBorderColour();
    this->colours[UI::Colours::lassoFill] = this->getLassoFillColour();
    this->colours[UI::Colours::lassoBorder] = this->getLassoBorderColour();
    this->colours[UI::Colours::blackKey] = this->getBlackKeyColour();
    this->colours[UI::Colours::whiteKey] = this->getWhiteKeyColour();
    this->colours[UI::Colours::row] = this->getRowColour();
    this->colours[UI::Colours::bar] = this->getBarColour();
    this->colours[UI::Colours::text] = this->getTextColour();
    this->colours[UI::Colours::iconBase] = this->getIconBaseColour();
    this->colours[UI::Colours::iconShadow] = this->getIconShadowColour();
}

SerializedData ColourScheme::serialize() const noexcept
{
    using namespace Serialization;

    SerializedData tree(UI::Colours::scheme);
    tree.setProperty(UI::Colours::name, this->name);

    SerializedData mapXml(UI::Colours::colourMap);

    for (const auto &i : this->colours)
    {
        mapXml.setProperty(i.first, i.second.toString());
    }

    tree.appendChild(mapXml);
    return tree;
}

void ColourScheme::deserialize(const SerializedData &data) noexcept
{
    using namespace Serialization;

    const auto root =
        data.hasType(UI::Colours::scheme) ?
        data : data.getChildWithName(UI::Colours::scheme);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(UI::Colours::name);

    const auto map = root.getChildWithName(UI::Colours::colourMap);
    for (int i = 0; i < map.getNumProperties(); ++i)
    {
        const auto propertyName = map.getPropertyName(i);
        const Colour c(Colour::fromString(map.getProperty(propertyName).toString()));
        this->colours[propertyName] = c;
    }
}

void ColourScheme::reset() noexcept
{
    using namespace Serialization;

    this->name.clear();
    this->colours.clear();
}

String ColourScheme::getResourceId() const noexcept
{
    return this->name;
}

bool ColourScheme::isEquivalentTo(const ColourScheme::Ptr other) const
{
    jassert(other != nullptr);
    return this->name == other->name;
}
