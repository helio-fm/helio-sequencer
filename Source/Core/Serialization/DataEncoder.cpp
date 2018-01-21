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
#include "DataEncoder.h"

static const std::string kBase64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static const int kMagicNumber = 
    static_cast<int>(ByteOrder::littleEndianInt("PR::"));

static const std::string kXorKey =
    "2V:-5?Vl%ulG+4-PG0`#:;[DUnB.Qs::"
    "v<{#]_oaa3NWyGtA[bq>Qf<i,28gV,,;"
    "y;W6rzn)ij}Ol%Eaxoq),+tx>l|@BS($"
    "7W9b9|46Fr&%pS!}[>5g5lly|bC]3aQu";

class TempFile
{
public:

    explicit TempFile(const File &target,  int optionFlags = 0) :
        temporaryFile (createTempFile (target.getParentDirectory(),
            target.getFileNameWithoutExtension()
            + "_temp_" + String::toHexString (Random::getSystemRandom().nextInt()),
            target.getFileExtension())),
        targetFile (target)
    {
        jassert (targetFile != File());
    }

    ~TempFile()
    {
        if (! deleteTemporaryFile())
        {
            jassertfalse;
        }
    }

    const File& getFile() const noexcept
    { return temporaryFile; }

    const File& getTargetFile() const noexcept
    { return targetFile; }

    bool overwriteTargetFileWithTemporary() const
    {
        jassert (targetFile != File());

        if (temporaryFile.exists())
        {
            for (int i = 5; --i >= 0;)
            {
                if (temporaryFile.moveFileTo (targetFile))
                {
                    return true;
                }
                // здесь был баг - иногда под виндой moveFileTo не срабатывает, не знаю, почему
                if (temporaryFile.copyFileTo (targetFile))
                {
                    return true;
                }

                Thread::sleep(100);
            }
        }
        else
        {
            jassertfalse;
        }

        return false;
    }


    bool deleteTemporaryFile() const
    {
        for (int i = 5; --i >= 0;)
        {
            if (temporaryFile.deleteFile())
            {
                return true;
            }

            Thread::sleep(50);
        }

        return false;
    }

private:

    static File createTempFile (const File& parentDirectory, String name, const String& suffix)
    {
        return parentDirectory.getNonexistentChildFile(name, suffix, false);
    }

    const File temporaryFile, targetFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempFile)
};

static inline std::string encodeBase64(unsigned char const *bytes_to_encode, size_t in_len)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);

        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4) ; i++)
            { ret += kBase64Chars[char_array_4[i]]; }

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
        { char_array_3[j] = '\0'; }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
        { ret += kBase64Chars[char_array_4[j]]; }

        while ((i++ < 3))
        { ret += '='; }
    }

    return ret;
}

static inline std::string encodeBase64(const std::string &s)
{
    return encodeBase64(reinterpret_cast<const unsigned char *>(s.data()), s.length());
}

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
            { char_array_4[i] = kBase64Chars.find(char_array_4[i]); }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
            { ret += char_array_3[i]; }

            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++)
        { char_array_4[j] = 0; }

        for (j = 0; j < 4; j++)
        { char_array_4[j] = kBase64Chars.find(char_array_4[j]); }

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

