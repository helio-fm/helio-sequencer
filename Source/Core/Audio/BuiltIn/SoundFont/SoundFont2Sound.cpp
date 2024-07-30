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

    This SoundFont implementation is based on SFZero,
    written by Steve Folta and extended by Leo Olivers and Cognitone,
    distributed under MIT license, see README.md for details.
*/

#include "Common.h"
#include "SoundFont2Sound.h"
#include "SoundFontSample.h"
#include "SoundFontRegion.h"

#include <memory>
#include <vector>

typedef char sf2fourcc[4];
typedef unsigned char sf2byte;
typedef unsigned long sf2dword;
typedef unsigned short sf2word;
typedef char sf2string[20];

#define FourCCEquals(value1, value2) \
    (value1[0] == value2[0] && \
     value1[1] == value2[1] && \
     value1[2] == value2[2] && \
     value1[3] == value2[3])

struct RIFFChunk final
{
    enum class Type
    {
        RIFF,
        LIST,
        Custom
    };

    bool isTypeOf(sf2fourcc chunkName) const
    {
        return this->id[0] == chunkName[0] &&
               this->id[1] == chunkName[1] &&
               this->id[2] == chunkName[2] &&
               this->id[3] == chunkName[3];
    };

    sf2fourcc id;
    sf2dword size;
    Type type;
    int64 start;

    void readFrom(InputStream &file)
    {
        file.read(&this->id, sizeof(sf2fourcc));
        this->size = static_cast<sf2dword>(file.readInt());
        this->start = file.getPosition();

        if (FourCCEquals(id, "RIFF"))
        {
            this->type = Type::RIFF;
            file.read(&this->id, sizeof(sf2fourcc));
            this->start += sizeof(sf2fourcc);
            this->size -= sizeof(sf2fourcc);
        }
        else if (FourCCEquals(id, "LIST"))
        {
            this->type = Type::LIST;
            file.read(&this->id, sizeof(sf2fourcc));
            this->start += sizeof(sf2fourcc);
            this->size -= sizeof(sf2fourcc);
        }
        else
        {
            type = Type::Custom;
        }
    }

    void seek(InputStream &file)
    {
        file.setPosition(this->start);
    }

    void seekAfter(InputStream &file)
    {
        int64 next = this->start + this->size;

        if (next % 2 != 0)
        {
            next += 1;
        }

        file.setPosition(next);
    }

    int64 end()
    {
        return (this->start + this->size);
    }

    String readString(InputStream &file)
    {
        return file.readEntireStreamAsString();
    }

    JUCE_LEAK_DETECTOR(RIFFChunk)
};

namespace SF2
{
struct rangesType final
{
    sf2byte lo, hi;
};

union genAmountType
{
    rangesType range;
    short shortAmount;
    sf2word wordAmount;
};

struct iver final
{
    sf2word major;
    sf2word minor;

    void readFrom(InputStream &file);

    JUCE_LEAK_DETECTOR(iver)
};

struct phdr final
{
    sf2string presetName;
    sf2word preset;
    sf2word bank;
    sf2word presetBagNdx;
    sf2dword library;
    sf2dword genre;
    sf2dword morphology;

    void readFrom(InputStream &file);

    static const int sizeInFile = 38;

    JUCE_LEAK_DETECTOR(phdr)
};

struct pbag final
{
    sf2word genNdx;
    sf2word modNdx;

    void readFrom(InputStream &file);

    static const int sizeInFile = 4;

    JUCE_LEAK_DETECTOR(pbag)
};

struct pmod final
{
    sf2word modSrcOper;
    sf2word modDestOper;
    short modAmount;
    sf2word modAmtSrcOper;
    sf2word modTransOper;

    void readFrom(InputStream &file);

    static const int sizeInFile = 10;

    JUCE_LEAK_DETECTOR(pmod)
};

struct pgen final
{
    sf2word genOper;
    genAmountType genAmount;

    void readFrom(InputStream &file);

    static const int sizeInFile = 4;

    JUCE_LEAK_DETECTOR(pgen)
};

struct inst final
{
    sf2string instName;
    sf2word instBagNdx;
    void readFrom(InputStream &file);

    static const int sizeInFile = 22;

    JUCE_LEAK_DETECTOR(inst)
};

struct ibag final
{
    sf2word instGenNdx;
    sf2word instModNdx;

    void readFrom(InputStream &file);

    static const int sizeInFile = 4;

    JUCE_LEAK_DETECTOR(ibag)
};

struct imod final
{
    sf2word modSrcOper;
    sf2word modDestOper;
    short modAmount;
    sf2word modAmtSrcOper;
    sf2word modTransOper;

    void readFrom(InputStream &file);

    static const int sizeInFile = 10;

    JUCE_LEAK_DETECTOR(imod)
};

struct igen final
{
    sf2word genOper;
    genAmountType genAmount;
    void readFrom(InputStream &file);

    static const int sizeInFile = 4;

