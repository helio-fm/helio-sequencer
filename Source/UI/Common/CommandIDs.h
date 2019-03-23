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
    enum UI
    {
        Back                            = 0x0001,
        Cancel                          = 0x0002,
        Browse                          = 0x0003,
        IconButtonPressed               = 0x0004,
        MenuButtonPressed               = 0x0005,
        RootTreeItemPressed             = 0x0006,
        HideDialog                      = 0x0007,
        HideCallout                     = 0x0008,
        DismissModalDialogAsync         = 0x0009,

        // TimeSignatureCommandPanel
        ChangeTimeSignature             = 0x0010,
        ChangeTimeSignatureConfirmed    = 0x0011,
        DeleteTimeSignature             = 0x0012,

        // AnnotationCommandPanel
        RenameAnnotation                = 0x0020,
        RenameAnnotationConfirmed       = 0x0021,
        SetAnnotationColour             = 0x0022, // more ids reserved for colours
        DeleteAnnotation                = 0x0100,

        // AnnotationsCommandPanel
        AddAnnotation                   = 0x0101,
        AddAnnotationConfirmed          = 0x0102,
        AddTimeSignature                = 0x0103,
        AddTimeSignatureConfirmed       = 0x0104,
        AddKeySignature                 = 0x0105,
        AddKeySignatureConfirmed        = 0x0106,
        JumpToAnnotation                = 0x0107, // more ids reserved for annotations

        CreateArpeggiatorFromSelection  = 0x0200,

        InitWorkspace                   = 0x0500,

        RenameInstrument                = 0x0501,
        UpdateInstrument                = 0x0502,
        DeleteInstrument                = 0x0503,

        ScanAllPlugins                  = 0x0504,
        ScanPluginsFolder               = 0x0505,

        DeleteTrack                     = 0x1000,
        MuteTrack                       = 0x1001,
        UnmuteTrack                     = 0x1002,
        RenameTrack                     = 0x1003,
        SelectAllEvents                 = 0x1004,
        SelectAllClips                  = 0x1005,
        NewTrackFromSelection           = 0x1006,

        // MidiRollCommandPanel
        DeleteEvents                    = 0x1601,
        CopyEvents                      = 0x1602,
        CutEvents                       = 0x1603,
        PasteEvents                     = 0x1604,

        DeleteClips                     = 0x1611,
        CopyClips                       = 0x1612,
        CutClips                        = 0x1613,
        PasteClips                      = 0x1614,
        EditClip                        = 0x1615,

        ClipTransposeUp                 = 0x1620,
        ClipTransposeDown               = 0x1621,
        ClipVolumeUp                    = 0x1622,
        ClipVolumeDown                  = 0x1623,

        ZoomIn                          = 0x1800,
        ZoomOut                         = 0x1801,
        ZoomEntireClip                  = 0x1802,

        Undo                            = 0x1900,
        Redo                            = 0x1901,

        TimelineJumpNext                = 0x2000,
        TimelineJumpPrevious            = 0x2001,

        ShowAnnotations                 = 0x2010,
        ResetVolumeChanges              = 0x2011,
        ApplyOpenGLRenderer             = 0x2012,

        TransportStartPlayback          = 0x2013,
        TransportPausePlayback          = 0x2014,

        PopupMenuDismiss                = 0x2015,
        PopupMenuDismissedAsDone        = 0x2016,
        PopupMenuDismissedAsCancel      = 0x2017,

        // TreePanel
        SelectRootItemPanel             = 0x2018,
        DeselectRootItemPanel           = 0x2019,
        UpdateRootItemPanel             = 0x201a,

        // WorkspaceMenu
        LoginLogout                     = 0x2020,
        OpenProject                     = 0x2021,
        CreateNewProject                = 0x2022,

        // ProjectMenu
        RenderToFLAC                    = 0x2030,
        RenderToWAV                     = 0x2031,

        AddItemsMenu                    = 0x2040,
        AddItemsMenuBack                = 0x2041,

        AddMidiTrack                    = 0x2042,
        AddMidiTrackConfirmed           = 0x2043,
        AddAutomationTrack              = 0x2044,

        ImportMidi                      = 0x2050,
        ExportMidi                      = 0x2051,

        UnloadProject                   = 0x2060,
        DeleteProject                   = 0x2061,
        DeleteProjectConfirmed1         = 0x2062,
        DeleteProjectConfirmed2         = 0x2063,

        ProjectTransposeUp              = 0x2070,
        ProjectTransposeDown            = 0x2071,
        ProjectRemoveOverlaps           = 0x2072,

        ProjectPatternEditor            = 0x2080,
        ProjectLinearEditor             = 0x2081,
        ProjectVersionsEditor           = 0x2082,

        ProjectMainMenu                 = 0x2090,
        ProjectRenderMenu               = 0x2091,
        ProjectBatchMenu                = 0x2092,
        ProjectBatchMenuBack            = 0x2093,

        AddTempoController              = 0x2100,
        BatchChangeInstrument           = 0x2110,

        SelectFunction                  = 0x2120, // more ids reserved for sub-items
        SelectScale                     = 0x2200, // more ids reserved for sub-items
        SelectTimeSignature             = 0x2300, // more ids reserved for sub-items
        SelectVersion                   = 0x2400, // more ids reserved for sub-items

        SwitchBetweenRolls              = 0x3300,
        SwitchToEditMode                = 0x3301,
        SwitchToArrangeMode             = 0x3302,
        SwitchToVersioningMode          = 0x3303,
        ShowPreviousPage                = 0x3304,
        ShowNextPage                    = 0x3305,
        ToggleShowHideConsole           = 0x3306,
        ToggleShowHideCombo             = 0x3307,

        StartDragViewport               = 0x3308,
        EndDragViewport                 = 0x3309,

        SelectAudioDeviceType           = 0x3400,
        SelectAudioDevice               = 0x3500,
        SelectSampleRate                = 0x3600,
        SelectBufferSize                = 0x3700, // more ids reserved for sub-items
        SelectFont                      = 0x3800, // more ids reserved for sub-items

        EditModeDefault                 = 0x4000,
        EditModeDraw                    = 0x4001,
        EditModePan                     = 0x4002,
        EditModeSelect                  = 0x4003,
        EditModeKnife                   = 0x4004,
        EditModeEraser                  = 0x4005,
        EditModeChordBuilder            = 0x4006,

        BeatShiftLeft                   = 0x4020,
        BeatShiftRight                  = 0x4021,
        BarShiftLeft                    = 0x4022,
        BarShiftRight                   = 0x4023,
        KeyShiftUp                      = 0x4024,
        KeyShiftDown                    = 0x4025,
        OctaveShiftUp                   = 0x4026,
        OctaveShiftDown                 = 0x4027,
        CleanupOverlaps                 = 0x4028,
        InvertChordUp                   = 0x4029,
        InvertChordDown                 = 0x402a,

        ShowArpeggiatorsPanel           = 0x4040,
        ShowVolumePanel                 = 0x4041,
        ShowRescalePanel                = 0x4042,
        ShowScalePanel                  = 0x4043,
        ShowChordPanel                  = 0x4044,

        TweakVolumeRandom               = 0x4050,
        TweakVolumeFadeOut              = 0x4051,

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

        YourNextCommandId               = 0x4100
    };

    int getIdForName(const String &command);

} // namespace CommandIDs
