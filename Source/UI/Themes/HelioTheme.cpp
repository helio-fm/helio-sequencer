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
#include "SequencerSidebarLeft.h"
#include "BinaryData.h"
#include "Icons.h"
#include "ColourIDs.h"
#include "Config.h"
#include "SerializationKeys.h"

#include "HybridRoll.h"
#include "InstrumentEditor.h"
#include "HelperRectangle.h"
#include "PianoRoll.h"
#include "PatternRoll.h"

#include "SelectionComponent.h"
#include "PanelBackgroundA.h"
#include "PanelBackgroundB.h"
#include "PanelBackgroundC.h"
#include "FramePanel.h"
#include "TrackScroller.h"

#include "BinaryData.h"
#include "ColourScheme.h"

#if HELIO_DESKTOP
#   define SCROLLBAR_WIDTH (17)
#   define TEXTBUTTON_FONT (21.f)
#elif HELIO_MOBILE
#   define SCROLLBAR_WIDTH (50)
#   define TEXTBUTTON_FONT (23.f)
#endif

HelioTheme::HelioTheme() :
    backgroundNoise(ImageCache::getFromMemory(BinaryData::noise_png, BinaryData::noise_pngSize)) {}

static const float kNoiseAlpha = 0.0175f;

void HelioTheme::drawNoise(Component *target, Graphics &g, float alphaMultiply /*= 1.f*/)
{
    g.setTiledImageFill(static_cast<HelioTheme &>(target->getLookAndFeel()).getBackgroundNoise(), 0, 0, kNoiseAlpha * alphaMultiply);
    g.fillRect(0, 0, target->getWidth(), target->getHeight());
}

void HelioTheme::drawNoise(const HelioTheme &theme, Graphics &g, float alphaMultiply /*= 1.f*/)
{
    g.setTiledImageFill(theme.getBackgroundNoise(), 0, 0, kNoiseAlpha * alphaMultiply);
    g.fillRect(0, 0, g.getClipBounds().getWidth(), g.getClipBounds().getHeight());
}

void HelioTheme::drawNoiseWithin(Rectangle<float> bounds, Component *target, Graphics &g, float alphaMultiply /*= 1.f*/)
{
    g.setTiledImageFill(static_cast<HelioTheme &>(target->getLookAndFeel()).getBackgroundNoise(), 0, 0, kNoiseAlpha * alphaMultiply);
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

    static Array<float> dashes(dashLength, spaceLength);
    PathStrokeType(dashThickness, PathStrokeType::mitered, PathStrokeType::rounded)
    .createDashedStroke(path, path, dashes.getRawDataPointer(), dashes.size());
    
    g.strokePath(path, PathStrokeType(dashThickness));
}

