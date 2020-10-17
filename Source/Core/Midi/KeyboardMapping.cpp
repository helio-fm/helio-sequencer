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
#include "KeyboardMapping.h"

CustomKeyboardMapping::CustomKeyboardMapping()
{
    this->reset();
}

SerializedData CustomKeyboardMapping::serialize() const
{
    SerializedData root;
    // todo
    return root;
}

void CustomKeyboardMapping::deserialize(const SerializedData &data)
{
    // todo
}

void CustomKeyboardMapping::reset()
{
    for (int key = 0; key < KeyboardMapping::maxMappedKeys; ++key)
    {
        this->index[key] = {
            int8(key % Globals::twelveToneKeyboardSize),
            int8(key / Globals::twelveToneKeyboardSize) };
    }
}

void CustomKeyboardMapping::loadScalaKbm(const Array<File> &files)
{
    this->reset();

    for (const auto &file : files)
    {
        StringArray nameTokens;
        nameTokens.addTokens(file.getFileNameWithoutExtension(), "_", "");
        /*
            http://www.huygens-fokker.org/scala/help.htm#mappings:
            multichannel mapping consists of a set of single mapping files
            with the same name followed by an underscore
            and a MIDI channel number from 1 .. 16.
            For example, if map_1.kbm, map_2.kbm and map_3.kbm
            are present in the same directory as the filename given
            (either one of those) then they function independently
            for MIDI input channels 1, 2 and 3 and messages from
            other channels will not be mapped, therefore ignored
        */
        int channelNumber = 0;
        if (nameTokens.size() > 1)
        {
            const auto c = nameTokens.getReference(nameTokens.size() - 1).getIntValue();
            channelNumber = jlimit(0, Globals::numChannels - 1, c - 1); // 0-based
        }

        // todo assign name?
        // assert the name is either empty or the same (which it should be)

        auto kbmStream = file.createInputStream();
        const auto lastMappedKey = KeyboardMapping::maxMappedKeys - 1;

        // read parameters

        int sizeOfMapPattern = Globals::twelveTonePeriodSize; // the pattern repeats every so many keys
        int firstNoteToRetune = 0;
        int lastNoteToRetune = lastMappedKey;
        int middleNote = 0; // where scale degree 0 is mapped to
        //int _referenceNote = 0; // not used
        //float _frequency = 0.f; // not used
        int periodSize = Globals::twelveTonePeriodSize;

        int paramNumber = 0;
        const auto totalNumParams = 7;
        while (!kbmStream->isExhausted() && paramNumber < totalNumParams)
        {
            const auto line = kbmStream->readNextLine().trim();
            if (line.isEmpty() || line.startsWithChar('!')) // comment
            {
                continue;
            }

            switch (paramNumber++)
            {
            case 0:
                sizeOfMapPattern = jlimit(1, 127, line.getIntValue());
                break;
            case 1:
                firstNoteToRetune = jlimit(0, lastMappedKey, line.getIntValue());
                break;
            case 2:
                lastNoteToRetune = jlimit(0, lastMappedKey, line.getIntValue());
                jassert(firstNoteToRetune < lastNoteToRetune);
                break;
            case 3:
                middleNote = jlimit(0, lastMappedKey, line.getIntValue());
                jassert(middleNote + sizeOfMapPattern <= lastMappedKey);
                break;
            //case 4:
            //    _referenceNote = jlimit(0, lastMappedKey, line.getIntValue());
            //    break;
            //case 5:
            //    _frequency = jlimit(0.001f, 100000.f, line.getFloatValue());
            //    break;
            case 6:
                periodSize = jlimit(0, 127, line.getIntValue());
                break;
            default:
                break;
            }
        }

        // read mapping

        Array<Note::Key> kbmMapping;
        for (int i = 0; i < Globals::twelveToneKeyboardSize; ++i)
        {
            kbmMapping.set(i, i);
        }

        for (int i = 0; i < sizeOfMapPattern; ++i)
        {
            // not re-tuned by default
            kbmMapping.set(i, -1);

            if (kbmStream->isExhausted())
            {
                continue; // after EOF unmapped keys may be left out
            }

            const auto line = kbmStream->readNextLine().trim();
            if (line.isEmpty() || line.startsWithChar('!')) // comment
            {
                i--;
                continue;
            }

            if (!line.startsWithChar('x'))
            {
                kbmMapping.set(i, jlimit(0, lastMappedKey, line.getIntValue()));
            }
        }

        // apply mapping

        for (int key = 0; key < Globals::twelveToneKeyboardSize; ++key)
        {
            if (key >= firstNoteToRetune && key <= lastNoteToRetune)
            {
                const auto noteOffset = key - middleNote;
                auto periodNumber = noteOffset / sizeOfMapPattern;
                auto keyInPeriod = noteOffset % sizeOfMapPattern;

                if (keyInPeriod < 0)
                {
                    periodNumber--;
                    keyInPeriod += sizeOfMapPattern;
                }

                if (kbmMapping[keyInPeriod] < 0 ||
                    kbmMapping[keyInPeriod] >= Globals::twelveToneKeyboardSize)
                {
                    continue; // not re-tuned
                }

                const auto mappedKey = kbmMapping[keyInPeriod] +
                    (middleNote + periodNumber * periodSize);

                this->index[mappedKey] = { int8(key), int8(channelNumber) };
            }
        }
    }
}
