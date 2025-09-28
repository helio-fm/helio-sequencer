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

class PianoRoll;
class NoteComponent;

#include "KeyboardCursor.h"

class PianoRollCursor final : public KeyboardCursor
{
public:

    explicit PianoRollCursor(PianoRoll &roll);
    ~PianoRollCursor();

    //===------------------------------------------------------------------===//
    // KeyboardCursor
    //===------------------------------------------------------------------===//

    void moveLeft() override;
    void moveRight() override;
    void moveUp() override;
    void moveDown() override;

    void selectLeft() override;
    void selectRight() override;
    void selectUp() override;
    void selectDown() override;

    void interact(RollEditMode editMode) override;

    Point<int> getTargetPoint() const override;
    NoteComponent *getTargetingNoteComponent() const;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void parentSizeChanged() override;

private:
    
    void updateBounds() override;

    PianoRoll &roll;

    int key = 0;
    inline const float getBeat() const noexcept;

    int selectionKeyDelta = 0;
    float selectionBeatDelta = 0.f;

    void doBeforeMoveChecks();
    void doBeforeSelectChecks();

    Rectangle<int> getRollViewArea() const;
    Rectangle<float> getSelectionArea() const;

    bool isLassoMode() const;
    void dragLasso();

    void scrollViewportToKeyIfNeeded();
    void scrollViewportToBeatIfNeeded();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollCursor)
};
