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
#include "HelioTheme.h"
#include "MainWindow.h"
#include "NavigationSidebar.h"
#include "BinaryData.h"
#include "Icons.h"
#include "ColourIDs.h"

#include "MainWindow.h"
#include "HybridRoll.h"
#include "NavigationSidebar.h"
#include "InstrumentEditor.h"
#include "HelperRectangle.h"
#include "PianoRoll.h"
#include "PatternRoll.h"

#include "HelioCallout.h"
#include "SelectionComponent.h"
#include "PanelBackgroundA.h"
#include "PanelBackgroundB.h"
#include "PanelBackgroundC.h"
#include "FramePanel.h"
#include "TrackScroller.h"
#include "App.h"

#include "BinaryData.h"
#include "ColourScheme.h"

#if HELIO_DESKTOP
#   define SCROLLBAR_WIDTH (17)
#   define POPUP_MENU_FONT (18.f)
#   define TEXTBUTTON_FONT (21.f)
#elif HELIO_MOBILE
#   define SCROLLBAR_WIDTH (50)
#   define POPUP_MENU_FONT (23.f)
#   define TEXTBUTTON_FONT (23.f)
#endif

HelioTheme::HelioTheme() :
    backgroundNoise(ImageCache::getFromMemory(BinaryData::Noise_png, BinaryData::Noise_pngSize)) {}

void HelioTheme::drawNoise(Component *target, Graphics &g, float alphaMultiply /*= 1.f*/)
{
    const float alpha = JUCE_LIVE_CONSTANT(0.0175f);
    g.setTiledImageFill(static_cast<HelioTheme &>(target->getLookAndFeel()).getBackgroundNoise(), 0, 0, alpha * alphaMultiply);
    g.fillRect(0, 0, target->getWidth(), target->getHeight());
}

void HelioTheme::drawNoise(const HelioTheme &theme, Graphics &g, float alphaMultiply /*= 1.f*/)
{
    const float alpha = (0.0175f);
    g.setTiledImageFill(theme.getBackgroundNoise(), 0, 0, alpha * alphaMultiply);
    g.fillRect(0, 0, g.getClipBounds().getWidth(), g.getClipBounds().getHeight());
}

void HelioTheme::drawNoiseWithin(Rectangle<float> bounds, Component *target, Graphics &g, float alphaMultiply /*= 1.f*/)
{
    const float alpha = (0.0175f);
    g.setTiledImageFill(static_cast<HelioTheme &>(target->getLookAndFeel()).getBackgroundNoise(), 0, 0, alpha * alphaMultiply);
    g.fillRect(bounds);
}

void HelioTheme::drawDashedRectangle(Graphics &g, const Rectangle<float> &r, const Colour &colour,
                                float dashLength, float spaceLength, float dashThickness, float cornerRadius)
{
    g.setColour(colour);
    
    Path path;
    path.addQuadrilateral(r.getBottomRight().getX(), r.getBottomRight().getY(),
                          r.getTopRight().getX(), r.getTopRight().getY(),
                          r.getTopLeft().getX(), r.getTopLeft().getY(),
                          r.getBottomLeft().getX(), r.getBottomLeft().getY());
    
    path = path.createPathWithRoundedCorners(cornerRadius);
    
    Array<float> dashes;
    dashes.add(dashLength);
    dashes.add(spaceLength);
    PathStrokeType(dashThickness, PathStrokeType::mitered, PathStrokeType::rounded)
    .createDashedStroke(path, path, dashes.getRawDataPointer(), dashes.size());
    
    g.strokePath(path, PathStrokeType(dashThickness));
}

Typeface::Ptr HelioTheme::getTypefaceForFont(const Font &font)
{
    if (font.getTypefaceName() == Font::getDefaultSansSerifFontName() ||
        font.getTypefaceName() == Font::getDefaultSerifFontName())
    {
        return this->getTextTypeface();
    }

    return Font::getDefaultTypefaceForFont(font);
}

void HelioTheme::drawStretchableLayoutResizerBar(Graphics &g,
        int w, int h, bool isVerticalBar,
        bool isMouseOver, bool isMouseDragging) {}

//===----------------------------------------------------------------------===//
// Text Editor
//===----------------------------------------------------------------------===//

