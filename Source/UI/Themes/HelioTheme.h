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

    void drawNoise(Graphics &g, float alphaMultiply = 1.f) const;
    static void drawNoise(const HelioTheme &theme, Graphics &g, float alphaMultiply = 1.f);
    static void drawStripes(Rectangle<float> bounds, Graphics &g, float alphaMultiply = 1.f);

    inline static void drawText(Graphics &g,
        const String &text, Rectangle<float> area,
        Justification justificationType, bool useEllipsesIfTooBig = false);
    inline static void drawFittedText(Graphics &g,
        const String &text, int x, int y, int width, int height,
        Justification justification,
        const int maximumNumberOfLines,
        const float minimumHorizontalScale = 0.f);
    inline static void drawText(Graphics &g,
        const String &text, int x, int y, int width, int height,
        Justification justificationType, bool useEllipsesIfTooBig = false)
    {
        HelioTheme::drawText(g, text,
            Rectangle<float>(float(x), float(y), float(width), float(height)),
            justificationType, useEllipsesIfTooBig);
    }

    static void drawFrame(Graphics &g, int width, int height,
        float lightAlphaMultiplier = 1.f, float darkAlphaMultiplier = 1.f);
    static void drawDashedFrame(Graphics &g, const Rectangle<int> &area, int dashLength = 4);

    // dashed lines of various thinckness, with a shape like this:
    //  ---   ---   ---   ---   --
    // ---   ---   ---   ---   ----
    static void drawDashedHorizontalLine(Graphics &g, float x, float y, float width, float dashLength = 4);
    static void drawDashedHorizontalLine2(Graphics &g, float x, float y, float width, float dashLength = 4);
    static void drawDashedVerticalLine(Graphics &g, float x, float y, float height, float dashLength = 4);

    void drawStretchableLayoutResizerBar(Graphics &g,
            int /*w*/, int /*h*/, bool /*isVerticalBar*/,
            bool isMouseOver, bool isMouseDragging) override;

    //===------------------------------------------------------------------===//
    // Text Editor
    //===------------------------------------------------------------------===//

    void fillTextEditorBackground(Graphics&, int w, int h, TextEditor&) override;
    void drawTextEditorOutline(Graphics&, int w, int h, TextEditor&) override {}

    template <typename TextEditorType = TextEditor>
    static UniquePointer<TextEditorType> makeSingleLineTextEditor(bool isEditable,
        float fontSize = Globals::UI::Fonts::M)
    {
        jassert(fontSize >= Globals::UI::Fonts::M && fontSize <= Globals::UI::Fonts::L);
        auto editor = make<TextEditorType>();
        editor->setMultiLine(false);
        editor->setReturnKeyStartsNewLine(false);
        editor->setReadOnly(!isEditable);
        editor->setScrollbarsShown(isEditable);
        editor->setCaretVisible(isEditable);
        editor->setPopupMenuEnabled(isEditable);
        editor->setInterceptsMouseClicks(isEditable, true);
        editor->setJustification(Justification::centredLeft);
        editor->setFont(fontSize);
        editor->setIndents(4, 0);
        return editor;
    }

    //===------------------------------------------------------------------===//
    // Labels
    //===------------------------------------------------------------------===//

    void drawLabel(Graphics &, Label &) override;
    void drawLabel(Graphics &, Label &, juce_wchar passwordCharacter);

    //===------------------------------------------------------------------===//
    // Buttons
    //===------------------------------------------------------------------===//

    Font getTextButtonFont(TextButton&, int buttonHeight) override;
    void drawButtonText(Graphics &, TextButton &button,
        bool isMouseOverButton, bool isButtonDown) override;
    void drawButtonBackground(Graphics &g, Button &button,
        const Colour &backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override;
    void drawToggleButton(Graphics &g, ToggleButton &button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

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
    
    inline const Image &getPageBackgroundA() const noexcept { return this->pageBackgroundA; }
    inline const Image &getPageBackgroundB() const noexcept { return this->pageBackgroundB; }
    inline const Image &getSidebarBackground() const noexcept { return this->sidebarBackground; }
    inline const Image &getBottomPanelBackground() const noexcept { return this->bottomPanelBackground; }
    inline const Image &getHeadlineBackground() const noexcept { return this->headlineBackground; }
    inline const Image &getDialogBackground() const noexcept { return this->dialogBackground; }

    inline bool isDark() const noexcept
    {
        return this->isDarkTheme;
    }

protected:
    
    const Image backgroundNoise;
    const Image backgroundStripes;

    Colour backgroundTextureBaseColour;
    Image cachedBackground;
    
    Typeface::Ptr textTypefaceCache;
    
    Image pageBackgroundA;
    Image pageBackgroundB;
    Image sidebarBackground;
    Image bottomPanelBackground;
    Image headlineBackground;
    Image dialogBackground;

    bool isDarkTheme = false;

    JUCE_LEAK_DETECTOR(HelioTheme);

};
