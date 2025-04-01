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
#include "Icons.h"
#include "ColourIDs.h"
#include "Config.h"

static String getIconFileName(const String &string)
{
    // to lower camel case:
    if (string.length() > 1)
    {
        return string.substring(0, 1).toLowerCase() +
            string.substring(1);
    }

    return string;
}

struct BuiltInImageData final
{
    BuiltInImageData() = default;

    BuiltInImageData(const BuiltInImageData &other) :
        data(other.data),
        numBytes(other.numBytes) {}

    BuiltInImageData(const String &name)
    {
        const auto assumedFileName = getIconFileName(name) + "_svg";
        this->data = BinaryData::getNamedResource(assumedFileName.toRawUTF8(), this->numBytes);
    }
    
    BuiltInImageData &operator= (const BuiltInImageData &other)
    {
        this->data = other.data;
        this->numBytes = other.numBytes;
        return *this;
    }
    
    const void *data;
    int numBytes = 0;
};

static FlatHashMap<Icons::Id, BuiltInImageData> builtInImages;

void Icons::clearBuiltInImages()
{
    builtInImages.clear();
}

#define setIconForKey(x) builtInImages[Icons::x] = BuiltInImageData(#x);

void Icons::initBuiltInImages()
{
    setIconForKey(helio);
    setIconForKey(project);
    setIconForKey(trackGroup);
    setIconForKey(pianoTrack);
    setIconForKey(automationTrack);
    setIconForKey(versionControl);
    setIconForKey(settings);
    setIconForKey(patterns);
    setIconForKey(orchestraPit);
    setIconForKey(instrument);
    setIconForKey(instrumentNode);
    setIconForKey(audioPlugin);
    setIconForKey(annotation);
    setIconForKey(colour);
    setIconForKey(revision);
    setIconForKey(routing);
    setIconForKey(piano);
    setIconForKey(meter);
    setIconForKey(metronome);

    setIconForKey(mute);
    setIconForKey(volumeUp);
    setIconForKey(volumeDown);
    setIconForKey(volume);
    setIconForKey(volumePanel);

    setIconForKey(list);
    setIconForKey(ellipsis);
    setIconForKey(progressIndicator);
    setIconForKey(console);
    setIconForKey(bottomBar);

    setIconForKey(browse);
    setIconForKey(apply);
    setIconForKey(toggleOn);
    setIconForKey(toggleOff);

    setIconForKey(play);
    setIconForKey(pause);
    setIconForKey(stop);
    setIconForKey(record);

    setIconForKey(undo);
    setIconForKey(redo);

    setIconForKey(copy);
    setIconForKey(cut);
    setIconForKey(paste);

    setIconForKey(create);
    setIconForKey(remove);
    setIconForKey(close);

    setIconForKey(fail);
    setIconForKey(success);

    setIconForKey(zoomIn);
    setIconForKey(zoomOut);
    setIconForKey(zoomToFit);
    setIconForKey(lockZoom);

    setIconForKey(cursorTool);
    setIconForKey(drawTool);
    setIconForKey(selectionTool);
    setIconForKey(dragTool);
    setIconForKey(cropTool);
    setIconForKey(cutterTool);
    setIconForKey(chordBuilder);
    setIconForKey(submenu);

    setIconForKey(expand);
    setIconForKey(stretchLeft);
    setIconForKey(stretchRight);
    setIconForKey(inverseDown);
    setIconForKey(inverseUp);
    setIconForKey(inversion);
    setIconForKey(retrograde);
    setIconForKey(legato);
    setIconForKey(staccato);
    setIconForKey(snap);
    setIconForKey(cleanup);

    setIconForKey(up);
    setIconForKey(down);
    setIconForKey(back);
    setIconForKey(forward);
    setIconForKey(reprise);

    setIconForKey(timelineNext);
    setIconForKey(timelinePrevious);
    setIconForKey(paint);
    setIconForKey(tag);

    setIconForKey(remote);
    setIconForKey(local);
    setIconForKey(github);

    setIconForKey(commit);
    setIconForKey(reset);
    setIconForKey(push);
    setIconForKey(pull);

    setIconForKey(arpeggiate);
    setIconForKey(refactor);
    setIconForKey(render);

    setIconForKey(selection);
    setIconForKey(selectAll);
    setIconForKey(selectNone);

    setIconForKey(flat);
    setIconForKey(sharp);
    setIconForKey(doubleFlat);
    setIconForKey(doubleSharp);
    setIconForKey(microtoneUp);
    setIconForKey(microtoneUp2);
    setIconForKey(microtoneDown);
    setIconForKey(microtoneDown2);
}

