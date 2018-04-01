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
#include "App.h"
#include "HelioTheme.h"
#include "ColourIDs.h"

const String Icons::empty = "empty";
const String Icons::menu = "menu";
const String Icons::project = "project";
const String Icons::play = "play";
const String Icons::pause = "pause";
const String Icons::instrumentSettings = "instrumentSettings";
const String Icons::instrumentGraph = "instrumentGraph";
const String Icons::vcs = "vcs";
const String Icons::automation = "automation";
const String Icons::workspace = "workspace";
const String Icons::layer = "layer";
const String Icons::group = "group";
const String Icons::backward = "backward";
const String Icons::forward = "forward";
const String Icons::left = "left";
const String Icons::right = "right";
const String Icons::settings = "settings";
const String Icons::saxophone = "saxophone";
const String Icons::trash = "trash";
const String Icons::ellipsis = "ellipsis";
const String Icons::undo = "undo";
const String Icons::redo = "redo";
const String Icons::close = "close";
const String Icons::apply = "apply";
const String Icons::create = "create";
const String Icons::open = "open";
const String Icons::minus = "minus";
const String Icons::plus = "plus";
const String Icons::colour = "colour";
const String Icons::login = "login";
const String Icons::fail = "fail";
const String Icons::success = "success";
const String Icons::progressIndicator= "progressIndicator";
const String Icons::remote = "remote";
const String Icons::local = "local";
const String Icons::commit = "commit";
const String Icons::reset = "reset";
const String Icons::push = "push";
const String Icons::pull = "pull";
const String Icons::copy = "copy";
const String Icons::paste = "paste";
const String Icons::cut = "cut";
const String Icons::zoomIn = "zoomIn";
const String Icons::zoomOut = "zoomOut";
const String Icons::update = "update";
const String Icons::up = "up";
const String Icons::down = "down";
const String Icons::previous = "previous";
const String Icons::next = "next";
const String Icons::volumeUp = "volumeUp";
const String Icons::volumeOff = "volumeOff";
const String Icons::render = "render";
const String Icons::cursorTool = "cursorTool";
const String Icons::drawTool = "drawTool";
const String Icons::selectionTool = "selectionTool";
const String Icons::zoomTool = "zoomTool";
const String Icons::dragTool = "dragTool";
const String Icons::insertSpaceTool = "insertSpaceTool";
const String Icons::wipeScapeTool = "wipeScapeTool";
const String Icons::cropTool = "cropTool";
const String Icons::annotation = "annotation";
const String Icons::stack = "stack";
const String Icons::switcher = "switcher";
const String Icons::stretchLeft = "stretchLeft";
const String Icons::stretchRight = "stretchRight";
const String Icons::toggleOn = "toggleOn";
const String Icons::toggleOff = "toggleOff";
const String Icons::roman1 = "roman1";
const String Icons::roman2 = "roman2";
const String Icons::roman3 = "roman3";
const String Icons::roman4 = "roman4";
const String Icons::roman5 = "roman5";
const String Icons::roman6 = "roman6";
const String Icons::roman7 = "roman7";


struct BuiltInImageData
{
    BuiltInImageData()
    {
        // empty constructor, needed for hashmap, should never be called.
        //jassertfalse;
    }
    
    BuiltInImageData(const void *_data, const size_t _numBytes) :
    data(_data), numBytes(_numBytes)
    { }
    
    BuiltInImageData &operator= (const BuiltInImageData &other)
    {
        this->data = other.data;
        this->numBytes = other.numBytes;
        return *this;
    }
    
    const void *data;
    size_t numBytes;
};


static HashMap<String, BuiltInImageData> builtInImages;

void Icons::clearBuiltInImages()
{
    builtInImages.clear();
}