Typeface::Ptr HelioTheme::getTypefaceForFont(const Font &font)
{
    if (font.getTypefaceName() == Font::getDefaultSansSerifFontName() ||
        font.getTypefaceName() == Font::getDefaultSerifFontName())
    {
        return this->textTypefaceCache;
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

    g.setColour(lassoComp.findColour(ColourIDs::SelectionComponent::fill));
    g.fillRoundedRectangle(lassoComp.getLocalBounds().toFloat(), cornersRound);
    
    g.setColour(lassoComp.findColour(ColourIDs::SelectionComponent::outline));

    const Rectangle<float> r(0.5f, 0.5f,
        float(lassoComp.getLocalBounds().getWidth()) - 1.0f,
        float(lassoComp.getLocalBounds().getHeight()) - 1.0f);

    Path path;
    path.addQuadrilateral(r.getBottomRight().getX(), r.getBottomRight().getY(),
        r.getTopRight().getX(), r.getTopRight().getY(),
        r.getTopLeft().getX(), r.getTopLeft().getY(),
        r.getBottomLeft().getX(), r.getBottomLeft().getY());
    
    path = path.createPathWithRoundedCorners(cornersRound);
    
    static Array<float> dashes(dashLength, dashLength);
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
        const Font font(this->getLabelFont(label));
        const String textToDraw = (passwordCharacter != 0) ?
            String::repeatedString(String::charToString(passwordCharacter), label.getText().length()) :
            label.getText();

        // Try to guess the right max number of lines depending on label height and font height:
        const int maxLines = int(float(label.getHeight()) / font.getHeight());

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

    //Image img(Icons::findByName(button.getName(), int(button.getHeight() / 1.5f)));
    //const int cX = button.getButtonText().isEmpty() ? (button.getWidth() / 2) : (button.getWidth() / 4);
    //const int cY = (button.getHeight() / 2);
    //Icons::drawImageRetinaAware(img, g, cX, cY);
    //const bool hasImg = (img.getWidth() > 2);

    const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
    const int yHeight = (button.getHeight() - (yIndent * 2));
    const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = int(font.getHeight() * 0.5f);
    //const int leftIndent  = hasImg ? button.getWidth() / 3 : fontHeight;
    const int leftIndent = fontHeight;
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
// Tables
//===----------------------------------------------------------------------===//

void HelioTheme::drawTableHeaderBackground(Graphics &g, TableHeaderComponent &header)
{
    Rectangle<int> r(header.getLocalBounds());

    g.setGradientFill (ColourGradient (Colour (0x35000000),
                                       static_cast<float> ((r.getWidth() / 2)), 0.0f,
                                       Colour (0x00000000),
                                       0.0f, 0.0f,
                                       true));
    g.drawHorizontalLine(r.getHeight() - 1, 0.f, float(r.getWidth()));

    g.setGradientFill (ColourGradient (Colour (0x15ffffff),
                                       static_cast<float> ((r.getWidth() / 2)), 0.0f,
                                       Colour (0x00ffffff),
                                       0.0f, 0.0f,
                                       true));
    g.drawHorizontalLine(r.getHeight() - 2, 0.f, float(r.getWidth()));
}

void HelioTheme::drawTableHeaderColumn(Graphics &g, TableHeaderComponent &header, const String &columnName,
    int columnId, int width, int height, bool isMouseOver, bool isMouseDown, int columnFlags)
{
    auto highlightColour = header.findColour(TableHeaderComponent::highlightColourId);

    if (isMouseDown)
        g.fillAll(highlightColour);
    else if (isMouseOver)
        g.fillAll(highlightColour.withMultipliedAlpha(0.625f));

    Rectangle<int> area(width, height);
    area.reduce(4, 0);

    g.setColour(header.findColour(TableHeaderComponent::textColourId));
    if ((columnFlags & (TableHeaderComponent::sortedForwards | TableHeaderComponent::sortedBackwards)) != 0)
    {
        Path sortArrow;
        sortArrow.addTriangle(0.0f, 0.0f,
            0.5f, (columnFlags & TableHeaderComponent::sortedForwards) != 0 ? -0.9f : 0.9f,
            1.0f, 0.0f);

        g.fillPath(sortArrow, sortArrow.getTransformToScaleToFit(area.removeFromRight(height / 2).reduced(2).toFloat(), true));
    }

    g.setFont(Font(height * 0.5f, Font::bold));
    g.drawFittedText(columnName, area, Justification::centredLeft, 1);
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
// Sliders
//===----------------------------------------------------------------------===//

void HelioTheme::drawRotarySlider(Graphics &g, int x, int y, int width, int height,
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle, Slider &slider)
{
    const auto fill = slider.findColour(Slider::rotarySliderFillColourId);
    const auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(8);
    const auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const auto lineW = jmin(8.0f, radius * 0.25f);
    const auto arcRadius = radius - lineW * 0.5f;

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
            arcRadius, arcRadius, 0.0f, rotaryStartAngle, toAngle, true);

        g.setColour(fill);
        g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
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

class HelioWindowButton final : public Button
{
public:

    HelioWindowButton(const String &name, const Path &normalShape, const Path &toggledShape) noexcept :
        Button(name),
        normalShape(normalShape),
        toggledShape(toggledShape) {}

    void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override
    {
        float alpha = isMouseOverButton ? (isButtonDown ? 1.0f : 0.8f) : 0.6f;
        if (!isEnabled())
        {
            alpha *= 0.5f;
        }

        float x = 0, y = 0, diam;

        if (this->getWidth() < this->getHeight())
        {
            diam = (float)this->getWidth();
            y = (this->getHeight() - this->getWidth()) * 0.5f;
        }
        else
        {
            diam = (float)this->getHeight();
            y = (this->getWidth() - this->getHeight()) * 0.5f;
        }

        x += diam * 0.05f;
        y += diam * 0.05f;
        diam *= 0.9f;

        const Colour colour(findDefaultColour(TextButton::textColourOnId));

        if (isMouseOverButton)
        {
            g.setColour(colour.withAlpha(alpha * 0.05f));
            //g.fillEllipse(x + 0.5f, y + 0.5f, diam - 1.f, diam - 1.f);
            g.fillAll();
        }

        const auto &p = getToggleState() ? toggledShape : normalShape;

        const AffineTransform t(p.getTransformToScaleToFit(x + diam * 0.3f,
            y + diam * 0.3f, diam * 0.4f, diam * 0.4f, true));

        g.setColour(colour);
        g.fillPath(p, t);
    }

private:

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
    g.setColour(findDefaultColour(ResizableWindow::backgroundColourId));
    g.fillAll();

    HelioTheme::drawNoise(*this, g, 1.f);

    g.setColour(Colour(0x32000000));
    g.drawHorizontalLine(h - 2, 0.f, float(w));

    g.setColour(Colour(0x0cffffff));
    g.drawHorizontalLine(h - 1, 0.f, float(w));

    Font font(16.f, Font::plain);
    g.setFont(font);
    g.setColour(findDefaultColour(Label::textColourId).withAlpha(0.33f));

    const String title = "Helio " + App::getAppReadableVersion();
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
        return new HelioWindowButton("close", shape, shape);
    }
    if (buttonType == DocumentWindow::minimiseButton)
    {
        shape.addLineSegment(Line<float>(0.0f, 0.0f, 0.0001f, 0.0f), 0.0001f);
        shape.addLineSegment(Line<float>(0.0f, 0.5f, 1.0f, 0.5f), 0.05f);
        return new HelioWindowButton("min", shape, shape);
    }
    else if (buttonType == DocumentWindow::maximiseButton)
    {
        shape.addLineSegment(Line<float>(0.0f, 0.0f, 0.0f, 0.8f), 0.05f);
        shape.addLineSegment(Line<float>(0.0f, 0.8f, 1.0f, 0.8f), 0.05f);
        shape.addLineSegment(Line<float>(1.0f, 0.8f, 1.0f, 0.0f), 0.05f);
        shape.addLineSegment(Line<float>(1.0f, 0.0f, 0.0f, 0.0f), 0.05f);
        return new HelioWindowButton("max", shape, shape);
    }

    jassertfalse;
    return nullptr;
}

void HelioTheme::positionDocumentWindowButtons(DocumentWindow &,
        int titleBarX, int titleBarY,
        int titleBarW, int titleBarH,
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
    Icons::initBuiltInImages();

#if HELIO_MOBILE
    this->textTypefaceCache = Font::getDefaultTypefaceForFont({ Font::getDefaultSansSerifFontName(), 0, 0 });
#elif HELIO_DESKTOP

    if (App::Config().containsProperty(Serialization::Config::lastUsedFont))
    {
        const String lastUsedFontName = App::Config().getProperty(Serialization::Config::lastUsedFont);
        Font lastUsedFont(lastUsedFontName, 10, Font::plain);
        this->textTypefaceCache = Typeface::createSystemTypefaceFor(lastUsedFont);
        return;
    }

    const auto startTime = Time::getMillisecondCounter();
    Array<Font> systemFonts;
    Font::findFonts(systemFonts);
    DBG("Fonts search done in " + String(Time::getMillisecondCounter() - startTime) + " ms");

    const Font *noto = nullptr;
    const StringArray notoNames = {"Noto Sans", "Noto Sans UI"};

    const Font *notoCJK = nullptr;
    const String notoCJKName = "Noto Sans CJK";

    for (const auto &systemFont : systemFonts)
    {
        if (notoNames.contains(systemFont.getTypeface()->getName()))
        {
            noto = &systemFont;
        }
        else if (systemFont.getTypeface()->getName().startsWithIgnoreCase(notoCJKName))
        {
            notoCJK = &systemFont;
        }
    }

    if (notoCJK != nullptr)
    {
        this->textTypefaceCache = Typeface::createSystemTypefaceFor(*notoCJK);
    }
    else if (noto != nullptr)
    {
        this->textTypefaceCache = Typeface::createSystemTypefaceFor(*noto);
    }
    else
    {
        // Verdana on Windows, Bitstream Vera Sans or something on Linux, Lucida Grande on macOS:
        this->textTypefaceCache = Font::getDefaultTypefaceForFont({ Font::getDefaultSansSerifFontName(), 0, 0 });
    }

    DBG("Using font: " + this->textTypefaceCache->getName());
    App::Config().setProperty(Serialization::Config::lastUsedFont, this->textTypefaceCache->getName());
#endif
}

void HelioTheme::updateFont(const Font &font)
{
    Typeface::clearTypefaceCache();
    this->textTypefaceCache = Typeface::createSystemTypefaceFor(font);
    const String fontName = font.getTypeface()->getName();
    App::Config().setProperty(Serialization::Config::lastUsedFont, fontName);
}

void HelioTheme::initColours(const ::ColourScheme::Ptr s)
{
    // JUCE component colour id's:

    // Sliders
    this->setColour(Slider::rotarySliderOutlineColourId, Colours::transparentBlack);
    this->setColour(Slider::rotarySliderFillColourId, s->getTextColour());
    this->setColour(Slider::thumbColourId, s->getTextColour());

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

    // TextEditor
    this->setColour(TextEditor::textColourId, s->getTextColour());
    this->setColour(TextEditor::highlightedTextColourId, s->getTextColour());
    this->setColour(TextEditor::outlineColourId, s->getPanelBorderColour().withAlpha(0.1f));
    this->setColour(TextEditor::focusedOutlineColourId, s->getTextColour().contrasting().withAlpha(0.2f));
    this->setColour(TextEditor::shadowColourId, s->getPrimaryGradientColourA().darker(0.05f));
    this->setColour(TextEditor::backgroundColourId, s->getPrimaryGradientColourA().darker(0.05f));
    this->setColour(TextEditor::highlightColourId, s->getTextColour().contrasting().withAlpha(0.25f));
    this->setColour(CaretComponent::caretColourId, s->getTextColour().withAlpha(0.35f));

    // TableListBox
    this->setColour(ListBox::textColourId, s->getTextColour());
    this->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->setColour(TableHeaderComponent::backgroundColourId, Colours::transparentBlack);
    this->setColour(TableHeaderComponent::outlineColourId, s->getPanelBorderColour().withAlpha(0.05f));
    this->setColour(TableHeaderComponent::highlightColourId, s->getPrimaryGradientColourA().brighter(0.04f));
    this->setColour(TableHeaderComponent::textColourId, s->getTextColour().withMultipliedAlpha(0.75f));

    // Check boxes, radio buttons
    this->setColour(ToggleButton::textColourId, s->getTextColour());
    this->setColour(ToggleButton::tickColourId, s->getTextColour());
    this->setColour(ToggleButton::tickDisabledColourId, s->getTextColour().withMultipliedAlpha(0.65f));

    // Helio colours:

    // Lasso
    this->setColour(ColourIDs::SelectionComponent::fill, s->getLassoFillColour().withAlpha(0.25f));
    this->setColour(ColourIDs::SelectionComponent::outline, s->getLassoBorderColour().withAlpha(0.4f));

    // A hack for icon base colors
    this->setColour(ColourIDs::Icons::fill, s->getIconBaseColour());
    this->setColour(ColourIDs::Icons::shadow, s->getIconShadowColour());

    // Panels
    this->setColour(ColourIDs::BackgroundA::fill, s->getPrimaryGradientColourA().brighter(0.005f));
    this->setColour(ColourIDs::BackgroundB::fill, s->getPrimaryGradientColourA().darker(0.02f));
    this->setColour(ColourIDs::BackgroundC::fill, s->getSecondaryGradientColourA());

    this->setColour(ColourIDs::Panel::fill, s->getPanelFillColour());
    this->setColour(ColourIDs::Panel::border, s->getPanelBorderColour().withAlpha(0.225f));

    this->setColour(ColourIDs::TrackScroller::borderLineDark, s->getPrimaryGradientColourA().darker(0.25f));
    this->setColour(ColourIDs::TrackScroller::borderLineLight, Colours::white.withAlpha(0.025f));
    this->setColour(ColourIDs::TrackScroller::screenRangeFill, s->getIconBaseColour().withMultipliedAlpha(0.45f));
    this->setColour(ColourIDs::TrackScroller::scrollerFill, s->getIconBaseColour().withMultipliedAlpha(0.2f));

    // InstrumentEditor
    this->setColour(ColourIDs::Instrument::midiIn, Colours::white.withAlpha(0.1f));
    this->setColour(ColourIDs::Instrument::midiOut, Colours::white.withAlpha(0.1f));
    this->setColour(ColourIDs::Instrument::audioIn, Colours::white.withAlpha(0.15f));
    this->setColour(ColourIDs::Instrument::audioOut, Colours::white.withAlpha(0.15f));
    this->setColour(ColourIDs::Instrument::midiConnector, Colours::black.withAlpha(0.35f));
    this->setColour(ColourIDs::Instrument::audioConnector, Colours::white.withAlpha(0.25f));
    this->setColour(ColourIDs::Instrument::shadowPin, Colours::black.withAlpha(0.1f));
    this->setColour(ColourIDs::Instrument::shadowConnector, Colours::black.withAlpha(0.2f));

    // Borders
    this->setColour(ColourIDs::Common::borderLineLight, Colours::white.withAlpha(0.06f));
    this->setColour(ColourIDs::Common::borderLineDark, Colours::black.withAlpha(0.2f));
    this->setColour(ColourIDs::ColourButton::outline, s->getTextColour());

    // CallOutBox
    this->setColour(ColourIDs::Callout::fill, s->getPrimaryGradientColourB().darker(0.025f));
    this->setColour(ColourIDs::Callout::frame, s->getPrimaryGradientColourB().darker(0.25f));

    // HybridRoll
    this->setColour(ColourIDs::Roll::blackKey, s->getBlackKeyColour().withMultipliedBrightness(0.95f));
    this->setColour(ColourIDs::Roll::blackKeyAlt, s->getBlackKeyColour());
    this->setColour(ColourIDs::Roll::whiteKey, s->getWhiteKeyColour());
    this->setColour(ColourIDs::Roll::whiteKeyAlt, s->getWhiteKeyColour().withMultipliedBrightness(1.05f));
    this->setColour(ColourIDs::Roll::rowLine, s->getRowColour());
    this->setColour(ColourIDs::Roll::barLine, s->getBarColour().withAlpha(0.8f));
    this->setColour(ColourIDs::Roll::barLineBevel, Colours::white.withAlpha(0.015f));
    this->setColour(ColourIDs::Roll::beatLine, s->getBarColour().withAlpha(0.4f));
    this->setColour(ColourIDs::Roll::snapLine, s->getBarColour().withAlpha(0.1f));
    this->setColour(ColourIDs::Roll::headerFill, s->getPrimaryGradientColourB().darker(0.025f));
    this->setColour(ColourIDs::Roll::headerSnaps, s->getPrimaryGradientColourB().darker(0.025f).contrasting().withMultipliedAlpha(0.37f));
    this->setColour(ColourIDs::Roll::playhead, s->getLassoBorderColour().withAlpha(0.6f));
    this->setColour(ColourIDs::Roll::playheadShade, Colours::black.withAlpha(0.1f));
    this->setColour(ColourIDs::Roll::trackHeaderFill, s->getWhiteKeyColour());
    this->setColour(ColourIDs::Roll::trackHeaderBorder, Colours::white.withAlpha(0.075f));
    this->setColour(ColourIDs::Roll::noteFill, s->getTextColour().interpolatedWith(Colours::white, 0.5f));

    this->setColour(ColourIDs::HelperRectangle::fill, s->getLassoFillColour().withAlpha(0.08f));
    this->setColour(ColourIDs::HelperRectangle::outline, s->getLassoBorderColour().withAlpha(0.3f));

    this->setColour(ColourIDs::ScriptEditor::comment, s->getTextColour().withMultipliedAlpha(0.5f));
    this->setColour(ColourIDs::ScriptEditor::error, s->getTextColour().interpolatedWith(Colours::red, 0.75f));
    this->setColour(ColourIDs::ScriptEditor::keyword, s->getTextColour().interpolatedWith(Colours::royalblue, 0.75f));
    this->setColour(ColourIDs::ScriptEditor::identifier, s->getTextColour().interpolatedWith(Colours::darkgoldenrod, 0.3f));
    this->setColour(ColourIDs::ScriptEditor::operatorToken, s->getTextColour().interpolatedWith(Colours::limegreen, 0.3f));
    this->setColour(ColourIDs::ScriptEditor::integerType, s->getTextColour().interpolatedWith(Colours::blueviolet, 0.5f));
    this->setColour(ColourIDs::ScriptEditor::floatType, s->getTextColour().interpolatedWith(Colours::darkmagenta, 0.5f));
    this->setColour(ColourIDs::ScriptEditor::stringType, s->getTextColour().interpolatedWith(Colours::tomato, 0.5f));
    this->setColour(ColourIDs::ScriptEditor::bracket, s->getTextColour().interpolatedWith(Colours::darkslateblue, 0.5f));
    this->setColour(ColourIDs::ScriptEditor::punctuation, s->getTextColour().interpolatedWith(Colours::darkslateblue, 0.75f));
    this->setColour(ColourIDs::ScriptEditor::builtInClass, s->getTextColour().interpolatedWith(Colours::blue, 0.75f));

    this->setColour(ColourIDs::Logo::fill, s->getTextColour().withMultipliedAlpha(0.85f));
    this->setColour(ColourIDs::AudioMonitor::foreground, s->getTextColour());

    this->setColour(ColourIDs::VersionControl::connector, s->getTextColour().withAlpha(0.2f));
    this->setColour(ColourIDs::VersionControl::outline, s->getTextColour().withAlpha(0.3f));
    this->setColour(ColourIDs::VersionControl::highlight, s->getTextColour().withAlpha(0.02f));
}

void HelioTheme::updateBackgroundRenders(bool force)
{
    if (force)
    {
        Icons::clearPrerenderedCache();
        this->bgCacheA = {};
        this->bgCacheB = {};
        this->bgCacheC = {};
    }
    
    PanelBackgroundA::updateRender(*this);
    PanelBackgroundB::updateRender(*this);
    PanelBackgroundC::updateRender(*this);
}
