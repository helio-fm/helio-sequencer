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

#pragma once

class Pattern;

// Just an instance of a midi sequence on a certain position.
// In future we it should have adjustable length too
// (and a stack of parametric modifiers like user-defined arps, etc.)

class Clip final : public Serializable
{
public:

    using Id = String;

    Clip();
    Clip(WeakReference<Pattern> owner, const Clip &parametersToCopy);
    explicit Clip(WeakReference<Pattern> owner, float beatVal = 0.f);

    Clip(const Clip &other) noexcept = default;
    Clip &operator= (const Clip &other) = default;

    Clip(Clip &&other) noexcept = default;
    Clip &operator= (Clip &&other) noexcept = default;

    Pattern *getPattern() const noexcept;
    int getKey() const noexcept;
    float getBeat() const noexcept;
    float getVelocity() const noexcept;
    const String &getId() const noexcept;
    const String &getKeyString() const noexcept;

    bool isValid() const noexcept;

    const String &getTrackId() const noexcept;
    Colour getTrackColour() const noexcept;
    int getTrackControllerNumber() const noexcept;

    Clip copyWithNewId(Pattern *newOwner = nullptr) const;
    Clip withParameters(const ValueTree &tree) const;
    Clip withBeat(float absPosition) const;
    Clip withDeltaBeat(float deltaPosition) const;
    Clip withKey(int absKey) const;
    Clip withDeltaKey(int deltaKey) const;
    Clip withVelocity(float absVelocity) const;
    Clip withDeltaVelocity(float deltaVelocity) const;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
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

    static int compareElements(const Clip &first, const Clip &second);
    static int compareElements(const Clip *const first, const Clip *const second);

    void applyChanges(const Clip &parameters);
    HashCode hashCode() const noexcept;

private:

    WeakReference<Pattern> pattern;

    int key;
    float beat;
    float velocity;

    String id;
    Id createId() const noexcept;

    mutable String keyString;
    void updateCaches() const;

    friend struct ClipHash;

    JUCE_LEAK_DETECTOR(Clip);
};

struct ClipHash
{
    inline HashCode operator()(const Clip &key) const noexcept
    {
        //const HashCode code = static_cast<HashCode>(key.beat)
        //    + static_cast<HashCode>(key.getId().hashCode());
        const auto *ptr = key.id.getCharPointer().getAddress();
        return 64 * static_cast<HashCode>(ptr[0]) + static_cast<HashCode>(ptr[1]);
    }
};