void Icons::setupBuiltInImages()
{
    builtInImages.set(Icons::workspace, BuiltInImageData(BinaryData::logo2_svg, BinaryData::logo2_svgSize));
    builtInImages.set(Icons::layer, BuiltInImageData(BinaryData::eight_note_svg, BinaryData::eight_note_svgSize));
    builtInImages.set(Icons::group, BuiltInImageData(BinaryData::beamed_note_svg, BinaryData::beamed_note_svgSize));
    builtInImages.set(Icons::automation, BuiltInImageData(BinaryData::bezier_svg, BinaryData::bezier_svgSize));
    builtInImages.set(Icons::project, BuiltInImageData(BinaryData::clef_svg, BinaryData::clef_svgSize));
    builtInImages.set(Icons::vcs, BuiltInImageData(BinaryData::hourglass_svg, BinaryData::hourglass_svgSize));
    builtInImages.set(Icons::saxophone, BuiltInImageData(BinaryData::saxophone_svg, BinaryData::saxophone_svgSize));
    builtInImages.set(Icons::settings, BuiltInImageData(BinaryData::settings2_svg, BinaryData::settings2_svgSize));
    
    builtInImages.set(Icons::apply, BuiltInImageData(BinaryData::check_svg, BinaryData::check_svgSize));
    
    builtInImages.set(Icons::menu, BuiltInImageData(BinaryData::menu_svg, BinaryData::menu_svgSize));
    builtInImages.set(Icons::login, BuiltInImageData(BinaryData::key_svg, BinaryData::key_svgSize));
    builtInImages.set(Icons::create, BuiltInImageData(BinaryData::plus2_svg, BinaryData::plus2_svgSize));
    builtInImages.set(Icons::open, BuiltInImageData(BinaryData::folder2_svg, BinaryData::folder2_svgSize));
    builtInImages.set(Icons::minus, BuiltInImageData(BinaryData::minus2_svg, BinaryData::minus2_svgSize));
    builtInImages.set(Icons::plus, BuiltInImageData(BinaryData::plus2_svg, BinaryData::plus2_svgSize));
    builtInImages.set(Icons::colour, BuiltInImageData(BinaryData::brush_svg, BinaryData::brush_svgSize));
    
    builtInImages.set(Icons::remote, BuiltInImageData(BinaryData::cloud2_svg, BinaryData::cloud2_svgSize));
    builtInImages.set(Icons::local, BuiltInImageData(BinaryData::drive_svg, BinaryData::drive_svgSize));
    
    builtInImages.set(Icons::play, BuiltInImageData(BinaryData::play2_svg, BinaryData::play2_svgSize));
    builtInImages.set(Icons::pause, BuiltInImageData(BinaryData::pause2_svg, BinaryData::pause2_svgSize));
    
    builtInImages.set(Icons::undo, BuiltInImageData(BinaryData::arrowback_svg, BinaryData::arrowback_svgSize));
    builtInImages.set(Icons::redo, BuiltInImageData(BinaryData::arrowforward_svg, BinaryData::arrowforward_svgSize));
    
    builtInImages.set(Icons::close, BuiltInImageData(BinaryData::times_svg, BinaryData::times_svgSize));
    builtInImages.set(Icons::fail, BuiltInImageData(BinaryData::times_svg, BinaryData::times_svgSize));
    builtInImages.set(Icons::success, BuiltInImageData(BinaryData::check_svg, BinaryData::check_svgSize));
    
    builtInImages.set(Icons::progressIndicator, BuiltInImageData(BinaryData::heptagram2_svg, BinaryData::heptagram2_svgSize));
    
    builtInImages.set(Icons::commit, BuiltInImageData(BinaryData::diskette_svg, BinaryData::diskette_svgSize));
    builtInImages.set(Icons::reset, BuiltInImageData(BinaryData::history_svg, BinaryData::history_svgSize));
    builtInImages.set(Icons::push, BuiltInImageData(BinaryData::cloudupload_svg, BinaryData::cloudupload_svgSize));
    builtInImages.set(Icons::pull, BuiltInImageData(BinaryData::clouddownload_svg, BinaryData::clouddownload_svgSize));
    
    builtInImages.set(Icons::copy, BuiltInImageData(BinaryData::copy_svg, BinaryData::copy_svgSize));
    builtInImages.set(Icons::paste, BuiltInImageData(BinaryData::paste_svg, BinaryData::paste_svgSize));
    builtInImages.set(Icons::cut, BuiltInImageData(BinaryData::scissors_svg, BinaryData::scissors_svgSize));
    builtInImages.set(Icons::trash, BuiltInImageData(BinaryData::cup_svg, BinaryData::cup_svgSize));
    builtInImages.set(Icons::ellipsis, BuiltInImageData(BinaryData::ellipsish_svg, BinaryData::ellipsish_svgSize));
    
    builtInImages.set(Icons::zoomIn, BuiltInImageData(BinaryData::zoomin_svg, BinaryData::zoomin_svgSize));
    builtInImages.set(Icons::zoomOut, BuiltInImageData(BinaryData::zoomout_svg, BinaryData::zoomout_svgSize));
    
    builtInImages.set(Icons::up, BuiltInImageData(BinaryData::angleup_svg, BinaryData::angleup_svgSize));
    builtInImages.set(Icons::down, BuiltInImageData(BinaryData::angledown_svg, BinaryData::angledown_svgSize));
    
    // todo remove that?
    builtInImages.set("inverseUp", BuiltInImageData(BinaryData::angledoubleup_svg, BinaryData::angledoubleup_svgSize));
    builtInImages.set("inverseDown", BuiltInImageData(BinaryData::angledoubledown_svg, BinaryData::angledoubledown_svgSize));
    
    builtInImages.set("shiftUp", BuiltInImageData(BinaryData::angleup_svg, BinaryData::angleup_svgSize));
    builtInImages.set("shiftDown", BuiltInImageData(BinaryData::angledown_svg, BinaryData::angledown_svgSize));
    
    builtInImages.set("shiftLeft", BuiltInImageData(BinaryData::angleleft_svg, BinaryData::angleleft_svgSize));
    builtInImages.set("shiftRight", BuiltInImageData(BinaryData::angleright_svg, BinaryData::angleright_svgSize));
    
    builtInImages.set("scalePlus", BuiltInImageData(BinaryData::angledoubleright_svg, BinaryData::angledoubleright_svgSize));
    builtInImages.set("scaleMinus", BuiltInImageData(BinaryData::angledoubleleft_svg, BinaryData::angledoubleleft_svgSize));
    
    builtInImages.set("shrinkLeft", BuiltInImageData(BinaryData::chevronright2_svg, BinaryData::chevronright2_svgSize));
    builtInImages.set("shrinkRight", BuiltInImageData(BinaryData::chevronleft2_svg, BinaryData::chevronleft2_svgSize));
    //===------------------------------------------------------------------===//
    
    builtInImages.set(Icons::stretchLeft, BuiltInImageData(BinaryData::chevronleft2_svg, BinaryData::chevronleft2_svgSize));
    builtInImages.set(Icons::stretchRight, BuiltInImageData(BinaryData::chevronright2_svg, BinaryData::chevronright2_svgSize));
    
    builtInImages.set(Icons::forward, BuiltInImageData(BinaryData::angledoubleright_svg, BinaryData::angledoubleright_svgSize));
    builtInImages.set(Icons::backward, BuiltInImageData(BinaryData::angledoubleleft_svg, BinaryData::angledoubleleft_svgSize));
    builtInImages.set(Icons::left, BuiltInImageData(BinaryData::chevronleft2_svg, BinaryData::chevronleft2_svgSize));
    builtInImages.set(Icons::right, BuiltInImageData(BinaryData::chevronright2_svg, BinaryData::chevronright2_svgSize));
    
    builtInImages.set(Icons::previous, BuiltInImageData(BinaryData::arrowleft2_svg, BinaryData::arrowleft2_svgSize));
    builtInImages.set(Icons::next, BuiltInImageData(BinaryData::arrowright2_svg, BinaryData::arrowright2_svgSize));
    
    builtInImages.set(Icons::volumeUp, BuiltInImageData(BinaryData::volumeup_svg, BinaryData::volumeup_svgSize));
    builtInImages.set(Icons::volumeOff, BuiltInImageData(BinaryData::volumeoff_svg, BinaryData::volumeoff_svgSize));
    
    builtInImages.set(Icons::update, BuiltInImageData(BinaryData::download2_svg, BinaryData::download2_svgSize));
    
    builtInImages.set(Icons::render, BuiltInImageData(BinaryData::waveform_svg, BinaryData::waveform_svgSize));
    
    builtInImages.set(Icons::cursorTool, BuiltInImageData(BinaryData::cursor2_svg, BinaryData::cursor2_svgSize));
    //builtInImages.set(Icons::drawTool, BuiltInImageData(BinaryData::pencil4_svg, BinaryData::pencil4_svgSize));
    builtInImages.set(Icons::drawTool, BuiltInImageData(BinaryData::poetry_svg, BinaryData::poetry_svgSize));
    builtInImages.set(Icons::selectionTool, BuiltInImageData(BinaryData::marquee_svg, BinaryData::marquee_svgSize));
    builtInImages.set(Icons::zoomTool, BuiltInImageData(BinaryData::zoomin_svg, BinaryData::zoomin_svgSize));
    builtInImages.set(Icons::dragTool, BuiltInImageData(BinaryData::arrows_svg, BinaryData::arrows_svgSize));
    
    builtInImages.set(Icons::insertSpaceTool, BuiltInImageData(BinaryData::insertspace_svg, BinaryData::insertspace_svgSize));
    builtInImages.set(Icons::wipeScapeTool, BuiltInImageData(BinaryData::wipespace_svg, BinaryData::wipespace_svgSize));
    builtInImages.set(Icons::cropTool, BuiltInImageData(BinaryData::crop_svg, BinaryData::crop_svgSize));
    
    //builtInImages.set(Icons::annotation, BuiltInImageData(BinaryData::poetry_svg, BinaryData::poetry_svgSize));
    builtInImages.set(Icons::annotation, BuiltInImageData(BinaryData::quote_svg, BinaryData::quote_svgSize));
    builtInImages.set(Icons::stack, BuiltInImageData(BinaryData::arpeggiator_svg, BinaryData::arpeggiator_svgSize));
    builtInImages.set(Icons::switcher, BuiltInImageData(BinaryData::switch_svg, BinaryData::switch_svgSize));
    
    builtInImages.set(Icons::instrumentGraph, BuiltInImageData(BinaryData::waveform_svg, BinaryData::waveform_svgSize));
    builtInImages.set(Icons::instrumentSettings, BuiltInImageData(BinaryData::waveform_svg, BinaryData::waveform_svgSize));
    
    builtInImages.set(Icons::toggleOn, BuiltInImageData(BinaryData::toggleon_svg, BinaryData::toggleon_svgSize));
    builtInImages.set(Icons::toggleOff, BuiltInImageData(BinaryData::toggleoff_svg, BinaryData::toggleoff_svgSize));

    builtInImages.set(Icons::roman1, BuiltInImageData(BinaryData::roman1_svg, BinaryData::roman1_svgSize));
    builtInImages.set(Icons::roman2, BuiltInImageData(BinaryData::roman2_svg, BinaryData::roman2_svgSize));
    builtInImages.set(Icons::roman3, BuiltInImageData(BinaryData::roman3_svg, BinaryData::roman3_svgSize));
    builtInImages.set(Icons::roman4, BuiltInImageData(BinaryData::roman4_svg, BinaryData::roman4_svgSize));
    builtInImages.set(Icons::roman5, BuiltInImageData(BinaryData::roman5_svg, BinaryData::roman5_svgSize));
    builtInImages.set(Icons::roman6, BuiltInImageData(BinaryData::roman6_svg, BinaryData::roman6_svgSize));
    builtInImages.set(Icons::roman7, BuiltInImageData(BinaryData::roman7_svg, BinaryData::roman7_svgSize));
}