    JUCE_LEAK_DETECTOR(igen)
};

struct shdr final
{
    sf2string sampleName;
    sf2dword start;
    sf2dword end;
    sf2dword startLoop;
    sf2dword endLoop;
    sf2dword sampleRate;
    sf2byte originalPitch;
    char pitchCorrection;
    sf2word sampleLink;
    sf2word sampleType;

    void readFrom(InputStream &file);

    static const int sizeInFile = 46;

    JUCE_LEAK_DETECTOR(shdr)
};

struct Hydra final
{
    std::vector<phdr> presetHeaderList;
    std::vector<pbag> pbagItems;
    std::vector<pmod> pmodItems;
    std::vector<pgen> pgenItems;
    std::vector<inst> instItems;
    std::vector<ibag> ibagItems;
    std::vector<imod> imodItems;
    std::vector<igen> igenItems;
    std::vector<shdr> shdrItems;

    template <typename T>
    void readChunkItems(const RIFFChunk &chunk,
        std::vector<T> &chunkItems,
        InputStream &file)
    {
        int numItems = (int)chunk.size / T::sizeInFile;
        for (int i = 0; i < numItems; ++i)
        {
            T t;
            t.readFrom(file);
            chunkItems.push_back(t);
        }
    }

    void readFrom(InputStream &file, int64 pdtaChunkEnd);
    bool isComplete();

    JUCE_LEAK_DETECTOR(Hydra)
};
} // namespace SF2

void SF2::iver::readFrom(InputStream &file)
{
    this->major = (sf2word)file.readShort();
    this->minor = (sf2word)file.readShort();
}

void SF2::phdr::readFrom(InputStream &file)
{
    file.read(this->presetName, 20);
    this->preset = (sf2word)file.readShort();
    this->bank = (sf2word)file.readShort();
    this->presetBagNdx = (sf2word)file.readShort();
    this->library = (sf2dword)file.readInt();
    this->genre = (sf2dword)file.readInt();
    this->morphology = (sf2dword)file.readInt();
}

void SF2::pbag::readFrom(InputStream &file)
{
    this->genNdx = (sf2word)file.readShort();
    this->modNdx = (sf2word)file.readShort();
}

void SF2::pmod::readFrom(InputStream &file)
{
    this->modSrcOper = (sf2word)file.readShort();
    this->modDestOper = (sf2word)file.readShort();
    this->modAmount = file.readShort();
    this->modAmtSrcOper = (sf2word)file.readShort();
    this->modTransOper = (sf2word)file.readShort();
}

void SF2::pgen::readFrom(InputStream &file)
{
    this->genOper = (sf2word)file.readShort();
    this->genAmount.shortAmount = file.readShort();
}

void SF2::inst::readFrom(InputStream &file)
{
    file.read(this->instName, 20);
    this->instBagNdx = (sf2word)file.readShort();
}

void SF2::ibag::readFrom(InputStream &file)
{
    this->instGenNdx = (sf2word)file.readShort();
    this->instModNdx = (sf2word)file.readShort();
}

void SF2::imod::readFrom(InputStream &file)
{
    this->modSrcOper = (sf2word)file.readShort();
    this->modDestOper = (sf2word)file.readShort();
    this->modAmount = file.readShort();
    this->modAmtSrcOper = (sf2word)file.readShort();
    this->modTransOper = (sf2word)file.readShort();
}

void SF2::igen::readFrom(InputStream &file)
{
    this->genOper = (sf2word)file.readShort();
    this->genAmount.shortAmount = file.readShort();
}

void SF2::shdr::readFrom(InputStream &file)
{
    file.read(this->sampleName, 20);
    this->start = (sf2dword)file.readInt();
    this->end = (sf2dword)file.readInt();
    this->startLoop = (sf2dword)file.readInt();
    this->endLoop = (sf2dword)file.readInt();
    this->sampleRate = (sf2dword)file.readInt();
    this->originalPitch = (sf2byte)file.readByte();
    this->pitchCorrection = file.readByte();
    this->sampleLink = (sf2word)file.readShort();
    this->sampleType = (sf2word)file.readShort();
}

