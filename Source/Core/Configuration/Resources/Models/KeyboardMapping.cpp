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
    using namespace Serialization::Midi;
    SerializedData data(KeyboardMappings::keyboardMapping);
    data.setProperty(KeyboardMappings::map, this->toString());
    data.setProperty(KeyboardMappings::name, this->name);
    return data;
}

void KeyboardMapping::deserialize(const SerializedData &data)
{
    using namespace Serialization::Midi;

    const auto root = data.hasType(KeyboardMappings::keyboardMapping) ?
        data : data.getChildWithName(KeyboardMappings::keyboardMapping);

    if (!root.isValid())
    {
        return;
    }

    this->name = root.getProperty(KeyboardMappings::name, this->name);
    this->loadMapFromString(root.getProperty(KeyboardMappings::map));
}

const String &KeyboardMapping::getName() const noexcept
{
    return this->name;
}

void KeyboardMapping::setName(const String &newName)
{
    this->name = newName;
    this->sendChangeMessage();
}

String KeyboardMapping::toString() const
{
    String result;

    // this will apply RLE-ish encoding and output a string like this:
    // "128:16/2,32/2,33/2,34/2,40/4 300:127/4,0/5,1/5,2/5 512:1/5"
    // or even: "128:16/2,32/2,2+,40/4 300:127/4,3+ 512:1/5"
    KeyChannel lastMappedKey;
    bool hasNewChunk = true;
    int rleSeriesInChunk = 0;

    for (int channel = 0; channel < KeyboardMapping::numMappedChannels; ++channel)
    {
        for (int i = 0; i < KeyboardMapping::numMappedKeys; ++i)
        {
            const auto mappedKey = this->index[i][channel];
            const auto mappingExpectedByRle = KeyboardMapping::getDefaultMappingFor(i, channel);

            if (mappedKey != mappingExpectedByRle)
            {
                if (hasNewChunk)
                {
                    hasNewChunk = false;

                    if (result.isNotEmpty())
                    {
                        result << " ";
                    }

                    result << i;

                    if (channel > 0)
                    {
                        result << "/" << String(channel + 1);
                    }

                    result << ":" << int(mappedKey.key) << "/" << int(mappedKey.channel);
                }
                else
                {
                    const auto defaultStep = lastMappedKey.getNextDefault();
                    if (mappedKey == defaultStep)
                    {
                        rleSeriesInChunk++;
                    }
                    else
                    {
                        if (rleSeriesInChunk > 0)
                        {
                            result << "," << rleSeriesInChunk << "+";
                            rleSeriesInChunk = 0;
                        }

                        result << "," << int(mappedKey.key) << "/" << int(mappedKey.channel);
                    }
                }
            }
            else
            {
                if (rleSeriesInChunk > 0)
                {
                    result << "," << rleSeriesInChunk << "+";
                    rleSeriesInChunk = 0;
                }

                hasNewChunk = true;
            }

            lastMappedKey = mappedKey;
        }
    }

    return result;
}

void KeyboardMapping::loadMapFromString(const String &str)
{
    if (str.isEmpty())
    {
        return;
    }

    // mark all initial valies as invalid,
    // so we can fill the missing keys with default values later
    for (int channel = 0; channel < KeyboardMapping::numMappedChannels; ++channel)
    {
        for (int key = 0; key < KeyboardMapping::numMappedKeys; ++key)
        {
            this->index[key][channel] = { -1, -1 };
        }
    }

    int a = 0; // accumulator
    int keyFrom = 0;
    int channelFrom = 1; // channels here are 1-based
    int chunkOffset = 0;
    int numStepsSkipped = 0;
    int8 keyTo = 0;
    int8 channelTo = 1;
    juce_wchar c;

    const auto updateIndex = [&]()
    {
        if (numStepsSkipped > 0)
        {
            auto keyIterator = KeyChannel(keyTo, channelTo);
            for (int i = 0; i < numStepsSkipped; ++i)
            {
                keyIterator = keyIterator.getNextDefault();
                this->index[keyFrom + chunkOffset + i][channelFrom - 1] = keyIterator;
            }
        }
        else
        {
            jassert(a < 128);
            channelTo = int8(a);
            if (channelTo > 0)
            {
                this->index[keyFrom + chunkOffset][channelFrom - 1] = KeyChannel(keyTo, channelTo);
            }
        }
    };

    bool inInSourceSection = true;
    bool hasChannelFrom = false;
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
        case ':':
            if (hasChannelFrom)
            {
                channelFrom = jlimit(1, 16, a);
            }
            else
            {
                keyFrom = a;
            }
            inInSourceSection = false;
            a = 0;
            break;
        case '/':
            if (inInSourceSection)
            {
                jassert(a >= 0 && a < 2048);
                keyFrom = int8(a);
                hasChannelFrom = true;
            }
            else
            {
                jassert(a >= 0 && a < 128);
                keyTo = int8(a);
            }
            a = 0;
            break;
        case '+':
            numStepsSkipped = a;
            a = 0;
            break;
        case ',':
            updateIndex();
            chunkOffset += jmax(numStepsSkipped, 1);
            numStepsSkipped = 0;
            a = 0;
            break;
        case 0:
        case ' ':
            updateIndex();
            chunkOffset = 0;
            numStepsSkipped = 0;
            channelFrom = 1;
            inInSourceSection = true;
            hasChannelFrom = false;
            a = 0;
            break;
        default:
            break;
        }
    } while (c != 0);

    for (int channel = 0; channel < KeyboardMapping::numMappedChannels; ++channel)
    {
        for (int key = 0; key < KeyboardMapping::numMappedKeys; ++key)
        {
            if (!this->index[key][channel].isValid())
            {
                this->index[key][channel] = KeyboardMapping::getDefaultMappingFor(key, channel);
            }
        }
    }

    this->sendChangeMessage();
}

