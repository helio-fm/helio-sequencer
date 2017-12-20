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

#pragma once

class HelioCallout;
class ColourScheme;

#define SHORT_FADE_TIME(component) (static_cast<HelioTheme &>((component)->getLookAndFeel()).getShortAnimationLength());
#define LONG_FADE_TIME(component) (static_cast<HelioTheme &>((component)->getLookAndFeel()).getLongAnimationLength());

class HelioTheme : public LookAndFeel_V4
{
public:

    HelioTheme();

    void initResources();
    void initColours(const ::ColourScheme &colours);
    void updateBackgroundRenders(bool force = false);

    Typeface::Ptr getTextTypeface() const
    { return this->textTypefaceCache; }
    
    Typeface::Ptr getHeaderTypeface() const
    { return this->headerTypefaceCache; }
        
    virtual int getShortAnimationLength() { return 150; }
    virtual int getLongAnimationLength() { return 250; }
    
    Typeface::Ptr getTypefaceForFont(const Font &) override;
    virtual Image getBackgroundNoise() const { return this->backgroundNoise; }

    static void drawNoise(Component *target, Graphics &g, float alphaMultiply = 1.f);
    static void drawNoise(const HelioTheme &theme, Graphics &g, float alphaMultiply = 1.f);
    static void drawNoiseWithin(Rectangle<float> bounds, Component *target, Graphics &g, float alphaMultiply = 1.f);
    static void drawDashedRectangle(Graphics &g,
        const Rectangle<float> &rectangle, const Colour &colour,
        float dashLength = 5.f, float spaceLength = 7.5,
        float dashThickness = 1.f, float cornerRadius = 5.f);
    void drawStretchableLayoutResizerBar(Graphics &g,
            int /*w*/, int /*h*/, bool /*isVerticalBar*/,
            bool isMouseOver, bool isMouseDragging) override;

    //===------------------------------------------------------------------===//
    // Text Editor
    //===------------------------------------------------------------------===//

    void fillTextEditorBackground(Graphics&, int w, int h, TextEditor&) override;
    void drawTextEditorOutline(Graphics&, int w, int h, TextEditor&) override;

    //===------------------------------------------------------------------===//
    // Selection
    //===------------------------------------------------------------------===//
    
    void drawLasso(Graphics &g, Component &lassoComp) override;
    
    //===------------------------------------------------------------------===//
    // Labels
    //===------------------------------------------------------------------===//

    void drawLabel(Graphics &, Label &) override;
    void drawLabel(Graphics &, Label &, juce_wchar passwordCharacter);
    Font getLabelFont(Label&) override;

    //===------------------------------------------------------------------===//
    // Button
    //===------------------------------------------------------------------===//

    Font getTextButtonFont(TextButton&, int buttonHeight) override;
    void drawButtonText(Graphics &, TextButton &button,
        bool isMouseOverButton, bool isButtonDown) override;
    void drawButtonBackground(Graphics &g, Button &button,
        const Colour &backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override;
    
    //===------------------------------------------------------------------===//
    // Scrollbars
    //===------------------------------------------------------------------===//

    int getDefaultScrollbarWidth() override;
    bool areScrollbarButtonsVisible() override { return false; }
    void drawScrollbarButton(Graphics &g,
                                     ScrollBar &bar,
                                     int width, int height,
                                     int buttonDirection,
                                     bool isScrollbarVertical,
                                     bool isMouseOverButton,
                                     bool isButtonDown) override { }
    void drawScrollbar(Graphics &g,
                               ScrollBar &bar,
                               int x, int y,
                               int width, int height,
                               bool isScrollbarVertical,
                               int thumbStartPosition,
                               int thumbSize,
                               bool isMouseOver,
                               bool isMouseDown) override;


    //===------------------------------------------------------------------===//
    // Menus
    //===------------------------------------------------------------------===//

    Font getPopupMenuFont() override;
    void drawPopupMenuBackground(Graphics &g, int width, int height) override;
    void drawPopupMenuItem(Graphics&, const Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const String& text,
                           const String& shortcutKeyText,
                           const Drawable* icon,
                           const Colour* textColour) override;
    
    //===------------------------------------------------------------------===//
    // Window
    //===------------------------------------------------------------------===//

    void drawCornerResizer(Graphics& g, int w, int h,
        bool /*isMouseOver*/, bool /*isMouseDragging*/) override;

    void drawResizableFrame(Graphics &g, int w, int h, const BorderSize<int> &border) override;
    void drawDocumentWindowTitleBar(DocumentWindow &window,
                                            Graphics &g, int w, int h,
                                            int titleSpaceX, int titleSpaceW,
                                            const Image *icon,
                                            bool drawTitleTextOnLeft) override;

    Button *createDocumentWindowButton(int buttonType) override;
    void positionDocumentWindowButtons(DocumentWindow &window,
            int titleBarX, int titleBarY,
            int titleBarW, int titleBarH,
            Button *minimiseButton,
            Button *maximiseButton,
            Button *closeButton,
            bool positionTitleBarButtonsOnLeft) override;
    
    inline Image &getBgCache1() noexcept { return this->bgCache1; }
    inline Image &getBgCache2() noexcept { return this->bgCache2; }
    inline Image &getBgCache3() noexcept { return this->bgCache3; }

protected:
    
    const Image backgroundNoise;
    Colour backgroundTextureBaseColour;
    Image cachedBackground;
    
    Typeface::Ptr textTypefaceCache;
    Typeface::Ptr headerTypefaceCache;
    
    Image bgCache1;
    Image bgCache2;
    Image bgCache3;

    JUCE_LEAK_DETECTOR(HelioTheme);

};
