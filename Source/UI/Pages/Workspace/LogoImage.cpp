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
#include "LogoImage.h"
#include "BinaryData.h"

LogoImage::LogoImage()
{
    logo = Drawable::createFromImageData (BinaryData::Logo_png, BinaryData::Logo_pngSize);
    setSize (400, 400);
}

LogoImage::~LogoImage()
{
    logo = nullptr;
}

void LogoImage::paint (Graphics& g)
{
    g.setColour (Colours::black);
    jassert (logo != 0);
    logo->drawWithin(g, Rectangle<float>(0.f, 0.f, float(this->getWidth()), float(this->getHeight())),
        RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.000f);
}
