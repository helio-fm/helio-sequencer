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
#include "Icons.h"
#include "BinaryData.h"
#include "ColourIDs.h"

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
    setIconForKey(microphone);
    setIconForKey(volume);
    setIconForKey(script);

    setIconForKey(list);
    setIconForKey(ellipsis);
    setIconForKey(progressIndicator);
    setIconForKey(console);

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

    setIconForKey(cursorTool);
    setIconForKey(drawTool);
    setIconForKey(selectionTool);
    setIconForKey(zoomTool);
    setIconForKey(dragTool);
    setIconForKey(cropTool);
    setIconForKey(cutterTool);
    setIconForKey(eraserTool);
    setIconForKey(chordTool);
    setIconForKey(chordBuilder);
    setIconForKey(stretchLeft);
    setIconForKey(stretchRight);
    setIconForKey(inverseDown);
    setIconForKey(inverseUp);
    setIconForKey(expand);

    setIconForKey(up);
    setIconForKey(down);
    setIconForKey(back);
    setIconForKey(forward);
    setIconForKey(mediaForward);
    setIconForKey(mediaRewind);
    setIconForKey(reprise);

    setIconForKey(pageUp);
    setIconForKey(pageDown);
    setIconForKey(timelineNext);
    setIconForKey(timelinePrevious);
    setIconForKey(paint);
    setIconForKey(tag);

    setIconForKey(menu);
    setIconForKey(submenu);

    setIconForKey(login);
    setIconForKey(remote);
    setIconForKey(local);
    setIconForKey(github);

    setIconForKey(commit);
    setIconForKey(reset);
    setIconForKey(push);
    setIconForKey(pull);

    setIconForKey(mute);
    setIconForKey(unmute);

    setIconForKey(arpeggiate);
    setIconForKey(refactor);
    setIconForKey(render);

    setIconForKey(selection);
    setIconForKey(selectAll);
    setIconForKey(selectNone);
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
    const Colour &iconBaseColour, const Colour &iconShadeColour)
{
    const auto foundImage = builtInImages.find(id);
    if (foundImage == builtInImages.end() || maxSize < 1)
    {
        return Image(Image::ARGB, 1, 1, true);
    }
    
    Image resultImage(Image::ARGB, maxSize, maxSize, true);
    Graphics g(resultImage);
    
    UniquePointer<Drawable> drawableSVG(Drawable::createFromImageData(foundImage->second.data, foundImage->second.numBytes));
    drawableSVG->replaceColour(Colours::black, iconBaseColour);

    Rectangle<int> area(0, 0, maxSize, maxSize);
    drawableSVG->drawWithin(g, area.toFloat(), RectanglePlacement::centred, 1.0f);
    
#if PLATFORM_DESKTOP
    GlowEffect glow;
    glow.setGlowProperties(1.25, iconShadeColour);
    glow.applyEffect(resultImage, g, 1.f, 1.f);
#endif
        
    drawableSVG->drawWithin(g, area.toFloat(), RectanglePlacement::centred, 1.0f);

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

static FlatHashMap<uint32, Image> prerenderedVectors;

void Icons::clearPrerenderedCache()
{
    prerenderedVectors.clear();
}

const int kRoundFactor = 8;

Image Icons::findByName(Icons::Id id, int maxSize)
{
    const auto &dis = Desktop::getInstance().getDisplays().getMainDisplay();

#if JUCE_ANDROID
    const int retinaFactor = 2;
#else
    const int retinaFactor = int(dis.scale);
#endif

    const int fixedSize = int(floorf(float(maxSize) / float(kRoundFactor))) * kRoundFactor * retinaFactor;
    const uint32 iconKey = (id * 1000) + fixedSize;
    
    if (prerenderedVectors.contains(iconKey))
    {
        return prerenderedVectors[iconKey];
    }
    
    const Colour iconBaseColour(findDefaultColour(ColourIDs::Icons::fill));
    const Colour iconShadeColour(findDefaultColour(ColourIDs::Icons::shadow));
    const Image prerenderedImage(renderVector(id, fixedSize, iconBaseColour, iconShadeColour));
    prerenderedVectors[iconKey] = prerenderedImage;

    return prerenderedImage;
}

Image Icons::renderForTheme(const LookAndFeel &lf, Icons::Id id, int maxSize)
{
    const auto &dis = Desktop::getInstance().getDisplays().getMainDisplay();

#if JUCE_ANDROID
    const int retinaFactor = 2;
#else
    const int retinaFactor = int(dis.scale);
#endif

    const int fixedSize = int(floorf(float(maxSize) / float(kRoundFactor))) * kRoundFactor * retinaFactor;
    const Colour iconBaseColour(lf.findColour(ColourIDs::Icons::fill));
    const Colour iconShadeColour(lf.findColour(ColourIDs::Icons::shadow));
    const Image prerenderedImage(renderVector(id, fixedSize, iconBaseColour, iconShadeColour));
    return prerenderedImage;
}

void Icons::drawImageRetinaAware(const Image &image, Graphics &g, int cx, int cy)
{
    const auto &dis = Desktop::getInstance().getDisplays().getMainDisplay();

#if JUCE_ANDROID
    const int scale = 2;
#else
    const int scale = int(dis.scale);
#endif

    const int w = image.getWidth();
    const int h = image.getHeight();

    if (scale > 1)
    {
        const int w2 = w / scale;
        const int h2 = h / scale;
        
        g.drawImage(image,
                    cx - int(w2 / 2),
                    cy - int(h2 / 2),
                    w2, h2,
                    0, 0,
                    w, h,
                    false);
    }
    else
    {
        g.drawImageAt(image, cx - int(w / 2), cy - int(h / 2));
    }
}
