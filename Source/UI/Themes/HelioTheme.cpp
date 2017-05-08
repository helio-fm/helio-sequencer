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
#include "TreePanel.h"
#include "Origami.h"
#include "BinaryData.h"
#include "Icons.h"

#include "MainWindow.h"
#include "Origami.h"
#include "MidiRoll.h"
#include "TreePanel.h"
#include "InstrumentEditor.h"
#include "HelperRectangle.h"
#include "SizeSwitcherComponent.h"
#include "PianoRoll.h"

#include "HelioCallout.h"
#include "MidiEventComponentLasso.h"
#include "PanelBackgroundA.h"
#include "PanelBackgroundB.h"
#include "PanelBackgroundC.h"
#include "PanelA.h"
#include "PanelB.h"
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
    backgroundNoise(ImageCache::getFromMemory(BinaryData::defaultPattern_png, BinaryData::defaultPattern_pngSize))
{
}

HelioTheme::~HelioTheme()
{
}

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
                          r.getBottomLeft().getX(), r.getBottomLeft().getY()
                          );
    
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
        bool isMouseOver, bool isMouseDragging)
{
    if (isMouseDragging)
    {
        g.setColour(findColour(Origami::resizerMovingShadowColourId));
        g.drawVerticalLine(w - 2, 0.f, float(h));
//        g.fillRect(0, 0, w, h);

        g.setColour(findColour(Origami::resizerMovingLineColourId));
    }
    else /*if (isMouseOver)*/
    {
        g.setColour(findColour(Origami::resizerShadowColourId));
        g.drawVerticalLine(w - 2, 0.f, float(h));
//        g.fillRect(0, 0, w, h);

        g.setColour(findColour(Origami::resizerLineColourId));
    }

    if (w == 2)
    {
        //g.drawVerticalLine(0, 0.f, float(h));
        //g.setColour(findColour(Origami::resizerLineColourId).withMultipliedAlpha(0.5f));
        g.drawVerticalLine(1, 0.f, float(h));
    }
    else
    {
        //g.drawHorizontalLine(0, 0.f, float(w));
        //g.setColour(findColour(Origami::resizerLineColourId).withMultipliedAlpha(0.5f));
        g.drawVerticalLine(0, 0.f, float(h));
    }
}


void HelioTheme::drawComboBox(Graphics &g, int width, int height,
                              bool isButtonDown, int buttonX, int buttonY,
                              int buttonW, int buttonH, ComboBox &box)
{
    g.setColour(box.findColour(ComboBox::backgroundColourId));
    g.fillRoundedRectangle(1.f, 1.f, float(width - 2), float(height - 2), 2.f);

    if (box.isEnabled() && box.hasKeyboardFocus(false))
    {
        g.setColour(box.findColour(ComboBox::buttonColourId));
        g.drawRoundedRectangle(1.f, 1.f, float(width - 2), float(height - 2), 2.0f, 1.0f);
    }
    else
    {
        g.setColour(box.findColour(ComboBox::outlineColourId));
        g.drawRoundedRectangle(1.f, 1.f, float(width - 2), float(height - 2), 2.0f, 1.0f);
    }

    const float outlineThickness = box.isEnabled() ? (isButtonDown ? 1.0f : 0.7f) : 0.7f;

    const bool needsHighlight = box.hasKeyboardFocus(true);
    const Colour baseColour(box.findColour(ComboBox::buttonColourId).darker(needsHighlight ? 0.2f : 0.0f));

    g.setColour(baseColour);
    g.drawRoundedRectangle(buttonX + outlineThickness,
                           buttonY + outlineThickness,
                           buttonW - outlineThickness * 2.0f,
                           buttonH - outlineThickness * 2.0f,
                           2.f, outlineThickness);

    if (box.isEnabled())
    {
        const float arrowX = 0.3f;
        const float arrowH = 0.2f;

        Path p;
        p.addTriangle(buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.4f + arrowH),
                      buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.4f,
                      buttonX + buttonW * arrowX,          buttonY + buttonH * 0.4f);

        g.setColour(box.findColour(ComboBox::arrowColourId));
        g.fillPath(p);
    }
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
    
    Rectangle<float> r(1.0f, 1.0f,
                       float(lassoComp.getLocalBounds().getWidth()) - 2.0f,
                       float(lassoComp.getLocalBounds().getHeight()) - 2.0f);
    
    g.setColour(lassoComp.findColour(0x1000440 /*lassoFillColourId*/));
    g.fillRoundedRectangle(lassoComp.getLocalBounds().toFloat(), cornersRound);
    
    g.setColour(lassoComp.findColour(0x1000441 /*lassoOutlineColourId*/));
    
    //Colour lassoOutlineColourId(lassoComp.findColour(MidiEventComponentLasso::lassoOutlineColourId));
    //HelioTheme::drawDashedRectangle(g, r, lassoOutlineColourId, dashLength, dashWidth, cornersRound);
    
    Path path;
    path.addQuadrilateral(r.getBottomRight().getX(), r.getBottomRight().getY(),
                          r.getTopRight().getX(), r.getTopRight().getY(),
                          r.getTopLeft().getX(), r.getTopLeft().getY(),
                          r.getBottomLeft().getX(), r.getBottomLeft().getY()
                          );
    
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