static inline MemoryBlock compress(const String &str)
{
    MemoryOutputStream memOut;
    GZIPCompressorOutputStream compressMemOut(&memOut, 1, false);
    compressMemOut.write(str.toRawUTF8(), str.getNumBytesAsUTF8());
    compressMemOut.flush();
    return MemoryBlock(memOut.getData(), memOut.getDataSize());
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

String DataEncoder::obfuscateString(const String &buffer)
{
    const MemoryBlock &compressed = compress(buffer);
    const MemoryBlock &xorBlock = doXor(compressed);
    const std::string &encoded = encodeBase64(reinterpret_cast<unsigned char const *>(xorBlock.getData()), xorBlock.getSize());
    const String &result = String::createStringFromData(encoded.data(), int(encoded.size()));
    return result;
}

String DataEncoder::deobfuscateString(const String &buffer)
{
    const std::string &decoded = decodeBase64(buffer.toStdString());
    const MemoryBlock &decodedMemblock = MemoryBlock(decoded.data(), decoded.size());
    const MemoryBlock &xorBlock = doXor(decodedMemblock);
    const String &uncompressed = decompress(xorBlock);
    return uncompressed;
}

bool DataEncoder::saveObfuscated(const File &file, XmlElement *xml)
{
    const String xmlString(xml->createDocument("", false, true, "UTF-8", 512));

// Writes as plain text for debugging purposes:
//#if defined _DEBUG
//
//    const String xmlEncoded = xmlString;
//    const File file2(file.getFullPathName() + "_debug");
//    file2.replaceWithText(xmlEncoded);
//
//#else
    
    const MemoryBlock &compressed = compress(xmlString);
    const MemoryBlock &xorBlock = doXor(compressed);
    
    MemoryInputStream obfuscatedStream(xorBlock, false);
    
    if (! file.existsAsFile())
    {
        Result creationResult = file.create();
        //Logger::writeToLog("Creating: " + file.getFullPathName() + ", result: " + creationResult.getErrorMessage());
    }
    
    TempFile tempFile(file);
    ScopedPointer <OutputStream> out(tempFile.getFile().createOutputStream());
    
    //Logger::writeToLog("Temp file: " + tempFile.getFile().getFullPathName());
    
    if (out != nullptr)
    {
        out->writeInt(kMagicNumber);
        out->writeFromInputStream(obfuscatedStream, -1);
        out->flush();
        
        if (tempFile.overwriteTargetFileWithTemporary())
        {
            return true;
        }
        
        Logger::writeToLog("DataEncoder::saveObfuscated failed overwriteTargetFileWithTemporary");
    }
    else
    {
        Logger::writeToLog("DataEncoder::saveObfuscated failed");
    }

    return false;

//#endif
}

XmlElement *DataEncoder::loadObfuscated(const File &file)
{
//#if defined _DEBUG
//    
//    const String result = file.loadFileAsString();
//    const String decodedResult = result;
//    
//    XmlElement *xml = XmlDocument::parse(decodedResult);
//    return xml;
//    
//#else
    
    FileInputStream fileStream(file);
    
    if (fileStream.openedOk())
    {
        const int magicNumber = fileStream.readInt();
        
        if (magicNumber == kMagicNumber)
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

//#endif
}

#define KEY_BLOCK_SIZE 64

MemoryBlock DataEncoder::encryptXml(const XmlElement &xmlTarget,
        const MemoryBlock &key)
{
    const String xmlString = xmlTarget.createDocument("", false, true, "UTF-8", 512);
    MemoryBlock compressed = compress(xmlString);
    
    const int modulo = (compressed.getSize() % 4);
    const int alignDelta = (modulo > 0) ? (4 - modulo) : 0;
    compressed.ensureSize(compressed.getSize() + alignDelta, true);

    MemoryInputStream xmlStream(compressed, false);
    MemoryInputStream keyStream(key, false);

    MemoryBlock cipher;
    MemoryOutputStream cipherStream(cipher, false);

    int currentCrypter = 0;
    OwnedArray<BlowFish> crypters;

    while (!keyStream.isExhausted())
    {
        MemoryBlock nextKey;
        const int numBytesRead = keyStream.readIntoMemoryBlock(nextKey, KEY_BLOCK_SIZE);
        jassert(numBytesRead == KEY_BLOCK_SIZE);

        crypters.add(new BlowFish(nextKey.getData(), nextKey.getSize()));
    }

    jassert(crypters.size() > 0);

    cipherStream.writeInt(kMagicNumber);

    while (!xmlStream.isExhausted())
    {
        uint32 int1(xmlStream.readInt());
        uint32 int2(xmlStream.readInt());

        crypters[currentCrypter]->encrypt(int1, int2);
        currentCrypter += 1;

        if (currentCrypter >= crypters.size())
        { currentCrypter = 0; }

        cipherStream.writeInt(int1);
        cipherStream.writeInt(int2);
    }

    // если этого не сделать, размер блока будет больше необходимого
    cipherStream.flush();

    return cipher;
}

XmlElement *DataEncoder::createDecryptedXml(const MemoryBlock &buffer,
        const MemoryBlock &key)
{
    MemoryInputStream bufferStream(buffer, false);
    MemoryInputStream keyStream(key, false);

    MemoryBlock decipher;
    MemoryOutputStream decipherStream(decipher, false);

    int currentCrypter = 0;
    OwnedArray<BlowFish> crypters;

    while (!keyStream.isExhausted())
    {
        MemoryBlock nextKey;
        const int numBytesRead = keyStream.readIntoMemoryBlock(nextKey, KEY_BLOCK_SIZE);
        jassert(numBytesRead == KEY_BLOCK_SIZE);
        
        if (numBytesRead != KEY_BLOCK_SIZE)
        {
            return nullptr;
        }

        crypters.add(new BlowFish(nextKey.getData(), nextKey.getSize()));
    }

    jassert(crypters.size() > 0);

    const int magicNumber = bufferStream.readInt();

    if (magicNumber != kMagicNumber)
    { 
        return nullptr;
    }

    while (!bufferStream.isExhausted())
    {
        uint32 int1(bufferStream.readInt());
        uint32 int2(bufferStream.readInt());

        crypters[currentCrypter]->decrypt(int1, int2);
        currentCrypter += 1;

        if (currentCrypter >= crypters.size())
        { currentCrypter = 0; }

        decipherStream.writeInt(int1);
        decipherStream.writeInt(int2);
    }

    const String &uncompressed = decompress(decipher);
    return XmlDocument::parse(uncompressed);
}
