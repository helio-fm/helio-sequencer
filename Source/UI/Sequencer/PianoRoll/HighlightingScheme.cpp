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

// todo remove this when migrating to C++17
constexpr int HighlightingScheme::periodsInTile;

Image HighlightingScheme::renderRowsPattern(const HelioTheme &theme,
    const Temperament::Ptr temperament,
    const Scale::Ptr scale, Note::Key root, int height)
{
    if (height < PianoRoll::minRowHeight)
    {
        jassertfalse;
        return Image(Image::RGB, 1, 1, true);
    }

    const auto periodSize = temperament->getPeriodSize();
    const auto numRowsToRender =
        periodSize * (HighlightingScheme::periodsInTile + 1);

    Image patternImage(Image::RGB, 4, height * numRowsToRender, false);
    Graphics g(patternImage);

    g.setImageResamplingQuality(Graphics::lowResamplingQuality);

    float currentHeight = float(height);
    float previousHeight = 0;
    float posY = patternImage.getHeight() - currentHeight;

    const int middleCOffset = periodSize - (temperament->getMiddleC() % periodSize);
    const int lastPeriodRemainder = (temperament->getNumKeys() % periodSize) - root + middleCOffset;

    const auto blackKeyColour = theme.findColour(ColourIDs::Roll::blackKey);
    const auto whiteKeyColour = theme.findColour(ColourIDs::Roll::whiteKey);
    const auto rootKeyColour = theme.findColour(ColourIDs::Roll::rootKey);
    const auto rowLineColour = theme.findColour(ColourIDs::Roll::rowLine);
    const auto bevelBrightness = theme.isDark() ? 0.025f : 0.1f;

    g.setColour(blackKeyColour);
    g.fillAll();

    HelioTheme::drawNoise(theme, g);

    for (int i = lastPeriodRemainder;
        (i < numRowsToRender + lastPeriodRemainder) && ((posY + previousHeight) >= 0.0f);
        i++)
    {
        const int noteNumber = i % periodSize;

        previousHeight = currentHeight;

        if (noteNumber == 0)
        {
            g.setColour(rootKeyColour);
            g.fillRect(0, int(posY + 1), patternImage.getWidth(), int(previousHeight - 1));
            g.setColour(rootKeyColour.brighter(bevelBrightness));
            g.drawHorizontalLine(int(posY + 1), 0.f, float(patternImage.getWidth()));
        }
        else if (scale->hasKey(noteNumber))
        {
            g.setColour(whiteKeyColour);
            g.fillRect(0, int(posY + 1), patternImage.getWidth(), int(previousHeight - 1));
            g.setColour(whiteKeyColour.brighter(bevelBrightness));
            g.drawHorizontalLine(int(posY + 1), 0.f, float(patternImage.getWidth()));
        }

        g.setColour(rowLineColour);
        g.drawHorizontalLine(int(posY), 0.f, float(patternImage.getWidth()));

        currentHeight = float(height);
        posY -= currentHeight;
    }

    return patternImage;
}
