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

namespace CommandIDs
{
    enum Id
    {
        None                            = 0x0000,

        Browse                          = 0x0001,
        IconButtonPressed               = 0x0002,
        DismissModalComponentAsync      = 0x0003,

        AddAnnotation                   = 0x0101,
        AddTimeSignature                = 0x0102,
        AddKeySignature                 = 0x0103,

        CreateArpeggiatorFromSelection  = 0x0200,

        InitWorkspace                   = 0x0500,

        ScanAllPlugins                  = 0x0504,
        ScanPluginsFolder               = 0x0505,

        DeleteTrack                     = 0x1000,
        RenameTrack                     = 0x1001,
        SetTrackTimeSignature           = 0x1002,
        SelectAllEvents                 = 0x1010,
        SelectAllClips                  = 0x1011,
        NewTrackFromSelection           = 0x1020,
        DuplicateTrack                  = 0x1021,
        InstanceToUniqueTrack           = 0x1022,
        EditCurrentInstrument           = 0x1023,
        SwitchToClipInViewport          = 0x1024,

        Tuplet1                         = 0x1500,
        Tuplet2                         = 0x1501,
        Tuplet3                         = 0x1502,
        Tuplet4                         = 0x1503,
        Tuplet5                         = 0x1504,
        Tuplet6                         = 0x1505,
        Tuplet7                         = 0x1506,
        Tuplet8                         = 0x1507,
        Tuplet9                         = 0x1508,

        QuantizeTo1_1                   = 0x1510,
        QuantizeTo1_2                   = 0x1511,
        QuantizeTo1_4                   = 0x1512,
        QuantizeTo1_8                   = 0x1513,
        QuantizeTo1_16                  = 0x1514,
        QuantizeTo1_32                  = 0x1515,

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
        ClipTransposeFifthUp            = 0x1624,
        ClipTransposeFifthDown          = 0x1625,
        ClipVolumeUp                    = 0x1626,
        ClipVolumeDown                  = 0x1627,

        ToggleMuteClips                 = 0x1630,
        ToggleSoloClips                 = 0x1631,
        ToggleScalesHighlighting        = 0x1632,
        ToggleNoteNameGuides            = 0x1633,
        ToggleLoopOverSelection         = 0x1634,
        ToggleMetronome                 = 0x1635,
        ToggleMuteModifiers             = 0x1636,

        ZoomIn                          = 0x1800,
        ZoomOut                         = 0x1801,
        ZoomEntireClip                  = 0x1802,
        ToggleLockZoomLevel             = 0x1803,

        Undo                            = 0x1900,
        Redo                            = 0x1901,

        TimelineJumpNext                = 0x2000,
        TimelineJumpPrevious            = 0x2001,
        TimelineJumpHome                = 0x2002,
        TimelineJumpEnd                 = 0x2003,

        TransportRecordingAwait         = 0x2015,
        TransportRecordingStart         = 0x2016,
        TransportPlaybackStart          = 0x2017,
        TransportStop                   = 0x2018,

        RenderToFLAC                    = 0x2030,
        RenderToWAV                     = 0x2031,
        RenderToOGG                     = 0x2032,

        ImportMidi                      = 0x2050,
        ExportMidi                      = 0x2051,

        DeleteProject                   = 0x2061,

        ProjectTransposeUp              = 0x2070,
        ProjectTransposeDown            = 0x2071,
        ProjectSetOneTempo              = 0x2072,
        TrackSetOneTempo                = 0x2073,
        ProjectTempoUp1Bpm              = 0x2074,
        ProjectTempoDown1Bpm            = 0x2075,
        TempoUp1Bpm                     = 0x2076,
        TempoDown1Bpm                   = 0x2077,

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
        SelectMidiInputDevice           = 0x3400, // more ids reserved for sub-items
        SelectMidiNoOutputDevice        = 0x3500,
        SelectMidiOutputDevice          = 0x3501, // more ids reserved for sub-items
        SelectLanguage                  = 0x3600, // more ids reserved for sub-items
        SelectFont                      = 0x3700, // more ids reserved for sub-items

        EditModeDefault                 = 0x4000,
        EditModeDraw                    = 0x4001,
        EditModePan                     = 0x4002,
        EditModeSelect                  = 0x4003,
        EditModeKnife                   = 0x4004,

        BeatShiftLeft                   = 0x4010,
        BeatShiftRight                  = 0x4011,
        LengthIncrease                  = 0x4012,
        LengthDecrease                  = 0x4013,
        TransposeUp                     = 0x4014,
        TransposeDown                   = 0x4015,
        TransposeScaleKeyUp             = 0x4016,
        TransposeScaleKeyDown           = 0x4017,
        TransposeOctaveUp               = 0x4018,
        TransposeOctaveDown             = 0x4019,
        TransposeFifthUp                = 0x401a,
        TransposeFifthDown              = 0x401b,
        AlignToScale                    = 0x401c,

