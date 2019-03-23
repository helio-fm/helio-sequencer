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

using namespace Serialization;

ColourScheme::ColourScheme(const ColourScheme &other)
{
    // Not the best solution but good enough for the simple class like this;
    operator=(other);
}

ColourScheme &ColourScheme::operator=(const ColourScheme &other)
{
    this->name = other.name;
    this->colours.clear();

    ColourMap::Iterator i(other.colours);
    while (i.next())
    {
        this->colours.set(i.getKey(), i.getValue());
    }

    return *this;
}

bool operator==(const ColourScheme &lhs, const ColourScheme &rhs)
{
    ColourScheme::ColourMap::Iterator i(lhs.colours);
    while (i.next())
    {
        if (rhs.colours[i.getKey()] != i.getValue())
        {
            return false;
        }
    }

    return lhs.name == rhs.name;
}

void ColourScheme::randomize()
{
    ColourMap newMap;
    ColourMap::Iterator i(this->colours);

    while (i.next())
    {
        Random rnd(Random::getSystemRandom());
        rnd.setSeedRandomly();

        const float r = rnd.nextFloat();
        const float g = rnd.nextFloat();
        const float b = rnd.nextFloat();
        const float a = 1.f;
        newMap.set(i.getKey(), Colour(r, g, b, a));
    }

    this->colours.swapWith(newMap);
}

String ColourScheme::getName() const noexcept
{
    return this->name;
}

Colour ColourScheme::getPrimaryGradientColourA() const
{
    const Colour c(this->colours[UI::Colours::primaryGradientA]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPrimaryGradientColourB() const
{
    const Colour c(this->colours[UI::Colours::primaryGradientB]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getSecondaryGradientColourA() const
{
    const Colour c(this->colours[UI::Colours::secondaryGradientA]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getSecondaryGradientColourB() const
{
    const Colour c(this->colours[UI::Colours::secondaryGradientB]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPanelFillColour() const
{
    const Colour c(this->colours[UI::Colours::panelFill]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPanelBorderColour() const
{
    const Colour c(this->colours[UI::Colours::panelBorder]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getLassoFillColour() const
{
    const Colour c(this->colours[UI::Colours::lassoFill]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getLassoBorderColour() const
{
    const Colour c(this->colours[UI::Colours::lassoBorder]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getBlackKeyColour() const
{
    const Colour c(this->colours[UI::Colours::blackKey]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getWhiteKeyColour() const
{
    const Colour c(this->colours[UI::Colours::whiteKey]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getRowColour() const
{
    const Colour c(this->colours[UI::Colours::row]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getBarColour() const
{
    const Colour c(this->colours[UI::Colours::bar]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getTextColour() const
{
    const Colour c(this->colours[UI::Colours::text]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getIconBaseColour() const
{
    const Colour c(this->colours[UI::Colours::iconBase]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getIconShadowColour() const
{
    const Colour c(this->colours[UI::Colours::iconShadow]);
    return JUCE_LIVE_CONSTANT(c);
}


//===----------------------------------------------------------------------===//
// Serialization
//===----------------------------------------------------------------------===//

void ColourScheme::syncWithLiveConstantEditor()
{
    this->reset();
    this->colours.set(UI::Colours::primaryGradientA, this->getPrimaryGradientColourA());
    this->colours.set(UI::Colours::primaryGradientB, this->getPrimaryGradientColourB());
    this->colours.set(UI::Colours::secondaryGradientA, this->getSecondaryGradientColourA());
    this->colours.set(UI::Colours::secondaryGradientB, this->getSecondaryGradientColourB());
    this->colours.set(UI::Colours::panelFill, this->getPanelFillColour());
    this->colours.set(UI::Colours::lassoBorder, this->getLassoBorderColour());
    this->colours.set(UI::Colours::panelBorder, this->getPanelBorderColour());
    this->colours.set(UI::Colours::lassoFill, this->getLassoFillColour());
    this->colours.set(UI::Colours::blackKey, this->getBlackKeyColour());
    this->colours.set(UI::Colours::whiteKey, this->getWhiteKeyColour());
    this->colours.set(UI::Colours::row, this->getRowColour());
    this->colours.set(UI::Colours::bar, this->getBarColour());
    this->colours.set(UI::Colours::text, this->getTextColour());
    this->colours.set(UI::Colours::iconBase, this->getIconBaseColour());
    this->colours.set(UI::Colours::iconShadow, this->getIconShadowColour());
}

ValueTree ColourScheme::serialize() const
{
    ValueTree tree(UI::Colours::scheme);
    tree.setProperty(UI::Colours::name, this->name, nullptr);

    ValueTree mapXml(UI::Colours::colourMap);

    ColourMap::Iterator i(this->colours);
    while (i.next())
    {
        mapXml.setProperty(i.getKey(), i.getValue().toString(), nullptr);
    }

    tree.appendChild(mapXml, nullptr);
    return tree;
}

void ColourScheme::deserialize(const ValueTree &tree)
{
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
        this->colours.set(propertyName.toString(), c);
    }
}

void ColourScheme::reset()
{
    this->name.clear();
    this->colours.clear();
    // todo set reasonable defaults?
    this->colours.set(UI::Colours::primaryGradientA, Colours::black);
    this->colours.set(UI::Colours::primaryGradientB, Colours::black);
    this->colours.set(UI::Colours::secondaryGradientA, Colours::black);
    this->colours.set(UI::Colours::secondaryGradientB, Colours::black);
    this->colours.set(UI::Colours::panelFill, Colours::black);
    this->colours.set(UI::Colours::lassoBorder, Colours::black);
    this->colours.set(UI::Colours::panelBorder, Colours::black);
    this->colours.set(UI::Colours::lassoFill, Colours::black);
    this->colours.set(UI::Colours::blackKey, Colours::black);
    this->colours.set(UI::Colours::whiteKey, Colours::black);
    this->colours.set(UI::Colours::row, Colours::black);
    this->colours.set(UI::Colours::bar, Colours::black);
    this->colours.set(UI::Colours::text, Colours::black);
    this->colours.set(UI::Colours::iconBase, Colours::black.withAlpha(0.25f));
    this->colours.set(UI::Colours::iconShadow, Colours::white.withAlpha(0.115f));
}

String ColourScheme::getResourceId() const noexcept
{
    return this->name;
}

Identifier ColourScheme::getResourceType() const noexcept
{
    return Serialization::Resources::colourSchemes;
}