void HelioTheme::fillTextEditorBackground(Graphics &g, int w, int h, TextEditor &ed)
{
    g.setColour(ed.findColour(TextEditor::backgroundColourId));
    g.fillRect(0, 0, w, h);
}

void HelioTheme::drawTextEditorOutline(Graphics &g, int w, int h, TextEditor &ed)
{
    g.setColour(ed.findColour(TextEditor::outlineColourId));
    g.drawVerticalLine(0, 1.f, h - 1.f);
    g.drawVerticalLine(w - 1, 1.f, h - 1.f);
    g.drawHorizontalLine(0, 1.f, w - 1.f);
    g.drawHorizontalLine(h - 1, 1.f, w - 1.f);
}

//===----------------------------------------------------------------------===//
// Selection
//===----------------------------------------------------------------------===//

void HelioTheme::drawLasso(Graphics &g, Component &lassoComp)
{
#if HELIO_DESKTOP
    const float dashWidth = 1.f;
    const float cornersRound = 5.f;
    const float dashLength = 5.f;
#elif HELIO_MOBILE
    const float dashWidth = 2.f;
    const float cornersRound = 10.f;
    const float dashLength = 7.f;
#endif
    
    const Rectangle<float> r(0.5f, 0.5f,
        float(lassoComp.getLocalBounds().getWidth()) - 1.0f,
        float(lassoComp.getLocalBounds().getHeight()) - 1.0f);
    
    g.setColour(lassoComp.findColour(ColourIDs::SelectionComponent::fill));
    g.fillRoundedRectangle(lassoComp.getLocalBounds().toFloat(), cornersRound);
    
    g.setColour(lassoComp.findColour(ColourIDs::SelectionComponent::outline));
    
    Path path;
    path.addQuadrilateral(r.getBottomRight().getX(), r.getBottomRight().getY(),
        r.getTopRight().getX(), r.getTopRight().getY(),
        r.getTopLeft().getX(), r.getTopLeft().getY(),
        r.getBottomLeft().getX(), r.getBottomLeft().getY());
    
    path = path.createPathWithRoundedCorners(cornersRound);
    
    Array<float> dashes;
    dashes.add(dashLength);
    dashes.add(dashLength);

    PathStrokeType(dashWidth).createDashedStroke(path, path, dashes.getRawDataPointer(), dashes.size());
    g.strokePath(path, PathStrokeType(dashWidth));
}


//===----------------------------------------------------------------------===//
// Labels
//===----------------------------------------------------------------------===//

Font HelioTheme::getLabelFont(Label &label)
{
    return label.getFont();
}

void HelioTheme::drawLabel(Graphics &g, Label &label)
{
    this->drawLabel(g, label, 0);
}

#define SMOOTH_RENDERED_FONT 1

// Label rendering is one of the most time-consuming bottlenecks in the app.
// Calling this method too often should be avoided at any costs:
// relatively small labels that are always on the screen (line headline titles,
// annotations, key/time signatures, track headers) should be set cached to images,
// and they also should have fixed sizes (not dependent on a container component's size)
// so that resizing a parent won't force resizing, re-rendering and re-caching a label.
// Such labels should be re-rendered only when their content changes.

// TODO test on retina screens

void HelioTheme::drawLabel(Graphics &g, Label &label, juce_wchar passwordCharacter)
{
    if (! label.isBeingEdited())
    {
        //Logger::writeToLog("drawLabel: " + label.getText());

        const Font font(this->getLabelFont(label));
        const String textToDraw = (passwordCharacter != 0) ?
            String::repeatedString(String::charToString(passwordCharacter), label.getText().length()) :
            label.getText();

        // For large labels assume this is a dialog text,
        // for every other force fit text into a single line:
        const int maxLines = (label.getHeight() < 64) ? 1 : 10;

        const float alpha = 0.5f + jlimit(0.f, 1.f, (font.getHeight() - 8.f) / 12.f) / 2.f;
        const Colour colour = label.findColour(Label::textColourId).withMultipliedAlpha(alpha);

#if SMOOTH_RENDERED_FONT

        Path textPath;
        GlyphArrangement glyphs;

        const Rectangle<float> textArea =
            label.getBorderSize().subtractedFrom(label.getLocalBounds()).toFloat();

        glyphs.addFittedText(font,
            textToDraw,
            textArea.getX(),
            textArea.getY(),
            textArea.getWidth(),
            textArea.getHeight(),
            label.getJustificationType(),
            maxLines,
            1.0);

        glyphs.createPath(textPath);

        g.setColour(colour);
        g.fillPath(textPath);

#else

        const Rectangle<int> textArea =
            label.getBorderSize().subtractedFrom(label.getLocalBounds());

        g.setFont(font);
        g.setColour(colour);
        g.drawFittedText(textToDraw,
            textArea.getX(), textArea.getY(),
            textArea.getWidth(), textArea.getHeight(),
            label.getJustificationType(),
            maxLines, 1.0);

#endif
    }
    else if (label.isEnabled())
    {
        g.setColour(label.findColour(Label::outlineColourId));
    }
}

