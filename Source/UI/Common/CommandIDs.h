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

namespace CommandIDs
{
    enum Id
    {
        None                            = 0x0000,

        Back                            = 0x0001,
        Cancel                          = 0x0002,
        Browse                          = 0x0003,
        IconButtonPressed               = 0x0004,
        MenuButtonPressed               = 0x0005,
        HideDialog                      = 0x0007,
        HideCallout                     = 0x0008,
        DismissModalDialogAsync         = 0x0009,

        AddAnnotation                   = 0x0101,
        AddTimeSignature                = 0x0103,
        AddKeySignature                 = 0x0105,
        JumpToAnnotation                = 0x0107, // more ids reserved for annotations

        CreateArpeggiatorFromSelection  = 0x0200,

        InitWorkspace                   = 0x0500,

        ScanAllPlugins                  = 0x0504,
        ScanPluginsFolder               = 0x0505,

        DeleteTrack                     = 0x1000,
        RenameTrack                     = 0x1001,
        SelectAllEvents                 = 0x1002,
        SelectAllClips                  = 0x1003,
        NewTrackFromSelection           = 0x1004,
        DuplicateTrack                  = 0x1005,

        Tuplet1                         = 0x1500,
        Tuplet2                         = 0x1501,
        Tuplet3                         = 0x1502,
        Tuplet4                         = 0x1503,
        Tuplet5                         = 0x1504,
        Tuplet6                         = 0x1505,
        Tuplet7                         = 0x1506,
        Tuplet8                         = 0x1507,
        Tuplet9                         = 0x1508,

        DeleteEvents                    = 0x1601,
        CopyEvents                      = 0x1602,
        CutEvents                       = 0x1603,
        PasteEvents                     = 0x1604,

        DeleteClips                     = 0x1611,
        CopyClips                       = 0x1612,
        CutClips                        = 0x1613,
        PasteClips                      = 0x1614,

        ClipTransposeUp                 = 0x1620,
        ClipTransposeDown               = 0x1621,
        ClipTransposeOctaveUp           = 0x1622,
        ClipTransposeOctaveDown         = 0x1623,
        ClipVolumeUp                    = 0x1624,
        ClipVolumeDown                  = 0x1625,

        ToggleMuteClips                 = 0x1630,
        ToggleSoloClips                 = 0x1631,
        ToggleScalesHighlighting        = 0x1632,
        ToggleNoteNameGuides            = 0x1633,

        ZoomIn                          = 0x1800,
        ZoomOut                         = 0x1801,
        ZoomEntireClip                  = 0x1802,

        Undo                            = 0x1900,
        Redo                            = 0x1901,

        TimelineJumpNext                = 0x2000,
        TimelineJumpPrevious            = 0x2001,

        ShowAnnotations                 = 0x2010,
        ResetPreviewChanges             = 0x2011,

        TransportRecordingAwait         = 0x2015,
        TransportRecordingStart         = 0x2016,
        TransportPlaybackStart          = 0x2017,
        TransportStop                   = 0x2018,

        PopupMenuDismiss                = 0x2020,

        RenderToFLAC                    = 0x2030,
        RenderToWAV                     = 0x2031,

        ImportMidi                      = 0x2050,
        ExportMidi                      = 0x2051,

        DeleteProject                   = 0x2061,

        ProjectTransposeUp              = 0x2070,
        ProjectTransposeDown            = 0x2071,

        SelectFunction                  = 0x2120, // more ids reserved for sub-items
        SelectScale                     = 0x2200, // more ids reserved for sub-items
        SelectTimeSignature             = 0x2300, // more ids reserved for sub-items
        SelectVersion                   = 0x2400, // more ids reserved for sub-items

        SwitchBetweenRolls              = 0x2500,
        SwitchToEditMode                = 0x2501,
        SwitchToArrangeMode             = 0x2502,
        SwitchToVersioningMode          = 0x2503,
        ShowPreviousPage                = 0x2504,
        ShowNextPage                    = 0x2505,
        ShowRootPage                    = 0x2506,
        ToggleShowHideCombo             = 0x2507,

        StartDragViewport               = 0x2510,
        EndDragViewport                 = 0x2511,

        SelectAudioDeviceType           = 0x3000,
        SelectAudioDevice               = 0x3100,
        SelectSampleRate                = 0x3200,
        SelectBufferSize                = 0x3300, // more ids reserved for sub-items
        SelectAllMidiInputDevices       = 0x3400,
        SelectMidiInputDevice           = 0x3401, // more ids reserved for sub-items
        SelectFont                      = 0x3500, // more ids reserved for sub-items

        EditModeDefault                 = 0x4000,
        EditModeDraw                    = 0x4001,
        EditModePan                     = 0x4002,
        EditModeSelect                  = 0x4003,
        EditModeKnife                   = 0x4004,
        EditModeEraser                  = 0x4005,
        EditModeChordBuilder            = 0x4006,

        BeatShiftLeft                   = 0x4020,
        BeatShiftRight                  = 0x4021,
        LengthIncrease                  = 0x4022,
        LengthDecrease                  = 0x4023,
        KeyShiftUp                      = 0x4024,
        KeyShiftDown                    = 0x4025,
        OctaveShiftUp                   = 0x4026,
        OctaveShiftDown                 = 0x4027,

        CleanupOverlaps                 = 0x4030,
        InvertChordUp                   = 0x4031,
        InvertChordDown                 = 0x4032,
        MelodicInversion                = 0x4033,
        Retrograde                      = 0x4034,
        TweakVolumeRandom               = 0x4035,
        TweakVolumeFadeOut              = 0x4036,

        ShowArpeggiatorsPanel           = 0x4040,
        ShowVolumePanel                 = 0x4041,
        ShowRescalePanel                = 0x4042,
        ShowScalePanel                  = 0x4043,
        ShowChordPanel                  = 0x4044,

        // Version control
        VersionControlToggleQuickStash  = 0x4060,
        VersionControlPushSelected      = 0x4061,
        VersionControlPullSelected      = 0x4062,
        VersionControlSyncAll           = 0x4063,
        VersionControlResetAll          = 0x4064,
        VersionControlCommitAll         = 0x4065,
        VersionControlSelectAll         = 0x4066,
        VersionControlSelectNone        = 0x4067,
        VersionControlResetSelected     = 0x4068,
        VersionControlCommitSelected    = 0x4069,
        VersionControlCheckout          = 0x406a,

        PatternsGroupByName             = 0x4100,
        PatternsGroupByColour           = 0x4101,
        PatternsGroupByInstrument       = 0x4102,
        PatternsGroupById               = 0x4103,

        CommandPalette                  = 0x4200,
        CommandPaletteWithMode          = 0x4201,

        YourNextCommandId               = 0x4300
    };

    CommandIDs::Id getIdForName(const String &command);
    Identifier getTranslationKeyFor(CommandIDs::Id id);

} // namespace CommandIDs