static const Path extractPathFromDrawable(const Drawable *d)
{
    for (int i = 0; i < d->getNumChildComponents(); ++i)
    {
        Component *child = d->getChildComponent(i);
        
        if (DrawablePath *dp = dynamic_cast<DrawablePath *>(child))
        {
            Path p(dp->getPath());
            return p;
        }
        
        if (DrawableComposite *dc = dynamic_cast<DrawableComposite *>(child))
        {
            for (int j = 0; j < dc->getNumChildComponents(); ++j)
            {
                Component *compChild = dc->getChildComponent(i);
                
                if (DrawablePath *dp = dynamic_cast<DrawablePath *>(compChild))
                {
                    Path p(dp->getPath());
                    return p;
                }
            }
        }
    }
    
    return Path();
}

static Image renderVector(const String &name, int maxSize,
    const Colour &iconBaseColour, const Colour &iconShadeColour)
{
    if (! builtInImages.contains(name) || maxSize < 1)
    {
        return Image(Image::ARGB, 1, 1, true);
    }
    
    Image resultImage(Image::ARGB, maxSize, maxSize, true);
    Graphics g(resultImage);
    
    ScopedPointer<Drawable> drawableSVG(Drawable::createFromImageData(builtInImages[name].data, builtInImages[name].numBytes));
    drawableSVG->replaceColour(Colours::black, iconBaseColour);

    Rectangle<int> area(0, 0, maxSize, maxSize);
    drawableSVG->drawWithin(g, area.toFloat(), RectanglePlacement::centred, 1.0f);
    
    if (name != Icons::workspace) // a hack -_-
    {

#if HELIO_DESKTOP
        GlowEffect glow;
        glow.setGlowProperties(1.25, iconShadeColour);
        glow.applyEffect(resultImage, g, 1.f, 1.f);
#endif
        
        drawableSVG->drawWithin(g, area.toFloat(), RectanglePlacement::centred, 1.0f);
    }
    
    //g.fillAll(Colours::red.withAlpha(0.1f));
    
    //Path iconPath(extractPathFromDrawable(drawableSVG));
    //g.setColour(Colours::black.withAlpha(0.3f));
    //g.strokePath(iconPath, PathStrokeType(0.5f), iconPath.getTransformToScaleToFit(area.toFloat(), true));
    
    return resultImage;
}


