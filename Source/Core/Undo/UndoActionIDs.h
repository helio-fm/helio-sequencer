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

using UndoActionId = int64;

namespace UndoActionIDs
{
    enum Id
    {
        None = 0x0000,

        KeyShiftUp = 0x0001,
        KeyShiftDown = 0x0002,

        ScaleKeyShiftUp = 0x0010,
        ScaleKeyShiftDown = 0x0011,
        SnapToScale = 0x0012,

        BeatShiftRight = 0x0020,
        BeatShiftLeft = 0x0021,

        LengthIncrease = 0x0030,
        LengthDecrease = 0x0031,

        ClipTransposeUp = 0x0040,
        ClipTransposeDown = 0x0041,

        ClipVolumeUp = 0x0050,
        ClipVolumeDown = 0x0051,

        NotesVolumeUp = 0x0060,
        NotesVolumeDown = 0x0061,

        AddNewTrack = 0x0070,
        DuplicateTrack = 0x0071,
        MakeUniqueTrack = 0x0072,

        MakeStaccato = 0x0080,
        MakeStaccatissimo = 0x0081,
        MakeLegato = 0x0082,
        MakeLegatoOverlapping = 0x0083,

        ShiftTempoUp = 0x0090,
        ShiftTempoDown = 0x0091,

        MakeChord = 0x0100,
        // ...

        MaxUndoActionId = 0x0100
    };
} // namespace UndoActionIDs
