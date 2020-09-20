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

#include "ColourScheme.h"

class HelioTheme final : public LookAndFeel_V4
{
public:

    HelioTheme();

    static HelioTheme &getCurrentTheme() noexcept;

    void initResources();
    void initColours(const ::ColourScheme::Ptr colours);
    void updateFont(const Font &font);
    
    Typeface::Ptr getTypefaceForFont(const Font &) override;

    static void drawNoise(Component *target, Graphics &g, float alphaMultiply = 1.f);
    static void drawNoise(const HelioTheme &theme, Graphics &g, float alphaMultiply = 1.f);
    static void drawNoiseWithin(Rectangle<int> bounds, Graphics &g, float alphaMultiply = 1.f);
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
    void drawTextEditorOutline(Graphics&, int w, int h, TextEditor&) override {}
        
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
    // Tables
    //===------------------------------------------------------------------===//

    void drawTableHeaderBackground(Graphics&, TableHeaderComponent&) override;
    void drawTableHeaderColumn(Graphics&, TableHeaderComponent&,
        const String &columnName, int columnId, int width, int height,
        bool isMouseOver, bool isMouseDown, int columnFlags) override;

    //===------------------------------------------------------------------===//
    // Scrollbars
    //===------------------------------------------------------------------===//

    int getDefaultScrollbarWidth() override;
    bool areScrollbarButtonsVisible() override { return false; }
    void drawScrollbarButton(Graphics &g, ScrollBar &bar,
        int width, int height, int buttonDirection,
        bool isScrollbarVertical, bool isMouseOverButton,
        bool isButtonDown) override {}
    void drawScrollbar(Graphics &g, ScrollBar &bar,
        int x, int y, int width, int height,
        bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
        bool isMouseOver, bool isMouseDown) override;

    //===------------------------------------------------------------------===//
    // Sliders
    //===------------------------------------------------------------------===//

    void drawRotarySlider(Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, Slider&) override;
        
    //===------------------------------------------------------------------===//
    // Window
    //===------------------------------------------------------------------===//

    void drawCornerResizer(Graphics& g, int w, int h,
        bool /*isMouseOver*/, bool /*isMouseDragging*/) override;

    void drawResizableFrame(Graphics &g, int w, int h,
        const BorderSize<int> &border) override;

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
    
    inline Image &getBgCacheA() noexcept { return this->bgCacheA; }
    inline const Image &getBgCacheA() const noexcept { return this->bgCacheA; }

    inline Image &getBgCacheB() noexcept { return this->bgCacheB; }
    inline const Image &getBgCacheB() const noexcept { return this->bgCacheB; }

    inline Image &getBgCacheC() noexcept { return this->bgCacheC; }
    inline const Image &getBgCacheC() const noexcept { return this->bgCacheC; }

    inline bool isDark() const noexcept
    {
        return this->isDarkTheme;
    }

protected:
    
    const Image backgroundNoise;
    Colour backgroundTextureBaseColour;
    Image cachedBackground;
    
    Typeface::Ptr textTypefaceCache;
    
    Image bgCacheA;
    Image bgCacheB;
    Image bgCacheC;

    bool isDarkTheme = false;

    JUCE_LEAK_DETECTOR(HelioTheme);

};
