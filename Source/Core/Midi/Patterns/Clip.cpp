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
*/

#include "Common.h"
#include "Clip.h"
#include "Pattern.h"
#include "MidiTrack.h"
#include "SerializationKeys.h"

#include "RefactoringSequenceModifier.h"
#include "TuningSequenceModifier.h"
#include "ArpeggiationSequenceModifier.h"

Clip::Clip() : pattern(nullptr) {}

Clip::Clip(WeakReference<Pattern> owner, float beatVal, int key) :
    pattern(owner),
    beat(roundBeat(beatVal)),
    key(key)
{
    id = this->createId();
    this->updateCaches();
}

Clip::Clip(WeakReference<Pattern> owner, const Clip &parametersToCopy) :
    pattern(owner),
    key(parametersToCopy.key),
    beat(parametersToCopy.beat),
    velocity(parametersToCopy.velocity),
    mute(parametersToCopy.mute),
    solo(parametersToCopy.solo),
    sequenceModifiers(parametersToCopy.sequenceModifiers),
    id(parametersToCopy.id)
{
    this->updateCaches();
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

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

const Clip::Id Clip::getId() const noexcept
{
    return this->id;
}

const String &Clip::getKeyAsString() const noexcept
{
    return this->keyString;
}

const Array<SequenceModifier::Ptr> &Clip::getModifiers() const noexcept
{
    return this->sequenceModifiers;
}

bool Clip::isValid() const noexcept
{
    return this->pattern != nullptr && this->id != 0;
}

bool Clip::isMuted() const noexcept
{
    return this->mute;
}

bool Clip::isSoloed() const noexcept
{
    return this->solo;
}

bool Clip::canBeSoloed() const noexcept
{
    return (this->pattern != nullptr) ?
        this->pattern->getTrack()->canBeSoloed() : false;
}

bool Clip::hasModifiers() const noexcept
{
    return !this->sequenceModifiers.isEmpty();
}

bool Clip::hasEnabledModifiers() const noexcept
{
    for (const auto &modifier : this->sequenceModifiers)
    {
        if (modifier->isEnabled())
        {
            return true;
        }
    }

    return false;
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

//===----------------------------------------------------------------------===//
// Builder
//===----------------------------------------------------------------------===//

Clip Clip::withNewId(Pattern *newOwner) const
{
    Clip c(*this);

    if (newOwner != nullptr)
    {
        c.pattern = newOwner;
    }

    c.id = c.createId();
    return c;
}

Clip Clip::withParameters(const SerializedData &tree) const
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

Clip Clip::withMute(bool mute) const
{
    Clip other(*this);
    other.mute = mute;
    // cannot be muted and soloed at the same time:
    other.solo = other.solo && !mute;
    return other;
}

Clip Clip::withSolo(bool solo) const
{
    Clip other(*this);
    other.solo = solo;
    // cannot be muted and soloed at the same time:
    other.mute = other.mute && !solo;
    return other;
}

Clip Clip::withModifiers(Array<SequenceModifier::Ptr> &&mods) const
{
    Clip other(*this);
    other.sequenceModifiers.swapWith(mods);
    return other;
}

Clip Clip::withAppendedModifier(SequenceModifier::Ptr mod) const
{
    Clip other(*this);
    other.sequenceModifiers.add(mod);
    return other;
}

Clip Clip::withRemovedModifier(SequenceModifier::Ptr mod) const
{
    Clip other(*this);
    other.sequenceModifiers.removeAllInstancesOf(mod);
    return other;
}

Clip Clip::withUpdatedModifier(SequenceModifier::Ptr before, SequenceModifier::Ptr after) const
{
    Clip other(*this);

    const auto index = other.sequenceModifiers.indexOf(before);
    jassert(index >= 0);
    if (index >= 0)
    {
        other.sequenceModifiers.setUnchecked(index, after);
    }

    return other;
}

Clip Clip::withShiftedModifier(SequenceModifier::Ptr mod, int delta) const
{
    Clip other(*this);

    const auto index = other.sequenceModifiers.indexOf(mod);
    jassert(index >= 0);
    if (index >= 0)
    {
        const auto otherIndex = jlimit(0, other.sequenceModifiers.size() - 1, index + delta);
        jassert(otherIndex != index);
        other.sequenceModifiers.swap(index, otherIndex);
    }

    return other;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Clip::serialize() const
{
    using namespace Serialization;

    SerializedData tree(Midi::clip);
    tree.setProperty(Midi::key, this->key);
    tree.setProperty(Midi::timestamp, int(this->beat * Globals::ticksPerBeat));
    tree.setProperty(Midi::volume, int(this->velocity * Globals::velocitySaveResolution));
    tree.setProperty(Midi::id, packId(this->id));

    if (this->mute)
    {
        tree.setProperty(Midi::mute, 1);
    }

    if (this->solo)
    {
        tree.setProperty(Midi::solo, 1);
    }

    for (const auto &modifier : this->sequenceModifiers)
    {
        tree.appendChild(modifier->serialize());
    }

    return tree;
}

void Clip::deserialize(const SerializedData &data)
{
    using namespace Serialization;
    this->key = data.getProperty(Midi::key, 0);
    this->beat = float(data.getProperty(Midi::timestamp)) / Globals::ticksPerBeat;
    this->id = unpackId(data.getProperty(Midi::id, this->id));
    this->velocity = jlimit(0.f, 1.f,
        float(data.getProperty(Midi::volume, Globals::velocitySaveResolution)) / Globals::velocitySaveResolution);
    this->mute = bool(data.getProperty(Midi::mute, 0));
    this->solo = bool(data.getProperty(Midi::solo, 0));

    for (const auto &modifierData : data)
    {
        if (auto modifier = this->makeSequenceModifierByTag(modifierData.getType()))
        {
            modifier->deserialize(modifierData);
            this->sequenceModifiers.add(modifier);
        }
#if DEBUG
        else
        {
            jassertfalse; // not expecting any child elements other than modifiers here
        }
#endif
    }

    this->updateCaches();
}

SequenceModifier::Ptr Clip::makeSequenceModifierByTag(const Identifier &tagName) const
{
    using namespace Serialization;

    if (tagName == Modifiers::refactoringModifier) { return new RefactoringSequenceModifier(); }
    else if (tagName == Modifiers::arpeggiationModifier) { return new ArpeggiationSequenceModifier(); }
    else if (tagName == Modifiers::tuningModifier) { return new TuningSequenceModifier(); }

    jassertfalse;
    return nullptr;
}

void Clip::reset()
{
    this->key = 0;
    this->beat = 0.f;
    this->velocity = 1.f;
    this->mute = false;
    this->solo = false;
    this->sequenceModifiers.clear();
    this->updateCaches();
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

int Clip::compareElements(const Clip &first, const Clip &second)
{
    if (&first == &second) { return 0; }

    const float diff = first.beat - second.beat;
    const int diffResult = (diff > 0.f) - (diff < 0.f);
    if (diffResult != 0) { return diffResult; }

    return first.id - second.id;
}

int Clip::compareElements(const Clip *const first, const Clip *const second)
{
    return Clip::compareElements(*first, *second);
}

bool Clip::hasEquivalentModifiers(const Clip &other) const
{
    if (this->sequenceModifiers.size() != other.sequenceModifiers.size())
    {
        return false;
    }

    for (int i = 0; i < this->sequenceModifiers.size(); ++i)
    {
        const auto mod = this->sequenceModifiers.getUnchecked(i);
        const auto otherMod = other.sequenceModifiers.getUnchecked(i);

        if (!mod->isEquivalentTo(otherMod))
        {
            return false;
        }
    }

    return true;
}

void Clip::applyChanges(const Clip &other)
{
    jassert(this->id == other.id);
    this->key = other.key;
    this->beat = other.beat;
    this->velocity = other.velocity;
    this->mute = other.mute;
    this->solo = other.solo;
    this->sequenceModifiers = other.sequenceModifiers;
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

String Clip::packId(Id id)
{
    const char c1 = static_cast<char>(id >> (0 * CHAR_BIT));
    const char c2 = static_cast<char>(id >> (1 * CHAR_BIT));
    const char c3 = static_cast<char>(id >> (2 * CHAR_BIT));
    const char c4 = static_cast<char>(id >> (3 * CHAR_BIT));

    String s;
    s = s + c1 + c2 + c3 + c4;
    return s;
}

Clip::Id Clip::unpackId(const String &str)
{
    Clip::Id id = 0;
    const auto *ptr = str.getCharPointer().getAddress();
    for (int i = 0; i < jmin(4, str.length()); ++i)
    {
        id |= ptr[i] << (i * CHAR_BIT);
    }
    return id;
}