static const Path extractPathFromDrawable(const Drawable *d)
{
    for (int i = 0; i < d->getNumChildComponents(); ++i)
    {
        auto *child = d->getChildComponent(i);
        
        if (auto *drawablePath = dynamic_cast<DrawablePath *>(child))
        {
            return drawablePath->getPath();
        }
        
        if (auto *drawableComposite = dynamic_cast<DrawableComposite *>(child))
        {
            for (int j = 0; j < drawableComposite->getNumChildComponents(); ++j)
            {
                auto *compositeChild = drawableComposite->getChildComponent(i);
                
                if (auto *drawablePath = dynamic_cast<DrawablePath *>(compositeChild))
                {
                    return drawablePath->getPath();
                }
            }
        }
    }
    
    return Path();
}

static Image renderVector(Icons::Id id, int maxSize,
    const Colour &iconBaseColour, const Colour &iconShadowColour,
    Rectangle<float> &outContentBounds,
    RectanglePlacement placement = RectanglePlacement::centred)
{
    const auto foundImage = builtInImages.find(id);
    if (foundImage == builtInImages.end() || maxSize < 1)
    {
        return Image(Image::ARGB, 1, 1, true);
    }
    
    Image resultImage(Image::ARGB, maxSize, maxSize, true);
    Graphics g(resultImage);
    
    UniquePointer<Drawable> drawable(Drawable::createFromImageData(foundImage->second.data, foundImage->second.numBytes));
    auto *drawableSvg = dynamic_cast<DrawableComposite *>(drawable.get());
    if (drawableSvg == nullptr)
    {
        jassertfalse; // expects svg's
        return {};
    }

    drawableSvg->replaceColour(Colours::black, iconBaseColour);

    const auto drawableBounds = drawableSvg->getDrawableBounds();
    const auto area = Rectangle<float>(0.f, 0.f, float(maxSize), float(maxSize));
    const auto transformToFit = placement.getTransformToFit(drawableBounds, area);

    outContentBounds = drawableBounds.transformedBy(transformToFit);

    drawableSvg->draw(g, 1.f, transformToFit);
    
#if PLATFORM_DESKTOP
    if (iconShadowColour.getAlpha() > 0)
    {
        GlowEffect glow;
        glow.setGlowProperties(1.25f, iconShadowColour);
        glow.applyEffect(resultImage, g, 1.f, 0.75f);
    }
#endif

    drawableSvg->draw(g, 1.f, transformToFit);

    return resultImage;
}

UniquePointer<Drawable> Icons::getDrawableByName(Icons::Id id)
{
    const auto foundImage = builtInImages.find(id);
    if (foundImage == builtInImages.end())
    {
        return nullptr;
    }
    
    return Drawable::createFromImageData(foundImage->second.data, foundImage->second.numBytes);
}

Path Icons::getPathByName(Icons::Id id)
{
    const auto foundImage = builtInImages.find(id);
    if (foundImage == builtInImages.end())
    {
        return {};
    }

    UniquePointer<Drawable> drawableSVG(Drawable::createFromImageData(foundImage->second.data, foundImage->second.numBytes));
    return Path(extractPathFromDrawable(drawableSVG.get()));
}

static FlatHashMap<uint32, Image> prerenderedSVGs;
static FlatHashMap<uint32, Rectangle<float>> prerenderedBounds;

void Icons::clearPrerenderedCache()
{
    prerenderedSVGs.clear();
}

static float getScaleFactor()
{
    return App::Config().getUiFlags()->getUiScaleFactor() *
        float(Desktop::getInstance().getDisplays().getPrimaryDisplay()->scale);
}

static const int kMaxIconSize = 30;
static const int kIconSizes[] = {
  8, // 0
  8, // 1
  8, // 2
  8, // 3
  8, // 4
  8, // 5
  8, // 6
  8, // 7
  8, // 8
  8, // 9
  8, // 10
  8, // 11
  8, // 12
  8, // 13
  8, // 14
  8, // 15
  14, // 16
  14, // 17
  14, // 18
  14, // 19
  14, // 20
  14, // 21
  14, // 22
  14, // 23
  22, // 24
  22, // 25
  22, // 26
  22, // 27
  22, // 28
  22, // 29
  22, // 30
  22, // 31
  22 // 32
};

