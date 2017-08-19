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
// all the properties and
// sequence with events and
// pattern with clips;
// MidiLayerTreeItem implements this

class MidiTrack
{
public:

    virtual ~MidiTrack() {}

    virtual Uuid getTrackId() const noexcept = 0;
    virtual int getTrackChannel() const noexcept = 0;

    virtual String getTrackName() const noexcept = 0;
    virtual void setTrackName(const String &val) = 0;

    virtual Colour getTrackColour() const noexcept = 0;
    virtual void setTrackColour(Colour colour) = 0;

    virtual String getTrackInstrumentId() const noexcept = 0;
    virtual void setTrackInstrumentId(const String &val) = 0;
        
    virtual int getTrackControllerNumber() const noexcept = 0;
    virtual void setTrackControllerNumber(int val) = 0;

    virtual bool isTrackMuted() const noexcept = 0;
    virtual void setTrackMuted(bool shouldBeMuted) = 0;

    // TODO
    //virtual bool isTrackSolo() const noexcept = 0;
    //virtual void setSolo(bool shouldBeSolo) = 0;

    virtual MidiSequence *getSequence() const noexcept = 0;
    virtual Pattern *getPattern() const noexcept = 0;

    // Some shorthands:

    static int compareElements(const MidiTrack &first, const MidiTrack &second)
    {
        if (&first == &second) { return 0; }
        return first.getTrackName().compareNatural(second.getTrackName());
    }


    enum DefaultControllers
    {
        sustainPedalController = 64,
        tempoController = 81,
    };

    bool isTempoTrack() const noexcept
    {
        return (this->getTrackControllerNumber() == MidiTrack::tempoController);
    }

    bool isSustainPedalTrack() const noexcept
    {
        return (this->getTrackControllerNumber() == MidiTrack::sustainPedalController);
    }

    bool isOnOffTrack() const noexcept
    {
        // hardcoded for now
        return (this->getTrackControllerNumber() >= 64 &&
            this->getTrackControllerNumber() <= 69);
    }

    String getTrackMuteStateAsString() const
    {
        return (this->isTrackMuted() ? "yes" : "no");
    }

    static bool isTrackMuted(const String &muteState)
    {
        return (muteState == "yes");
    }
};

class EmptyMidiTrack : public MidiTrack
{
public:

    EmptyMidiTrack() {}

    virtual Uuid getTrackId() const noexcept override { return {}; }
    int getTrackChannel() const noexcept override { return 0; }

    String getTrackName() const noexcept override { return String::empty; }
    void setTrackName(const String &val) override {}

    Colour getTrackColour() const noexcept override { return Colours::white; }
    void setTrackColour(Colour colour) override {};

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
