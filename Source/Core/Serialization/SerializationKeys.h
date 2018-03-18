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
        static const Identifier treeRoot = "tree";
        static const Identifier treeItem = "node";
        static const Identifier treeItemType = "type";
        static const Identifier treeItemName = "name";
        static const Identifier treeState = "treeState";
        static const Identifier selectedTreeItem = "selectedNode";
        static const Identifier treeItemId = "nodeId";

        static const Identifier workspace = "workspace";
        static const Identifier root = "root";
        static const Identifier settings = "settings";
        static const Identifier instrumentsList = "instruments";
        static const Identifier instrumentRoot = "instrument";

        static const Identifier project = "project";
        static const Identifier projectInfo = "projectInfo";
        static const Identifier projectTimeStamp = "projectTimeStamp";
        static const Identifier versionControl = "versionControl";
        static const Identifier patternSet = "patternSet";
        static const Identifier trackGroup = "group";
        static const Identifier track = "track";
        static const Identifier pianoTrack = "pianoTrack";
        static const Identifier automationTrack = "automationTrack";
        static const Identifier projectTimeline = "projectTimeline";

        // Properties
        static const Identifier trackId = "trackId";
        static const Identifier trackColour = "colour";
        static const Identifier trackChannel = "channel";
        static const Identifier trackInstrumentId = "instrumentId";
        static const Identifier trackControllerNumber = "controller";
        static const Identifier trackMuteState = "mute";
        static const Identifier trackSoloState = "solo";

        // Timeline
        static const Identifier annotationsTrackId = "annotationsTrackId";
        static const Identifier keySignaturesTrackId = "keySignaturesTrackId";
        static const Identifier timeSignaturesTrackId = "timeSignaturesTrackId";

        static const Identifier machineID = "deviceId";
        static const Identifier globalConfig = "config";
        static const Identifier openGLState = "openGL";
        static const Identifier enabledState = "enabled";
        static const Identifier disabledState = "disabled";

        static const Identifier recentFiles = "recentFiles";
        static const Identifier recentFileItem = "file";
        static const Identifier recentFileTitle = "title";
        static const Identifier recentFilePath = "path";
        static const Identifier recentFileProjectId = "id";
        static const Identifier recentFileTime = "time";

        static const Identifier filePath = "filePath";

        static const Identifier clipboard = "helioClipboard";
    } // namespace Core

    namespace Midi
    {
        // Scales
        static const Identifier scales = "scales";
        static const Identifier scale = "scale";
        static const Identifier scaleName = "name";
        static const Identifier scaleIntervals = "intervals";

        // Sequences
        static const Identifier track = "track";
        static const Identifier automation = "automation";
        static const Identifier annotations = "annotations";
        static const Identifier timeSignatures = "timeSignatures";
        static const Identifier keySignatures = "keySignatures";

        // Events
        static const Identifier note = "note";
        static const Identifier automationEvent = "event";
        static const Identifier annotation = "annotation";
        static const Identifier timeSignature = "timeSignature";
        static const Identifier keySignature = "keySignature";

        // Patterns
        static const Identifier clip = "clip";
        static const Identifier pattern = "pattern";

        // Properties

        // Notes are the most common records in the savefile
        // therefore their properties are so 
        static const Identifier id = "id";
        static const Identifier key = "key";
        static const Identifier timestamp = "ts";
        static const Identifier length = "len";
        static const Identifier volume = "vol";

        static const Identifier text = "text";
        static const Identifier colour = "colour";

        static const Identifier value = "value";
        static const Identifier curve = "curve";

        static const Identifier numerator = "numerator";
        static const Identifier denominator = "denominator";
    } // namespace Events

    namespace Audio
    {
        static const Identifier instrument = "instrument";
        static const Identifier instrumentId = "id";
        static const Identifier instrumentName = "name";

        static const Identifier node = "node";
        static const Identifier nodeId = "id";
        static const Identifier nodeHash = "hash";
        static const Identifier sourceNodeId = "sourceNodeId";
        static const Identifier destinationNodeId = "destinationNodeId";
        static const Identifier sourceChannel = "sourceChannel";
        static const Identifier destinationChannel = "destinationChannel";
        static const Identifier connection = "connection";

        static const Identifier transport = "transport";
        static const Identifier transportSeekPosition = "seekPosition";

        static const Identifier audioPlugin = "pluginSettings";

        static const Identifier plugin = "plugin";
        static const Identifier pluginState = "state";
        static const Identifier pluginName = "name";
        static const Identifier pluginDescription = "descriptiveName";
        static const Identifier pluginFormat = "format";
        static const Identifier pluginCategory = "category";
        static const Identifier pluginManufacturer = "manufacturer";
        static const Identifier pluginVersion = "version";
        static const Identifier pluginFile = "file";
        static const Identifier pluginFileModTime = "fileTime";
        static const Identifier pluginId = "id";
        static const Identifier pluginIsInstrument = "isInstrument";
        static const Identifier pluginNumInputs = "numInputs";
        static const Identifier pluginNumOutputs = "numOutputs";

        static const Identifier midiInput = "midiInput";
        static const Identifier midiInputName = "name";
        static const Identifier defaultMidiOutput = "defaultMidiOutput";

        static const Identifier pluginsList = "plugins";
        static const Identifier audioCore = "audioCore";
        static const Identifier orchestra = "orchestra";

        static const Identifier audioDevice = "audioDevice";
        static const Identifier audioDeviceType = "deviceType";
        static const Identifier audioInputDeviceName = "inputDeviceName";
        static const Identifier audioOutputDeviceName = "outputDeviceName";
        static const Identifier audioDeviceRate = "sampleRate";
        static const Identifier audioDeviceBufferSize = "bufferSize";
        static const Identifier audioDeviceInputChannels = "inputChannels";
        static const Identifier audioDeviceOutputChannels = "outputChannels";
    } // namespace Audio

    namespace Config
    {
        static const Identifier activeWorkspace = "activeWorkspace";
        static const Identifier activeColourScheme = "activeColourScheme";
        static const Identifier activeHotkeyScheme = "activeHotkeyScheme";
        static const Identifier currentLocale = "currentLocale";
        static const Identifier lastShownPageId = "lastShownPageId";
        static const Identifier lastUsedScale = "lastUsedScale";
        static const Identifier lastUsedLogin = "lastUsedLogin";
        static const Identifier lastUpdatesInfo = "lastUpdatesInfo";
    } // namespace Config

    // Available types of dynamically fetched resources/configs
    namespace Resources
    {
        static const Identifier scales = "scales";
        static const Identifier arpeggiators = "arpeggiators";
        static const Identifier translations = "translations";
        static const Identifier colourSchemes = "colourSchemes";
        static const Identifier hotkeySchemes = "hotkeySchemes";
    }

    namespace UI
    {
        static const Identifier sequencer = "sequencer";

        static const Identifier pianoRoll = "pianoRoll";
        static const Identifier patternRoll = "patternRoll";

        static const Identifier startBar = "startBar";
        static const Identifier endBar = "endBar";
        static const Identifier barWidth = "barWidth";
        static const Identifier rowHeight = "rowHeight";
        static const Identifier viewportPositionX = "viewportX";
        static const Identifier viewportPositionY = "viewportY";

        static const Identifier positionX = "positionX";
        static const Identifier positionY = "positionY";

        namespace Hotkeys
        {
            static const Identifier schemes = "hotkeySchemes";
            static const Identifier scheme = "hotkeyScheme";
            static const Identifier schemeName = "name";
            static const Identifier keyPress = "keyPress";
            static const Identifier keyDown = "keyDown";
            static const Identifier keyUp = "keyUp";
            static const Identifier hotkeyDescription = "key";
            static const Identifier hotkeyReceiver = "receiver";
            static const Identifier hotkeyCommand = "command";
        }

        namespace Colours
        {
            static const Identifier schemes = "colourSchemes";
            static const Identifier scheme = "colourScheme";
            static const Identifier colourMap = "colourMap";
            static const Identifier name = "name";
            static const Identifier id = "id";

            static const Identifier primaryGradientA = "primaryGradientA";
            static const Identifier primaryGradientB = "primaryGradientB";
            static const Identifier secondaryGradientA = "secondaryGradientA";
            static const Identifier secondaryGradientB = "secondaryGradientB";

            static const Identifier panelFill = "panelFill";
            static const Identifier panelBorder = "panelBorder";

            static const Identifier lassoFill = "lassoFill";
            static const Identifier lassoBorder = "lassoBorder";

            static const Identifier blackKey = "blackKey";
            static const Identifier whiteKey = "whiteKey";

            static const Identifier row = "row";
            static const Identifier beat = "beat";
            static const Identifier bar = "bar";

            static const Identifier text = "text";

            static const Identifier iconBase = "iconBase";
            static const Identifier iconShadow = "iconShadow";
        } // namespace Colours
        
    } // namespace UI
    
    namespace Clipboard
    {
        static const Identifier clipboard = "clipboard";
        static const Identifier layer = "layer";
        static const Identifier layerId = "layerId";
        static const Identifier pattern = "pattern";
        static const Identifier patternId = "patternId";
        static const Identifier firstBeat = "firstBeat";
        static const Identifier lastBeat = "lastBeat";
    } // namespace Clipboard

    namespace VCS
    {
        static const Identifier vcsHistoryKey = "historyKey";
        static const Identifier vcsHistoryKeyData = "data";
        static const Identifier vcsHistoryId = "historyId";
        static const Identifier vcsHistoryVersion = "historyVersion";
        
        static const Identifier stashesRepository = "stashesRepository";
        static const Identifier userStashes = "userStashes";
        static const Identifier quickStash = "quickStash";
        static const Identifier quickStashId = "quickStashId";

        static const Identifier pack = "deltaPack";
        static const Identifier packItem = "record";
        static const Identifier packItemRevId = "itemId";
        static const Identifier packItemDeltaId = "deltaId";

        static const Identifier revision = "revision";
        static const Identifier head = "head";
        static const Identifier headIndex = "snapshot";
        static const Identifier headIndexData = "snapshotData";
        static const Identifier headRevisionId = "headRevisionId";
        static const Identifier commitMessage = "message";
        static const Identifier commitTimeStamp = "date";
        static const Identifier commitVersion = "version";
        static const Identifier commitId = "id";

        static const Identifier vcsItemId = "vcsId";

        static const Identifier revisionItem = "revisionItem";
        static const Identifier revisionItemType = "type";
        static const Identifier revisionItemName = "name";
        static const Identifier revisionItemDiffLogic = "diffLogic";

        static const Identifier delta = "delta";
        static const Identifier deltaId = "id";
        static const Identifier deltaName = "name";
        static const Identifier deltaIntParam = "intParam";
        static const Identifier deltaStringParam = "stringParam";
        static const Identifier deltaTypeId = "type";

        static const Identifier headStateDelta = "headState";

        namespace ProjectInfoDeltas
        {
            static const Identifier projectLicense = "license";
            static const Identifier projectTitle = "title";
            static const Identifier projectAuthor = "author";
            static const Identifier projectDescription = "description";
        }

        namespace MidiTrackDeltas
        {
            static const Identifier trackPath = "path";
            static const Identifier trackMute = "mute";
            static const Identifier trackSolo = "solo";
            static const Identifier trackColour = "colour";
            static const Identifier trackInstrument = "instrument";
            static const Identifier trackController = "controller";
        }

        namespace PatternDeltas
        {
            static const Identifier clipsAdded = "clipsAdded";
            static const Identifier clipsRemoved = "clipsRemoved";
            static const Identifier clipsChanged = "clipsChanged";
        }

        namespace AutoSequenceDeltas
        {
            static const Identifier eventsAdded = "eventsAdded";
            static const Identifier eventsRemoved = "eventsRemoved";
            static const Identifier eventsChanged = "eventsChanged";
        }

        namespace PianoSequenceDeltas
        {
            static const Identifier notesAdded = "notesAdded";
            static const Identifier notesRemoved = "notesRemoved";
            static const Identifier notesChanged = "notesChanged";
        }

        namespace ProjectTimelineDeltas
        {
            static const Identifier annotationsAdded = "annotationsAdded";
            static const Identifier annotationsRemoved = "annotationsRemoved";
            static const Identifier annotationsChanged = "annotationsChanged";

            static const Identifier timeSignaturesAdded = "timeSignaturesAdded";
            static const Identifier timeSignaturesRemoved = "timeSignaturesRemoved";
            static const Identifier timeSignaturesChanged = "timeSignaturesChanged";

            static const Identifier keySignaturesAdded = "keySignaturesAdded";
            static const Identifier keySignaturesRemoved = "keySignaturesRemoved";
            static const Identifier keySignaturesChanged = "keySignaturesChanged";
        }
    } // namespace VCS

    namespace Api
    {
        // Session
        static const Identifier sessionLastUpdateTime = "sessionLastUpdateTime";
        static const Identifier sessionLastToken = "sessionLastToken";
        static const Identifier sessionUserProfile = "sessionUserProfile";

        static const Identifier updatesInfo = "updatesInfo";

        namespace PlatformIds
        {
            static const Identifier windows32 = "windows32";
            static const Identifier windows64 = "windows64";
            static const Identifier linux32 = "linux32";
            static const Identifier linux64 = "linux64";
            static const Identifier mac = "mac";
            static const Identifier ios = "ios";
            static const Identifier android = "android";
            static const Identifier unknown = "unknown";
        }

        // JSON keys
        namespace V1
        {
            static const Identifier rootElementSuccess = "data";
            static const Identifier rootElementErrors = "errors";

            static const Identifier status = "status";
            static const Identifier message = "message";

            static const Identifier user = "user";
            static const Identifier session = "session";

            static const Identifier name = "name";
            static const Identifier email = "email";
            static const Identifier login = "login";
            static const Identifier password = "password";
            static const Identifier passwordConfirmation = "passwordConfirmation";

            static const Identifier bearer = "bearer";
            static const Identifier deviceId = "deviceId";
            static const Identifier platformId = "platformId";

            static const Identifier token = "token";

            static const Identifier versionInfo = "versionInfo";
            static const Identifier version = "version";
            static const Identifier link = "link";
            static const Identifier resourceInfo = "resourceInfo";
            static const Identifier resourceName = "resourceName";
            static const Identifier hash = "hash";
        } // namespace V1
    } // namespace Api
    
    namespace Translations
    {
        static const Identifier metaSymbol = "{x}";

        static const Identifier wrapperClassName = "pluralForm";
        static const Identifier wrapperMethodName = "detect";
        
        static const Identifier translations = "translations";
        static const Identifier locale = "locale";
        static const Identifier literal = "literal";
        static const Identifier author = "author";
        static const Identifier name = "name";
        static const Identifier translation = "translation";
        static const Identifier id = "id";

        static const Identifier pluralEquation = "pluralEquation";
        static const Identifier pluralLiteral = "pluralLiteral";
        static const Identifier pluralForm = "pluralForm";
    }  // namespace Translations
    
    namespace Arps
    {
        static const Identifier arpeggiator = "arpeggiator";
        static const Identifier arpeggiators = "arpeggiators";
        
        static const Identifier sequence = "sequence";
        static const Identifier isReversed = "isReversed";
        static const Identifier relativeMapping = "relativeMapping";
        static const Identifier limitsToChord = "limitsToChord";
        static const Identifier scale = "scale";

        static const Identifier id = "id";
        static const Identifier name = "name";
    }  // namespace Arps
    
    namespace Undo
    {
        static const Identifier undoStack = "undoStack";
        static const Identifier transaction = "transaction";

        static const Identifier name = "name";
        static const Identifier xPath = "path";
        static const Identifier trackId = "trackId";
        static const Identifier group = "group";

        static const Identifier xPathBefore = "pathBefore";
        static const Identifier xPathAfter = "pathAfter";
        static const Identifier colourBefore = "colourBefore";
        static const Identifier colourAfter = "colourAfter";
        static const Identifier instrumentIdBefore = "instrumentIdBefore";
        static const Identifier instrumentIdAfter = "instrumentIdAfter";
        static const Identifier muteStateBefore = "muteStateBefore";
        static const Identifier muteStateAfter = "muteStateAfter";
        
        static const Identifier annotationBefore = "annotationBefore";
        static const Identifier annotationAfter = "annotationAfter";
        static const Identifier timeSignatureBefore = "timeSignatureBefore";
        static const Identifier timeSignatureAfter = "timeSignatureAfter";
        static const Identifier keySignatureBefore = "keySignatureBefore";
        static const Identifier keySignatureAfter = "keySignatureAfter";
        static const Identifier eventBefore = "eventBefore";
        static const Identifier eventAfter = "eventAfter";
        static const Identifier noteBefore = "noteBefore";
        static const Identifier noteAfter = "noteAfter";
        static const Identifier groupBefore = "groupBefore";
        static const Identifier groupAfter = "groupAfter";
        static const Identifier instanceBefore = "instanceBefore";
        static const Identifier instanceAfter = "instanceAfter";

        static const Identifier pianoTrackInsertAction = "pianoTrackInsert";
        static const Identifier pianoTrackRemoveAction = "pianoTrackRemove";
        
        static const Identifier automationTrackInsertAction = "automationTrackInsert";
        static const Identifier automationTrackRemoveAction = "automationTrackRemove";
        
        static const Identifier midiTrackRenameAction = "midiTrackRename";
        
        static const Identifier midiTrackChangeColourAction = "midiTrackChangeColour";
        static const Identifier midiTrackChangeInstrumentAction = "midiTrackChangeInstrument";
        static const Identifier midiTrackMuteAction = "midiTrackMute";
        
        static const Identifier patternClipInsertAction = "patternClipInsert";
        static const Identifier patternClipRemoveAction = "patternClipRemove";
        static const Identifier patternClipChangeAction = "patternClipChange";

        static const Identifier noteInsertAction = "noteInsert";
        static const Identifier noteRemoveAction = "noteRemove";
        static const Identifier noteChangeAction = "noteChange";
        static const Identifier notesGroupInsertAction = "notesInsert";
        static const Identifier notesGroupRemoveAction = "notesRemove";
        static const Identifier notesGroupChangeAction = "notesChange";
        
        static const Identifier annotationEventInsertAction = "annotationInsert";
        static const Identifier annotationEventRemoveAction = "annotationRemove";
        static const Identifier annotationEventChangeAction = "annotationChange";
        static const Identifier annotationEventsGroupInsertAction = "annotationsInsert";
        static const Identifier annotationEventsGroupRemoveAction = "annotationsRemove";
        static const Identifier annotationEventsGroupChangeAction = "annotationsChange";

        static const Identifier timeSignatureEventInsertAction = "timeSignatureInsert";
        static const Identifier timeSignatureEventRemoveAction = "timeSignatureRemove";
        static const Identifier timeSignatureEventChangeAction = "timeSignatureChange";
        static const Identifier timeSignatureEventsGroupInsertAction = "timeSignaturesInsert";
        static const Identifier timeSignatureEventsGroupRemoveAction = "timeSignaturesRemove";
        static const Identifier timeSignatureEventsGroupChangeAction = "timeSignaturesChange";

        static const Identifier keySignatureEventInsertAction = "keySignatureInsert";
        static const Identifier keySignatureEventRemoveAction = "keySignatureRemove";
        static const Identifier keySignatureEventChangeAction = "keySignatureChange";
        static const Identifier keySignatureEventsGroupInsertAction = "keySignaturesInsert";
        static const Identifier keySignatureEventsGroupRemoveAction = "keySignaturesRemove";
        static const Identifier keySignatureEventsGroupChangeAction = "keySignaturesChange";

        static const Identifier automationEventInsertAction = "automationEventInsert";
        static const Identifier automationEventRemoveAction = "automationEventRemove";
        static const Identifier automationEventChangeAction = "automationEventChange";
        static const Identifier automationEventsGroupInsertAction = "automationEventsInsert";
        static const Identifier automationEventsGroupRemoveAction = "automationEventsRemove";
        static const Identifier automationEventsGroupChangeAction = "automationEventsChange";
    } // namespace Undo
}  // namespace Serialization