//===----------------------------------------------------------------------===//
// Button
//===----------------------------------------------------------------------===//

Font HelioTheme::getTextButtonFont(TextButton &button, int buttonHeight)
{
    return Font(Font::getDefaultSerifFontName(), jmin(float(TEXTBUTTON_FONT), float(buttonHeight * 0.75f)), Font::plain);
}

void HelioTheme::drawButtonText(Graphics &g, TextButton &button,
                                bool isMouseOverButton, bool isButtonDown)
{
    Font font(getTextButtonFont(button, button.getHeight()));
    g.setFont(font);
    g.setColour(button.findColour(TextButton::buttonColourId).contrasting().withMultipliedAlpha(button.isEnabled() ? 0.85f : 0.5f));

    Image img(Icons::findByName(button.getName(), int(button.getHeight() / 1.5f)));

    const int cX = button.getButtonText().isEmpty() ? (button.getWidth() / 2) : (button.getWidth() / 4);
    const int cY = (button.getHeight() / 2);
    Icons::drawImageRetinaAware(img, g, cX, cY);

    const bool hasImg = (img.getWidth() > 2);

    const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
    const int yHeight = (button.getHeight() - (yIndent * 2));
    const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = roundToInt(font.getHeight() * 0.5f);
    const int leftIndent  = hasImg ? button.getWidth() / 3 : fontHeight;
    const int rightIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));

    g.setColour(button.findColour(TextButton::textColourOnId).withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

    g.drawFittedText(button.getButtonText(),
                     leftIndent,
                     yIndent,
                     button.getWidth() - leftIndent - rightIndent,
                     yHeight,
                     Justification::centred, 4, 1.f);
}

void HelioTheme::drawButtonBackground(Graphics &g, Button &button,
                                      const Colour &backgroundColour,
                                      bool isMouseOverButton, bool isButtonDown)
{
    Colour baseColour(backgroundColour
                      .withMultipliedAlpha(button.isEnabled() ? 0.75f : 0.3f));

    const bool flatOnLeft   = button.isConnectedOnLeft();
    const bool flatOnRight  = button.isConnectedOnRight();
    const bool flatOnTop    = button.isConnectedOnTop();
    const bool flatOnBottom = button.isConnectedOnBottom();

    const float width  = float(button.getWidth());
    const float height = float(button.getHeight());

    if (width > 0 && height > 0)
    {
        const float cornerSize = 7.f;

        Path outline;
        outline.addRoundedRectangle(0.f, -cornerSize, width, height + cornerSize, cornerSize, cornerSize,
                                    !(flatOnLeft  || flatOnTop),
                                    !(flatOnRight || flatOnTop),
                                    !(flatOnLeft  || flatOnBottom),
                                    !(flatOnRight || flatOnBottom));

        const float mainBrightness = baseColour.getBrightness();
        const float mainAlpha = baseColour.getFloatAlpha();

        g.setGradientFill(ColourGradient(baseColour.darker(0.1f), 0.0f, height / 2 - 2,
                                         baseColour.darker(0.2f), 0.0f, height / 2 + 2, false));
        //g.setColour(baseColour.darker(0.2f));
        g.fillPath(outline);

        if (isButtonDown || isMouseOverButton)
        {
            g.setColour(baseColour.brighter(isButtonDown ? 0.1f : 0.01f));
            g.fillPath(outline);
        }

        g.setColour(Colours::white.withAlpha(mainAlpha));
        g.strokePath(outline, PathStrokeType(1.0f));

        g.setColour(Colours::black.withAlpha(0.5f * mainAlpha));
        g.strokePath(outline, PathStrokeType(1.0f),
                     AffineTransform::translation(0.0f, 1.0f).scaled(1.0f, (height + 2.0f) / height));
    }
}

