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
#include "FontSerializer.h"

FontSerializer::FontSerializer() :
    glyphCount(DEFAULT_GLYPH_COUNT)
{
}

FontSerializer::~FontSerializer()
{
}

void FontSerializer::run(const String &commandLine)
{
    StringArray toks;
    int i = 0;
    toks.addTokens(commandLine, true);

    if (toks.size() < 4)
    {
        printf("FontSerializer::run -f (font file name) -F (font name) [-n (glyph count)]\n\n");
        return;
    }

    for (i = 0; i < toks.size(); i++)
    {
        if (toks[i] == ("-f"))
        {
            if (File::isAbsolutePath(toks[i + 1].unquoted()))
            {
                fontFile = File(toks[i + 1].unquoted());
            }
            else
            {
                fontFile = File::getCurrentWorkingDirectory().getChildFile(toks[i + 1].unquoted());
            }
        }

        if (toks[i] == ("-F"))
        {
            fontName = toks[i + 1].unquoted();
        }

        if (toks[i] == ("-n"))
        {
            glyphCount = toks[i + 1].getIntValue();
        }
    }

    //printf(fontFile.getFullPathName().toUTF8());
    printf("\n");

    Font::findFonts(this->systemFonts);

    if (!fontFile.hasWriteAccess())
    {
        printf("Fserialize::initialise ERROR can't write to destination file:\n");
        //printf(fontFile.getFullPathName().toUTF8());
        return;
    }

    if (fontName == String::empty)
    {
        printf("FontSerializer::initialise ERROR no font name given\n");
        return;
    }

    if (glyphCount <= 0)
    {
        printf("FontSerializer::initialise ERROR glyph count has to be more then 0\n");
        return;
    }


    serializeFont();

    //AlertWindow::showMessageBox(AlertWindow::InfoIcon, "done", "done sir.");
}

void FontSerializer::serializeFont()
{
    streamPlain     = new MemoryOutputStream();
    //streamBold      = new MemoryOutputStream();
    //streamItalic    = new MemoryOutputStream();
    //streamAll       = new MemoryOutputStream();

    printf("Fserialize::serializeFont looking for font in system list [%s]\n", fontName.toUTF8().getAddress());

    for (auto && systemFont : systemFonts)
    {
        if (systemFont.getTypeface()->getName() == fontName)
        {
            printf("FontSerializer::serializeFont font found, attempt to create typeface\n");
            Font fPlain(systemFont);
            //Font fBold(systemFonts[i]);
            //Font fItalic(systemFonts[i]);
            //Font fAll(systemFonts[i]);

            fPlain.setStyleFlags(Font::plain);
            Typeface *tPlain = fPlain.getTypeface();

            //fBold.setStyleFlags(Font::bold);
            //Typeface *tBold = fBold.getTypeface();

            //fItalic.setStyleFlags(Font::italic);
            //Typeface *tItalic = fItalic.getTypeface();

            //fAll.setStyleFlags(Font::italic | Font::bold);
            //Typeface *tAll = fAll.getTypeface();

            if (tPlain)
            {
                printf("FontSerializer::serializeFont copying glyphs to CustomTypeface[plain]\n");
                customTypefacePlain.addGlyphsFromOtherTypeface(*tPlain, 0, glyphCount);

                //printf("Fserialize::serializeFont copying glyphs to CustomTypeface[bold]\n");
                //customTypefaceBold.addGlyphsFromOtherTypeface(*tBold, 0, glyphCount);

                //printf("Fserialize::serializeFont copying glyphs to CustomTypeface[italic]\n");
                //customTypefaceItalic.addGlyphsFromOtherTypeface(*tItalic, 0, glyphCount);

                //printf("Fserialize::serializeFont copying glyphs to CustomTypeface[italic+bold]\n");
                //customTypefaceAll.addGlyphsFromOtherTypeface(*tAll, 0, glyphCount);

                //printf("Fserialize::serializeFont typeface created, attempt to write to file, glyphs=%d\n", glyphCount);

                printf("FontSerializer customTypefacePlain.writeToStream");
                customTypefacePlain.writeToStream(*streamPlain);
                //customTypefaceBold.writeToStream(*streamBold);
                //customTypefaceItalic.writeToStream(*streamItalic);
                //customTypefaceAll.writeToStream(*streamAll);

                size_t streamPlainSize = streamPlain->getDataSize();
                //size_t streamBoldSize = streamBold->getDataSize();
                //size_t streamItalicSize = streamItalic->getDataSize();
                //size_t streamAllSize = streamAll->getDataSize();

                printf("FontSerializer::serializeFont truncate font file to 0");
                fontFile.replaceWithData(nullptr, 0);

                printf("FontSerializer::serializeFont writing normal font, size=%zu\n", streamPlainSize);
                //fontFile.appendData(&streamPlainSize, sizeof(uint32));
                fontFile.appendData(streamPlain->getData(), streamPlain->getDataSize());

                //printf("Fserialize::serializeFont writing bold font, size=%d\n", streamBoldSize);
                //fontFile.appendData(&streamBoldSize, sizeof(uint32));
                //fontFile.appendData(streamBold->getData(), streamBold->getDataSize());

                //printf("Fserialize::serializeFont writing italic font, size=%d\n", streamItalicSize);
                //fontFile.appendData(&streamItalicSize, sizeof(uint32));
                //fontFile.appendData(streamItalic->getData(), streamItalic->getDataSize());

                //printf("Fserialize::serializeFont writing bold+italic font, size=%d\n", streamAllSize);
                //fontFile.appendData(&streamAllSize, sizeof(uint32));
                //fontFile.appendData(streamAll->getData(), streamAll->getDataSize());
            }
            else
            {
                printf("FontSerializer::serializeFont createSystemTypefaceFor() failed\n");
            }
        }
    }

    printf("Fserialize::serializeFont finished\n");

    deleteAndZero(streamPlain);
    //deleteAndZero(streamBold);
    //deleteAndZero(streamItalic);
    //deleteAndZero(streamAll);
}