ScopedPointer<Drawable> Icons::getDrawableByName(const String &name)
{
    if (!builtInImages.contains(name))
    {
        return nullptr;
    }
    
    return Drawable::createFromImageData(builtInImages[name].data, builtInImages[name].numBytes);
}

Path Icons::getPathByName(const String &name)
{
    if (!builtInImages.contains(name))
    {
        return Path();
    }

    ScopedPointer<Drawable> drawableSVG(Drawable::createFromImageData(builtInImages[name].data, builtInImages[name].numBytes));
    return Path(extractPathFromDrawable(drawableSVG));
}

static HashMap<String, Image> prerenderedVectors;

void Icons::clearPrerenderedCache()
{
    prerenderedVectors.clear();
}

const int kRoundFactor = 8;

Image Icons::findByName(const String &name, int maxSize)
{
    const Desktop::Displays::Display &dis = Desktop::getInstance().getDisplays().getMainDisplay();

#if JUCE_ANDROID
    const int retinaFactor = 2;
#else
    const int retinaFactor = int(dis.scale);
#endif

    const int fixedSize = int(floorf(float(maxSize) / float(kRoundFactor))) * kRoundFactor * retinaFactor;
    const String nameKey = name + "@" + String(fixedSize);
    
    if (prerenderedVectors.contains(nameKey))
    {
        return prerenderedVectors[nameKey];
    }
    
    const Colour iconBaseColour(App::Helio().getTheme()->findColour(ColourIDs::Icons::fill));
    const Colour iconShadeColour(App::Helio().getTheme()->findColour(ColourIDs::Icons::shadow));
    Image prerenderedImage = renderVector(name, fixedSize, iconBaseColour, iconShadeColour);
    prerenderedVectors.set(nameKey, prerenderedImage);

    return prerenderedImage;
}