void SF2::Hydra::readFrom(InputStream &file, int64 pdtaChunkEnd)
{
    auto check = [](RIFFChunk &chunk, sf2fourcc chunkName)
    {
        sf2fourcc &chunkID = chunk.id;
        return chunkID[0] == chunkName[0] &&
               chunkID[1] == chunkName[1] &&
               chunkID[2] == chunkName[2] &&
               chunkID[3] == chunkName[3];
    };

    sf2fourcc phdrType = {'p', 'h', 'd', 'r'};
    sf2fourcc pbagType = {'p', 'b', 'a', 'g'};
    sf2fourcc pmodType = {'p', 'm', 'o', 'd'};
    sf2fourcc pgenType = {'p', 'g', 'e', 'n'};
    sf2fourcc instType = {'i', 'n', 's', 't'};
    sf2fourcc ibagType = {'i', 'b', 'a', 'g'};
    sf2fourcc imodType = {'i', 'm', 'o', 'd'};
    sf2fourcc igenType = {'i', 'g', 'e', 'n'};
    sf2fourcc shdrType = {'s', 'h', 'd', 'r'};

    while (file.getPosition() < pdtaChunkEnd)
    {
        RIFFChunk chunk;
        chunk.readFrom(file);

        if (check(chunk, phdrType))
        {
            this->readChunkItems(chunk, this->presetHeaderList, file);
        }
        else if (check(chunk, pbagType))
        {
            this->readChunkItems(chunk, this->pbagItems, file);
        }
        else if (check(chunk, pmodType))
        {
            this->readChunkItems(chunk, this->pmodItems, file);
        }
        else if (check(chunk, pgenType))
        {
            this->readChunkItems(chunk, this->pgenItems, file);
        }
        else if (check(chunk, instType))
        {
            this->readChunkItems(chunk, this->instItems, file);
        }
        else if (check(chunk, ibagType))
        {
            this->readChunkItems(chunk, this->ibagItems, file);
        }
        else if (check(chunk, imodType))
        {
            this->readChunkItems(chunk, this->imodItems, file);
        }
        else if (check(chunk, igenType))
        {
            this->readChunkItems(chunk, this->igenItems, file);
        }
        else if (check(chunk, shdrType))
        {
            this->readChunkItems(chunk, this->shdrItems, file);
        }

        chunk.seekAfter(file);
    }
}

bool SF2::Hydra::isComplete()
{
    return !this->presetHeaderList.empty() &&
           !this->pbagItems.empty() &&
           !this->pmodItems.empty() &&
           !this->pgenItems.empty() &&
           !this->instItems.empty() &&
           !this->ibagItems.empty() &&
           !this->imodItems.empty() &&
           !this->igenItems.empty() &&
           !this->shdrItems.empty();
}

//===----------------------------------------------------------------------===//
// SoundFont2Generator
//===----------------------------------------------------------------------===//

struct SoundFont2Generator final
{
    enum Type
    {
        Word,
        Short,
        Range
    };

    const char *name;
    Type type;

    enum
    {
        startAddrsOffset,
        endAddrsOffset,
        startloopAddrsOffset,
        endloopAddrsOffset,
        startAddrsCoarseOffset,
        modLfoToPitch,
        vibLfoToPitch,
        modEnvToPitch,
        initialFilterFc,
        initialFilterQ,
        modLfoToFilterFc,
        modEnvToFilterFc,
        endAddrsCoarseOffset,
        modLfoToVolume,
        unused1,
        chorusEffectsSend,
        reverbEffectsSend,
        pan,
        unused2,
        unused3,
        unused4,
        delayModLFO,
        freqModLFO,
        delayVibLFO,
        freqVibLFO,
        delayModEnv,
        attackModEnv,
        holdModEnv,
        decayModEnv,
        sustainModEnv,
        releaseModEnv,
        keynumToModEnvHold,
        keynumToModEnvDecay,
        delayVolEnv,
        attackVolEnv,
        holdVolEnv,
        decayVolEnv,
        sustainVolEnv,
        releaseVolEnv,
        keynumToVolEnvHold,
        keynumToVolEnvDecay,
        instrument,
        reserved1,
        keyRange,
        velRange,
        startloopAddrsCoarseOffset,
        keynum,
        velocity,
        initialAttenuation,
        reserved2,
        endloopAddrsCoarseOffset,
        coarseTune,
        fineTune,
        sampleID,
        sampleModes,
        reserved3,
        scaleTuning,
        exclusiveClass,
        overridingRootKey,
        unused5,
        endOper
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFont2Generator)
};

