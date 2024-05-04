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
class NoteNameGuide;
class Lasso;
class MidiTrack;

#include "RollListener.h"
#include "Temperament.h"
#include "Scale.h"
#include "Note.h"

class NoteNameGuidesBar final : public Component,
    public RollListener,
    public AsyncUpdater,
    private ChangeListener // listens to roll's selection
{
public:

    NoteNameGuidesBar(PianoRoll &roll, WeakReference<MidiTrack> keySignatures);
    ~NoteNameGuidesBar();

    void syncWithTemperament(Temperament::Ptr temperament);
    void syncWithSelection(const Lasso *selection);

    //===------------------------------------------------------------------===//
    // RollListener
    //===------------------------------------------------------------------===//

    void onMidiRollMoved(RollBase *targetRoll) override;
    void onMidiRollResized(RollBase *targetRoll) override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void visibilityChanged() override;

private:

    PianoRoll &roll;

    OwnedArray<NoteNameGuide> guides;

    Temperament::Ptr temperament;

    WeakReference<MidiTrack> keySignatures;

    const Scale::Ptr defaultScale = Scale::makeNaturalMajorScale();
    Scale::Ptr scale;
    String scaleRootKeyName;
    int scaleRootKey = 0;

    Optional<float> selectionStartBeat;
    FlatHashSet<Note::Key> selectedKeys;

    static constexpr auto defaultWidth = 36;
    static constexpr auto extendedWidth = 48;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    void handleAsyncUpdate() override;

    void updatePosition();
    void updateBounds();
    void updateContent();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteNameGuidesBar)
};