Image Icons::findByName(const String &name, int maxSize, LookAndFeel &lf)
{
    const Desktop::Displays::Display &dis = Desktop::getInstance().getDisplays().getMainDisplay();

#if JUCE_ANDROID
    const int retinaFactor = 2;
#else
    const int retinaFactor = int(dis.scale);
#endif

    const int fixedSize = int(floorf(float(maxSize) / float(kRoundFactor))) * kRoundFactor * retinaFactor;

    const Colour iconBaseColour(lf.findColour(ColourIDs::Icons::fill));
    const Colour iconShadeColour(lf.findColour(ColourIDs::Icons::shadow));
    Image prerenderedImage = renderVector(name, fixedSize, iconBaseColour, iconShadeColour);
    return prerenderedImage;
}

void Icons::drawImageRetinaAware(const Image &image, Graphics &g, int cx, int cy)
{
//#if JUCE_IOS || JUCE_MAC
    const Desktop::Displays::Display &dis = Desktop::getInstance().getDisplays().getMainDisplay();

#if JUCE_ANDROID
    const int scale = 2;
#else
    const int scale = int(dis.scale);
#endif

    const int w = image.getWidth();
    const int h = image.getHeight();

    if (scale > 1)
    {
        //Logger::writeToLog(String(x) + ":" + String(y));

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
//#else
//        g.drawImageAt(image, cx - int(w / 2), cy - int(h / 2));
//#endif
}
