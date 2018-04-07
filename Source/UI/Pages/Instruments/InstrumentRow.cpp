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
#include "InstrumentRow.h"


InstrumentRow::InstrumentRow(PluginDescription description) :
    pluginDescription(description),
    isSelected(false)
{
    this->setInterceptsMouseClicks(false, false);
}

void InstrumentRow::refreshPluginDescription(PluginDescription val)
{
    this->pluginDescription = val;
}

void InstrumentRow::setSelected(bool val)
{
    this->isSelected = val;
}


//===----------------------------------------------------------------------===//
// Component
//

void InstrumentRow::paint(Graphics &g)
{
    if (this->isSelected)
    {
        g.setColour(Colours::black.withAlpha(0.1f));
        g.fillRoundedRectangle(0.f, 0.f, float(this->getWidth()), float(this->getHeight()), 2.f);
    }

    const int &height = this->getHeight();
    const int &width = this->getWidth();

    g.setFont(Font(Font::getDefaultSansSerifFontName(), height * 0.27f, Font::plain));

    const int numIns = this->pluginDescription.numInputChannels;
    const String inputChannelsString = TRANS_PLURAL("{x} input channels", numIns);

    const int numOuts = this->pluginDescription.numOutputChannels;
    const String outputChannelsString = TRANS_PLURAL("{x} output channels", numOuts);

    const int margin = height / 12;

    g.setColour(Colours::white);
    g.drawText(this->pluginDescription.descriptiveName,
               margin, margin, width, height,
               Justification::topLeft, false);

    g.setColour(Colours::white.withAlpha(0.7f));
    g.drawText(this->pluginDescription.manufacturerName,
               margin, 0, width, height,
               Justification::centredLeft, false);

    g.setColour(Colours::white.withAlpha(0.5f));
    g.drawText(this->pluginDescription.version + ", " + inputChannelsString + ", " + outputChannelsString,
               margin, -margin, width, height,
               Justification::bottomLeft, false);

    g.setColour(Colours::white.withAlpha(0.7f));
    g.drawText(this->pluginDescription.pluginFormatName,
               -margin, margin, width, height,
               Justification::topRight, false);

    g.setColour(Colours::white.withAlpha(0.5f));
    g.drawText(this->pluginDescription.category,
               -margin, 0, width, height,
               Justification::centredRight, false);

    //g.setColour(Colours::white.withAlpha(0.3f));
    //g.drawText(version,
    //           -margin, -margin, width, height,
    //           Justification::bottomRight, false);

}