static const SoundFont2Generator generators[] = {
    { "startAddrsOffset", SoundFont2Generator::Short },
    { "endAddrsOffset", SoundFont2Generator::Short },
    { "startloopAddrsOffset", SoundFont2Generator::Short },
    { "endloopAddrsOffset", SoundFont2Generator::Short },
    { "startAddrsCoarseOffset", SoundFont2Generator::Short },
    { "modLfoToPitch", SoundFont2Generator::Short },
    { "vibLfoToPitch", SoundFont2Generator::Short },
    { "modEnvToPitch", SoundFont2Generator::Short },
    { "initialFilterFc", SoundFont2Generator::Short },
    { "initialFilterQ", SoundFont2Generator::Short },
    { "modLfoToFilterFc", SoundFont2Generator::Short },
    { "modEnvToFilterFc", SoundFont2Generator::Short },
    { "endAddrsCoarseOffset", SoundFont2Generator::Short },
    { "modLfoToVolume", SoundFont2Generator::Short },
    { "unused1", SoundFont2Generator::Short },
    { "chorusEffectsSend", SoundFont2Generator::Short },
    { "reverbEffectsSend", SoundFont2Generator::Short },
    { "pan", SoundFont2Generator::Short },
    { "unused2", SoundFont2Generator::Short },
    { "unused3", SoundFont2Generator::Short },
    { "unused4", SoundFont2Generator::Short },
    { "delayModLFO", SoundFont2Generator::Short },
    { "freqModLFO", SoundFont2Generator::Short },
    { "delayVibLFO", SoundFont2Generator::Short },
    { "freqVibLFO", SoundFont2Generator::Short },
    { "delayModEnv", SoundFont2Generator::Short },
    { "attackModEnv", SoundFont2Generator::Short },
    { "holdModEnv", SoundFont2Generator::Short },
    { "decayModEnv", SoundFont2Generator::Short },
    { "sustainModEnv", SoundFont2Generator::Short },
    { "releaseModEnv", SoundFont2Generator::Short },
    { "keynumToModEnvHold", SoundFont2Generator::Short },
    { "keynumToModEnvDecay", SoundFont2Generator::Short },
    { "delayVolEnv", SoundFont2Generator::Short },
    { "attackVolEnv", SoundFont2Generator::Short },
    { "holdVolEnv", SoundFont2Generator::Short },
    { "decayVolEnv", SoundFont2Generator::Short },
    { "sustainVolEnv", SoundFont2Generator::Short },
    { "releaseVolEnv", SoundFont2Generator::Short },
    { "keynumToVolEnvHold", SoundFont2Generator::Short },
    { "keynumToVolEnvDecay", SoundFont2Generator::Short },
    { "instrument", SoundFont2Generator::Word },
    { "reserved1", SoundFont2Generator::Short },
    { "keyRange", SoundFont2Generator::Range },
    { "velRange", SoundFont2Generator::Range },
    { "startloopAddrsCoarseOffset", SoundFont2Generator::Short },
    { "keynum", SoundFont2Generator::Short },
    { "velocity", SoundFont2Generator::Short },
    { "initialAttenuation", SoundFont2Generator::Short },
    { "reserved2", SoundFont2Generator::Short },
    { "endloopAddrsCoarseOffset", SoundFont2Generator::Short },
    { "coarseTune", SoundFont2Generator::Short },
    { "fineTune", SoundFont2Generator::Short },
    { "sampleID", SoundFont2Generator::Word },
    { "sampleModes", SoundFont2Generator::Word },
    { "reserved3", SoundFont2Generator::Short },
    { "scaleTuning", SoundFont2Generator::Short },
    { "exclusiveClass", SoundFont2Generator::Short },
    { "overridingRootKey", SoundFont2Generator::Short },
    { "unused5", SoundFont2Generator::Short },
    { "endOper", SoundFont2Generator::Short }
};

const SoundFont2Generator *GeneratorFor(int index)
{
    static const int numGenerators = sizeof(generators) / sizeof(generators[0]);

    if (index >= numGenerators)
    {
        return nullptr;
    }

    return &generators[index];
}

//===----------------------------------------------------------------------===//
// SoundFont2Reader
//===----------------------------------------------------------------------===//

class SoundFont2Reader
{
public:

    virtual ~SoundFont2Reader() = default;

    SoundFont2Reader(SoundFont2Sound &sound, const File &file) :
        sf2Sound(sound),
        fileInputStream(file.createInputStream()) {}

    void readRegions();

    SharedAudioSampleBuffer::Ptr readSamples();

protected:

    SoundFont2Sound &sf2Sound;

    UniquePointer<FileInputStream> fileInputStream;

    Optional<RIFFChunk> seekToSampleSection();

    void addGeneratorToRegion(sf2word genOper, SF2::genAmountType *amount, SoundFontRegion *region);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFont2Reader)
};

