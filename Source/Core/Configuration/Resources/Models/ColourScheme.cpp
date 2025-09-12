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

// the next 7 methods check for legacy ids, if present:

Colour ColourScheme::getPageFillColour() const
{
    using namespace Serialization;
    const auto c = this->colours.contains(UI::Colours::primaryGradientA) ?
        this->colours.at(UI::Colours::primaryGradientA) :
        this->colours.at(UI::Colours::pageFill);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getTimelineColour() const
{
    using namespace Serialization;
    const auto c = this->colours.contains(UI::Colours::primaryGradientB) ?
        this->colours.at(UI::Colours::primaryGradientB) :
        this->colours.at(UI::Colours::timeline);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getHeadlineFillColour() const
{
    using namespace Serialization;
    const auto c = this->colours.contains(UI::Colours::primaryGradientA) ?
        this->colours.at(UI::Colours::primaryGradientA) :
        this->colours.at(UI::Colours::headlineFill);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getSidebarFillColour() const
{
    using namespace Serialization;
    const auto c = this->colours.contains(UI::Colours::secondaryGradientA) ?
        this->colours.at(UI::Colours::secondaryGradientA) :
        this->colours.at(UI::Colours::sidebarFill);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getDialogFillColour() const
{
    using namespace Serialization;
    const auto c = this->colours.contains(UI::Colours::secondaryGradientA) ?
        this->colours.at(UI::Colours::secondaryGradientA) :
        this->colours.at(UI::Colours::dialogFill);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getFrameBorderColour() const
{
    using namespace Serialization;
    const auto c = this->colours.contains(UI::Colours::panelBorder) ?
        this->colours.at(UI::Colours::panelBorder) :
        this->colours.at(UI::Colours::frameBorder);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getLassoFillColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::lassoFill);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getLassoBorderColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::lassoBorder);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getBlackKeyColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::blackKey);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getWhiteKeyColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::whiteKey);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getRowColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::row);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getBarColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::bar);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getTextColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::text);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getIconBaseColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::iconBase);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getIconShadowColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::iconShadow);
    return JUCE_LIVE_CONSTANT(c);
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

SerializedData ColourScheme::serialize() const
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

void ColourScheme::deserialize(const SerializedData &data)
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

void ColourScheme::reset()
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

Identifier ColourScheme::getResourceType() const noexcept
{
    return Serialization::Resources::colourSchemes;
}
