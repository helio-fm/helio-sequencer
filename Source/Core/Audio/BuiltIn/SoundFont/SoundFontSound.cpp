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
#include "SoundFontSound.h"
#include "SoundFontRegion.h"
#include "SoundFontSample.h"

class SoundFontReader final
{
public:

    explicit SoundFontReader(SoundFontSound *sound) : sound(sound) {}
    ~SoundFontReader() = default;

    void read(const File &file);
    void read(const char *text, unsigned int length);

private:

    const char *handleLineEnd(const char *p);
    const char *readPathInto(String *pathOut, const char *p, const char *end);
    int parseKeyValue(const String &str);
    void finishRegion(SoundFontRegion &regionToCopyFrom);
    void addError(const String &message);

    static SoundFontRegion::Trigger parseTriggerValue(const String &str);
    static SoundFontRegion::LoopMode parseLoopModeValue(const String &str);

    SoundFontSound *sound = nullptr;
    int line = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontReader)
};

class StringSlice final
{
public:

    StringSlice(const char *startIn, const char *endIn) : start(startIn), end(endIn) {}

    unsigned int length() { return static_cast<int>(this->end - this->start); }
    bool operator==(const char *other) { return (strncmp(this->start, other, length()) == 0); }
    bool operator!=(const char *other) { return (strncmp(this->start, other, length()) != 0); }
    const char *getStart() const { return this->start; }
    const char *getEnd() const { return this->end; }

private:

    const char *start = nullptr;
    const char *end = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringSlice)
};

void SoundFontReader::read(const File &file)
{
    MemoryBlock contents;

    if (!file.loadFileAsData(contents))
    {
        this->sound->addError("Couldn't read \"" + file.getFullPathName() + "\"");
        return;
    }

    this->read(static_cast<const char *>(contents.getData()), static_cast<int>(contents.getSize()));
}