void SoundFont2Reader::readRegions()
{
    if (this->fileInputStream == nullptr)
    {
        this->sf2Sound.addError("Couldn't open file.");
        return;
    }

    this->fileInputStream->setPosition(0);
    RIFFChunk riffChunk;
    riffChunk.readFrom(*this->fileInputStream);

    // Read the hydra.
    SF2::Hydra hydra;
    while (this->fileInputStream->getPosition() < riffChunk.end())
    {
        RIFFChunk chunk;
        chunk.readFrom(*this->fileInputStream);
        if (FourCCEquals(chunk.id, "pdta"))
        {
            hydra.readFrom(*this->fileInputStream, chunk.end());
            break;
        }
        chunk.seekAfter(*this->fileInputStream);
    }

    if (!hydra.isComplete())
    {
        this->sf2Sound.addError("Invalid SF2 file (missing or incomplete hydra).");
        return;
    }

    // Read each preset.
    for (size_t whichPreset = 0; whichPreset < hydra.presetHeaderList.size() - 1; ++whichPreset)
    {
        SF2::phdr *presetHeader = &hydra.presetHeaderList[whichPreset];
        auto preset = make<SoundFont2Sound::Preset>(presetHeader->presetName, presetHeader->bank, presetHeader->preset);

        // Zones.
        //*** TODO: Handle global zone (modulators only).
        int zoneEnd = hydra.presetHeaderList[whichPreset + 1].presetBagNdx;
        for (int whichZone = presetHeader->presetBagNdx; whichZone < zoneEnd; ++whichZone)
        {
            SF2::pbag *presetZone = &hydra.pbagItems[whichZone];
            SoundFontRegion presetRegion;
            presetRegion.clearForRelativeSF2();

            // Generators.
            int genEnd = hydra.pbagItems[whichZone + 1].genNdx;
            for (int whichGen = presetZone->genNdx; whichGen < genEnd; ++whichGen)
            {
                SF2::pgen *presetGenerator = &hydra.pgenItems[whichGen];

                // Instrument.
                if (presetGenerator->genOper == SoundFont2Generator::instrument)
                {
                    sf2word whichInst = presetGenerator->genAmount.wordAmount;
                    if (whichInst < hydra.instItems.size())
                    {
                        SoundFontRegion instRegion;
                        instRegion.clearForSF2();
                        // Preset generators are supposed to be "relative" modifications of
                        // the instrument settings, but that makes no sense for ranges.
                        // For those, we'll have the instrument's generator take
                        // precedence, though that may not be correct.
                        instRegion.lokey = presetRegion.lokey;
                        instRegion.hikey = presetRegion.hikey;
                        instRegion.lovel = presetRegion.lovel;
                        instRegion.hivel = presetRegion.hivel;

                        SF2::inst *inst = &hydra.instItems[whichInst];
                        int firstZone = inst->instBagNdx;
                        int zoneEnd2 = inst[1].instBagNdx;
                        for (int whichZone2 = firstZone; whichZone2 < zoneEnd2; ++whichZone2)
                        {
                            SF2::ibag *ibag = &hydra.ibagItems[whichZone2];

                            // Generators.
                            SoundFontRegion zoneRegion = instRegion;
                            bool hadSampleID = false;
                            int genEnd2 = ibag[1].instGenNdx;
                            for (int whichGen2 = ibag->instGenNdx; whichGen2 < genEnd2; ++whichGen2)
                            {
                                SF2::igen *igen = &hydra.igenItems[whichGen2];
                                if (igen->genOper == SoundFont2Generator::sampleID)
                                {
                                    int whichSample = igen->genAmount.wordAmount;
                                    SF2::shdr *shdr = &hydra.shdrItems[whichSample];
                                    zoneRegion.addForSF2(&presetRegion);
                                    zoneRegion.sf2ToSFZ();
                                    zoneRegion.offset += shdr->start;
                                    zoneRegion.end += shdr->end;
                                    zoneRegion.loopStart += shdr->startLoop;
                                    zoneRegion.loopEnd += shdr->endLoop;
                                    if (shdr->endLoop > 0)
                                    {
                                        zoneRegion.loopEnd -= 1;
                                    }
                                    if (zoneRegion.pitchKeyCenter == -1)
                                    {
                                        zoneRegion.pitchKeyCenter = shdr->originalPitch;
                                    }
                                    zoneRegion.tune += shdr->pitchCorrection;

                                    // Pin initialAttenuation to max +6dB.
                                    if (zoneRegion.volume > 6.0)
                                    {
                                        zoneRegion.volume = 6.0;
                                        this->sf2Sound.addUnsupportedOpcode("extreme gain in initialAttenuation");
                                    }

                                    // SoundFontRegion *newRegion = new SoundFontRegion();
                                    auto newRegion = make<SoundFontRegion>();
                                    *newRegion = zoneRegion;
                                    newRegion->sample = this->sf2Sound.getSampleFor(shdr->sampleRate).get();
                                    preset->addRegion(move(newRegion));
                                    hadSampleID = true;
                                }
                                else
                                {
                                    this->addGeneratorToRegion(igen->genOper, &igen->genAmount, &zoneRegion);
                                }
                            }

                            // Handle instrument's global zone.
                            if ((whichZone2 == firstZone) && !hadSampleID)
                            {
                                instRegion = zoneRegion;
                            }

                            // Modulators.
                            int modEnd = ibag[1].instModNdx;
                            int whichMod = ibag->instModNdx;
                            if (whichMod < modEnd)
                            {
                                this->sf2Sound.addUnsupportedOpcode("any modulator");
                            }
                        }
                    }
                    else
                    {
                        this->sf2Sound.addError("Instrument out of range.");
                    }
                }
                // Other generators.
                else
                {
                    addGeneratorToRegion(presetGenerator->genOper, &presetGenerator->genAmount, &presetRegion);
                }
            }

            // Modulators.
            int modEnd = presetZone[1].modNdx;
            int whichMod = presetZone->modNdx;
            if (whichMod < modEnd)
            {
                this->sf2Sound.addUnsupportedOpcode("any modulator");
            }
        }

        this->sf2Sound.addPreset(move(preset));
    }
}

