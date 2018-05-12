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

class NoteComponent final : public MidiEventComponent
{
public:

    NoteComponent(PianoRoll &gridRef, const Note &eventRef, bool ghostMode = false);

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

    int getKey() const noexcept;
    float getLength() const noexcept;
    float getVelocity() const noexcept;

    const Note &getNote() const noexcept;
    PianoRoll &getRoll() const noexcept;

    void updateColours() override;

    //===------------------------------------------------------------------===//
    // MidiEventComponent
    //===------------------------------------------------------------------===//

    void setSelected(bool selected) override;
    String getSelectionGroupId() const override;
    float getBeat() const override;
    String getId() const override;

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

    const MidiEvent &midiEvent;

    inline void paintNewLook(Graphics &g);
    inline void paintLegacyLook(Graphics &g);
    
    Note anchor;
    Note groupScalingAnchor;

    bool belongsToAnyTrack(const Array<WeakReference<MidiTrack>> &tracks) const;
    void activateCorrespondingTrack(bool selectOthers, bool deselectOthers);

    bool isResizing() const;
    
    void startResizingRight(bool sendMidiMessage);
    bool getResizingRightDelta(const MouseEvent &e, float &deltaLength) const;
    Note continueResizingRight(float deltaLength);
    void endResizingRight();
    
    void startResizingLeft(bool sendMidiMessage);
    bool getResizingLeftDelta(const MouseEvent &e, float &deltaLength) const;
    Note continueResizingLeft(float deltaLength);
    void endResizingLeft();
    
    void startTuning();
    Note continueTuning(const MouseEvent &e);
    Note continueTuningLinear(float delta);
    Note continueTuningMultiplied(float factor);
    Note continueTuningSine(float factor, float midline, float phase);
    void endTuning();
    
    void startGroupScalingRight(float groupStartBeat);
    bool getGroupScaleRightFactor(const MouseEvent &e, float &absScaleFactor) const;
    Note continueGroupScalingRight(float absScaleFactor);
    void endGroupScalingRight();
    
    void startGroupScalingLeft(float groupEndBeat);
    bool getGroupScaleLeftFactor(const MouseEvent &e, float &absScaleFactor) const;
    Note continueGroupScalingLeft(float absScaleFactor);
    void endGroupScalingLeft();
    
    void startDragging(bool sendMidiMessage);
    bool getDraggingDelta(const MouseEvent &e, float &deltaBeat, int &deltaKey);
    Note continueDragging(float deltaBeat, int deltaKey, bool sendMidiMessage);
    void endDragging(bool sendMidiMessage = true);
    
    bool canResize() const noexcept;

    State state;

    Colour colour;
    Colour colourLighter;
    Colour colourDarker;
    Colour colourVolume;

    friend class PianoRoll;
    friend class SequencerOperations;
    friend class NoteResizerLeft;
    friend class NoteResizerRight;
    
    bool firstChangeDone;
    void checkpointIfNeeded();
    void setNoCheckpointNeededForNextAction();

    bool shouldGoQuickSelectLayerMode(const ModifierKeys &modifiers) const;
    void setQuickSelectLayerMode(bool value);

    void stopSound();
    void sendMidiMessage(const MidiMessage &message);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteComponent)
    
};
