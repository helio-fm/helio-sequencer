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

namespace Serialization
{
    namespace Core
    {
        static const Identifier midiRoll = "MidiRoll";
        static const Identifier treeItem = "Node";
        static const Identifier treeItemType = "Type";
        static const Identifier treeItemName = "Name";
        static const Identifier treeState = "TreeState";
        static const Identifier selectedTreeItem = "SelectedNode";
        static const Identifier treeItemId = "NodeId";

        static const Identifier workspace = "Workspace";
        static const Identifier root = "Root";
        static const Identifier settings = "Settings";
        static const Identifier instrumentsList = "Instruments";
        static const Identifier instrumentRoot = "Instrument";

        static const Identifier project = "Project";
        static const Identifier projectInfo = "ProjectInfo";
        static const Identifier projectTimeStamp = "ProjectTimeStamp";
        static const Identifier versionControl = "VersionControl";
        static const Identifier patternSet = "PatternSet";
        static const Identifier layerGroup = "Group";
        static const Identifier layer = "Layer";
        static const Identifier pianoLayer = "PianoLayer";
        static const Identifier autoLayer = "AutoLayer";
        static const Identifier projectTimeline = "ProjectTimeline";

        // Sequences
        static const Identifier track = "Track";
        static const Identifier automation = "Automation";
        static const Identifier annotations = "Annotations";
        static const Identifier timeSignatures = "TimeSignatures";
        static const Identifier keySignatures = "KeySignatures";

        // Properties
        static const Identifier trackId = "TrackId";
        static const Identifier trackColour = "Colour";
        static const Identifier trackChannel = "Channel";
        static const Identifier trackInstrumentId = "InstrumentId";
        static const Identifier trackControllerNumber = "Controller";
        static const Identifier trackMuteState = "Mute";
        static const Identifier trackSoloState = "Solo";

        // Events
        static const Identifier note = "Note";
        static const Identifier event = "Event";
        static const Identifier annotation = "Annotation";
        static const Identifier timeSignature = "TimeSignature";
        static const Identifier keySignature = "KeySignature";

        // Patterns
        static const Identifier clip = "Clip";
        static const Identifier pattern = "Pattern";

        // Scales
        static const Identifier scales = "Scales";
        static const Identifier scale = "Scale";
        static const Identifier scaleName = "Name";
        static const Identifier scaleIntervals = "Intervals";
        static const Identifier lastUsedScale = "LastUsedScale";

        static const Identifier machineID = "DeviceId";
        static const Identifier globalConfig = "Config";
        static const Identifier openGLState = "OpenGL";
        static const Identifier enabledState = "Enabled";
        static const Identifier disabledState = "Disabled";

        static const Identifier recentFiles = "RecentFiles";
        static const Identifier recentFileItem = "File";
        
        static const Identifier clipboard = "HelioClipboard";

        static const Identifier lastUsedLogin = "LastUsedLogin";
        static const Identifier firstLaunchSetupDone = "FirstLaunchSetupDone";
    } // namespace Core

    namespace Audio
    {
        static const Identifier instrument = "Instrument";
        static const Identifier instrumentId = "Uuid";
        static const Identifier instrumentName = "Name";
        static const Identifier instrumentNode = "Node";
        static const Identifier instrumentConnection = "Connection";
        static const Identifier audioPlugin = "PluginSettings";

        static const Identifier plugin = "Plugin";
        static const Identifier pluginState = "State";
        static const Identifier pluginName = "Name";
        static const Identifier pluginDescription = "DescriptiveName";
        static const Identifier pluginFormat = "Format";
        static const Identifier pluginCategory = "Category";
        static const Identifier pluginManufacturer = "Manufacturer";
        static const Identifier pluginVersion = "Version";
        static const Identifier pluginFile = "File";
        static const Identifier pluginFileModTime = "FileTime";
        static const Identifier pluginUid = "Uid";
        static const Identifier pluginIsInstrument = "IsInstrument";
        static const Identifier pluginNumInputs = "NumInputs";
        static const Identifier pluginNumOutputs = "NumOutputs";