void SoundFontReader::read(const char *text, unsigned int length)
{
    const char *p = text;
    const char *end = text + length;
    char c = 0;

    SoundFontRegion currentGroup;
    SoundFontRegion currentRegion;
    SoundFontRegion *buildingRegion = nullptr;
    bool inControl = false;
    String defaultPath;

    while (p < end)
    {
        // We're at the start of a line; skip any whitespace.
        while (p < end)
        {
            c = *p;
            if ((c != ' ') && (c != '\t'))
            {
                break;
            }
            p += 1;
        }
        if (p >= end)
        {
            break;
        }

        // Check if it's a comment line.
        if (c == '/')
        {
            // Skip to end of line.
            while (p < end)
            {
                c = *++p;
                if ((c == '\n') || (c == '\r'))
                {
                    break;
                }
            }
            p = this->handleLineEnd(p);
            continue;
        }

        // Check if it's a blank line.
        if ((c == '\r') || (c == '\n'))
        {
            p = this->handleLineEnd(p);
            continue;
        }

        // Handle elements on the line.
        while (p < end)
        {
            c = *p;

            // Tag.
            if (c == '<')
            {
                p += 1;
                const char *tagStart = p;
                while (p < end)
                {
                    c = *p++;
                    if ((c == '\n') || (c == '\r'))
                    {
                        this->addError("Unterminated tag");
                        goto fatalError;
                    }
                    else if (c == '>')
                    {
                        break;
                    }
                }
                if (p >= end)
                {
                    this->addError("Unterminated tag");
                    goto fatalError;
                }
                StringSlice tag(tagStart, p - 1);
                if (tag == "region")
                {
                    if (buildingRegion && (buildingRegion == &currentRegion))
                    {
                        this->finishRegion(currentRegion);
                    }
                    currentRegion = currentGroup;
                    buildingRegion = &currentRegion;
                    inControl = false;
                }
                else if (tag == "group")
                {
                    if (buildingRegion && (buildingRegion == &currentRegion))
                    {
                        this->finishRegion(currentRegion);
                    }
                    currentGroup.clear();
                    buildingRegion = &currentGroup;
                    inControl = false;
                }
                else if (tag == "control")
                {
                    if (buildingRegion && (buildingRegion == &currentRegion))
                    {
                        this->finishRegion(currentRegion);
                    }
                    currentGroup.clear();
                    buildingRegion = nullptr;
                    inControl = true;
                }
                else
                {
                    this->addError("Illegal tag");
                }
            }
            // Comment.
            else if (c == '/')
            {
                // Skip to end of line.
                while (p < end)
                {
                    c = *p;
                    if ((c == '\r') || (c == '\n'))
                    {
                        break;
                    }
                    p += 1;
                }
            }
            // Parameter.
            else
            {
                // Get the parameter name.
                const char *parameterStart = p;
                while (p < end)
                {
                    c = *p++;
                    if ((c == '=') || (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
                    {
                        break;
                    }
                }
                if ((p >= end) || (c != '='))
                {
                    this->addError("Malformed parameter");
                    goto nextElement;
                }
                StringSlice opcode(parameterStart, p - 1);
                if (inControl)
                {
                    if (opcode == "default_path")
                    {
                        p = this->readPathInto(&defaultPath, p, end);
                    }
                    else
                    {
                        const char *valueStart = p;
                        while (p < end)
                        {
                            c = *p;
                            if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
                            {
                                break;
                            }
                            p++;
                        }
                        String value(valueStart, p - valueStart);
                        String fauxOpcode = String(opcode.getStart(), opcode.length()) + " (in <control>)";
                        this->sound->addUnsupportedOpcode(fauxOpcode);
                    }
                }
                else if (opcode == "sample")
                {
                    String path;
                    p = this->readPathInto(&path, p, end);
                    if (!path.isEmpty())
                    {
                        if (buildingRegion)
                        {
                            buildingRegion->sample = this->sound->addSample(path, defaultPath);
                        }
                        else
                        {
                            this->addError("Adding sample outside a group or region");
                        }
                    }
                    else
                    {
                        this->addError("Empty sample path");
                    }
                }
                else
                {
                    const char *valueStart = p;
                    while (p < end)
                    {
                        c = *p;
                        if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
                        {
                            break;
                        }
                        p++;
                    }
                    String value(valueStart, p - valueStart);
                    if (buildingRegion == nullptr)
                    {
                        this->addError("Setting a parameter outside a region or group");
                    }
                    else if (opcode == "lokey")
                    {
                        buildingRegion->lokey = this->parseKeyValue(value);
                    }
                    else if (opcode == "hikey")
                    {
                        buildingRegion->hikey = this->parseKeyValue(value);
                    }
                    else if (opcode == "key")
                    {
                        buildingRegion->hikey = buildingRegion->lokey = buildingRegion->pitchKeyCenter = parseKeyValue(value);
                    }
                    else if (opcode == "lovel")
                    {
                        buildingRegion->lovel = value.getIntValue();
                    }
                    else if (opcode == "hivel")
                    {
                        buildingRegion->hivel = value.getIntValue();
                    }
                    else if (opcode == "trigger")
                    {
                        buildingRegion->trigger = this->parseTriggerValue(value);
                    }
                    else if (opcode == "group")
                    {
                        buildingRegion->group = static_cast<int>(value.getLargeIntValue());
                    }
                    else if (opcode == "off_by")
                    {
                        buildingRegion->offBy = value.getLargeIntValue();
                    }
                    else if (opcode == "offset")
                    {
                        buildingRegion->offset = value.getLargeIntValue();
                    }
                    else if (opcode == "end")
                    {
                        int64 end2 = value.getLargeIntValue();
                        if (end2 < 0)
                        {
                            buildingRegion->negativeEnd = true;
                        }
                        else
                        {
                            buildingRegion->end = end2;
                        }
                    }
                    else if (opcode == "loop_mode")
                    {
                        bool modeIsSupported = value == "no_loop" || value == "one_shot" || value == "loop_continuous";
                        if (modeIsSupported)
                        {
                            buildingRegion->loopMode = this->parseLoopModeValue(value);
                        }
                        else
                        {
                            const auto fauxOpcode = String(opcode.getStart(), opcode.length()) + "=" + value;
                            this->sound->addUnsupportedOpcode(fauxOpcode);
                        }
                    }
                    else if (opcode == "loop_start")
                    {
                        buildingRegion->loopStart = value.getLargeIntValue();
                    }
                    else if (opcode == "loop_end")
                    {
                        buildingRegion->loopEnd = value.getLargeIntValue();
                    }
                    else if (opcode == "transpose")
                    {
                        buildingRegion->transpose = value.getIntValue();
                    }
                    else if (opcode == "tune")
                    {
                        buildingRegion->tune = value.getIntValue();
                    }
                    else if (opcode == "pitch_keycenter")
                    {
                        buildingRegion->pitchKeyCenter = parseKeyValue(value);
                    }
                    else if (opcode == "pitch_keytrack")
                    {
                        buildingRegion->pitchKeyTrack = value.getIntValue();
                    }
                    else if (opcode == "bthis->endup")
                    {
                        buildingRegion->bendUp = value.getIntValue();
                    }
                    else if (opcode == "bthis->enddown")
                    {
                        buildingRegion->bendDown = value.getIntValue();
                    }
                    else if (opcode == "volume")
                    {
                        buildingRegion->volume = value.getFloatValue();
                    }
                    else if (opcode == "pan")
                    {
                        buildingRegion->pan = value.getFloatValue();
                    }
                    else if (opcode == "amp_veltrack")
                    {
                        buildingRegion->ampVelTrack = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_delay")
                    {
                        buildingRegion->ampeg.delay = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_start")
                    {
                        buildingRegion->ampeg.start = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_attack")
                    {
                        buildingRegion->ampeg.attack = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_hold")
                    {
                        buildingRegion->ampeg.hold = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_decay")
                    {
                        buildingRegion->ampeg.decay = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_sustain")
                    {
                        buildingRegion->ampeg.sustain = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_release")
                    {
                        buildingRegion->ampeg.release = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2delay")
                    {
                        buildingRegion->ampegVelTrack.delay = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2attack")
                    {
                        buildingRegion->ampegVelTrack.attack = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2hold")
                    {
                        buildingRegion->ampegVelTrack.hold = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2decay")
                    {
                        buildingRegion->ampegVelTrack.decay = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2sustain")
                    {
                        buildingRegion->ampegVelTrack.sustain = value.getFloatValue();
                    }
                    else if (opcode == "ampeg_vel2release")
                    {
                        buildingRegion->ampegVelTrack.release = value.getFloatValue();
                    }
                    else if (opcode == "default_path")
                    {
                        this->addError("\"default_path\" outside of <control> tag");
                    }
                    else
                    {
                        this->sound->addUnsupportedOpcode(String(opcode.getStart(), opcode.length()));
                    }
                }
            }

            // Skip to next element.
        nextElement:
            c = 0;
            while (p < end)
            {
                c = *p;
                if ((c != ' ') && (c != '\t'))
                {
                    break;
                }
                p += 1;
            }
            if ((c == '\r') || (c == '\n'))
            {
                p = this->handleLineEnd(p);
                break;
            }
        }
    }

fatalError:

    if (buildingRegion && (buildingRegion == &currentRegion))
    {
        this->finishRegion(currentRegion);
    }
}

const char *SoundFontReader::handleLineEnd(const char *p)
{
    // Check for DOS-style line ending.
    char lineEndChar = *p++;

    if ((lineEndChar == '\r') && (*p == '\n'))
    {
        p += 1;
    }

    this->line += 1;
    return p;
}

const char *SoundFontReader::readPathInto(String *pathOut, const char *pIn, const char *endIn)
{
    // Paths are kind of funny to parse because they can contain whitespace.
    const char *p = pIn;
    const char *end = endIn;
    const char *pathStart = p;
    const char *potentialEnd = nullptr;

    while (p < end)
    {
        char c = *p;
        if (c == ' ')
        {
            // Is this space part of the path?  Or the start of the next opcode?  We
            // don't know yet.
            potentialEnd = p;
            p += 1;
            // Skip any more spaces.
            while (p < end && *p == ' ')
            {
                p += 1;
            }
        }
        else if ((c == '\n') || (c == '\r') || (c == '\t'))
        {
            break;
        }
        else if (c == '=')
        {
            // We've been looking at an opcode; we need to rewind to
            // potentialEnd.
            p = potentialEnd;
            break;
        }
        p += 1;
    }
    if (p > pathStart)
    {
        // Can't do this:
        //      String path(CharPointer_UTF8(pathStart), CharPointer_UTF8(p));
        // It won't compile for some unfathomable reason.
        CharPointer_UTF8 end2(p);
        String path(CharPointer_UTF8(pathStart), end2);
        *pathOut = path;
    }
    else
    {
        *pathOut = String();
    }
    return p;
}

int SoundFontReader::parseKeyValue(const String &str)
{
    auto chars = str.toRawUTF8();

    char c = chars[0];

    if ((c >= '0') && (c <= '9'))
    {
        return str.getIntValue();
    }

    int note = 0;
    static const int notes[] = {
        12 + 0,
        12 + 2,
        3,
        5,
        7,
        8,
        10,
    };

    if ((c >= 'A') && (c <= 'G'))
    {
        note = notes[c - 'A'];
    }
    else if ((c >= 'a') && (c <= 'g'))
    {
        note = notes[c - 'a'];
    }

    int octaveStart = 1;

    c = chars[1];
    if ((c == 'b') || (c == '#'))
    {
        octaveStart += 1;
        if (c == 'b')
        {
            note -= 1;
        }
        else
        {
            note += 1;
        }
    }

    const int octave = str.substring(octaveStart).getIntValue();
    // A3 == 57.
    return octave * 12 + note + (57 - 4 * 12);
}

SoundFontRegion::Trigger SoundFontReader::parseTriggerValue(const String &str)
{
    if (str == "release")
    {
        return SoundFontRegion::Trigger::release;
    }
    if (str == "first")
    {
        return SoundFontRegion::Trigger::first;
    }
    if (str == "legato")
    {
        return SoundFontRegion::Trigger::legato;
    }

    return SoundFontRegion::Trigger::attack;
}

SoundFontRegion::LoopMode SoundFontReader::parseLoopModeValue(const String &str)
{
    if (str == "no_loop")
    {
        return SoundFontRegion::LoopMode::noLoop;
    }
    if (str == "one_shot")
    {
        return SoundFontRegion::LoopMode::oneShot;
    }
    if (str == "loop_continuous")
    {
        return SoundFontRegion::LoopMode::loopContinuous;
    }
    if (str == "loop_sustain")
    {
        return SoundFontRegion::LoopMode::loopSustain;
    }

    return SoundFontRegion::LoopMode::sampleLoop;
}

void SoundFontReader::finishRegion(SoundFontRegion &regionToCopyFrom)
{
    auto newRegion = make<SoundFontRegion>();
    *newRegion = regionToCopyFrom;
    this->sound->addRegion(move(newRegion));
}

void SoundFontReader::addError(const String &message)
{
    const auto fullMessage = message + " (line " + String(this->line) + ").";
    this->sound->addError(fullMessage);
}

//===----------------------------------------------------------------------===//
// SoundFontSound
//===----------------------------------------------------------------------===//

SoundFontSound::SoundFontSound(const File &fileIn) : file(fileIn)
{
    this->preset = make<Preset>();
}

SoundFontSound::~SoundFontSound() = default;

bool SoundFontSound::appliesToNote(int /*midiNoteNumber*/)
{
    // Just say yes; we can't truly know unless we're told the velocity as well.
    return true;
}

bool SoundFontSound::appliesToChannel(int /*midiChannel*/) { return true; }

void SoundFontSound::addRegion(UniquePointer<SoundFontRegion> &&region)
{
    this->regions.add(region.get());
    this->preset->addRegion(move(region));
}

WeakReference<SoundFontSample> SoundFontSound::addSample(String path, String defaultPath)
{
    path = path.replaceCharacter('\\', '/');
    defaultPath = defaultPath.replaceCharacter('\\', '/');

    File sampleFile;
    if (defaultPath.isEmpty())
    {
        sampleFile = this->file.getSiblingFile(path);
    }
    else
    {
        const auto defaultDir = this->file.getSiblingFile(defaultPath);
        sampleFile = defaultDir.getChildFile(path);
    }

    const auto samplePath = sampleFile.getFullPathName();
    if (!this->samples.contains(samplePath))
    {
        this->samples[samplePath] = make<SoundFontSample>(sampleFile);
    }

    return this->samples[samplePath].get();
}

void SoundFontSound::addError(const String &message) { this->errors.add(message); }

void SoundFontSound::addUnsupportedOpcode(const String &opcode)
{
    if (!this->unsupportedOpcodes.contains(opcode))
    {
        this->unsupportedOpcodes[opcode] = opcode;
        String warning = "unsupported opcode: ";
        warning << opcode;
        this->warnings.add(warning);
    }
}

void SoundFontSound::loadRegions()
{
    SoundFontReader reader(this);
    reader.read(this->file);
}

void SoundFontSound::loadSamples(AudioFormatManager &formatManager)
{
    for (auto &it : this->samples)
    {
        const bool ok = it.second->load(formatManager);
        if (!ok)
        {
            this->addError("Couldn't load sample \"" + it.second->getShortName() + "\"");
        }
    }
}

SoundFontRegion *SoundFontSound::getRegionFor(int note,
    int velocity, SoundFontRegion::Trigger trigger) const
{
    jassert(this->temperament != nullptr);
    const auto periodSize = this->temperament->getPeriodSize();

    for (auto *region : this->regions)
    {
        if (region->matches(note, velocity, trigger, periodSize))
        {
            return region;
        }
    }

    return nullptr;
}

int SoundFontSound::getNumRegions() const { return this->regions.size(); }

SoundFontRegion *SoundFontSound::regionAt(int index) { return this->regions[index]; }

int SoundFontSound::getNumPresets() const { return 1; }

String SoundFontSound::getPresetName(int) const { return this->preset->name; }

void SoundFontSound::setSelectedPreset(int) {}

int SoundFontSound::getSelectedPreset() const { return 0; }