void HelioTheme::drawLabel(Graphics &g, Label &label, juce_wchar passwordCharacter)
{
    if (! label.isBeingEdited())
    {
        String textToDraw = label.getText();
        
        if (passwordCharacter != 0)
        {
            textToDraw = String::repeatedString(String::charToString(passwordCharacter), label.getText().length());
        }
        
        const float alpha = label.isEnabled() ? 1.0f : 0.5f;
        const Font font(this->getLabelFont(label));

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
                             10,
                             1.0);

        glyphs.createPath(textPath);

        // заголовки будут без обводки, это стремно смотрится
        if (label.getName().contains("outline"))
        {
            g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
            PathStrokeType strokeType(label.getFont().getHeight() / 10.f);
            g.strokePath(textPath, strokeType);
        }

        g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
        g.fillPath(textPath);
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
                      .withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                      .withMultipliedAlpha(button.isEnabled() ? 0.5f : 0.3f));

    if (isButtonDown || isMouseOverButton)
    {
        baseColour = baseColour.withMultipliedAlpha(isButtonDown ? 1.3f : 1.15f);
    }

    //{ baseColour = baseColour.contrasting(isButtonDown ? 0.15f : 0.075f); }
    //{ baseColour = baseColour.withMultipliedBrightness(isButtonDown ? 1.2f : 1.1f); }

    const bool flatOnLeft   = button.isConnectedOnLeft();
    const bool flatOnRight  = button.isConnectedOnRight();
    const bool flatOnTop    = button.isConnectedOnTop();
    const bool flatOnBottom = button.isConnectedOnBottom();

    const float width  = float(button.getWidth());
    const float height = float(button.getHeight());

    if (width > 0 && height > 0)
    {
        //const float cornerSize = width / 4;
        const float cornerSize = 7.f;

        Path outline;
        outline.addRoundedRectangle(0.f, -cornerSize, width, height + cornerSize, cornerSize, cornerSize,
                                    !(flatOnLeft  || flatOnTop),
                                    !(flatOnRight || flatOnTop),
                                    !(flatOnLeft  || flatOnBottom),
                                    !(flatOnRight || flatOnBottom));

        // shape itself
        const float mainBrightness = baseColour.getBrightness();
        const float mainAlpha = baseColour.getFloatAlpha();


        g.setGradientFill(ColourGradient(baseColour.brighter(0.1f), 0.0f, height / 2 - 2,
                                         baseColour.darker(0.15f), 0.0f, height / 2 + 2, false));
        g.fillPath(outline);

        if (isButtonDown || isMouseOverButton)
        {
            g.setGradientFill(ColourGradient(findColour(PanelBackgroundA::panelFillStartId).withMultipliedAlpha(0.5f).withMultipliedBrightness(1.2f),
                                             float((width / 2)), 0,
                                             Colours::transparentBlack,
                                             0.0f, height,
                                             true));
            g.fillPath(outline);
        }

        //g.setColour(Colours::white.withAlpha(0.4f * mainAlpha * mainBrightness * mainBrightness));
        //g.strokePath(outline, PathStrokeType(1.0f),
        //             AffineTransform::translation(0.0f, 1.0f).scaled(1.0f, (height - 1.6f) / height));

        g.setColour(Colours::white.withAlpha(mainAlpha));
        g.strokePath(outline, PathStrokeType(1.0f));

        g.setColour(Colours::black.withAlpha(0.5f * mainAlpha));
        g.strokePath(outline, PathStrokeType(1.0f),
                     AffineTransform::translation(0.0f, 1.0f).scaled(1.0f, (height + 2.0f) / height));
    }
}

