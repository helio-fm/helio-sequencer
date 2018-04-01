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

        // ArpeggiatorEditorPanel
        ResetArpeggiatorChanges         = 0x0200,
        ApplyArpeggiator                = 0x0201, // more ids reserved for arps

        InitWorkspace                   = 0x0500,

        RenameInstrument                = 0x0501,
        UpdateInstrument                = 0x0502,
        DeleteInstrument                = 0x0503,

        ScanAllPlugins                  = 0x0504,
        ScanPluginsFolder               = 0x0505,
        CreateInstrument                = 0x0506, // more ids reserved for instruments

        // MidiTrackCommandPanel
        DeleteTrack                     = 0x1000,
        MuteTrack                       = 0x1001,
        UnmuteTrack                     = 0x1002,
        RenameTrack                     = 0x1003,
        SelectTrackColour               = 0x1005,
        SelectTrackInstrument           = 0x1006,
        SelectAllEvents                 = 0x1007,
        DuplicateTrackTo                = 0x1008,

        SetTrackColour                  = 0x1009, // more ids reserved for colours
        SetTrackInstrument              = 0x1100, // more ids reserved for instruments
        MoveTrackToProject              = 0x1500, // more ids reserved for projects

        // MidiRollCommandPanel
        DeleteEvents                    = 0x1601,
        CopyEvents                      = 0x1602,
        CutEvents                       = 0x1603,
        PasteEvents                     = 0x1604,

        CursorTool                      = 0x2000,
        DrawTool                        = 0x2001,
        SelectionTool                   = 0x2002,
        ZoomTool                        = 0x2003,
        DragTool                        = 0x2004,
        InsertSpaceTool                 = 0x2005,
        WipeSpaceTool                   = 0x2006,
        ScissorsTool                    = 0x2007,

        ZoomIn                          = 0x2008,
        ZoomOut                         = 0x2009,

        Undo                            = 0x200a,
        Redo                            = 0x200b,

        ArpeggiateNotes                 = 0x200e,
        RefactorNotes                   = 0x200c,
        BatchTweakNotes                 = 0x200d,
        TweakNotesVolume                = 0x200f,

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

        // Version control
        VersionControlForcePull         = 0x201b,
        VersionControlReset             = 0x201c,
        VersionControlAmend             = 0x201d,
        VersionControlCommit            = 0x201e,

        // WorkspaceMenu
        LoginLogout                     = 0x2020,
        OpenProject                     = 0x2021,
        CreateNewProject                = 0x2022,

        // ProjectCommandPanel
        RenderToFLAC                    = 0x2030,
        RenderToOGG                     = 0x2031,
        RenderToWAV                     = 0x2032,

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

        RefactorTransposeUp             = 0x2070,
        RefactorTransposeDown           = 0x2071,
        RefactorRemoveOverlaps          = 0x2072,

        ProjectPatternEditor            = 0x2080,
        ProjectLinearEditor             = 0x2081,
        ProjectVersionsEditor           = 0x2082,

        ProjectMainMenu                 = 0x2090,
        ProjectRenderMenu               = 0x2091,
        ProjectBatchMenu                = 0x2092,
        ProjectBatchMenuBack            = 0x2093,
        ProjectInstrumentsMenu          = 0x2094, // more ids reserved for instruments

        AddTempoController              = 0x2500,
        AddCustomController             = 0x2501, // more ids reserved for controllers

        BatchChangeInstrument           = 0x2600,
        BatchSetInstrument              = 0x2601, // more ids reserved for instruments

        DismissModalDialogAsync         = 0x3000,

        SelectFunction                  = 0x3010,
        SelectScale                     = 0x3011, // more ids reserved for sub-items
        SelectTimeSignature             = 0x3200, // more ids reserved for sub-items

        SwitchBetweenRolls              = 0x3300,
        ShowPreviousPage                = 0x3301,
        ShowNextPage                    = 0x3302,
        ToggleShowHideConsole           = 0x3303,
        ToggleShowHideCombo             = 0x3304,

        StartDragViewport               = 0x3305,
        EndDragViewport                 = 0x3306,

        SelectAudioDeviceType           = 0x3400,
        SelectAudioDevice               = 0x3500,
        SelectSampleRate                = 0x3600,
        SelectBufferSize                = 0x3700, // more ids reserved for sub-items

        EditModeDefault                 = 0x4000,
        EditModeDraw                    = 0x4001,
        EditModePan                     = 0x4002,
        EditModeWipeSpace               = 0x4003,
        EditModeInsertSpace             = 0x4004,
        EditModeSelect                  = 0x4005,
        
        BeatShiftLeft                   = 0x4050,
        BeatShiftRight                  = 0x4051,
        BarShiftLeft                    = 0x4052,
        BarShiftRight                   = 0x4053,
        KeyShiftUp                      = 0x4054,
        KeyShiftDown                    = 0x4055,
        OctaveShiftUp                   = 0x4056,
        OctaveShiftDown                 = 0x4057,
        CleanupOverlaps                 = 0x4058,
        InvertChordUp                   = 0x4059,
        InvertChordDown                 = 0x405a,
        
        ToggleQuickStash                = 0x405b,
        ShowArpeggiatorsPanel           = 0x405c,
        ShowVolumePanel                 = 0x405d,
        TweakVolumeRandom               = 0x405e,
        TweakVolumeFadeOut              = 0x405f,

        CreateArpeggiatorFromSelection  = 0x4060,

        YourNextCommandId               = 0x4060
    };

    int getIdForName(const String &command);

} // namespace CommandIDs