Optional<RIFFChunk> SoundFont2Reader::seekToSampleSection()
{
    if (this->fileInputStream == nullptr)
    {
        this->sf2Sound.addError("Couldn't open file.");
        return {};
    }

    // Find the "sdta" chunk.
    this->fileInputStream->setPosition(0);
    RIFFChunk riffChunk;
    riffChunk.readFrom(*this->fileInputStream);
    bool found = false;
    RIFFChunk chunk;

    while (this->fileInputStream->getPosition() < riffChunk.end())
    {
        chunk.readFrom(*this->fileInputStream);
        if (FourCCEquals(chunk.id, "sdta"))
        {
            found = true;
            break;
        }
        chunk.seekAfter(*this->fileInputStream);
    }

    int64 sdtaEnd = chunk.end();
    found = false;
    while (this->fileInputStream->getPosition() < sdtaEnd)
    {
        chunk.readFrom(*this->fileInputStream);
        if (FourCCEquals(chunk.id, "smpl"))
        {
            found = true;
            break;
        }
        chunk.seekAfter(*this->fileInputStream);
    }

    if (!found)
    {
        this->sf2Sound.addError("SF2 is missing its \"smpl\" chunk.");
        return {};
    }

    return chunk;
}

SharedAudioSampleBuffer::Ptr SoundFont2Reader::readSamples()
{
    const auto samplesChunk = this->seekToSampleSection();
    if (!samplesChunk.hasValue())
    {
        jassertfalse;
        return nullptr;
    }

    const int numSamples = (int)samplesChunk->size / sizeof(short);
    SharedAudioSampleBuffer::Ptr sampleBuffer(new SharedAudioSampleBuffer(1, numSamples));

    // Read and convert.
    constexpr int bufferSize = 32768;
    HeapBlock<short> buffer(bufferSize);
    int samplesLeft = numSamples;
    float *out = sampleBuffer->getWritePointer(0);
    while (samplesLeft > 0)
    {
        // Read <= 32768 bytes at a time from the buffer
        int samplesToRead = bufferSize;
        if (samplesToRead > samplesLeft)
        {
            samplesToRead = samplesLeft;
        }

        this->fileInputStream->read(buffer.getData(), samplesToRead * sizeof(short));

        // Convert from signed 16-bit to float.
        int samplesToConvert = samplesToRead;
        short *in = buffer.getData();
        for (; samplesToConvert > 0; --samplesToConvert)
        {
            // If we ever need to compile for big-endian platforms,
            // we'll need to byte-swap here.
            *out++ = *in++ / 32767.f;
        }

        samplesLeft -= samplesToRead;
    }

    return sampleBuffer;
}

void SoundFont2Reader::addGeneratorToRegion(sf2word genOper, SF2::genAmountType *amount, SoundFontRegion *region)
{
    switch (genOper)
    {
        case SoundFont2Generator::startAddrsOffset:
            region->offset += amount->shortAmount;
            break;

        case SoundFont2Generator::endAddrsOffset:
            region->end += amount->shortAmount;
            break;

        case SoundFont2Generator::startloopAddrsOffset:
            region->loopStart += amount->shortAmount;
            break;

        case SoundFont2Generator::endloopAddrsOffset:
            region->loopEnd += amount->shortAmount;
            break;

        case SoundFont2Generator::startAddrsCoarseOffset:
            region->offset += amount->shortAmount * 32768;
            break;

        case SoundFont2Generator::endAddrsCoarseOffset:
            region->end += amount->shortAmount * 32768;
            break;

        case SoundFont2Generator::pan:
            region->pan = amount->shortAmount * (2.0f / 10.0f);
            break;

        case SoundFont2Generator::delayVolEnv:
            region->ampeg.delay = amount->shortAmount;
            break;

        case SoundFont2Generator::attackVolEnv:
            region->ampeg.attack = amount->shortAmount;
            break;

        case SoundFont2Generator::holdVolEnv:
            region->ampeg.hold = amount->shortAmount;
            break;

        case SoundFont2Generator::decayVolEnv:
            region->ampeg.decay = amount->shortAmount;
            break;

        case SoundFont2Generator::sustainVolEnv:
            region->ampeg.sustain = amount->shortAmount;
            break;

        case SoundFont2Generator::releaseVolEnv:
            region->ampeg.release = amount->shortAmount;
            break;

        case SoundFont2Generator::keyRange:
            region->lokey = amount->range.lo;
            region->hikey = amount->range.hi;
            break;

        case SoundFont2Generator::velRange:
            region->lovel = amount->range.lo;
            region->hivel = amount->range.hi;
            break;

        case SoundFont2Generator::startloopAddrsCoarseOffset:
            region->loopStart += amount->shortAmount * 32768;
            break;

        case SoundFont2Generator::initialAttenuation:
            // The spec says "initialAttenuation" is in centibels.  But everyone
            // seems to treat it as millibels.
            region->volume += -amount->shortAmount / 100.0f;
            break;

        case SoundFont2Generator::endloopAddrsCoarseOffset:
            region->loopEnd += amount->shortAmount * 32768;
            break;

        case SoundFont2Generator::coarseTune:
            region->transpose += amount->shortAmount;
            break;

        case SoundFont2Generator::fineTune:
            region->tune += amount->shortAmount;
            break;

        case SoundFont2Generator::sampleModes: {
            SoundFontRegion::LoopMode loopModes[] = {
                SoundFontRegion::LoopMode::noLoop,
                SoundFontRegion::LoopMode::loopContinuous,
                SoundFontRegion::LoopMode::noLoop,
                SoundFontRegion::LoopMode::loopSustain};

            region->loopMode = loopModes[amount->wordAmount & 0x03];
        }
        break;

        case SoundFont2Generator::scaleTuning:
            region->pitchKeyTrack = amount->shortAmount;
            break;

        case SoundFont2Generator::exclusiveClass:
            region->offBy = amount->wordAmount;
            region->group = static_cast<int>(region->offBy);
            break;

        case SoundFont2Generator::overridingRootKey:
            region->pitchKeyCenter = amount->shortAmount;
            break;

        case SoundFont2Generator::endOper:
            // Ignore.
            break;

        case SoundFont2Generator::modLfoToPitch:
        case SoundFont2Generator::vibLfoToPitch:
        case SoundFont2Generator::modEnvToPitch:
        case SoundFont2Generator::initialFilterFc:
        case SoundFont2Generator::initialFilterQ:
        case SoundFont2Generator::modLfoToFilterFc:
        case SoundFont2Generator::modEnvToFilterFc:
        case SoundFont2Generator::modLfoToVolume:
        case SoundFont2Generator::unused1:
        case SoundFont2Generator::chorusEffectsSend:
        case SoundFont2Generator::reverbEffectsSend:
        case SoundFont2Generator::unused2:
        case SoundFont2Generator::unused3:
        case SoundFont2Generator::unused4:
        case SoundFont2Generator::delayModLFO:
        case SoundFont2Generator::freqModLFO:
        case SoundFont2Generator::delayVibLFO:
        case SoundFont2Generator::freqVibLFO:
        case SoundFont2Generator::delayModEnv:
        case SoundFont2Generator::attackModEnv:
        case SoundFont2Generator::holdModEnv:
        case SoundFont2Generator::decayModEnv:
        case SoundFont2Generator::sustainModEnv:
        case SoundFont2Generator::releaseModEnv:
        case SoundFont2Generator::keynumToModEnvHold:
        case SoundFont2Generator::keynumToModEnvDecay:
        case SoundFont2Generator::keynumToVolEnvHold:
        case SoundFont2Generator::keynumToVolEnvDecay:
        case SoundFont2Generator::instrument:
            // Only allowed in certain places, where we already special-case it.
        case SoundFont2Generator::reserved1:
        case SoundFont2Generator::keynum:
        case SoundFont2Generator::velocity:
        case SoundFont2Generator::reserved2:
        case SoundFont2Generator::sampleID:
            // Only allowed in certain places, where we already special-case it.
        case SoundFont2Generator::reserved3:
        case SoundFont2Generator::unused5: {
            const SoundFont2Generator *generator = GeneratorFor(static_cast<int>(genOper));
            this->sf2Sound.addUnsupportedOpcode(generator->name);
        }
        break;
    }
}