void HelioTheme::drawFileBrowserRow(Graphics &g, int width, int height,
                                    const String &filename, Image *icon,
                                    const String &fileSizeDescription,
                                    const String &fileTimeDescription,
                                    const bool isDirectory, const bool isItemSelected,
                                    const int /*itemIndex*/, DirectoryContentsDisplayComponent &dcc)
{
    Component *const fileListComp = dynamic_cast<Component *>(&dcc);

    if (isItemSelected) {
        g.fillAll(fileListComp != nullptr ? fileListComp->findColour(DirectoryContentsDisplayComponent::highlightColourId)
                  : findColour(DirectoryContentsDisplayComponent::highlightColourId));
}

    const int x = 32;
    g.setColour(Colours::black);

    if (icon != nullptr && icon->isValid())
    {
        g.drawImageWithin(*icon, 2, 2, x - 4, height - 4,
                          RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize,
                          false);
    }
    else
    {
        if (const Drawable *d = isDirectory ? getDefaultFolderImage()
                                : getDefaultDocumentFileImage()) {
            d->drawWithin(g, Rectangle<float> (2.0f, 2.0f, x - 4.0f, height - 4.0f),
                          RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
}
    }

    g.setColour(fileListComp != nullptr ? fileListComp->findColour(DirectoryContentsDisplayComponent::textColourId)
                : findColour(DirectoryContentsDisplayComponent::textColourId));
    g.setFont(Font(Font::getDefaultSansSerifFontName(), height * 0.7f, Font::plain));

    if (width > 450 && ! isDirectory)
    {
        const int sizeX = roundToInt(width * 0.7f);
        const int dateX = roundToInt(width * 0.8f);

        g.drawFittedText(filename,
                         x, 0, sizeX - x, height,
                         Justification::centredLeft, 1);

        g.setFont(Font(Font::getDefaultSansSerifFontName(), height * 0.6f, Font::plain));

        if (! isDirectory)
        {
            g.drawFittedText(fileSizeDescription,
                             sizeX, 0, dateX - sizeX - 8, height,
                             Justification::centredRight, 1);

            g.drawFittedText(fileTimeDescription,
                             dateX, 0, width - 8 - dateX, height,
                             Justification::centredRight, 1);
        }
    }
    else
    {
        g.drawFittedText(filename,
                         x, 0, width - x, height,
                         Justification::centredLeft, 1);

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
    g.setColour(findColour(ComboBox::backgroundColourId));
    g.fillRect(0, 0, width, height);
    
    g.setColour(findColour(ComboBox::outlineColourId));
    g.drawRect(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 1.000f);
}


//===----------------------------------------------------------------------===//
// Window
//===----------------------------------------------------------------------===//

void HelioTheme::drawCornerResizer(Graphics& g, int w, int h,
    bool /*isMouseOver*/, bool /*isMouseDragging*/)
{
    const float lineThickness = jmin(w, h) * 0.05f;
    const Colour col1(this->findColour(ResizableWindow::backgroundColourId).darker(0.2));
    const Colour col2(this->findColour(ResizableWindow::backgroundColourId).brighter(0.04));

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

void HelioTheme::initColours(const ColourScheme &colours)
{
    // A hack for icon base colors
    this->setColour(Icons::iconColourId, colours.getIconBaseColour());
    this->setColour(Icons::iconShadowColourId, colours.getIconShadowColour());

    // Sliders
    this->setColour(Slider::rotarySliderOutlineColourId, Colours::transparentBlack);
    this->setColour(Slider::rotarySliderFillColourId, colours.getTextColour());

    // Labels
    this->setColour(Label::textColourId, colours.getTextColour());
    this->setColour(Label::outlineColourId, colours.getTextColour().contrasting());

    // Panels
    this->setColour(PanelBackgroundA::panelFillStartId, colours.getPrimaryGradientColourA());
    this->setColour(PanelBackgroundA::panelFillEndId, colours.getPrimaryGradientColourB());
    this->setColour(PanelBackgroundA::panelShadeStartId, colours.getShadingGradientColourA());
    this->setColour(PanelBackgroundA::panelShadeEndId, colours.getShadingGradientColourB());

    this->setColour(PanelBackgroundB::panelFillStartId, colours.getPrimaryGradientColourA());
    this->setColour(PanelBackgroundB::panelFillEndId, colours.getPrimaryGradientColourB());
    this->setColour(PanelBackgroundB::panelShadeStartId, colours.getShadingGradientColourA());
    this->setColour(PanelBackgroundB::panelShadeEndId, colours.getShadingGradientColourB());

    this->setColour(PanelBackgroundC::panelFillStartId, colours.getSecondaryGradientColourA());
    this->setColour(PanelBackgroundC::panelFillEndId, colours.getSecondaryGradientColourB());
    this->setColour(PanelBackgroundC::panelShadeStartId, colours.getShadingGradientColourA());
    this->setColour(PanelBackgroundC::panelShadeEndId, colours.getShadingGradientColourB());

    this->setColour(PanelA::panelFillColourId, colours.getPanelFillColour());
    this->setColour(PanelA::panelBorderColourId, colours.getPanelBorderColour());

    // MainWindow
    this->setColour(ResizableWindow::backgroundColourId, colours.getPrimaryGradientColourA().brighter(0.045f));
    this->setColour(ScrollBar::backgroundColourId, Colours::transparentBlack);
    this->setColour(ScrollBar::thumbColourId, colours.getPanelFillColour().withAlpha(1.f));

    // TextButton
    this->setColour(TextButton::buttonColourId, colours.getPanelFillColour());
    this->setColour(TextButton::buttonOnColourId, colours.getPanelBorderColour());
    this->setColour(TextButton::textColourOnId, colours.getTextColour().withMultipliedAlpha(0.75f));
    this->setColour(TextButton::textColourOffId, colours.getTextColour().withMultipliedAlpha(0.5f));

    // FileTreeComponent
    this->setColour(FileTreeComponent::backgroundColourId, Colours::transparentBlack);
    this->setColour(FileTreeComponent::textColourId, Colours::black);
    this->setColour(FileTreeComponent::selectedItemBackgroundColourId, colours.getPanelFillColour());
    this->setColour(FileTreeComponent::highlightColourId, Colours::black.withAlpha(0.2f));

    // InstrumentEditor
    this->setColour(InstrumentEditor::midiInColourId, Colour(0x3f000000));
    this->setColour(InstrumentEditor::midiOutColourId, Colour(0x3f000000));
    this->setColour(InstrumentEditor::audioInColourId, Colour(0x25ffffff));
    this->setColour(InstrumentEditor::audioOutColourId, Colour(0x25ffffff));

    // Origami
    this->setColour(Origami::resizerShadowColourId, Colours::white.withAlpha(0.07f));
    this->setColour(Origami::resizerMovingShadowColourId, Colours::white.withAlpha(0.15f));
    this->setColour(Origami::resizerLineColourId, Colours::black.withAlpha(0.12f));
    this->setColour(Origami::resizerMovingLineColourId, Colours::black.withAlpha(0.25f));

    // ComboBox
    this->setColour(ComboBox::backgroundColourId, colours.getSecondaryGradientColourA().darker(0.05f));
    this->setColour(ComboBox::textColourId, colours.getTextColour());
    this->setColour(ComboBox::buttonColourId, Colours::black.withAlpha(0.6f));
    this->setColour(ComboBox::outlineColourId, Colours::black.withAlpha(0.5f));
    this->setColour(ComboBox::arrowColourId, Colours::white.withAlpha(0.7f));

    // CallOutBox
    this->setColour(HelioCallout::blurColourId, colours.getSecondaryGradientColourB().darker(2.0f).withAlpha(0.35f));
    this->setColour(HelioCallout::fillColourId, colours.getPrimaryGradientColourB().darker(2.0f).withAlpha(0.65f));
    this->setColour(HelioCallout::frameColourId, colours.getPrimaryGradientColourB().brighter(0.75f).withAlpha(0.14f));

    // PopupMenu
    this->setColour(PopupMenu::backgroundColourId, Colours::darkgrey.darker(0.5f));
    this->setColour(PopupMenu::highlightedBackgroundColourId, Colours::black.withAlpha(0.2f));
    this->setColour(PopupMenu::textColourId, Colours::lightgrey);
    this->setColour(PopupMenu::highlightedTextColourId, Colours::white);

    // TextEditor
    this->setColour(TextEditor::textColourId, Colours::white);
    this->setColour(TextEditor::highlightedTextColourId, Colours::white);
    this->setColour(TextEditor::outlineColourId, colours.getPanelBorderColour().withAlpha(0.1f));
    this->setColour(TextEditor::focusedOutlineColourId, Colours::black.withAlpha(0.2f));
    this->setColour(TextEditor::shadowColourId, colours.getShadingGradientColourB());
    this->setColour(TextEditor::backgroundColourId, colours.getPrimaryGradientColourA().darker(0.05f));
    this->setColour(TextEditor::highlightColourId, Colours::black.withAlpha(0.2f));

    // Tree stuff
    this->setColour(TreeView::linesColourId, Colours::white.withAlpha(0.1f));
    this->setColour(TreeView::selectedItemBackgroundColourId, Colours::transparentBlack);
    this->setColour(TreeView::backgroundColourId, Colours::transparentBlack);
    this->setColour(TreeView::dragAndDropIndicatorColourId, Colours::black.withAlpha(0.15f));
    this->setColour(SizeSwitcherComponent::borderColourId, Colours::white.withAlpha(0.09f));

    // MidiRoll
    this->setColour(MidiRoll::blackKeyColourId, colours.getBlackKeyColour());
    this->setColour(MidiRoll::blackKeyBrightColourId, colours.getBlackKeyColour().withMultipliedBrightness(1.15f));
    this->setColour(MidiRoll::whiteKeyColourId, colours.getWhiteKeyColour());
    this->setColour(MidiRoll::whiteKeyBrightColourId, colours.getWhiteKeyColour().withMultipliedBrightness(1.15f));
    this->setColour(MidiRoll::rowLineColourId, colours.getRowColour());
    this->setColour(MidiRoll::barLineColourId, colours.getBarColour().withAlpha(0.9f));
    this->setColour(MidiRoll::barLineBevelColourId, Colours::white.withAlpha(0.015f));
    this->setColour(MidiRoll::beatLineColourId, colours.getBarColour().withAlpha(0.45f));
    this->setColour(MidiRoll::snapLineColourId, colours.getBarColour().withAlpha(0.1f));
    this->setColour(MidiRoll::headerColourId, colours.getPrimaryGradientColourB());

    this->setColour(MidiRoll::indicatorColourId, colours.getLassoBorderColour().withAlpha(0.5f));
    this->setColour(LassoComponent<void *>::lassoFillColourId, colours.getLassoFillColour().withAlpha(0.15f));
    this->setColour(LassoComponent<void *>::lassoOutlineColourId, colours.getLassoBorderColour().withAlpha(0.4f));

    this->setColour(HelperRectangle::fillColourId, colours.getLassoFillColour().withAlpha(0.08f));
    this->setColour(HelperRectangle::outlineColourId, colours.getLassoBorderColour().withAlpha(0.3f));
}

void HelioTheme::updateBackgroundRenders(bool force)
{
    if (force)
    {
        Icons::clearPrerenderedCache();
        this->getPanelsBgCache().clear();
        this->getRollBgCache().clear();
    }
    
#if PIANOROLL_HAS_PRERENDERED_BACKGROUND
    PianoRoll::repaintBackgroundsCache(*this);
#endif
    
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