        static const Identifier midiInput = "MidiInput";
        static const Identifier midiInputName = "Name";
        static const Identifier defaultMidiOutput = "DefaultMidiOutput";

        static const Identifier pluginManager = "PluginManager";
        static const Identifier audioCore = "AudioCore";
        static const Identifier orchestra = "Orchestra";

        static const Identifier audioDevice = "AudioDevice";
        static const Identifier audioDeviceType = "AudioDeviceType";
        static const Identifier audioOutputDeviceName = "AudioOutputDeviceName";
        static const Identifier audioInputDeviceName = "AudioInputDeviceName";
        static const Identifier audioDeviceRate = "AudioDeviceRate";
        static const Identifier audioDeviceBufferSize = "AudioDeviceBufferSize";
        static const Identifier audioDeviceInputChannels = "AudioDeviceInputChannels";
        static const Identifier audioDeviceOutputChannels = "AudioDeviceOutputChannels";
    } // namespace Audio

    namespace UI
    {
        static const Identifier lastShownPageId = "LastShownPageId";
        static const Identifier editorState = "EditorState";

        namespace Hotkeys
        {
            static const Identifier schemes = "HotkeySchemes";
            static const Identifier scheme = "HotkeyScheme";
            static const Identifier schemeName = "Name";
            static const Identifier keyPress = "KeyPress";
            static const Identifier keyDown = "KeyDown";
            static const Identifier keyUp = "KeyUp";
            static const Identifier hotkeyDescription = "Key";
            static const Identifier hotkeyReceiver = "Receiver";
            static const Identifier hotkeyCommand = "Command";
        }

        namespace Colours
        {
            static const Identifier schemes = "ColourSchemes";
            static const Identifier appliedScheme = "AppliedScheme";
            static const Identifier scheme = "Scheme";
            static const Identifier colourMap = "ColourMap";
            static const Identifier name = "Name";
            static const Identifier id = "Id";

            static const Identifier primaryGradientA = "PrimaryGradientA";
            static const Identifier primaryGradientB = "PrimaryGradientB";
            static const Identifier secondaryGradientA = "SecondaryGradientA";
            static const Identifier secondaryGradientB = "SecondaryGradientB";

            static const Identifier panelFill = "PanelFill";
            static const Identifier panelBorder = "PanelBorder";

            static const Identifier lassoFill = "LassoFill";
            static const Identifier lassoBorder = "LassoBorder";

            static const Identifier blackKey = "BlackKey";
            static const Identifier whiteKey = "WhiteKey";

            static const Identifier row = "Row";
            static const Identifier beat = "Beat";
            static const Identifier bar = "Bar";

            static const Identifier text = "Text";

            static const Identifier iconBase = "IconBase";
            static const Identifier iconShadow = "IconShadow";
        } // namespace Colours
        
    } // namespace UI
    
    namespace Clipboard
    {
        static const Identifier clipboard = "Clipboard";
        static const Identifier layer = "Layer";
        static const Identifier layerId = "LayerId";
        static const Identifier pattern = "Pattern";
        static const Identifier patternId = "PatternId";
        static const Identifier firstBeat = "FirstBeat";
        static const Identifier lastBeat = "LastBeat";
    } // namespace Clipboard

    namespace VCS
    {
        static const Identifier vcsHistoryKey = "HistoryKey";
        static const Identifier vcsHistoryId = "HistoryId";
        static const Identifier vcsHistoryVersion = "HistoryVersion";
        
        static const Identifier stashesRepository = "StashesRepository";
        static const Identifier userStashes = "UserStashes";
        static const Identifier quickStash = "QuickStash";
        static const Identifier quickStashId = "QuickStashId";

        static const Identifier pack = "Pack";
        static const Identifier packItem = "Record";
        static const Identifier packItemRevId = "ItemId";
        static const Identifier packItemDeltaId = "DeltaId";

        static const Identifier revision = "Revision";
        static const Identifier head = "Head";
        static const Identifier headIndex = "HeadIndex";
        static const Identifier headIndexData = "HeadIndexData";
        static const Identifier headRevisionId = "HeadRevisionId";
        static const Identifier commitMessage = "Message";
        static const Identifier commitTimeStamp = "Date";
        static const Identifier commitVersion = "Version";
        static const Identifier commitId = "Uuid";

