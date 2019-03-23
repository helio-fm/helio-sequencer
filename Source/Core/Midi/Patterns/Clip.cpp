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

Clip::Clip() :
    pattern(nullptr),
    key(0),
    beat(0.f),
    velocity(1.f) {}

Clip::Clip(WeakReference<Pattern> owner, float beatVal) :
    pattern(owner),
    key(0),
    beat(roundBeat(beatVal)),
    velocity(1.f)
{
    id = this->createId();
    this->updateCaches();
}

Clip::Clip(WeakReference<Pattern> owner, const Clip &parametersToCopy) :
    pattern(owner),
    key(parametersToCopy.key),
    beat(parametersToCopy.beat),
    velocity(parametersToCopy.velocity),
    id(parametersToCopy.id) 
{
    this->updateCaches();
}

Pattern *Clip::getPattern() const noexcept
{
    //jassert(this->pattern != nullptr);
    return this->pattern;
}

int Clip::getKey() const noexcept
{
    return this->key;
}

float Clip::getBeat() const noexcept
{
    return this->beat;
}

float Clip::getVelocity() const noexcept
{
    return this->velocity;
}

const String &Clip::getId() const noexcept
{
    return this->id;
}

const String &Clip::getKeyString() const noexcept
{
    return this->keyString;
}

bool Clip::isValid() const noexcept
{
    return this->pattern != nullptr && this->id.isNotEmpty();
}

const String &Clip::getTrackId() const noexcept
{
    jassert(this->pattern);
    return this->pattern->getTrack()->getTrackId();
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

Clip Clip::withKey(int absKey) const
{
    Clip other(*this);
    other.key = jlimit(-128, 128, absKey);
    other.updateCaches();
    return other;
}

Clip Clip::withDeltaKey(int deltaKey) const
{
    Clip other(*this);
    other.key = jlimit(-128, 128, other.key + deltaKey);
    other.updateCaches();
    return other;
}

Clip Clip::withVelocity(float absVelocity) const
{
    Clip other(*this);
    other.velocity = jlimit(0.f, 1.f, absVelocity);
    return other;
}

Clip Clip::withDeltaVelocity(float deltaVelocity) const
{
    Clip other(*this);
    other.velocity = jlimit(0.f, 1.f, other.velocity + deltaVelocity);
    return other;
}

ValueTree Clip::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Midi::clip);
    tree.setProperty(Midi::key, this->key, nullptr);
    tree.setProperty(Midi::timestamp, int(this->beat * TICKS_PER_BEAT), nullptr);
    tree.setProperty(Midi::volume, int(this->velocity * VELOCITY_SAVE_ACCURACY), nullptr);
    tree.setProperty(Midi::id, this->id, nullptr);
    return tree;
}

void Clip::deserialize(const ValueTree &tree)
{
    using namespace Serialization;
    this->key = tree.getProperty(Midi::key, 0);
    this->beat = float(tree.getProperty(Midi::timestamp)) / TICKS_PER_BEAT;
    this->id = tree.getProperty(Midi::id, this->id);
    const auto vol = float(tree.getProperty(Midi::volume, VELOCITY_SAVE_ACCURACY)) / VELOCITY_SAVE_ACCURACY;
    this->velocity = jmax(jmin(vol, 1.f), 0.f);
    this->updateCaches();
}

void Clip::reset()
{
    this->key = 0;
    this->beat = 0.f;
    this->velocity = 1.f;
    this->updateCaches();
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
    this->key = other.key;
    this->beat = other.beat;
    this->velocity = other.velocity;
    this->updateCaches();
}

Clip::Id Clip::createId() const noexcept
{
    if (this->pattern != nullptr)
    {
        return this->pattern->createUniqueClipId();
    }

    return {};
}

void Clip::updateCaches() const
{
    if (this->key > 0)
    {
        this->keyString = " +" + String(this->key);
    }
    else if (this->key < 0)
    {
        this->keyString = String(this->key);
    }
    else
    {
        this->keyString = {};
    }
}
