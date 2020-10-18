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
#include "SerializationKeys.h"

KeyboardMapping::KeyboardMapping()
{
    this->reset();
}

SerializedData KeyboardMapping::serialize() const
{
    using namespace Serialization;
    SerializedData data(Midi::keyboardMapping);

    // someday we will have all channels in the sequencer
    String channel1;

    // this will apply RLE-ish encoding and output a string like this:
    // "128:16/2,32/2,33/2,34/2,40/4 300:127/4,0/5,1/5,2/5 512:1/5"
    // or even: "128:16/2,32/2,2+,40/4 300:127/4,3+ 512:1/5"
    KeyChannel lastKey;
    bool hasNewChunk = true;
    int rleSeriesInChunk = 0;
    for (int i = 0; i < KeyboardMapping::maxMappedKeys; ++i)
    {
        const auto key = this->index[i];
        const auto defaultKey = KeyboardMapping::getDefaultMappingFor(i);

        if (key != defaultKey)
        {
            if (hasNewChunk)
            {
                if (channel1.isNotEmpty())
                {
                    channel1 << " ";
                }

                channel1 << i << ":" << int(key.key) << "/" << int(key.channel);
                hasNewChunk = false;
            }
            else
            {
                const auto defaultStep = lastKey.getNextDefault();
                if (key == defaultStep)
                {
                    rleSeriesInChunk++;
                }
                else
                {
                    if (rleSeriesInChunk > 0)
                    {
                        channel1 << "," << rleSeriesInChunk << "+";
                        rleSeriesInChunk = 0;
                    }

                    channel1 << "," << int(key.key) << "/" << int(key.channel);
                }
            }
        }
        else
        {
            if (rleSeriesInChunk > 0)
            {
                channel1 << "," << rleSeriesInChunk << "+";
                rleSeriesInChunk = 0;
            }

            hasNewChunk = true;
        }

        lastKey = key;
    }

    data.setProperty(Midi::channel1, channel1);

    return data;
}

void KeyboardMapping::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;

    const auto root = data.hasType(Midi::keyboardMapping) ?
        data : data.getChildWithName(Midi::keyboardMapping);

    if (!root.isValid()) { return; }

    const String channel1 = root.getProperty(Midi::channel1);

    if (channel1.isEmpty())
    {
        return;
    }

    int a = 0; // accumulator
    int keyOffset = 0;
    int chunkOffset = 0;
    int numStepsSkipped = 0;
    int8 key = 0;
    int8 channel = 0;
    juce_wchar c;

    const auto updateIndex = [&]()
    {
        if (numStepsSkipped > 0)
        {
            auto keyIterator = KeyChannel(key, channel);
            for (int i = 0; i < numStepsSkipped; ++i)
            {
                keyIterator = keyIterator.getNextDefault();
                this->index[keyOffset + chunkOffset + i] = keyIterator;
            }
            numStepsSkipped = 0;
        }
        else
        {
            jassert(a < 128);
            channel = int8(a);
            if (channel > 0)
            {
                this->index[keyOffset + chunkOffset] = KeyChannel(key, channel);
            }
        }
    };

    auto ptr = channel1.getCharPointer();
    do
    {
        c = ptr.getAndAdvance();
        switch (c)
        {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            a = a * 10 + ((int)c) - '0';
            break;
        case ':':
            keyOffset = a;
            a = 0;
            break;
        case '/':
            jassert(a < 128);
            key = int8(a);
            jassert(key >= 0);
            a = 0;
            break;
        case '+':
            numStepsSkipped = a;
            a = 0;
            break;
        case ',':
            updateIndex();
            chunkOffset++;
            a = 0;
            break;
        case 0:
        case ' ':
            updateIndex();
            chunkOffset = 0;
            a = 0;
            break;
        default:
            break;
        }
    } while (c != 0);

    this->sendChangeMessage();
}

KeyboardMapping::KeyChannel KeyboardMapping::getDefaultMappingFor(int key) noexcept
{
    return {
        int8(key % Globals::twelveToneKeyboardSize),
        int8(key / Globals::twelveToneKeyboardSize) + 1 };
}

void KeyboardMapping::reset()
{
    for (int key = 0; key < KeyboardMapping::maxMappedKeys; ++key)
    {
        this->index[key] = KeyboardMapping::getDefaultMappingFor(key);
    }

    this->sendChangeMessage();
}