//===----------------------------------------------------------------------===//
// Scrollbars
//===----------------------------------------------------------------------===//

int HelioTheme::getDefaultScrollbarWidth()
{
    return SCROLLBAR_WIDTH;
}

void HelioTheme::drawScrollbar(Graphics &g, ScrollBar &scrollbar,
                               int x, int y, int width, int height,
                               bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                               bool isMouseOver, bool isMouseDown)
{
    Path thumbPath;

    if (thumbSize > 0)
    {
        const float thumbIndent = (isScrollbarVertical ? width : height) * 0.25f;
        const float thumbIndentx2 = thumbIndent * 2.0f;

        if (isScrollbarVertical)
        {
            thumbPath.addRoundedRectangle(x + thumbIndent, thumbStartPosition + thumbIndent,
                                          width - thumbIndentx2, thumbSize - thumbIndentx2,
                                          (width - thumbIndentx2) * 0.5f);
        }
        else
        {
            thumbPath.addRoundedRectangle(thumbStartPosition + thumbIndent, y + thumbIndent,
                                          thumbSize - thumbIndentx2, height - thumbIndentx2,
                                          (height - thumbIndentx2) * 0.5f);
        }
    }

    Colour thumbCol(scrollbar.findColour(ScrollBar::thumbColourId, true));

    if (isMouseOver || isMouseDown)
    { thumbCol = thumbCol.withMultipliedAlpha(0.5f); }
    else
    { thumbCol = thumbCol.withMultipliedAlpha(0.1f); }

    g.setColour(thumbCol);
    g.fillPath(thumbPath);

    g.setColour(thumbCol.contrasting((isMouseOver  || isMouseDown) ? 0.2f : 0.1f));
    g.strokePath(thumbPath, PathStrokeType(1.0f));
}


//===----------------------------------------------------------------------===//
// Menus
//===----------------------------------------------------------------------===//

Font HelioTheme::getPopupMenuFont()
{
    return Font(Font(Font::getDefaultSansSerifFontName(), POPUP_MENU_FONT, Font::plain));
}

void HelioTheme::drawPopupMenuBackground(Graphics &g, int width, int height)
{
    g.setColour(findColour(TextEditor::backgroundColourId));
    g.fillRect(0, 0, width, height);
    
    g.setColour(findColour(TextEditor::outlineColourId));
    g.drawRect(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 1.000f);
}

void HelioTheme::drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
                                   const bool isSeparator, const bool isActive,
                                   const bool isHighlighted, const bool isTicked,
                                   const bool hasSubMenu, const String& text,
                                   const String& shortcutKeyText,
                                   const Drawable* icon, const Colour* const textColourToUse)
{
    if (isSeparator)
    {
        Rectangle<int> r (area.reduced (5, 0));
        r.removeFromTop (r.getHeight() / 2 - 1);
        
        g.setColour (Colour (0x33000000));
        g.fillRect (r.removeFromTop (1));
        
        g.setColour (Colour (0x66ffffff));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        Colour textColour (findColour (PopupMenu::textColourId));
        
        if (textColourToUse != nullptr)
            textColour = *textColourToUse;
        
        Rectangle<int> r (area.reduced (1));
        
        if (isHighlighted)
        {
            g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
            g.fillRect (r);
            
            g.setColour (findColour (PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour);
        }
        
        if (! isActive)
            g.setOpacity (0.3f);
        
        Font font (getPopupMenuFont());
        
        const float maxFontHeight = area.getHeight() / 1.3f;
        
        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);
        
        g.setFont (font);
        
        Rectangle<float> iconArea (r.removeFromLeft ((r.getHeight() * 5) / 4).reduced (3).toFloat());
        
        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
        }
        else if (isTicked)
        {
            //const Path tick (getTickShape (1.0f));
            //g.fillPath (tick, tick.getTransformToScaleToFit (iconArea, true));
        }
        
        if (hasSubMenu)
        {
            const float arrowH = 0.6f * getPopupMenuFont().getAscent();
            
            const float x = (float) r.removeFromRight ((int) arrowH).getX();
            const float halfH = (float) r.getCentreY();
            
            Path p;
            p.addTriangle (x, halfH - arrowH * 0.5f,
                           x, halfH + arrowH * 0.5f,
                           x + arrowH * 0.6f, halfH);
            
            g.fillPath (p);
        }
        
        r.removeFromRight (3);
        g.drawFittedText (text, r, Justification::centredLeft, 1);
        
        if (shortcutKeyText.isNotEmpty())
        {
            Font f2 (font);
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);
            
            g.drawText (shortcutKeyText, r, Justification::centredRight, true);
        }
    }
}


