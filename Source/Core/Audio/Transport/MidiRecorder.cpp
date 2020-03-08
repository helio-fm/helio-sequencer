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

#include "Common.h"
#include "MidiRecorder.h"

#include "UndoStack.h"
#include "MidiTrack.h"

MidiRecorder::MidiRecorder(UndoStack &undoStack) :
    undoStack(undoStack) {}

void MidiRecorder::setSelectedScope(WeakReference<MidiTrack> track, const Clip &clip)
{
    this->activeTrack = track;
    this->activeClip = clip;
}

void MidiRecorder::onRecord()
{
    // start a named transaction?
    // or set a flag the recording has started?
}

void MidiRecorder::onStop()
{
    // set a flag the recording has stopped?
}

void MidiRecorder::onMidiMessageArrived(const MidiMessage &message)
{
    this->buffer.add(message);
    this->triggerAsyncUpdate();
}

// called from the message thread, so we can insert new midi events
// (note that the track selection may change during recording)
void MidiRecorder::handleAsyncUpdate()
{
    // if something is selected (can be both rolls), simply insert messages,
    // if nothing is selected (pattern roll), first create a new track and select it
    // if multiple tracks are selected (also pattern roll) - same as ^

    // todo show some hints on where to expect new notes?
}