void KeyboardMapping::loadScalaKbmFile(InputStream &fileContentStream,
    const String &fileNameWithoutExtension)
{
    StringArray nameTokens;
    nameTokens.addTokens(fileNameWithoutExtension, "_", "");
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
    int channelNumber = 1;
    if (nameTokens.size() > 1)
    {
        const auto c = nameTokens.getReference(nameTokens.size() - 1).getIntValue();
        channelNumber = jlimit(0, Globals::numChannels - 1, c);
    }

    // todo assign name?

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
    while (!fileContentStream.isExhausted() && paramNumber < totalNumParams)
    {
        const auto line = fileContentStream.readNextLine().trim();
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
            middleNote = jlimit(0, 127, line.getIntValue());
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

        if (fileContentStream.isExhausted())
        {
            continue; // after EOF unmapped keys may be left out
        }

        const auto line = fileContentStream.readNextLine().trim();
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

    for (int key = 0; key < KeyboardMapping::maxMappedKeys; ++key)
    {
        if (key >= firstNoteToRetune && key <= lastNoteToRetune)
        {
            const auto noteOffset = (key % Globals::twelveToneKeyboardSize) - middleNote;
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

            this->index[key] = { int8(mappedKey), int8(channelNumber) };
        }
    }

    this->sendChangeMessage();
}

void KeyboardMapping::updateKey(int key, const KeyChannel &keyChannel)
{
    this->updateKey(key, keyChannel.key, keyChannel.channel);
}

void KeyboardMapping::updateKey(int key, int8 targetKey, int8 targetChannel)
{
    jassert(key < KeyboardMapping::maxMappedKeys);
    jassert(targetKey >= 0);
    jassert(targetChannel > 0);
    this->index[key] = { targetKey, targetChannel };
    this->sendChangeMessage();
}

//===----------------------------------------------------------------------===//
// KeyboardMapping::KeyChannel
//===----------------------------------------------------------------------===//

KeyboardMapping::KeyChannel KeyboardMapping::KeyChannel::getNextDefault() const noexcept
{
    int k = this->key + 1;
    int8 c = this->channel;
    if (k >= Globals::twelveToneKeyboardSize)
    {
        k = 0;
        c++;
    }

    return { int8(k), c };
}

KeyboardMapping::KeyChannel KeyboardMapping::KeyChannel::fromString(const String &str)
{
    KeyChannel result;

    int a = 0;
    juce_wchar c = 0;
    auto ptr = str.getCharPointer();
    do
    {
        c = ptr.getAndAdvance();
        switch (c)
        {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            a = a * 10 + ((int)c) - '0';
            break;
        case '/':
            jassert(a < 128);
            result.key = int8(a);
            a = 0;
            break;
        case 0:
            jassert(a > 0 && a < 128);
            result.channel = int8(a);
            a = 0;
            break;
        default:
            break;
        }
    } while (c != 0);

    return result;
}

String KeyboardMapping::KeyChannel::toString() const noexcept
{
    return String(this->key) + "/" + String(this->channel);
}

bool KeyboardMapping::KeyChannel::isValid() const noexcept
{
    return this->key >= 0 &&
        this->channel > 0 && this->channel <= 16;
}

//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

class KeyboardMappingTests final : public UnitTest
{
public:
    KeyboardMappingTests() : UnitTest("Keyboard mapping tests", UnitTestCategories::helio) {}

    void runTest() override
    {
        beginTest("Keyboard mapping serialization and import from Scala");

        using namespace Serialization;

        const auto channel1 = [](const SerializedData &data)
        {
            return data.getProperty(Midi::channel1).toString();
        };

        KeyboardMapping map;
        expectEquals(channel1(map.serialize()), String());

        // test the simple setter
        map.updateKey(500, 64, 5);
        map.updateKey(600, { 64, 6 });
        expectEquals(channel1(map.serialize()), String("500:64/5 600:64/6"));

        // test RLE
        SerializedData rleTest(Midi::keyboardMapping);
        rleTest.setProperty(Midi::channel1,
            "0:0/1,1/1,120+  128:16/2,32/2,33/2,34/2,40/4 300:127/4,0/5,1/5,2/5 512:1/5 640:0/6 ");

        map.deserialize(rleTest);
        expectEquals(channel1(map.serialize()),
            String("128:16/2,32/2,2+,40/4 300:127/4,3+ 512:1/5"));

        // test resetting
        map.reset();
        expectEquals(channel1(map.serialize()), String());

        // check that loading the mapping for the default channel works as expected
        String kbm1 = "! Size of map. \n\
            12 \n\
            !First MIDI note number to re-tune: \n\
            60 \n\
            !Last MIDI note number to re-tune: \n\
            82 \n\
            !Middle note where the first entry of the mapping is mapped to : \n\
            60 \n\
            !Ignored - Reference note for which frequency is given : \n\
            69 \n\
            !Ignored - Frequency to tune the above note to : \n\
            440.0 \n\
            !Scale degree to consider as formal octave: \n\
            12 \n\
            !Mapping. \n\
            12 \n\
            13 \n\
            14 \n\
            15 \n\
            16 \n\
            x \n\
            x \n\
            19 \n\
            20 \n\
            21";

        MemoryInputStream in1(kbm1.toRawUTF8(), kbm1.getNumBytesAsUTF8(), false);
        map.loadScalaKbmFile(in1, "file");
        const auto serialized1 = map.serialize();
        expectEquals(channel1(serialized1),
            String("60:72/1,4+ 67:79/1,2+ 72:84/1,4+ 79:91/1,2+"));

        // check that loading the mapping for another explicitly specified channel works
        String kbm2 = "! Size of map. \n 31 \n\
            !First MIDI note number to re-tune: \n 128 \n\
            !Last MIDI note number to re-tune: \n 190 \n\
            0 \n 0 \n 0.0 \n\
            !Scale degree to consider as formal octave: \n 31 \n\
            !Mapping. \n\
            31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n\
            47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n";

        MemoryInputStream in2(kbm2.toRawUTF8(), kbm2.getNumBytesAsUTF8(), false);
        map.loadScalaKbmFile(in2, "file_5");
        const auto serialized2 = map.serialize();
        expectEquals(channel1(serialized2),
            String("60:72/1,4+ 67:79/1,2+ 72:84/1,4+ 79:91/1,2+ 128:31/5,62+"));

        // check that serialization/deserialization is consistent
        map.deserialize(serialized2);
        const auto serialized3 = map.serialize();
        expectEquals(channel1(serialized2), channel1(serialized3));
    }
};

static KeyboardMappingTests keyboardMappingTests;

#endif