Image Icons::findByName(Icons::Id id, int maxSize)
{
    const auto retinaFactor = getScaleFactor();
    const auto lookupSize = sizeof(kIconSizes) / sizeof(kIconSizes[0]);
    const int fixedSize = (maxSize < 0 || maxSize >= lookupSize) ?
        int(ceilf(kMaxIconSize * retinaFactor)) : int(ceilf(kIconSizes[maxSize] * retinaFactor));

    const uint32 iconKey = (id * 1000) + fixedSize;
    if (prerenderedSVGs.contains(iconKey))
    {
        return prerenderedSVGs[iconKey];
    }
    
    Rectangle<float> contentBounds;
    const Colour iconBaseColour(findDefaultColour(ColourIDs::Icons::fill));
    const Colour iconShadeColour(findDefaultColour(ColourIDs::Icons::shadow));
    const Image prerenderedImage(renderVector(id, fixedSize,
        iconBaseColour, iconShadeColour, contentBounds));

    prerenderedSVGs[iconKey] = prerenderedImage;

    return prerenderedImage;
}

Image Icons::findByName(Icons::Id id, int exactSize,
    RectanglePlacement alignment,
    const Colour &fillColour, const Colour &shadowColour,
    Rectangle<float> &outContentBounds)
{
    const auto retinaFactor = getScaleFactor();
    const int scaledSize = int(ceilf(exactSize * retinaFactor));

    const uint32 iconKey = (id * 1000) + scaledSize;
    if (prerenderedSVGs.contains(iconKey))
    {
        outContentBounds = prerenderedBounds[iconKey];
        return prerenderedSVGs[iconKey];
    }
    
    const Image prerenderedImage(renderVector(id, scaledSize,
        fillColour, shadowColour, outContentBounds, alignment));

    outContentBounds /= retinaFactor;
    //DBG(outContentBounds.toString());

    prerenderedSVGs[iconKey] = prerenderedImage;
    prerenderedBounds[iconKey] = outContentBounds;

    return prerenderedImage;
}

Image Icons::renderForTheme(const LookAndFeel &lf, Icons::Id id, int maxSize)
{
    const auto retinaFactor = getScaleFactor();
    const auto lookupSize = sizeof(kIconSizes) / sizeof(kIconSizes[0]);
    const int fixedSize = (maxSize < 0 || maxSize >= lookupSize) ?
        int(ceilf(kMaxIconSize * retinaFactor)) : int(ceilf(kIconSizes[maxSize] * retinaFactor));

    Rectangle<float> contentBounds;
    const Colour iconBaseColour(lf.findColour(ColourIDs::Icons::fill));
    const Colour iconShadeColour(lf.findColour(ColourIDs::Icons::shadow));
    const Image prerenderedImage(renderVector(id, fixedSize, iconBaseColour, iconShadeColour, contentBounds));
    return prerenderedImage;
}

void Icons::drawImageRetinaAware(const Image &image, Graphics &g, int cx, int cy)
{
    const auto scale = getScaleFactor();
    const int w = image.getWidth();
    const int h = image.getHeight();

    if (scale > 1.f)
    {
        const auto w2 = roundToIntAccurate(float(w) / scale);
        const auto h2 = roundToIntAccurate(float(h) / scale);

        g.drawImage(image,
            cx - (w2 / 2),
            cy - (h2 / 2),
            w2, h2,
            0, 0,
            w, h,
            false);
    }
    else
    {
        g.drawImageAt(image, cx - (w / 2), cy - (h / 2));
    }
}

MouseCursor Icons::getCopyingCursor()
{
    static auto image = ImageFileFormat::loadFrom(BinaryData::copyingCursor_gif, BinaryData::copyingCursor_gifSize);
    static MouseCursor cursor(image, 1, 0);
    return cursor;
}

MouseCursor Icons::getErasingCursor()
{
    static auto image = ImageFileFormat::loadFrom(BinaryData::erasingCursor_gif, BinaryData::erasingCursor_gifSize);
    static MouseCursor cursor(image, 1, 0);
    return cursor;
}
