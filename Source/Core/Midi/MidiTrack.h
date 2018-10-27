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

class MidiSequence;
class Pattern;

// A track is a meta-object that has
// - all the properties
// - sequence with events
// - pattern with clips
// MidiTrackTreeItem implements this

class MidiTrack
{
public:

    MidiTrack() = default;
    virtual ~MidiTrack() {};

    //===------------------------------------------------------------------===//
    // Properties
    //===------------------------------------------------------------------===//

    virtual const String &getTrackId() const noexcept = 0;
    virtual int getTrackChannel() const noexcept = 0;

    // sendNotifications argument here is only meant to be true,
    // when changing properties from user actions (see undo/redo action classes);
    // deserialization and resetting VCS state should always call
    // setX(123, false) to prevent notification hell

    virtual String getTrackName() const noexcept = 0;
    virtual void setTrackName(const String &val, bool sendNotifications) = 0;

    virtual Colour getTrackColour() const noexcept = 0;
    virtual void setTrackColour(const Colour &val, bool sendNotifications) = 0;

    virtual String getTrackInstrumentId() const noexcept = 0;
    virtual void setTrackInstrumentId(const String &val, bool sendNotifications) = 0;
        
    virtual int getTrackControllerNumber() const noexcept = 0;
    virtual void setTrackControllerNumber(int val, bool sendNotifications) = 0;

    virtual bool isTrackMuted() const noexcept = 0;
    virtual void setTrackMuted(bool shouldBeMuted, bool sendNotifications) = 0;

    virtual MidiSequence *getSequence() const noexcept = 0;
    virtual Pattern *getPattern() const noexcept = 0;

    //===------------------------------------------------------------------===//
    // Shorthands
    //===------------------------------------------------------------------===//

    static int compareElements(const MidiTrack &first, const MidiTrack &second);
    static int compareElements(const MidiTrack *const first, const MidiTrack *const second);

    inline HashCode hashCode() const noexcept
    {
        return static_cast<HashCode>(this->getTrackId().hashCode());
    }

    void serializeTrackProperties(ValueTree &tree) const;
    void deserializeTrackProperties(const ValueTree &tree);

    enum DefaultControllers
    {
        sustainPedalController = 64,
        tempoController = 81,
    };

    bool isTempoTrack() const noexcept;
    bool isOnOffAutomationTrack() const noexcept;

    String getTrackMuteStateAsString() const;
    static bool isTrackMuted(const String &muteState);

protected:

    virtual void setTrackId(const String &val) = 0;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiTrack)
    JUCE_DECLARE_WEAK_REFERENCEABLE(MidiTrack)
};

struct MidiTrackHash
{
    inline HashCode operator()(const MidiTrack *const key) const noexcept
    {
        return key->hashCode();
    }
};

class EmptyMidiTrack : public MidiTrack
{
public:

    EmptyMidiTrack() {}

    void setTrackId(const String &val) override {};
    const String &getTrackId() const noexcept override { return this->trackId; }
    int getTrackChannel() const noexcept override { return 0; }

    String getTrackName() const noexcept override { return {}; }
    void setTrackName(const String &val, bool sendNotifications) override {}

    Colour getTrackColour() const noexcept override { return Colours::white; }
    void setTrackColour(const Colour &val, bool sendNotifications) override {};

    String getTrackInstrumentId() const noexcept override { return {}; }
    void setTrackInstrumentId(const String &val, bool sendNotifications) override {};

    int getTrackControllerNumber() const noexcept override { return 0; }
    void setTrackControllerNumber(int val, bool sendNotifications) override {};

    bool isTrackMuted() const noexcept override { return false; }
    void setTrackMuted(bool shouldBeMuted, bool sendNotifications) override {};

    MidiSequence *getSequence() const noexcept override { return nullptr; }
    Pattern *getPattern() const noexcept override { return nullptr; }

    String trackId;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EmptyMidiTrack)
};