//===----------------------------------------------------------------------===//
// SoundFont2Sound
//===----------------------------------------------------------------------===//

SoundFont2Sound::SoundFont2Sound(const File &file) : SoundFontSound(file) {}

SoundFont2Sound::~SoundFont2Sound() = default;

class PresetComparator final
{
public:

    static int compareElements(const SoundFont2Sound::Preset *first, const SoundFont2Sound::Preset *second)
    {
        const auto bankDiff = first->bank - second->bank;
        if (bankDiff != 0)
        {
            return bankDiff;
        }

        return first->preset - second->preset;
    }
};

void SoundFont2Sound::loadRegions()
{
    SoundFont2Reader reader(*this, this->file);
    reader.readRegions();

    PresetComparator comparator;
    this->presets.sort(comparator);

    this->setSelectedPreset(0);
}

void SoundFont2Sound::loadSamples(AudioFormatManager &formatManager)
{
    /*
        each SoundFont2Sound is given a File as the source of the actual sample data when they're created
        this reader adds any errors encountered while reading to the SoundFont2Sound object
    */
    SoundFont2Reader reader(*this, this->file);
    const auto buffer = reader.readSamples();

    if (buffer)
    {
        // All the SFZSamples will share the buffer.
        for (auto &sample : this->samplesByRate)
        {
            sample.second->setBuffer(buffer);
        }
    }
}

void SoundFont2Sound::addPreset(UniquePointer<SoundFont2Sound::Preset> &&preset)
{
    this->presets.add(preset.release());
}

int SoundFont2Sound::getNumPresets() const
{
    return this->presets.size();
}

String SoundFont2Sound::getPresetName(int whichSubsound) const
{
    Preset *preset = this->presets[whichSubsound];
    String result;

    if (preset->bank != 0)
    {
        result += preset->bank;
        result += "/";
    }
    result += preset->preset;
    result += ": ";
    result += preset->name;
    return result;
}

void SoundFont2Sound::setSelectedPreset(int whichPreset)
{
    this->selectedPreset = whichPreset;
    this->regions.clear();
    if (!this->presets.isEmpty())
    {
        this->regions.addArray(this->presets[whichPreset]->regions);
    }
}

