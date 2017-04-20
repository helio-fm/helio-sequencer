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

// 0x700
//#define DEFAULT_GLYPH_COUNT 1792
// 0x530
#define DEFAULT_GLYPH_COUNT 1328

class FontSerializer
{
public:

    FontSerializer();

    virtual ~FontSerializer();

    void run(const String &commandLine);

private:

    void serializeFont();

    File fontFile;
    String fontName;
    uint32 glyphCount;
    Array <Font> systemFonts;
    MemoryOutputStream *streamPlain, *streamBold, *streamItalic, *streamAll;
    CustomTypeface customTypefacePlain, customTypefaceBold, customTypefaceItalic, customTypefaceAll;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FontSerializer);

};