        static const Identifier vcsItemId = "VCSUuid";

        static const Identifier revisionItem = "RevisionItem";
        static const Identifier revisionItemType = "Type";
        static const Identifier revisionItemName = "Name";
        static const Identifier revisionItemDiffLogic = "DiffLogic";

        static const Identifier delta = "Delta";
        static const Identifier deltaId = "Uuid";
        static const Identifier deltaName = "Name";
        static const Identifier deltaIntParam = "IntParam";
        static const Identifier deltaStringParam = "StringParam";
        static const Identifier deltaType = "Type";

        static const Identifier headStateDelta = "HeadState";
    } // namespace VCS

    namespace Api
    {
        // Session
        static const Identifier sessionLastUpdateTime = "SessionLastUpdateTime";
        static const Identifier sessionLastToken = "SessionLastToken";

        // JSON payload keys
        namespace V1
        {
            static const Identifier user = "user";
            static const Identifier session = "session";

            static const Identifier name = "name";
            static const Identifier email = "email";
            static const Identifier login = "login";
            static const Identifier password = "password";
            static const Identifier passwordConfirmation = "password_confirmation";

            static const Identifier bearer = "bearer";
            static const Identifier deviceId = "device_id";
            static const Identifier platformId = "platform_id";

            static const Identifier token = "token";
            static const Identifier errors = "errors";
        } // namespace V1
    } // namespace Api
    
    namespace Locales
    {
        static const Identifier metaSymbol = "{x}";

        static const Identifier wrapperClassName = "PluralForm";
        static const Identifier wrapperMethodName = "Detect";
        static const Identifier currentLocale = "CurrentLocale";
        
        static const Identifier translations = "Translations";
        static const Identifier locale = "Locale";
        static const Identifier literal = "Literal";
        static const Identifier author = "Author";
        static const Identifier name = "Name";
        static const Identifier id = "Id";
        
        static const Identifier pluralForms = "PluralForms";
        static const Identifier equation = "Equation";
        
        static const Identifier pluralLiteral = "PluralLiteral";
        static const Identifier pluralForm = "PluralForm";
        static const Identifier translation = "Translation";
    }  // namespace Locales
    
    namespace Arps
    {
        static const Identifier arpeggiator = "Arpeggiator";
        static const Identifier arpeggiators = "Arpeggiators";
        
        static const Identifier sequence = "Sequence";
        static const Identifier isReversed = "IsReversed";
        static const Identifier relativeMapping = "RelativeMapping";
        static const Identifier limitsToChord = "LimitsToChord";
        static const Identifier scale = "Scale";

        static const Identifier id = "Id";
        static const Identifier name = "Name";
    }  // namespace Arps
    
    namespace Undo
    {
        static const Identifier undoStack = "UndoStack";
        static const Identifier transaction = "Transaction";

        static const Identifier name = "Name";
        static const Identifier xPath = "Path";
        static const Identifier trackId = "TrackId";
        static const Identifier group = "Group";

        static const Identifier xPathBefore = "PathBefore";
        static const Identifier xPathAfter = "PathAfter";
        static const Identifier colourBefore = "ColourBefore";
        static const Identifier colourAfter = "ColourAfter";
        static const Identifier instrumentIdBefore = "InstrumentIdBefore";
        static const Identifier instrumentIdAfter = "InstrumentIdAfter";
        static const Identifier muteStateBefore = "MuteStateBefore";
        static const Identifier muteStateAfter = "MuteStateAfter";
        
        static const Identifier annotationBefore = "AnnotationBefore";
        static const Identifier annotationAfter = "AnnotationAfter";
        static const Identifier timeSignatureBefore = "TimeSignatureBefore";
        static const Identifier timeSignatureAfter = "TimeSignatureAfter";
        static const Identifier keySignatureBefore = "KeySignatureBefore";
        static const Identifier keySignatureAfter = "KeySignatureAfter";
        static const Identifier eventBefore = "EventBefore";
        static const Identifier eventAfter = "EventAfter";
        static const Identifier noteBefore = "NoteBefore";
        static const Identifier noteAfter = "NoteAfter";
        static const Identifier groupBefore = "GroupBefore";
        static const Identifier groupAfter = "GroupAfter";
        static const Identifier instanceBefore = "InstanceBefore";
        static const Identifier instanceAfter = "InstanceAfter";

