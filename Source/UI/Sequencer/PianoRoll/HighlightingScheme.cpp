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
#include "HighlightingScheme.h"
#include "PianoRoll.h"
#include "HelioTheme.h"
#include "ColourIDs.h"

HighlightingScheme::HighlightingScheme(Note::Key rootKey,
    const Scale::Ptr scale) noexcept :
    rootKey(rootKey),
    scale(scale) {}

void HighlightingScheme::renderBackgroundCache(Temperament::Ptr temperament)
{
    Array<Image> result;
    const auto &theme = HelioTheme::getCurrentTheme();

    //static PNGImageFormat png;

    for (int j = 0; j < PianoRoll::minRowHeight; ++j)
    {
        result.add({});
    }

    for (int j = PianoRoll::minRowHeight; j <= PianoRoll::maxRowHeight; ++j)
    {
        Image img(HighlightingScheme::renderRowsPattern(theme,
            temperament,this->getScale(), this->getRootKey(), j));

        //File f("test" + String(j) + ".png");
        //FileOutputStream outStream(f);
        //png.writeImageToStream(img, outStream);
        //outStream.flush();

        result.add(img);
    }

    this->rows.swapWith(result);
}

Image HighlightingScheme::renderRowsPattern(const HelioTheme &theme,
    const Temperament::Ptr temperament,
    const Scale::Ptr scale, Note::Key root, int height)
{
    if (height < PianoRoll::minRowHeight)
    {
        return Image(Image::RGB, 1, 1, true);
    }

    const auto periodSize = temperament->getPeriodSize();
    const auto numRowsToRender = periodSize * 2;

    Image patternImage(Image::RGB, 8, height * numRowsToRender, false);
    Graphics g(patternImage);

    float currentHeight = float(height);
    float previousHeight = 0;
    float posY = patternImage.getHeight() - currentHeight;

    const int middleCOffset = periodSize - (temperament->getMiddleC() % periodSize);
    const int lastPeriodRemainder = (temperament->getNumKeys() % periodSize) - root + middleCOffset;

    //g.setColour(whiteKeyOddColour);
    //g.fillRect(patternImage.getBounds());

    const Colour blackKeyEvenColour(theme.findColour(ColourIDs::Roll::blackKey));
    const Colour blackKeyOddColour(theme.findColour(ColourIDs::Roll::blackKeyAlt));
    const Colour whiteKeyEvenColour(theme.findColour(ColourIDs::Roll::whiteKey));
    const Colour whiteKeyOddColour(theme.findColour(ColourIDs::Roll::whiteKeyAlt));
    const Colour rootKeyEvenColour(whiteKeyEvenColour.brighter(0.1f));
    const Colour rootKeyOddColour(whiteKeyOddColour.brighter(0.1f));
    const Colour rowLineColour(theme.findColour(ColourIDs::Roll::rowLine));

    // draw rows
    for (int i = lastPeriodRemainder;
        (i < numRowsToRender + lastPeriodRemainder) && ((posY + previousHeight) >= 0.0f);
        i++)
    {
        const int noteNumber = i % periodSize;
        const int periodNumber = i / periodSize;
        const bool periodIsOdd = ((periodNumber % 2) > 0);

        previousHeight = currentHeight;

        if (noteNumber == 0)
        {
            const auto c = periodIsOdd ? rootKeyOddColour : rootKeyEvenColour;
            g.setColour(c);
            g.fillRect(0, int(posY + 1), patternImage.getWidth(), int(previousHeight - 1));
            g.setColour(c.brighter(0.025f));
            g.drawHorizontalLine(int(posY + 1), 0.f, float(patternImage.getWidth()));
        }
        else if (scale->hasKey(noteNumber))
        {
            const auto c = periodIsOdd ? whiteKeyOddColour : whiteKeyEvenColour;
            g.setColour(c);
            g.fillRect(0, int(posY + 1), patternImage.getWidth(), int(previousHeight - 1));
            g.setColour(c.brighter(0.025f));
            g.drawHorizontalLine(int(posY + 1), 0.f, float(patternImage.getWidth()));
        }
        else
        {
            g.setColour(periodIsOdd ? blackKeyOddColour : blackKeyEvenColour);
            g.fillRect(0, int(posY + 1), patternImage.getWidth(), int(previousHeight - 1));
        }

        // fill divider line
        g.setColour(rowLineColour);
        g.drawHorizontalLine(int(posY), 0.f, float(patternImage.getWidth()));

        currentHeight = float(height);
        posY -= currentHeight;
    }

    HelioTheme::drawNoise(theme, g, 1.5f);

    return patternImage;
}
