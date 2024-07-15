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

#pragma once

class Pattern;

#include "SequenceModifier.h"

// Clip is an instance of a sequence on a certain position.
// Optionally, with key delta, velocity multiplier, muted or soloed.
// Optionally, has a stack of parametric modifiers like arps and refactorings.
// (todo in future: a scripting language for parametric modifiers)

class Clip final : public Serializable
{
public:

    using Id = int32;

    Clip();
    Clip(WeakReference<Pattern> owner, const Clip &parametersToCopy);
    explicit Clip(WeakReference<Pattern> owner, float beatVal = 0.f, int key = 0);

    Clip(const Clip &other) = default;
    Clip &operator= (const Clip &other) = default;

    Clip(Clip &&other) noexcept = default;
    Clip &operator= (Clip &&other) noexcept = default;

    Pattern *getPattern() const noexcept;
    int getKey() const noexcept;
    float getBeat() const noexcept;
    float getVelocity() const noexcept;
    const Id getId() const noexcept;
    const String &getKeyAsString() const noexcept;
    const Array<SequenceModifier::Ptr> &getModifiers() const noexcept;
    
    bool isValid() const noexcept;
    bool isMuted() const noexcept;
    bool isSoloed() const noexcept;
    bool canBeSoloed() const noexcept;
    bool hasModifiers() const noexcept;
    bool hasEnabledModifiers() const noexcept;

    const String &getTrackId() const noexcept;
    Colour getTrackColour() const noexcept;
    int getTrackControllerNumber() const noexcept;

    Clip withParameters(const SerializedData &tree) const;
    Clip withBeat(float absPosition) const;
    Clip withDeltaBeat(float deltaPosition) const;
    Clip withKey(int absKey) const;
    Clip withDeltaKey(int deltaKey) const;
    Clip withVelocity(float absVelocity) const;
    Clip withDeltaVelocity(float deltaVelocity) const;
    Clip withMute(bool mute) const;
    Clip withSolo(bool solo) const;

    Clip withModifiers(Array<SequenceModifier::Ptr> &&mods) const;
    Clip withAppendedModifier(SequenceModifier::Ptr mod) const;
    Clip withRemovedModifier(SequenceModifier::Ptr mod) const;
    Clip withShiftedModifier(SequenceModifier::Ptr mod, int orderDelta) const;
    Clip withUpdatedModifier(SequenceModifier::Ptr before,
        SequenceModifier::Ptr after) const;

    Clip withNewId(Pattern *newOwner = nullptr) const;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    friend inline bool operator==(const Clip &l, const Clip &r)
    {
        return (&l == &r ||
            ((l.pattern == nullptr || r.pattern == nullptr) && l.id == r.id) ||
            (l.pattern != nullptr && l.pattern == r.pattern && l.id == r.id));
    }

    friend inline bool operator!=(const Clip &l, const Clip &r)
    {
        return !(l == r);
    }

    static int compareElements(const Clip &first, const Clip &second);
    static int compareElements(const Clip *const first, const Clip *const second);

    // this is used to compare modifiers in version control:
    // simply comparing them as arrays of ptr's will sometimes return false negatives
    // (there may be different objects with same parameters), which is ok for
    // updating generated sequences, but vcs needs a more accurate check
    bool hasEquivalentModifiers(const Clip &other) const;

    void applyChanges(const Clip &parameters);

private:

    WeakReference<Pattern> pattern;

    int key = 0;
    float beat = 0.f;
    float velocity = 1.f;

    bool mute = false;
    bool solo = false;

private:

    Array<SequenceModifier::Ptr> sequenceModifiers;

    SequenceModifier::Ptr makeSequenceModifierByTag(const Identifier &tagName) const;

private:

    Id id = 0;
    Id createId() const noexcept;
    static String packId(Id id);
    static Id unpackId(const String &str);

    mutable String keyString;
    void updateCaches() const;

    friend struct ClipHash;
    friend class LegacyClipFormatSupportTests;

    JUCE_LEAK_DETECTOR(Clip);
};

struct ClipHash
{
    inline HashCode operator()(const Clip &key) const noexcept
    {
        return static_cast<HashCode>(key.id);
    }
};
