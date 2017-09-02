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
    enum
    {
        Back                            = 0x000001,
        Cancel                          = 0x000002,
        Browse                          = 0x000003,
        IconButtonPressed               = 0x000004,
        MenuButtonPressed               = 0x000005,
        RootTreeItemPressed             = 0x000006,
        HideRollover                    = 0x000007,
        HideDialog                      = 0x000008,
        HideCallout                     = 0x000009,
        
        // TimeSignatureCommandPanel
        ChangeTimeSignature             = 0x000100,
        ChangeTimeSignatureConfirmed    = 0x000101,
        DeleteTimeSignature             = 0x000102,

        // AnnotationCommandPanel
        RenameAnnotation                = 0x000110,
        RenameAnnotationConfirmed       = 0x000111,
        SetAnnotationColour             = 0x000112, // more ids reserved for colours
        DeleteAnnotation                = 0x001000,
        
        // AnnotationsCommandPanel
        AddAnnotation                   = 0x001001,
        AddAnnotationConfirmed          = 0x001002,
        AddTimeSignature                = 0x001003,
        AddTimeSignatureConfirmed       = 0x001004,
        JumpToAnnotation                = 0x001005, // more ids reserved for annotations

        // ArpeggiatorEditorPanel
        ResetArpeggiatorChanges         = 0x002000,
        ApplyArpeggiator                = 0x002001, // more ids reserved for arps
        
        InitWorkspace                   = 0x003001,
        
        RenameInstrument                = 0x003002,
        UpdateInstrument                = 0x003003,
        DeleteInstrument                = 0x003004,

        ScanAllPlugins                  = 0x003005,
        ScanPluginsFolder               = 0x003006,
        CreateInstrument                = 0x003007, // more ids reserved for instruments
        
        // LayerCommandPanel
        DeleteLayer                     = 0x004000,
        MuteLayer                       = 0x004001,
        UnmuteLayer                     = 0x004002,
        RenameLayer                     = 0x004003,
        RenameLayerConfirmed            = 0x004004,
        SelectLayerColour               = 0x004005,
        SelectLayerInstrument           = 0x004006,
        SelectAllEvents                 = 0x004007,
        DuplicateLayerTo                = 0x004008,
        
        SetLayerColour                  = 0x004009, // more ids reserved for layers
        SetLayerInstrument              = 0x005000, // more ids reserved for instruments
        MoveLayerToProject              = 0x006000, // more ids reserved for projects
        
        // MidiRollCommandPanel
        EditEvents                      = 0x007000,
        DeleteEvents                    = 0x007001,
        
        CopyEvents                      = 0x007002,
        CutEvents                       = 0x007003,
        PasteEvents                     = 0x007004,
        MoveEventsToLayer               = 0x007005, // more ids reserved for layers
        
        CursorTool                      = 0x008000,
        DrawTool                        = 0x008001,
        SelectionTool                   = 0x008002,
        ZoomTool                        = 0x008003,
        DragTool                        = 0x008004,
        InsertSpaceTool                 = 0x008005,
        WipeSpaceTool                   = 0x008006,
        ScissorsTool                    = 0x008007,
        
        ZoomIn                          = 0x008008,
        ZoomOut                         = 0x008009,
        
        Undo                            = 0x008010,
        Redo                            = 0x008011,
        
        DeleteNotes                     = 0x008012,
        DuplicateNotes                  = 0x008013,
        ArpNotes                        = 0x008014,
        TweakNotesVolume                = 0x008015,
        
        ShowAnnotations                 = 0x008016,

        ResetVolumeChanges              = 0x009000,
        
        ApplyOpenGLRenderer             = 0x010000,
        
        TransportStartPlayback          = 0x011000,
        TransportPausePlayback          = 0x011001,

        PopupMenuDismiss                = 0x012000,
        PopupMenuDismissedAsDone        = 0x012001,
        PopupMenuDismissedAsCancel      = 0x012002,

        // TreePanel
        SelectRootItemPanel             = 0x013000,
        DeselectRootItemPanel           = 0x013001,
        UpdateRootItemPanel             = 0x013002,
        
        // Version control
        VersionControlForcePull         = 0x014000,
        VersionControlReset             = 0x014001,
        VersionControlAmend             = 0x014002,
        VersionControlCommit            = 0x014003,
        
        // WorkspaceMenu
        LoginLogout                     = 0x015000,
        OpenProject                     = 0x015001,
        CreateNewProject                = 0x015002,
        
        // ProjectCommandPanel
        RenderToFLAC                    = 0x016000,
        RenderToOGG                     = 0x016001,
        RenderToWAV                     = 0x016002,
        
        AddLayer                        = 0x016003,
        AddLayerConfirmed               = 0x016004,
        AddAutomation                   = 0x016005,
        
        ImportMidi                      = 0x016006,
        ExportMidi                      = 0x016007,
        
        UnloadProject                   = 0x016008,
        DeleteProject                   = 0x016009,
        DeleteProjectConfirmed1         = 0x016010,
        DeleteProjectConfirmed2         = 0x016011,
        
        RefactorTransposeUp             = 0x016012,
        RefactorTransposeDown           = 0x016013,
        RefactorRemoveOverlaps          = 0x016014,
        
        ProjectMainMenu                 = 0x016015,
        ProjectRenderMenu               = 0x016016,
        ProjectBatchMenu                = 0x016017,
        ProjectAutomationsMenu          = 0x016018,
        ProjectInstrumentsMenu          = 0x016019, // more ids reserved for instruments
        
        AddTempoController              = 0x017000,
        AddCustomController             = 0x017001, // more ids reserved for controllers
        
        BatchChangeInstrument           = 0x018000,
        BatchSetInstrument              = 0x018001, // more ids reserved for instruments

        DismissModalDialogAsync         = 0x020000,

        HeadlineSelectSubitem           = 0x020010, // more ids reserved for sub-items

        YourNextCommandId               = 0x021000
    };
} // namespace CommandIDs
