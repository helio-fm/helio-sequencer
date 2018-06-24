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

class PianoRoll;
class MidiTrack;

#include "MidiEventComponent.h"
#include "Note.h"
#include "Clip.h"

class NoteComponent final : public MidiEventComponent
{
public:

    NoteComponent(PianoRoll &gridRef, const Note &note,
        const Clip &clip, bool ghostMode = false);

    enum State
    {
        None,
        Initializing,       // changes length & key
        Dragging,           // changes beat & key
        ResizingRight,      // changes length
        ResizingLeft,       // changes beat & length
        GroupScalingRight,  
        GroupScalingLeft,
        Tuning
    };
    
    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//

    inline int getKey() const noexcept { return this->note.getKey(); }
    inline float getLength() const noexcept { return this->note.getLength(); }
    inline float getVelocity() const noexcept { return this->note.getVelocity(); }
    inline const Note &getNote() const noexcept { return this->note; }
    inline const Clip &getClip() const noexcept { return this->clip; }

    PianoRoll &getRoll() const noexcept;

    void updateColours() override;

    //===------------------------------------------------------------------===//
    // MidiEventComponent
    //===------------------------------------------------------------------===//

    void setSelected(bool selected) override;
    const String &getSelectionGroupId() const noexcept override;
    const String &getId() const noexcept override { return this->note.getId(); }
    float getBeat() const noexcept override { return this->note.getBeat(); }

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    bool keyStateChanged(bool isKeyDown) override;
    void modifierKeysChanged(const ModifierKeys &modifiers) override;
    void mouseMove(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDoubleClick(const MouseEvent &e) override;
    void paint(Graphics &g) override;

protected:

    const Note &note;
    const Clip &clip;

    Note anchor;
    Note groupScalingAnchor;

    bool belongsTo(const WeakReference<MidiTrack> &track, const Clip &clip) const noexcept;
    void switchActiveSegmentToSelected(bool zoomToScope) const;

    bool isInitializing() const;
    void startInitializing();
    bool getInitializingDelta(const MouseEvent &e, float &deltaLength, int &deltaKey) const;
    Note continueInitializing(float deltaLength, int deltaKey, bool sendMidi) const noexcept;
    void endInitializing();

    void startResizingRight(bool sendMidiMessage);
    bool getResizingRightDelta(const MouseEvent &e, float &deltaLength) const;
    Note continueResizingRight(float deltaLength) const noexcept;
    void endResizingRight();
    
    void startResizingLeft(bool sendMidiMessage);
    bool getResizingLeftDelta(const MouseEvent &e, float &deltaLength) const;
    Note continueResizingLeft(float deltaLength) const noexcept;
    void endResizingLeft();
    
    void startTuning();
    Note continueTuning(const MouseEvent &e) const noexcept;
    Note continueTuningLinear(float delta) const noexcept;
    Note continueTuningMultiplied(float factor) const noexcept;
    Note continueTuningSine(float factor, float midline, float phase) const noexcept;
    void endTuning();
    
    void startGroupScalingRight(float groupStartBeat);
    bool getGroupScaleRightFactor(const MouseEvent &e, float &absScaleFactor) const;
    Note continueGroupScalingRight(float absScaleFactor) const noexcept;
    void endGroupScalingRight();
    
    void startGroupScalingLeft(float groupEndBeat);
    bool getGroupScaleLeftFactor(const MouseEvent &e, float &absScaleFactor) const;
    Note continueGroupScalingLeft(float absScaleFactor) const noexcept;
    void endGroupScalingLeft();
    
    void startDragging(bool sendMidiMessage);
    bool getDraggingDelta(const MouseEvent &e, float &deltaBeat, int &deltaKey);
    Note continueDragging(float deltaBeat, int deltaKey, bool sendMidiMessage) const noexcept;
    void endDragging(bool sendMidiMessage = true);
    
    bool canResize() const noexcept;

    State state;

    Colour colour;
    Colour colourLighter;
    Colour colourDarker;
    Colour colourVolume;

    friend class PianoRoll;
    friend class NoteResizerLeft;
    friend class NoteResizerRight;
    friend struct SequencerOperations;

    bool firstChangeDone;
    void checkpointIfNeeded();

    bool shouldGoQuickSelectLayerMode(const ModifierKeys &modifiers) const;
    void setQuickSelectLayerMode(bool value);

    void stopSound();
    void sendMidiMessage(const MidiMessage &message) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteComponent)
    
};