//===----------------------------------------------------------------------===//
// Window
//===----------------------------------------------------------------------===//

void HelioTheme::drawCornerResizer(Graphics& g, int w, int h,
    bool /*isMouseOver*/, bool /*isMouseDragging*/)
{
    const float lineThickness = jmin(w, h) * 0.05f;
    const Colour col1(this->findColour(ResizableWindow::backgroundColourId).darker(0.2f));
    const Colour col2(this->findColour(ResizableWindow::backgroundColourId).brighter(0.04f));

    for (float i = 0.8f; i > 0.2f; i -= 0.25f)
    {
        g.setColour(col1);

        g.drawLine(w * i,
            h + 1.0f,
            w + 1.0f,
            h * i,
            lineThickness);

        g.setColour(col2);

        g.drawLine(w * i + lineThickness,
            h + 1.0f,
            w + 1.0f,
            h * i + lineThickness,
            lineThickness);
    }
}

void HelioTheme::drawResizableFrame(Graphics &g, int w, int h, const BorderSize<int> &border)
{
    if (!border.isEmpty())
    {
        const Rectangle<int> fullSize(0, 0, w, h);
        const Rectangle<int> centreArea(border.subtractedFrom(fullSize));

        g.saveState();

        g.excludeClipRegion(centreArea);

        g.setColour(this->findColour(ResizableWindow::backgroundColourId));
        g.drawRect(fullSize, 5);

        g.restoreState();
    }
}

class HelioWindowButton : public Button
{
public:

    HelioWindowButton(const String& name, Colour col, const Path& normalShape_, const Path& toggledShape_) noexcept
        : Button(name),
        colour(col),
        normalShape(normalShape_),
        toggledShape(toggledShape_)
    {
    }

    void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        float alpha = isMouseOverButton ? (isButtonDown ? 1.0f : 0.8f) : 0.6f;

        if (!isEnabled())
            alpha *= 0.5f;

        float x = 0, y = 0, diam;

        if (getWidth() < getHeight())
        {
            diam = (float)getWidth();
            y = (getHeight() - getWidth()) * 0.5f;
        }
        else
        {
            diam = (float)getHeight();
            y = (getWidth() - getHeight()) * 0.5f;
        }

        x += diam * 0.05f;
        y += diam * 0.05f;
        diam *= 0.9f;

        if (isMouseOverButton)
        {
            g.setColour(Colours::white.withAlpha(alpha * 0.05f));
            //g.fillEllipse(x + 0.5f, y + 0.5f, diam - 1.f, diam - 1.f);
            g.fillAll();
        }

        Path& p = getToggleState() ? toggledShape : normalShape;

        const AffineTransform t(p.getTransformToScaleToFit(x + diam * 0.3f, y + diam * 0.3f,
            diam * 0.4f, diam * 0.4f, true));

        g.setColour(Colours::white.withAlpha(alpha * 0.75f));
        g.fillPath(p, t);
    }

private:

    Colour colour;
    Path normalShape, toggledShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelioWindowButton)
};