        ViewportPanLeft                 = 0x4020,
        ViewportPanRight                = 0x4021,
        ViewportPanUp                   = 0x4022,
        ViewportPanDown                 = 0x4023,
        CursorMoveLeft                  = 0x4024,
        CursorMoveRight                 = 0x4025,
        CursorMoveUp                    = 0x4026,
        CursorMoveDown                  = 0x4027,
        CursorSelectLeft                = 0x4028,
        CursorSelectRight               = 0x4029,
        CursorSelectUp                  = 0x402a,
        CursorSelectDown                = 0x402b,
        CursorInteract                  = 0x402c,
        CursorEditHarmonicContext       = 0x402d,
        CursorEditTimeContext           = 0x402e,

        CleanupOverlaps                 = 0x4030,
        InvertChordUp                   = 0x4031,
        InvertChordDown                 = 0x4032,
        MelodicInversion                = 0x4033,
        Retrograde                      = 0x4034,
        NotesVolumeRandom               = 0x4035,
        NotesVolumeFadeOut              = 0x4036,
        NotesVolumeUp                   = 0x4037,
        NotesVolumeDown                 = 0x4038,

        ShowArpeggiatorsPanel           = 0x4040,
        ShowRescalePanel                = 0x4041,
        ShowChordPanel                  = 0x4043,
        ShowNewTrackPanel               = 0x4044,
        ShowMetronomeSettings           = 0x4045,
        ToggleVolumePanel               = 0x4046,
        ToggleBottomMiniMap             = 0x4047,

        VersionControlToggleQuickStash  = 0x4060,
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
        BreadcrumbsMenu                 = 0x4202,

        KeyMapReset                     = 0x4210,
        KeyMapLoadScala                 = 0x4211,
        KeyMapNextPage                  = 0x4212,
        KeyMapPreviousPage              = 0x4213,
        KeyMapNextChannel               = 0x4214,
        KeyMapPreviousChannel           = 0x4215,
        KeyMapCopyToClipboard           = 0x4216,
        KeyMapPasteFromClipboard        = 0x4217,

        SavePreset                      = 0x4300,
        SelectNextPreset                = 0x4301,
        SelectPreviousPreset            = 0x4302,
        SelectPreset                    = 0x4303, // more ids reserved for sub-items

        OpenMetronomeSample             = 0x4500, // more ids reserved for sub-items
        ResetMetronomeSample            = 0x4550, // more ids reserved for sub-items

        MakeStaccato                    = 0x4601, // make notes short
        MakeStaccatissimo               = 0x4602, // make notes very short
        MakeLegato                      = 0x4603, // connect notes in time
        MakeLegatoOverlapping           = 0x4604, // connect notes together, but with a slight overlap for use in certain VSTs

        CommandPaletteClear             = 0x4700,
        CommandPaletteDismiss           = 0x4701,
        CommandPaletteCursorUp          = 0x4702,
        CommandPaletteCursorDown        = 0x4703,
        CommandPaletteCursorPageUp      = 0x4704,
        CommandPaletteCursorPageDown    = 0x4705,

        ChordToolDismissApply           = 0x4710,
        ChordToolDismissCancel          = 0x4711,
        ChordToolRootKeyUp              = 0x4712,
        ChordToolRootKeyDown            = 0x4713,
        ChordToolBeatShiftLeft          = 0x4714,
        ChordToolBeatShiftRight         = 0x4715,
        ChordToolNextPreset             = 0x4716,
        ChordToolPreviousPreset         = 0x4717,
        ChordToolPreset1                = 0x4718,
        ChordToolPreset2                = 0x4719,
        ChordToolPreset3                = 0x471a,
        ChordToolPreset4                = 0x471b,
        ChordToolPreset5                = 0x471c,
        ChordToolPreset6                = 0x471d,
        ChordToolPreset7                = 0x471e,
        ChordToolPreset8                = 0x471f,
        ChordToolPreset9                = 0x4720,
        ChordToolPreset10               = 0x4721,
        ChordToolPreset11               = 0x4722,
        ChordToolPreset12               = 0x4723,

        MenuDismiss                     = 0x4730,
        MenuSelect                      = 0x4731,
        MenuCursorUp                    = 0x4732,
        MenuCursorDown                  = 0x4733,
        MenuCursorPageUp                = 0x4734,
        MenuCursorPageDown              = 0x4735,
        MenuForward                     = 0x4736,
        MenuBack                        = 0x4737,
        MenuCursorHide                  = 0x4738,
        MenuCursorTryExitUp             = 0x4739,
        MenuCursorTryExitDown           = 0x473a,

        DialogDismissCancel             = 0x4740,
        DialogDismissApply              = 0x4741,
        DialogDismissDelete             = 0x4742,
        DialogNextPreset                = 0x4743,
        DialogPreviousPreset            = 0x4744,
        DialogPreviewPreset             = 0x4745,
        DialogStopPreviewPreset         = 0x4746,
        DialogShowPresetsList           = 0x4747,

        ComboDismissCancel              = 0x4750,
        ComboDismissApply               = 0x4751,
        ComboCursorUp                   = 0x4752,
        ComboCursorDown                 = 0x4753,
        ComboCursorPageUp               = 0x4754,
        ComboCursorPageDown             = 0x4755,

        YourNextCommandId               = 0x4800
    };

    CommandIDs::Id getIdForName(const String &command);
    I18n::Key getTranslationKeyFor(CommandIDs::Id id);

} // namespace CommandIDs
