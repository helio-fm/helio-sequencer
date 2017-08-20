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
// MidiLayerTreeItem implements this

class MidiTrack
{
public:

    virtual ~MidiTrack() {}

    //===------------------------------------------------------------------===//
    // Properties
    //===------------------------------------------------------------------===//

    virtual Uuid getTrackId() const noexcept = 0;
    virtual int getTrackChannel() const noexcept = 0;

    virtual String getTrackName() const noexcept = 0;
    virtual void setTrackName(const String &val) = 0;

    virtual Colour getTrackColour() const noexcept = 0;
    virtual void setTrackColour(const Colour &val) = 0;

    virtual String getTrackInstrumentId() const noexcept = 0;
    virtual void setTrackInstrumentId(const String &val) = 0;
        
    virtual int getTrackControllerNumber() const noexcept = 0;
    virtual void setTrackControllerNumber(int val) = 0;

    virtual bool isTrackMuted() const noexcept = 0;
    virtual void setTrackMuted(bool shouldBeMuted) = 0;

    virtual MidiSequence *getSequence() const noexcept = 0;
    virtual Pattern *getPattern() const noexcept = 0;

    //===------------------------------------------------------------------===//
    // Shorthands
    //===------------------------------------------------------------------===//

    static int compareElements(const MidiTrack &first, const MidiTrack &second);
    static int compareElements(const MidiTrack *first, const MidiTrack *second);

    void serializeTrackProperties(XmlElement &xml) const;
    void deserializeTrackProperties(const XmlElement &xml);

    enum DefaultControllers
    {
        sustainPedalController = 64,
        tempoController = 81,
    };

    bool isTempoTrack() const noexcept;
    bool isSustainPedalTrack() const noexcept;
    bool isOnOffTrack() const noexcept;

    String getTrackMuteStateAsString() const;
    static bool isTrackMuted(const String &muteState);

protected:

    virtual void setTrackId(const Uuid &val) = 0;

};

class EmptyMidiTrack : public MidiTrack
{
public:

    EmptyMidiTrack() {}

    Uuid getTrackId() const noexcept override { return {}; }
    void setTrackId(const Uuid &val) override {};
    int getTrackChannel() const noexcept override { return 0; }

    String getTrackName() const noexcept override { return String::empty; }
    void setTrackName(const String &val) override {}

    Colour getTrackColour() const noexcept override { return Colours::white; }
    void setTrackColour(const Colour &val) override {};

    String getTrackInstrumentId() const noexcept override { return String::empty; }
    void setTrackInstrumentId(const String &val) override {};

    int getTrackControllerNumber() const noexcept override { return 0; }
    void setTrackControllerNumber(int val) override {};

    bool isTrackMuted() const noexcept override { return false; }
    void setTrackMuted(bool shouldBeMuted) override {};

    MidiSequence *getSequence() const noexcept override { return nullptr; }
    Pattern *getPattern() const noexcept override { return nullptr; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EmptyMidiTrack)
};