void KeyboardMapping::loadMapFromPreset(KeyboardMapping *preset)
{
    jassert(preset != nullptr);
    memcpy(this->index, preset->index, sizeof(preset->index));
    this->sendChangeMessage();
}

KeyboardMapping::KeyChannel KeyboardMapping::getDefaultMappingFor(int key, int channel) const noexcept
{
    if (channel == 0)
    {
        return { int8(key % Globals::twelveToneKeyboardSize), 1 };
    }

    auto basedOnPrevChannel = this->index[key][channel - 1];
    basedOnPrevChannel.channel = (basedOnPrevChannel.channel % Globals::numChannels) + 1;
    return basedOnPrevChannel;
}

void KeyboardMapping::reset()
{
    for (int channel = 0; channel < KeyboardMapping::numMappedChannels; ++channel)
    {
        for (int key = 0; key < KeyboardMapping::numMappedKeys; ++key)
        {
            this->index[key][channel] = KeyboardMapping::getDefaultMappingFor(key, channel);
        }
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

    const auto lastMappedKey = KeyboardMapping::numMappedKeys - 1;

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

    for (int channel = 0; channel < KeyboardMapping::numMappedChannels; ++channel)
    {
        for (int key = 0; key < KeyboardMapping::numMappedKeys; ++key)
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

                this->index[key][channel] = { int8(mappedKey), int8(channelNumber) };
            }
        }
    }

    this->sendChangeMessage();
}

void KeyboardMapping::updateKey(int sourceKey, int sourceChannel, const KeyChannel &keyChannel)
{
    this->updateKey(sourceKey, sourceChannel, keyChannel.key, keyChannel.channel);
}

void KeyboardMapping::updateKey(int sourceKey, int sourceChannel, int8 targetKey, int8 targetChannel)
{
    jassert(sourceKey < KeyboardMapping::numMappedKeys);
    jassert(sourceChannel > 0 && sourceChannel <= KeyboardMapping::numMappedChannels);
    jassert(targetKey >= 0);
    jassert(targetChannel > 0);
    this->index[sourceKey][sourceChannel - 1] = { targetKey, targetChannel };
    this->sendChangeMessage();
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String KeyboardMapping::getResourceId() const noexcept
{
    return this->name; // assumed to be unique
}

Identifier KeyboardMapping::getResourceType() const noexcept
{
    return Serialization::Resources::keyboardMappings;
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
    juce_wchar c;
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

        using namespace Serialization::Midi;

        const auto getMap = [](const SerializedData &data)
        {
            return data.getProperty(KeyboardMappings::map).toString();
        };

        KeyboardMapping map;
        expectEquals(getMap(map.serialize()), String());

        // test the simple setter
        map.updateKey(500, 16, 64, 5);
        map.updateKey(600, 16, { 64, 6 });

        expectEquals(getMap(map.serialize()), String("500/16:64/5 600/16:64/6"));

        // test RLE
        SerializedData rleTest(KeyboardMappings::keyboardMapping);
        rleTest.setProperty(KeyboardMappings::map,
            "0:0/1,1/1,120+  128:16/2,32/2,33/2,34/2,40/4 300:127/4,0/5,1/5,2/5 512:1/5 640/1:0/6 ");

        map.deserialize(rleTest);
        expectEquals(getMap(map.serialize()),
            String("128:16/2,32/2,2+,40/4 300:127/4,3+ 512:1/5"));

        // from string / to string
        const String long31("0:0/15,30+ 31:0/16,30+ 62:0/1,30+ 93:0/2,30+ 124:0/3,30+ 155:0/4,30+ 186:0/5,30+ 217:0/6,30+ 248:0/7,30+ 279:0/8,30+ 310:0/9,30+");
        const String short31("0:0/15,30+,0/16,30+,0/1,30+,0/2,30+,0/3,30+,0/4,30+,0/5,30+,0/6,30+,0/7,30+,0/8,30+,0/9,30+");
        map.loadMapFromString(long31);
        expectEquals(map.toString(), short31);
        map.loadMapFromString(short31);
        expectEquals(map.toString(), short31);

        // test resetting
        map.reset();
        expectEquals(getMap(map.serialize()), String());

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
        expect(getMap(serialized1).startsWith("60:72/1,4+ 67:79/1,2+ 72:84/1,4+ 79:91/1,2+"));

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
        expect(getMap(serialized2).startsWith("60:72/1,4+ 67:79/1,2+ 72:84/1,4+ 79:91/1,2+ 128:31/5,62+"));
    }
};

static KeyboardMappingTests keyboardMappingTests;

#endif
