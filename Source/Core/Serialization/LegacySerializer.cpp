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
#include "LegacySerializer.h"
#include "SerializationKeys.h"
#include "Note.h"

// This file now encapsulates all the ugliness
// of a legacy serializer used in the first version of the app.
// The purpose is being able to read old project files.

// Please do not read this file.

static const std::string kBase64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char *kHelioHeaderV1String = "PR::";
static const uint32 kHelioHeaderV1 = ByteOrder::littleEndianInt(kHelioHeaderV1String);

static const std::string kXorKey =
    "2V:-5?Vl%ulG+4-PG0`#:;[DUnB.Qs::"
    "v<{#]_oaa3NWyGtA[bq>Qf<i,28gV,,;"
    "y;W6rzn)ij}Ol%Eaxoq),+tx>l|@BS($"
    "7W9b9|46Fr&%pS!}[>5g5lly|bC]3aQu";

static inline bool isBase64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

static inline std::string decodeBase64(const std::string &encoded_string)
{
    size_t in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && isBase64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;

        if (i == 4)
        {
            for (i = 0; i < 4; i++)
            {
                char_array_4[i] = (unsigned char)kBase64Chars.find(char_array_4[i]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
            {
                ret += char_array_3[i];
            }

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
        {
            char_array_4[j] = 0;
        }

        for (j = 0; j < 4; j++)
        {
            char_array_4[j] = (unsigned char)kBase64Chars.find(char_array_4[j]);
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) { ret += char_array_3[j]; }
    }

    return ret;
}

static inline MemoryBlock doXor(const MemoryBlock &input)
{
    MemoryBlock encoded;

    for (unsigned long i = 0; i < input.getSize(); i++)
    {
        char orig = input[i];
        char key = kXorKey[i % kXorKey.length()];
        char xorChar = orig ^ key;
        encoded.append(&xorChar, 1);
    }

    return encoded;
}

static inline String decompress(const MemoryBlock &str)
{
    MemoryInputStream input(str.getData(), str.getSize(), false);
    GZIPDecompressorInputStream gzInput(input);
    MemoryBlock decompressedData(0, true);
    MemoryBlock buf(512);

    while (!gzInput.isExhausted())
    {
        const int data = gzInput.read(buf.getData(), 512);
        decompressedData.append(buf.getData(), data);
    }

    return decompressedData.toString();
}

static String deobfuscateString(const String &buffer)
{
    const std::string &decoded = decodeBase64(buffer.toStdString());
    const MemoryBlock &decodedMemblock = MemoryBlock(decoded.data(), decoded.size());
    const MemoryBlock &xorBlock = doXor(decodedMemblock);
    const String &uncompressed = decompress(xorBlock);
    return uncompressed;
}

static XmlElement *loadObfuscated(const File &file)
{
    FileInputStream fileStream(file);

    if (fileStream.openedOk())
    {
        const auto magicNumber = static_cast<uint32>(fileStream.readInt());
        if (magicNumber == kHelioHeaderV1)
        {
            SubregionStream subStream(&fileStream, 4, -1, false);
            MemoryBlock subBlock;
            subStream.readIntoMemoryBlock(subBlock);
            const MemoryBlock &xorBlock = doXor(subBlock);
            const String &uncompressed = decompress(xorBlock);
            XmlElement *xml = XmlDocument::parse(uncompressed);
            return xml;
        }
    }

    return nullptr;
}

Result LegacySerializer::saveToFile(File file, const ValueTree &tree) const
{
    return Result::fail("Not supported");
}

static String toLowerCamelCase(const String &string)
{
    if (string.length() > 1)
    {
        return string.substring(0, 1).toLowerCase() +
            string.substring(1);
    }

    return string;
}

static void processDeprecations(String &key, String &value)
{
    using namespace Serialization;

    if (key == "beat")
    {
        key = Midi::timestamp.toString();
        value = String(int32(value.getFloatValue() * TICKS_PER_BEAT));
    }
    else if (key == "len")
    {
        key = Midi::length.toString();
        value = String(int32(value.getFloatValue() * TICKS_PER_BEAT));
    }
    else if (key == "vel")
    {
        key = Midi::volume.toString();
    }
    else if (key == "val")
    {
        key = Midi::value.toString();
    }
    else if (key == "Type")
    {
        value = toLowerCamelCase(value);
    }
}

static String transformXmlTag(const String &tagOrAttribute)
{
    using namespace Serialization;
    using namespace Serialization::VCS;

    static HashMap<String, Identifier> oldKeys;
    if (oldKeys.size() == 0)
    {
        oldKeys.set("ProjectInfo", Core::projectInfo);
        oldKeys.set("ProjectTimeline", Core::projectTimeline);
        oldKeys.set("ProjectLicense", ProjectInfoDeltas::projectLicense);
        oldKeys.set("ProjectFullName", ProjectInfoDeltas::projectTitle);
        oldKeys.set("ProjectAuthor", ProjectInfoDeltas::projectAuthor);
        oldKeys.set("ProjectDescription", ProjectInfoDeltas::projectDescription);
        oldKeys.set("LayerPath", MidiTrackDeltas::trackPath);
        oldKeys.set("LayerMute", MidiTrackDeltas::trackMute);
        oldKeys.set("LayerColour", MidiTrackDeltas::trackColour);
        oldKeys.set("LayerInstrument", MidiTrackDeltas::trackInstrument);
        oldKeys.set("LayerController", MidiTrackDeltas::trackController);
        oldKeys.set("HeadIndex", VCS::snapshot);
        oldKeys.set("HeadIndexData", VCS::snapshotData);
        oldKeys.set("annotationsId", Core::annotationsTrackId);
        oldKeys.set("keySignaturesId", Core::keySignaturesTrackId);
        oldKeys.set("timeSignaturesId", Core::timeSignaturesTrackId);
        oldKeys.set("fullPath", Core::filePath);
        oldKeys.set("Path", Core::filePath);
        oldKeys.set("Uuid", Audio::instrumentId);
        oldKeys.set("Uid", Audio::pluginId);
        oldKeys.set("PluginManager", Audio::pluginsList);
        oldKeys.set("Pack", VCS::pack);
        oldKeys.set("VCSUuid", VCS::vcsItemId);
        oldKeys.set("GlobalConfig", Core::globalConfig);
        oldKeys.set("Layer", Core::track);
        oldKeys.set("PianoLayer", Core::pianoTrack);
        oldKeys.set("AutoLayer", Core::automationTrack);
        oldKeys.set("VersionControl", Core::versionControl);
        oldKeys.set("SourceNode", Audio::sourceNodeId);
        oldKeys.set("DestinationNode", Audio::destinationNodeId);
        oldKeys.set("SourceChannel", Audio::sourceChannel);
        oldKeys.set("DestinationChannel", Audio::destinationChannel);
        oldKeys.set("srcFilter", Audio::sourceNodeId);
        oldKeys.set("dstFilter", Audio::destinationNodeId);
        oldKeys.set("srcChannel", Audio::sourceChannel);
        oldKeys.set("dstChannel", Audio::destinationChannel);
        oldKeys.set("uiLastX", UI::positionX);
        oldKeys.set("uiLastY", UI::positionY);
    }

    if (oldKeys.contains(tagOrAttribute))
    {
        return oldKeys[tagOrAttribute].toString();
    }

    return tagOrAttribute;
}

static ValueTree valueTreeFromXml(const XmlElement &xml)
{
    if (!xml.isTextElement())
    {
        ValueTree v(toLowerCamelCase(transformXmlTag(xml.getTagName())));

        for (int i = 0; i < xml.getNumAttributes(); ++i)
        {
            auto attName = transformXmlTag(xml.getAttributeName(i));
            auto attValue = transformXmlTag(xml.getAttributeValue(i));

            processDeprecations(attName, attValue);

            if (attName.startsWith("base64:"))
            {
                MemoryBlock mb;
                if (mb.fromBase64Encoding(attValue))
                {
                    v.setProperty(toLowerCamelCase(attName.substring(7)), var(mb), nullptr);
                    continue;
                }
            }

            v.setProperty(toLowerCamelCase(attName), var(attValue), nullptr);
        }

        forEachXmlChildElement(xml, e)
        {
            v.appendChild(valueTreeFromXml(*e), nullptr);
        }

        return v;
    }

    //jassertfalse;
    return {};
}

Result LegacySerializer::loadFromFile(const File &file, ValueTree &tree) const
{
    ScopedPointer<XmlElement> xml(loadObfuscated(file));
    if (xml != nullptr && !xml->isTextElement())
    {
        tree = valueTreeFromXml(*xml);
        return Result::ok();
    }

    return Result::fail({});
}

Result LegacySerializer::saveToString(String &string, const ValueTree &tree) const
{
    return Result::fail("Not supported");
}

Result LegacySerializer::loadFromString(const String &string, ValueTree &tree) const
{
    XmlDocument document(deobfuscateString(string));
    ScopedPointer<XmlElement> xml(document.getDocumentElement());
    tree = ValueTree::fromXml(*xml);
    return Result::ok();
}

bool LegacySerializer::supportsFileWithExtension(const String &extension) const
{
    return extension.endsWithIgnoreCase("hp") ||
        extension.endsWithIgnoreCase("helio") ||
        extension.endsWithIgnoreCase("pack");
}

bool LegacySerializer::supportsFileWithHeader(const String &header) const
{
    return header.startsWith(kHelioHeaderV1String);
}
