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

using UndoActionId = int64;

namespace UndoActionIDs
{
    enum Id
    {
        None = 0x0000,

        KeyShiftUp = 0x0001,
        KeyShiftDown = 0x0002,

        BeatShiftRight = 0x0003,
        BeatShiftLeft = 0x0004,

        LengthIncrease = 0x0005,
        LengthDecrease = 0x0006,

        ClipTransposeUp = 0x0010,
        ClipTransposeDown = 0x0011,

        ClipVolumeUp = 0x0012,
        ClipVolumeDown = 0x0013,

        AddNewTrack = 0x0020,

        // ...

        MaxUndoActionId = 0x0100
    };
    
} // namespace UndoActionIDs