int SoundFont2Sound::getSelectedPreset() const
{
    return this->selectedPreset;
}

WeakReference<SoundFontSample> SoundFont2Sound::getSampleFor(double sampleRate)
{
    if (!this->samplesByRate.contains(int(sampleRate)))
    {
        this->samplesByRate[int(sampleRate)] = make<SoundFontSample>(sampleRate);
    }

    return this->samplesByRate[int(sampleRate)].get();
}

//===----------------------------------------------------------------------===//
// SF3
//===----------------------------------------------------------------------===//

class SoundFont3Reader final : public SoundFont2Reader
{
public:

    SoundFont3Reader(SoundFont2Sound &sound, const File &file) :
        SoundFont2Reader(sound, file) {}

    MemoryBlock readSamplesSection()
    {
        const auto samplesChunk = this->seekToSampleSection();
        if (!samplesChunk.hasValue())
        {
            jassertfalse;
            return {};
        }

        MemoryBlock result;
        this->fileInputStream->readIntoMemoryBlock(result, samplesChunk->size);
        return result;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFont3Reader)
};

struct SampleRangeHash
{
    inline HashCode operator()(const Range<int64> &key) const noexcept
    {
        // using only the start point should be enough for the hash,
        // we don't expect different samples to start at the same point
        return static_cast<HashCode>(key.getStart());
    }
};

SoundFont3Sound::SoundFont3Sound(const File &file) : SoundFont2Sound(file) {}

SoundFont3Sound::~SoundFont3Sound() = default;

void SoundFont3Sound::loadSamples(AudioFormatManager &formatManager)
{
    SoundFont3Reader soundFontReader(*this, this->file);
    const auto samplesBlock = soundFontReader.readSamplesSection();
    if (samplesBlock.isEmpty())
    {
        jassertfalse;
        return;
    }

    const auto *sampleBlockStart = static_cast<const char *>(samplesBlock.getData());

    OggVorbisAudioFormat oggVorbisAudioFormat;
    FlacAudioFormat flacAudioFormat;

    // ranges in bytes in compressed stream
    // to ranges in 16-bit sample data points in uncompressed stream
    FlatHashMap<Range<int64>, Range<int64>, SampleRangeHash> decompressedRanges;

    SharedAudioSampleBuffer::Ptr sampleBuffer(new SharedAudioSampleBuffer(1, 0));
    int64 currentSampleOffset = 0; // in result buffer

    // TODO lazy loading, i.e. only load one preset at a time in setSelectedPreset()?

    jassert(!this->presets.isEmpty()); // should have called loadRegions before
    for (auto *preset : this->presets)
    {
        // decompress samples and re-calculate regions' sample offsets
        for (auto *region : preset->regions)
        {
            const Range<int64> compressedByteRange(region->offset, region->end);
            const auto foundDecompressedRange = decompressedRanges.find(compressedByteRange);
            if (foundDecompressedRange != decompressedRanges.end())
            {
                // the decompressed region is already present in the shared buffer
                region->offset = foundDecompressedRange->second.getStart();
                region->end = foundDecompressedRange->second.getEnd();
                continue;
            }

            DBG("Reading sample at " + String(region->offset));

            jassert(region->end <= int64(samplesBlock.getSize()));
            jassert(region->offset < int64(samplesBlock.getSize()));

            const auto *readStart = static_cast<const void *>(sampleBlockStart + region->offset);
            const auto readLength = region->end > 0 ? size_t(region->end - region->offset) :
                size_t(samplesBlock.getSize() - region->offset);

            UniquePointer<AudioFormatReader> sampleReader(oggVorbisAudioFormat
                .createReaderFor(new MemoryInputStream(readStart, readLength, false), true));

            if (sampleReader == nullptr)
            {
                sampleReader = UniquePointer<AudioFormatReader>(flacAudioFormat
                    .createReaderFor(new MemoryInputStream(readStart, readLength, false), true));
            }

            if (sampleReader == nullptr)
            {
                jassertfalse;
                continue;
            }

            sampleBuffer->setSize(sampleReader->numChannels, int(currentSampleOffset + sampleReader->lengthInSamples), true, true);
            sampleReader->read(sampleBuffer.get(), int(currentSampleOffset), int(sampleReader->lengthInSamples), 0, true, true);

            const Range<int64> decompressedSampleRange(currentSampleOffset, currentSampleOffset + sampleReader->lengthInSamples);

            decompressedRanges[compressedByteRange] = decompressedSampleRange;

            region->offset = decompressedSampleRange.getStart();
            region->end = decompressedSampleRange.getEnd();

            currentSampleOffset += sampleReader->lengthInSamples;
        }
    }

    // In SF3, loop start and end are based on the beginning of each sample,
    // we need them to be based on the beginning of the decompressed sample buffer
    for (auto *preset : this->presets)
    {
        for (auto *region : preset->regions)
        {
            region->loopStart += region->offset;
            region->loopEnd += region->offset;
        }
    }

    for (auto &sample : this->samplesByRate)
    {
        sample.second->setBuffer(sampleBuffer);
    }
}