void HelioTheme::drawDocumentWindowTitleBar(DocumentWindow &window,
        Graphics &g, int w, int h,
        int titleSpaceX, int titleSpaceW,
        const Image *icon,
        bool drawTitleTextOnLeft)
{
#if HELIO_DESKTOP
    const Colour bgColour = this->findColour(ResizableWindow::backgroundColourId);

    g.setColour(bgColour);
    g.fillAll();

    HelioTheme::drawNoise(*this, g, 1.f);

    g.setColour(Colour(0x32000000));
    g.drawHorizontalLine(h - 2, 0.f, float(w));

    g.setColour(Colour(0x0cffffff));
    g.drawHorizontalLine(h - 1, 0.f, float(w));

    Font font(16.f, Font::plain);
    g.setFont(font);
    g.setColour(Colours::white.withAlpha(0.125f));

    String title = "Helio " + App::getAppReadableVersion();

#if JUCE_64BIT
    title += " (64-bit)";
#elif JUCE_32BIT
    title += " (32-bit)";
#endif

    const int textW = font.getStringWidth(title);
    g.drawText(title, titleSpaceX + 2, 0, textW, h - 2, Justification::centredLeft, true);
#endif
}


Button *HelioTheme::createDocumentWindowButton(int buttonType)
{
    Path shape;

    if (buttonType == DocumentWindow::closeButton)
    {
        shape.addLineSegment(Line<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.05f);
        shape.addLineSegment(Line<float>(1.0f, 0.0f, 0.0f, 1.0f), 0.05f);
        return new HelioWindowButton("close", Colour(0xffdd1100), shape, shape);
    }
    if (buttonType == DocumentWindow::minimiseButton)
    {
        shape.addLineSegment(Line<float>(0.0f, 0.0f, 0.0001f, 0.0f), 0.0001f);
        shape.addLineSegment(Line<float>(0.0f, 0.5f, 1.0f, 0.5f), 0.05f);
        return new HelioWindowButton("min", Colour(0xffdd1100), shape, shape);
    }
    else if (buttonType == DocumentWindow::maximiseButton)
    {
        shape.addLineSegment(Line<float>(0.0f, 0.0f, 0.0f, 0.8f), 0.05f);
        shape.addLineSegment(Line<float>(0.0f, 0.8f, 1.0f, 0.8f), 0.05f);
        shape.addLineSegment(Line<float>(1.0f, 0.8f, 1.0f, 0.0f), 0.05f);
        shape.addLineSegment(Line<float>(1.0f, 0.0f, 0.0f, 0.0f), 0.05f);
        return new HelioWindowButton("max", Colour(0xffdd1100), shape, shape);
    }

    jassertfalse;
    return nullptr;
}


void HelioTheme::positionDocumentWindowButtons(DocumentWindow &,
        int titleBarX,
        int titleBarY,
        int titleBarW,
        int titleBarH,
        Button *minimiseButton,
        Button *maximiseButton,
        Button *closeButton,
        bool positionTitleBarButtonsOnLeft)
{
    const int buttonW = titleBarH - titleBarH / 8;
    int x = titleBarX + titleBarW - buttonW - buttonW / 4;

    if (closeButton != nullptr)
    {
        closeButton->setBounds(x, titleBarY, buttonW, buttonW);
        x += -(buttonW + buttonW / 4);
    }

    if (maximiseButton != nullptr)
    {
        maximiseButton->setBounds(x, titleBarY, buttonW, buttonW);
        x += -(buttonW + buttonW / 4);
    }

    if (minimiseButton != nullptr)
        minimiseButton->setBounds(x, titleBarY, buttonW, buttonW);
}


void HelioTheme::initResources()
{
//    // Search for Lato if present,
//    // Or fallback to default sans serif font
//    
//    Logger::writeToLog("Fonts search started");
//    Array <Font> systemFonts;
//    Font::findFonts(systemFonts);
//    
//    for (auto && systemFont : systemFonts)
//    {
//        if (systemFont.getTypeface()->getName().toLowerCase().startsWith("lato"))
//        {
//            Logger::writeToLog("Found " + systemFont.getTypeface()->getName());
//            Font font(systemFont);
//            this->textTypefaceCache = Typeface::createSystemTypefaceFor(font);
//        }
//    }
//    
//    if (this->textTypefaceCache == nullptr)
//    {
//        // Verdana on win32, Bitstream Vera Sans or something on Linux,
//        // Lucida Grande on mac, Helvetica on iOS
//        Logger::writeToLog("Falling back to system sans serif");
//        this->textTypefaceCache =
//        Font::getDefaultTypefaceForFont(Font(Font::getDefaultSansSerifFontName(), 0, 0));
//        Logger::writeToLog("Done");
//    }
    
//    MemoryInputStream is(BinaryData::comfortaa_font, BinaryData::comfortaa_fontSize, false);
//    this->textTypefaceCache = (new CustomTypeface(is));

    MemoryInputStream is(BinaryData::lato_fnt, BinaryData::lato_fntSize, false);
    this->textTypefaceCache = (new CustomTypeface(is));

    Icons::setupBuiltInImages();
}

