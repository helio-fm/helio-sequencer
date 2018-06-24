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
#include "Clip.h"
#include "Pattern.h"
#include "MidiTrack.h"
#include "SerializationKeys.h"

Clip::Clip() : pattern(nullptr), beat(0.f)
{
    // needed for juce's Array to work
    //jassertfalse;
}

Clip::Clip(const Clip &other) :
    pattern(other.pattern),
    beat(other.beat),
    id(other.id) {}

Clip::Clip(WeakReference<Pattern> owner, float beatVal) :
    pattern(owner),
    beat(roundBeat(beatVal))
{
    id = this->createId();
}

Clip::Clip(WeakReference<Pattern> owner, const Clip &parametersToCopy) :
    pattern(owner),
    beat(parametersToCopy.beat),
    id(parametersToCopy.id) {}

Pattern *Clip::getPattern() const noexcept
{
    //jassert(this->pattern != nullptr);
    return this->pattern;
}

float Clip::getBeat() const noexcept
{
    return this->beat;
}

const String &Clip::getId() const noexcept
{
    return this->id;
}

bool Clip::isValid() const noexcept
{
    return this->pattern != nullptr && this->id.isNotEmpty();
}

Colour Clip::getTrackColour() const noexcept
{
    jassert(this->pattern);
    return this->pattern->getTrack()->getTrackColour();
}

int Clip::getTrackControllerNumber() const noexcept
{
    jassert(this->pattern);
    return this->pattern->getTrack()->getTrackControllerNumber();
}

Clip Clip::copyWithNewId(Pattern *newOwner) const
{
    Clip c(*this);

    if (newOwner != nullptr)
    {
        c.pattern = newOwner;
    }

    c.id = c.createId();
    return c;
}

Clip Clip::withParameters(const ValueTree &tree) const
{
    Clip c(*this);
    c.deserialize(tree);
    return c;
}

Clip Clip::withBeat(float absPosition) const
{
    Clip other(*this);
    other.beat = roundBeat(absPosition);
    return other;
}

Clip Clip::withDeltaBeat(float deltaPosition) const
{
    Clip other(*this);
    other.beat = roundBeat(other.beat + deltaPosition);
    return other;
}

ValueTree Clip::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Midi::clip);
    tree.setProperty(Midi::timestamp, roundToInt(this->beat * TICKS_PER_BEAT), nullptr);
    tree.setProperty(Midi::id, this->id, nullptr);
    return tree;
}

void Clip::deserialize(const ValueTree &tree)
{
    using namespace Serialization;
    this->beat = float(tree.getProperty(Midi::timestamp)) / TICKS_PER_BEAT;
    this->id = tree.getProperty(Midi::id, this->id);
}

void Clip::reset()
{
    this->beat = 0.f;
}

int Clip::compareElements(const Clip &first, const Clip &second)
{
    if (&first == &second) { return 0; }

    const float diff = first.beat - second.beat;
    const int diffResult = (diff > 0.f) - (diff < 0.f);
    if (diffResult != 0) { return diffResult; }

    return first.id.compare(second.id);
}

int Clip::compareElements(const Clip *const first, const Clip *const second)
{
    return Clip::compareElements(*first, *second);
}

void Clip::applyChanges(const Clip &other)
{
    jassert(this->id == other.id);
    this->beat = other.beat;
}

HashCode Clip::hashCode() const noexcept
{
    const HashCode code = static_cast<HashCode>(this->beat)
        + static_cast<HashCode>(this->getId().hashCode());
    return code;
}

Clip::Id Clip::createId() const noexcept
{
    if (this->pattern != nullptr)
    {
        return this->pattern->createUniqueClipId();
    }

    return {};
}
