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
#include "ColourScheme.h"
#include "SerializationKeys.h"
#include <utility>

ColourScheme::ColourScheme(const ColourScheme &other)
{
    // Not the best solution but good enough for the simple class like this;
    operator=(other);
}

ColourScheme &ColourScheme::operator=(const ColourScheme &other)
{
    this->name = other.name;
    this->colours.clear();

    for (const auto &i : other.colours)
    {
        this->colours[i.first] = i.second;
    }

    return *this;
}

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

Colour ColourScheme::getPrimaryGradientColourA() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::primaryGradientA);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPrimaryGradientColourB() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::primaryGradientB);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getSecondaryGradientColourA() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::secondaryGradientA);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getSecondaryGradientColourB() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::secondaryGradientB);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPanelFillColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::panelFill);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPanelBorderColour() const
{
    using namespace Serialization;
    const auto c = this->colours.at(UI::Colours::panelBorder);
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

    this->colours[UI::Colours::primaryGradientA] = this->getPrimaryGradientColourA();
    this->colours[UI::Colours::primaryGradientB] = this->getPrimaryGradientColourB();
    this->colours[UI::Colours::secondaryGradientA] = this->getSecondaryGradientColourA();
    this->colours[UI::Colours::secondaryGradientB] = this->getSecondaryGradientColourB();
    this->colours[UI::Colours::panelFill] = this->getPanelFillColour();
    this->colours[UI::Colours::lassoBorder] = this->getLassoBorderColour();
    this->colours[UI::Colours::panelBorder] = this->getPanelBorderColour();
    this->colours[UI::Colours::lassoFill] = this->getLassoFillColour();
    this->colours[UI::Colours::blackKey] = this->getBlackKeyColour();
    this->colours[UI::Colours::whiteKey] = this->getWhiteKeyColour();
    this->colours[UI::Colours::row] = this->getRowColour();
    this->colours[UI::Colours::bar] = this->getBarColour();
    this->colours[UI::Colours::text] = this->getTextColour();
    this->colours[UI::Colours::iconBase] = this->getIconBaseColour();
    this->colours[UI::Colours::iconShadow] = this->getIconShadowColour();
}

ValueTree ColourScheme::serialize() const
{
    using namespace Serialization;

    ValueTree tree(UI::Colours::scheme);
    tree.setProperty(UI::Colours::name, this->name, nullptr);

    ValueTree mapXml(UI::Colours::colourMap);

    for (const auto &i : this->colours)
    {
        mapXml.setProperty(i.first, i.second.toString(), nullptr);
    }

    tree.appendChild(mapXml, nullptr);
    return tree;
}

void ColourScheme::deserialize(const ValueTree &tree)
{
    using namespace Serialization;

    const auto root =
        tree.hasType(UI::Colours::scheme) ?
        tree : tree.getChildWithName(UI::Colours::scheme);

    if (!root.isValid()) { return; }

    this->reset();

    this->name = root.getProperty(UI::Colours::name);

    const auto map = root.getChildWithName(UI::Colours::colourMap);
    for (int i = 0; i < map.getNumProperties(); ++i)
    {
        const auto propertyName = map.getPropertyName(i);
        const Colour c(Colour::fromString(map[propertyName].toString()));
        this->colours[propertyName] = c;
    }
}

void ColourScheme::reset()
{
    using namespace Serialization;

    this->name.clear();
    this->colours.clear();

    // todo set reasonable defaults?
    this->colours[UI::Colours::primaryGradientA] = Colours::black;
    this->colours[UI::Colours::primaryGradientB] = Colours::black;
    this->colours[UI::Colours::secondaryGradientA] = Colours::black;
    this->colours[UI::Colours::secondaryGradientB] = Colours::black;
    this->colours[UI::Colours::panelFill] = Colours::black;
    this->colours[UI::Colours::lassoBorder] = Colours::black;
    this->colours[UI::Colours::panelBorder] = Colours::black;
    this->colours[UI::Colours::lassoFill] = Colours::black;
    this->colours[UI::Colours::blackKey] = Colours::black;
    this->colours[UI::Colours::whiteKey] = Colours::black;
    this->colours[UI::Colours::row] = Colours::black;
    this->colours[UI::Colours::bar] = Colours::black;
    this->colours[UI::Colours::text] = Colours::black;
    this->colours[UI::Colours::iconBase] = Colours::black.withAlpha(0.25f);
    this->colours[UI::Colours::iconShadow] = Colours::white.withAlpha(0.115f);
}

String ColourScheme::getResourceId() const noexcept
{
    return this->name;
}

Identifier ColourScheme::getResourceType() const noexcept
{
    return Serialization::Resources::colourSchemes;
}