        static const Identifier pianoTrackInsertAction = "PianoTrackInsertAction";
        static const Identifier pianoTrackRemoveAction = "PianoTrackRemoveAction";
        
        static const Identifier automationTrackInsertAction = "AutomationTrackInsertAction";
        static const Identifier automationTrackRemoveAction = "AutomationTrackRemoveAction";
        
        static const Identifier midiTrackRenameAction = "MidiTrackRenameAction";
        
        static const Identifier midiTrackChangeColourAction = "MidiTrackChangeColourAction";
        static const Identifier midiTrackChangeInstrumentAction = "MidiTrackChangeInstrumentAction";
        static const Identifier midiTrackMuteAction = "MidiTrackMuteAction";
        
        static const Identifier patternClipInsertAction = "PatternClipInsertAction";
        static const Identifier patternClipRemoveAction = "PatternClipRemoveAction";
        static const Identifier patternClipChangeAction = "PatternClipChangeAction";

        static const Identifier noteInsertAction = "NoteInsertAction";
        static const Identifier noteRemoveAction = "NoteRemoveAction";
        static const Identifier noteChangeAction = "NoteChangeAction";
        static const Identifier notesGroupInsertAction = "NotesGroupInsertAction";
        static const Identifier notesGroupRemoveAction = "NotesGroupRemoveAction";
        static const Identifier notesGroupChangeAction = "NotesGroupChangeAction";
        
        static const Identifier annotationEventInsertAction = "AnnotationEventInsertAction";
        static const Identifier annotationEventRemoveAction = "AnnotationEventRemoveAction";
        static const Identifier annotationEventChangeAction = "AnnotationEventChangeAction";
        static const Identifier annotationEventsGroupInsertAction = "AnnotationEventsGroupInsertAction";
        static const Identifier annotationEventsGroupRemoveAction = "AnnotationEventsGroupRemoveAction";
        static const Identifier annotationEventsGroupChangeAction = "AnnotationEventsGroupChangeAction";

        static const Identifier timeSignatureEventInsertAction = "TimeSignatureEventInsertAction";
        static const Identifier timeSignatureEventRemoveAction = "TimeSignatureEventRemoveAction";
        static const Identifier timeSignatureEventChangeAction = "TimeSignatureEventChangeAction";
        static const Identifier timeSignatureEventsGroupInsertAction = "TimeSignatureEventsGroupInsertAction";
        static const Identifier timeSignatureEventsGroupRemoveAction = "TimeSignatureEventsGroupRemoveAction";
        static const Identifier timeSignatureEventsGroupChangeAction = "TimeSignatureEventsGroupChangeAction";

        static const Identifier keySignatureEventInsertAction = "KeySignatureEventInsertAction";
        static const Identifier keySignatureEventRemoveAction = "KeySignatureEventRemoveAction";
        static const Identifier keySignatureEventChangeAction = "KeySignatureEventChangeAction";
        static const Identifier keySignatureEventsGroupInsertAction = "KeySignatureEventsGroupInsertAction";
        static const Identifier keySignatureEventsGroupRemoveAction = "KeySignatureEventsGroupRemoveAction";
        static const Identifier keySignatureEventsGroupChangeAction = "KeySignatureEventsGroupChangeAction";

        static const Identifier automationEventInsertAction = "AutomationEventInsertAction";
        static const Identifier automationEventRemoveAction = "AutomationEventRemoveAction";
        static const Identifier automationEventChangeAction = "AutomationEventChangeAction";
        static const Identifier automationEventsGroupInsertAction = "AutomationEventsGroupInsertAction";
        static const Identifier automationEventsGroupRemoveAction = "AutomationEventsGroupRemoveAction";
        static const Identifier automationEventsGroupChangeAction = "AutomationEventsGroupChangeAction";
    } // namespace Undo
}  // namespace Serialization