void HelioTheme::initColours(const ::ColourScheme::Ptr s)
{
    // JUCE component colour id's:

    // Sliders
    this->setColour(Slider::rotarySliderOutlineColourId, Colours::transparentBlack);
    this->setColour(Slider::rotarySliderFillColourId, s->getTextColour());

    // Labels
    this->setColour(Label::textColourId, s->getTextColour());
    this->setColour(Label::outlineColourId, s->getTextColour().contrasting());

    // MainWindow
    this->setColour(ResizableWindow::backgroundColourId, s->getPrimaryGradientColourA().brighter(0.045f));
    this->setColour(ScrollBar::backgroundColourId, Colours::transparentBlack);
    this->setColour(ScrollBar::thumbColourId, s->getPanelFillColour().withAlpha(1.f));

    // TextButton
    this->setColour(TextButton::buttonColourId, s->getPanelFillColour());
    this->setColour(TextButton::buttonOnColourId, s->getPanelBorderColour());
    this->setColour(TextButton::textColourOnId, s->getTextColour().withMultipliedAlpha(0.75f));
    this->setColour(TextButton::textColourOffId, s->getTextColour().withMultipliedAlpha(0.5f));

    // PopupMenu
    this->setColour(PopupMenu::backgroundColourId, s->getSecondaryGradientColourA().darker(0.05f));
    this->setColour(PopupMenu::highlightedBackgroundColourId, s->getSecondaryGradientColourA());
    this->setColour(PopupMenu::textColourId, s->getTextColour());
    this->setColour(PopupMenu::highlightedTextColourId, s->getTextColour().brighter(0.1f));

    // TextEditor
    this->setColour(TextEditor::textColourId, s->getTextColour());
    this->setColour(TextEditor::highlightedTextColourId, s->getTextColour());
    this->setColour(TextEditor::outlineColourId, s->getPanelBorderColour().withAlpha(0.1f));
    this->setColour(TextEditor::focusedOutlineColourId, Colours::black.withAlpha(0.2f));
    this->setColour(TextEditor::shadowColourId, s->getPrimaryGradientColourA().darker(0.05f));
    this->setColour(TextEditor::backgroundColourId, s->getPrimaryGradientColourA().darker(0.05f));
    this->setColour(TextEditor::highlightColourId, Colours::black.withAlpha(0.25f));
    this->setColour(CaretComponent::caretColourId, Colours::white.withAlpha(0.35f));

    // Tree stuff
    this->setColour(TreeView::linesColourId, Colours::white.withAlpha(0.1f));
    this->setColour(TreeView::selectedItemBackgroundColourId, Colours::transparentBlack);
    this->setColour(TreeView::backgroundColourId, Colours::transparentBlack);
    this->setColour(TreeView::dragAndDropIndicatorColourId, Colours::black.withAlpha(0.15f));

    // Lasso
    this->setColour(ColourIDs::SelectionComponent::fill, s->getLassoFillColour().withAlpha(0.15f));
    this->setColour(ColourIDs::SelectionComponent::outline, s->getLassoBorderColour().withAlpha(0.4f));
   
    // Helio colours:

    // A hack for icon base colors
    this->setColour(ColourIDs::Icons::fill, s->getIconBaseColour());
    this->setColour(ColourIDs::Icons::shadow, s->getIconShadowColour());

    // Panels
    this->setColour(ColourIDs::BackgroundA::fillStart, s->getPrimaryGradientColourA());
    this->setColour(ColourIDs::BackgroundA::fillEnd, s->getPrimaryGradientColourB());
    this->setColour(ColourIDs::BackgroundB::fill, s->getPrimaryGradientColourA());
    this->setColour(ColourIDs::BackgroundC::fill, s->getSecondaryGradientColourA());

    this->setColour(ColourIDs::Panel::fill, s->getPanelFillColour());
    this->setColour(ColourIDs::Panel::border, s->getPanelBorderColour().withAlpha(0.225f));

    this->setColour(ColourIDs::TrackScroller::borderLineDark, s->getPrimaryGradientColourA().darker(0.25f));
    this->setColour(ColourIDs::TrackScroller::borderLineLight, Colours::white.withAlpha(0.025f));

    // InstrumentEditor
    this->setColour(ColourIDs::Instrument::midiIn, Colour(0x3f000000));
    this->setColour(ColourIDs::Instrument::midiOut, Colour(0x3f000000));
    this->setColour(ColourIDs::Instrument::audioIn, Colour(0x25ffffff));
    this->setColour(ColourIDs::Instrument::audioOut, Colour(0x25ffffff));

    // Borders
    this->setColour(ColourIDs::Common::borderLineLight, Colours::white.withAlpha(0.06f));
    this->setColour(ColourIDs::Common::borderLineDark, Colours::black.withAlpha(0.2f));

    // CallOutBox
    this->setColour(ColourIDs::Callout::blur, s->getSecondaryGradientColourB().darker(2.0f).withAlpha(0.35f));
    this->setColour(ColourIDs::Callout::fill, s->getPrimaryGradientColourB().darker(2.0f).withAlpha(0.65f));
    this->setColour(ColourIDs::Callout::frame, s->getPrimaryGradientColourB().brighter(0.75f).withAlpha(0.14f));

    // HybridRoll
    this->setColour(ColourIDs::Roll::blackKey, s->getBlackKeyColour());
    this->setColour(ColourIDs::Roll::blackKeyAlt, s->getBlackKeyColour().withMultipliedBrightness(1.15f));
    this->setColour(ColourIDs::Roll::whiteKey, s->getWhiteKeyColour());
    this->setColour(ColourIDs::Roll::whiteKeyAlt, s->getWhiteKeyColour().withMultipliedBrightness(1.15f));
    this->setColour(ColourIDs::Roll::rowLine, s->getRowColour());
    this->setColour(ColourIDs::Roll::barLine, s->getBarColour().withAlpha(0.9f));
    this->setColour(ColourIDs::Roll::barLineBevel, Colours::white.withAlpha(0.015f));
    this->setColour(ColourIDs::Roll::beatLine, s->getBarColour().withAlpha(0.45f));
    this->setColour(ColourIDs::Roll::snapLine, s->getBarColour().withAlpha(0.1f));
    this->setColour(ColourIDs::Roll::headerFill, s->getPrimaryGradientColourB().darker(0.025f));
    this->setColour(ColourIDs::Roll::headerSnaps, s->getPrimaryGradientColourB().darker(0.025f).contrasting().withMultipliedAlpha(0.35f));
    this->setColour(ColourIDs::Roll::playhead, s->getLassoBorderColour().withAlpha(0.6f));
    this->setColour(ColourIDs::Roll::playheadShade, Colours::black.withAlpha(0.1f));
    this->setColour(ColourIDs::Roll::trackHeaderFill, s->getWhiteKeyColour().darker(0.035f));
    this->setColour(ColourIDs::Roll::trackHeaderBorderLight, Colours::white.withAlpha(0.03f));
    this->setColour(ColourIDs::Roll::trackHeaderBorderDark, Colours::black.withAlpha(0.15f));

    this->setColour(ColourIDs::HelperRectangle::fill, s->getLassoFillColour().withAlpha(0.08f));
    this->setColour(ColourIDs::HelperRectangle::outline, s->getLassoBorderColour().withAlpha(0.3f));
}

void HelioTheme::updateBackgroundRenders(bool force)
{
    if (force)
    {
        Icons::clearPrerenderedCache();
        this->bgCache1 = Image();
        this->bgCache2 = Image();
        this->bgCache3 = Image();
    }

#if PANEL_A_HAS_PRERENDERED_BACKGROUND
    PanelBackgroundA::updateRender(*this);
#endif
    
#if PANEL_B_HAS_PRERENDERED_BACKGROUND
    PanelBackgroundB::updateRender(*this);
#endif
    
#if PANEL_C_HAS_PRERENDERED_BACKGROUND
    PanelBackgroundC::updateRender(*this);
#endif
}
