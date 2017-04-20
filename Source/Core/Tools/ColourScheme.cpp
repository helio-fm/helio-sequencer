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

ColourScheme::ColourScheme(const String &scheme)
{
    ScopedPointer<XmlElement> xml(XmlDocument::parse(scheme));
    this->deserialize(*xml);
}

ColourScheme::ColourScheme(const ColourScheme &other)
{
    // Not the best solution but good enough for the simple class like this;
    operator=(other);
}

ColourScheme &ColourScheme::operator=(const ColourScheme &other)
{
    this->id = other.id;
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
            return false;
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

String ColourScheme::getId() const
{
    return this->id;
}

String ColourScheme::getName() const
{
    return this->name;
}

Colour ColourScheme::getPrimaryGradientColourA() const
{
    const Colour c(this->colours[Serialization::UI::Colours::primaryGradientA]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPrimaryGradientColourB() const
{
    const Colour c(this->colours[Serialization::UI::Colours::primaryGradientB]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getSecondaryGradientColourA() const
{
    const Colour c(this->colours[Serialization::UI::Colours::secondaryGradientA]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getSecondaryGradientColourB() const
{
    const Colour c(this->colours[Serialization::UI::Colours::secondaryGradientB]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getShadingGradientColourA() const
{
    const Colour c(this->colours[Serialization::UI::Colours::shadingGradientA]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getShadingGradientColourB() const
{
    const Colour c(this->colours[Serialization::UI::Colours::shadingGradientB]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPanelFillColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::panelFill]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getPanelBorderColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::panelBorder]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getLassoFillColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::lassoFill]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getLassoBorderColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::lassoBorder]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getBlackKeyColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::blackKey]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getWhiteKeyColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::whiteKey]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getRowColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::row]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getBeatColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::beat]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getBarColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::bar]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getTextColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::text]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getIconBaseColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::iconBase]);
    return JUCE_LIVE_CONSTANT(c);
}

Colour ColourScheme::getIconShadowColour() const
{
    const Colour c(this->colours[Serialization::UI::Colours::iconShadow]);
    return JUCE_LIVE_CONSTANT(c);
}


//===----------------------------------------------------------------------===//
// Serialization
//===----------------------------------------------------------------------===//

void ColourScheme::exportColourChanges()
{
    // this WTF-alike-looking code is here
    // to sync JUCE_LIVE_CONSTANT values
    // back to the hashmap
    this->reset();
    this->colours.set(Serialization::UI::Colours::primaryGradientA, this->getPrimaryGradientColourA());
    this->colours.set(Serialization::UI::Colours::primaryGradientB, this->getPrimaryGradientColourB());
    this->colours.set(Serialization::UI::Colours::secondaryGradientA, this->getSecondaryGradientColourA());
    this->colours.set(Serialization::UI::Colours::secondaryGradientB, this->getSecondaryGradientColourB());
    this->colours.set(Serialization::UI::Colours::shadingGradientA, this->getShadingGradientColourA());
    this->colours.set(Serialization::UI::Colours::shadingGradientB, this->getShadingGradientColourB());
    this->colours.set(Serialization::UI::Colours::panelFill, this->getPanelFillColour());
    this->colours.set(Serialization::UI::Colours::lassoBorder, this->getLassoBorderColour());
    this->colours.set(Serialization::UI::Colours::panelBorder, this->getPanelBorderColour());
    this->colours.set(Serialization::UI::Colours::lassoFill, this->getLassoFillColour());
    this->colours.set(Serialization::UI::Colours::blackKey, this->getBlackKeyColour());
    this->colours.set(Serialization::UI::Colours::whiteKey, this->getWhiteKeyColour());
    this->colours.set(Serialization::UI::Colours::row, this->getRowColour());
    this->colours.set(Serialization::UI::Colours::beat, this->getBeatColour());
    this->colours.set(Serialization::UI::Colours::bar, this->getBarColour());
    this->colours.set(Serialization::UI::Colours::text, this->getTextColour());
    this->colours.set(Serialization::UI::Colours::iconBase, this->getIconBaseColour());
    this->colours.set(Serialization::UI::Colours::iconShadow, this->getIconShadowColour());
}

XmlElement *ColourScheme::serialize() const
{
    auto xml = new XmlElement(Serialization::UI::Colours::scheme);
    xml->setAttribute(Serialization::UI::Colours::name, this->name);
    xml->setAttribute(Serialization::UI::Colours::id, this->id);

    auto mapXml = new XmlElement(Serialization::UI::Colours::colourMap);

    ColourMap::Iterator i(this->colours);
    while (i.next())
    {
        mapXml->setAttribute(i.getKey(), i.getValue().toString());
    }

    xml->addChildElement(mapXml);
    return xml;
}

void ColourScheme::deserialize(const XmlElement &xml)
{
    const XmlElement *root =
        xml.hasTagName(Serialization::UI::Colours::scheme) ?
        &xml : xml.getChildByName(Serialization::UI::Colours::scheme);

    if (root == nullptr) { return; }

    this->reset();

    this->name = root->getStringAttribute(Serialization::UI::Colours::name);
    this->id = root->getStringAttribute(Serialization::UI::Colours::id);

    const XmlElement *mapXml =
        root->getChildByName(Serialization::UI::Colours::colourMap);

    for (int i = 0; i < mapXml->getNumAttributes(); ++i)
    {
        const Colour c(Colour::fromString(mapXml->getAttributeValue(i)));
        this->colours.set(mapXml->getAttributeName(i), c);
    }
}

void ColourScheme::reset()
{
    this->id.clear();
    this->name.clear();
    this->colours.clear();
    // todo set reasonable defaults?
    this->colours.set(Serialization::UI::Colours::primaryGradientA, Colours::black);
    this->colours.set(Serialization::UI::Colours::primaryGradientB, Colours::black);
    this->colours.set(Serialization::UI::Colours::secondaryGradientA, Colours::black);
    this->colours.set(Serialization::UI::Colours::secondaryGradientB, Colours::black);
    this->colours.set(Serialization::UI::Colours::shadingGradientA, Colours::black);
    this->colours.set(Serialization::UI::Colours::shadingGradientB, Colours::black);
    this->colours.set(Serialization::UI::Colours::panelFill, Colours::black);
    this->colours.set(Serialization::UI::Colours::lassoBorder, Colours::black);
    this->colours.set(Serialization::UI::Colours::panelBorder, Colours::black);
    this->colours.set(Serialization::UI::Colours::lassoFill, Colours::black);
    this->colours.set(Serialization::UI::Colours::blackKey, Colours::black);
    this->colours.set(Serialization::UI::Colours::whiteKey, Colours::black);
    this->colours.set(Serialization::UI::Colours::row, Colours::black);
    this->colours.set(Serialization::UI::Colours::beat, Colours::black);
    this->colours.set(Serialization::UI::Colours::bar, Colours::black);
    this->colours.set(Serialization::UI::Colours::text, Colours::black);
    this->colours.set(Serialization::UI::Colours::iconBase, Colours::black.withAlpha(0.25f));
    this->colours.set(Serialization::UI::Colours::iconShadow, Colours::white.withAlpha(0.115f));
}
