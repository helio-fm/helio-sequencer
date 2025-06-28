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

namespace Serialization
{
    namespace Core
    {
        static const Identifier treeRoot = "tree";
        static const Identifier treeNode = "node";
        static const Identifier treeNodeType = "type";
        static const Identifier treeNodeName = "name";
        static const Identifier treeState = "treeState";
        static const Identifier selectedTreeNode = "selectedNode";
        static const Identifier treeNodeId = "nodeId";

        static const Identifier workspace = "workspace";
        static const Identifier root = "root";
        static const Identifier settings = "settings";
        static const Identifier instrumentsList = "instruments";
        static const Identifier instrumentRoot = "instrument";

        static const Identifier project = "project";
        static const Identifier projectId = "projectId";
        static const Identifier projectInfo = "projectInfo";
        static const Identifier projectTimeStamp = "projectTimeStamp";
        static const Identifier versionControl = "versionControl";
        static const Identifier patternSet = "patternSet";
        static const Identifier trackGroup = "group";
        static const Identifier track = "track";
        static const Identifier pianoTrack = "pianoTrack";
        static const Identifier automationTrack = "automationTrack";
        static const Identifier projectTimeline = "projectTimeline";
        static const Identifier filePath = "filePath";

        // Properties
        static const Identifier trackId = "trackId";
        static const Identifier trackColour = "colour";
        static const Identifier trackChannel = "channel";
        static const Identifier trackInstrumentId = "instrumentId";
        static const Identifier trackControllerNumber = "controller";

        // Timeline
        static const Identifier annotationsTrackId = "annotationsTrackId";
        static const Identifier keySignaturesTrackId = "keySignaturesTrackId";
        static const Identifier timeSignaturesTrackId = "timeSignaturesTrackId";

        static const Identifier globalConfig = "config";
        static const Identifier clipboard = "helioClipboard";
    } // namespace Core

    namespace Midi
    {
        // Scales
        static const Identifier scale = "scale";
        static const Identifier scaleName = "name";
        static const Identifier scalePeriod = "period";
        static const Identifier scaleIntervals = "intervals";

        // Temperaments
        static const Identifier temperament = "temperament";
        static const Identifier temperamentId = "id";
        static const Identifier temperamentName = "name";
        static const Identifier temperamentPeriod = "period";
        static const Identifier temperamentPeriodRange = "periodRange";
        static const Identifier temperamentHighlighting = "highlighting";
        static const Identifier temperamentChromaticMap = "chromaticMap";
        static const Identifier temperamentChromaticScale = "chromaticScale";

        // Chords
        static const Identifier chord = "chord";
        static const Identifier chordName = "name";
        static const Identifier chordScaleKeys = "keys";

        // Meters
        static const Identifier meter = "meter";
        static const Identifier meterName = "name";
        static const Identifier meterTime = "time";
        static const Identifier metronomeScheme = "metronome";

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

        // notes are the most common records in the savefile
        // therefore their properties are shortened
        static const Identifier id = "id";
        static const Identifier key = "key";
        static const Identifier timestamp = "ts";
        static const Identifier length = "len";
        static const Identifier volume = "vol";
        static const Identifier tuplet = "div";

        static const Identifier mute = "mute";
        static const Identifier solo = "solo";

        static const Identifier text = "text";
        static const Identifier colour = "colour";
        static const Identifier keyName = "keyName";

        static const Identifier value = "value";
        static const Identifier curve = "curve";

        static const Identifier numerator = "numerator";
        static const Identifier denominator = "denominator";

        namespace KeyboardMappings
        {
            static const Identifier keyboardMapping = "keyboardMapping";
            static const Identifier name = "name";
            static const Identifier map = "map";
        }
    } // namespace Midi

    namespace Modifiers
    {
        static const Identifier refactoringModifier = "refactoring";
        static const Identifier arpeggiationModifier = "arpeggiation";
        static const Identifier tuningModifier = "tuning";

        static const Identifier isEnabled = "enabled";

        static const Identifier refactoringType = "type";
        static const Identifier refactoringParameter = "parameter";
        static const Identifier refactoringMelodicInversion = "inversion";
        static const Identifier refactoringRetrograde = "retrograde";
        static const Identifier refactoringChordInversion = "chordInversion";
        static const Identifier refactoringInScaleTransposition = "inScaleShift";
        static const Identifier refactoringQuantization = "quantization";
        static const Identifier refactoringLegato = "legato";
        static const Identifier refactoringStaccato = "staccato";
        static const Identifier refactoringCleanupOverlaps = "cleanup";

        static const Identifier arpeggiationSpeed = "speed";
    } // namespace Modifiers

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
        static const Identifier transportSeekBeat = "seekBeat";

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

        static const Identifier midiInputName = "midiInputName";
        static const Identifier midiInputId = "midiInputId";
        static const Identifier midiInputReadjusting = "midiInputReadjusting";
        static const Identifier midiOutputName = "midiOutputName";
        static const Identifier midiOutputId = "midiOutputId";

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

        namespace Metronome
        {
            static const Identifier metronomeConfig = "metronome";
            static const Identifier customSample = "sample";
            static const Identifier syllableName = "name";
            static const Identifier filePath = "filePath";
        } // namespace Metronome

        namespace SoundFont
        {
            static const Identifier soundFontConfig = "soundFontPlayer";
            static const Identifier filePath = "filePath";
            static const Identifier programIndex = "programIndex";
        } // namespace SoundFont
    } // namespace Audio

    namespace Config
    {
        static const Identifier activeWorkspace = "activeWorkspace";
        static const Identifier activeColourScheme = "activeColourScheme";
        static const Identifier activeHotkeyScheme = "activeHotkeyScheme";
        static const Identifier activeUiFlags = "activeUiFlags";
        static const Identifier currentLocale = "currentLocale";
        static const Identifier maxSavedUndoActions = "maxSavedUndoActions";
        static const Identifier windowBounds = "windowBounds";
        static const Identifier lastShownPageId = "lastShownPageId";
        static const Identifier lastUsedScale = "lastUsedScale";
        static const Identifier lastUsedFont = "lastUsedFont";
    } // namespace Config

    // Available types of dynamically fetched resources/configs
    namespace Resources
    {
        static const Identifier scales = "scales";
        static const Identifier chords = "chords";
        static const Identifier meters = "meters";
        static const Identifier temperaments = "temperaments";
        static const Identifier arpeggiators = "arpeggiators";
        static const Identifier translations = "translations";
        static const Identifier colourSchemes = "colourSchemes";
        static const Identifier hotkeySchemes = "hotkeySchemes";
        static const Identifier keyboardMappings = "keyboardMappings";
    }

    namespace UI
    {
        static const Identifier sequencer = "sequencer";

        static const Identifier pianoRoll = "pianoRoll";
        static const Identifier patternRoll = "patternRoll";

        static const Identifier trackGrouping = "trackGrouping";

        static const Identifier startBeat = "startBeat";
        static const Identifier endBeat = "endBeat";
        static const Identifier beatWidth = "beatWidth";
        static const Identifier rowHeight = "rowHeight";

        //static const Identifier viewportPositionX = "viewportX";
        static const Identifier viewportPositionY = "viewportY";
        static const Identifier positionX = "positionX";
        static const Identifier positionY = "positionY";

        static const Identifier defaultNoteLength = "defaultNoteLength";
        static const Identifier defaultNoteVolume = "defaultNoteVolume";

        static const Identifier lastRenderPath = "lastRenderPath";

        namespace Flags
        {
            static const Identifier uiFlags = "uiFlags";
            static const Identifier nativeTitleBar = "nativeTitleBar";
            static const Identifier openGlRenderer = "openGlRenderer";
            static const Identifier noteNameGuides = "noteNameGuides";
            static const Identifier scalesHighlighting = "scalesHighlighting";
            static const Identifier useFixedDoNotation = "useFixedDoNotation";
            static const Identifier experimentalFeaturesOn = "experimentalFeatures";
            static const Identifier followPlayhead ="catchPlayhead";
            static const Identifier animations = "animations";
            static const Identifier lockZoomLevel = "lockZoom";
            static const Identifier showFullProjectMap = "miniMap";
            static const Identifier uiScaleFactor = "uiScaleFactor";
            static const Identifier leftSidebarWidth = "leftSidebarWidth";
            static const Identifier rightSidebarWidth = "rightSidebarWidth";
            static const Identifier mouseWheelAltMode = "wheelAltMode";
            static const Identifier mouseWheelVerticalPanningByDefault = "wheelVerticalPan";
            static const Identifier mouseWheelVerticalZoomingByDefault = "wheelVerticalZoom";
            static const Identifier metronomeEnabled = "metronome";
            static const Identifier pluginsSorting = "pluginsSorting";
            } // namespace Flags

        namespace Hotkeys
        {
            static const Identifier scheme = "hotkeyScheme";
            static const Identifier schemeName = "name";
            static const Identifier keyPress = "keyPress";
            static const Identifier keyDown = "keyDown";
            static const Identifier keyUp = "keyUp";
            static const Identifier hotkeyDescription = "key";
            static const Identifier hotkeyReceiver = "receiver";
            static const Identifier hotkeyCommand = "command";
            static const Identifier group = "group";
        }

        namespace Colours
        {
            static const Identifier scheme = "colourScheme";
            static const Identifier colourMap = "colourMap";
            static const Identifier name = "name";
            
            // legacy ids, to be removed in future versions:
            static const Identifier primaryGradientA = "primaryGradientA";
            static const Identifier primaryGradientB = "primaryGradientB";
            static const Identifier secondaryGradientA = "secondaryGradientA";
            static const Identifier panelFill = "panelFill";
            static const Identifier panelBorder = "panelBorder";
            // new ones instead of ^^:
            static const Identifier pageFill = "pageFill";
            static const Identifier headlineFill = "headlineFill";
            static const Identifier sidebarFill = "sidebarFill";
            static const Identifier dialogFill = "dialogFill";
            static const Identifier buttonFill = "buttonFill";
            static const Identifier frameBorder = "frameBorder";
            static const Identifier timeline = "timeline";

            static const Identifier lassoFill = "lassoFill";
            static const Identifier lassoBorder = "lassoBorder";

            static const Identifier blackKey = "blackKey";
            static const Identifier whiteKey = "whiteKey";

            static const Identifier row = "row";
            static const Identifier bar = "bar";

            static const Identifier text = "text";

            static const Identifier iconBase = "iconBase";
            static const Identifier iconShadow = "iconShadow";
        } // namespace Colours
        
    } // namespace UI

    namespace Translations
    {
        static const Identifier metaSymbol = "{x}";

        static const Identifier wrapperClassName = "pluralForm";
        static const Identifier wrapperMethodName = "detect";

        // old keys
        static const Identifier translationIdOld = "name";
        static const Identifier translationValueOld = "translation";
        // new keys
        static const Identifier translationId = "id";
        static const Identifier translationValue = "tr";

        static const Identifier locale = "locale";
        static const Identifier localeId = "id";
        static const Identifier localeName = "name";

        static const Identifier literal = "literal";
        static const Identifier pluralEquation = "pluralEquation";
        static const Identifier pluralLiteral = "pluralLiteral";
        static const Identifier pluralForm = "pluralForm";
        static const Identifier pluralName = "name";
    }  // namespace Translations

    namespace Arps
    {
        static const Identifier arpeggiator = "arpeggiator";
        static const Identifier name = "name";
        static const Identifier sequence = "sequence";
        static const Identifier key = "key";

        namespace Keys
        {
            static const Identifier key = "key";
            static const Identifier period = "period";
            static const Identifier timestamp = "ts";
            static const Identifier length = "len";
            static const Identifier volume = "vol";
            static const Identifier isBarStart = "barStart";
        } // namespace Key
    } // namespace Arps

    namespace Clipboard
    {
        static const Identifier clipboard = "clipboard";
        static const Identifier track = "track";
        static const Identifier trackId = "trackId";
        static const Identifier firstBeat = "firstBeat";
    } // namespace Clipboard

    namespace User
    {
        static const Identifier profile = "userProfile";

        namespace RecentProjects
        {
            static const Identifier recentProject = "recentProject";
            static const Identifier localProjectInfo = "localProjectInfo";
            static const Identifier path = "path";
            static const Identifier file = "file";
            static const Identifier title = "title";
            static const Identifier projectId = "id";
            static const Identifier updatedAt = "updatedAt";
        } // namespace RecentProjects
    } // namespace User

    namespace VCS
    {
        static const Identifier stashesRepository = "stashesRepository";
        static const Identifier userStashes = "userStashes";
        static const Identifier quickStash = "quickStash";
        static const Identifier quickStashId = "quickStashId";
        static const Identifier diffFormatVersion = "diffFormatVersion";

        static const Identifier revision = "revision";
        static const Identifier head = "head";
        static const Identifier snapshot = "snapshot";
        static const Identifier headRevisionId = "headRevisionId";
        static const Identifier commitMessage = "message";
        static const Identifier commitTimeStamp = "date";
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
            static const Identifier projectTemperament = "temperament";
        }

        namespace MidiTrackDeltas
        {
            static const Identifier trackPath = "path";
            static const Identifier trackColour = "colour";
            static const Identifier trackChannel = "channel";
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

        namespace AnnotationDeltas
        {
            static const Identifier annotationsAdded = "annotationsAdded";
            static const Identifier annotationsRemoved = "annotationsRemoved";
            static const Identifier annotationsChanged = "annotationsChanged";
        }

        namespace TimeSignatureDeltas
        {
            static const Identifier timeSignaturesAdded = "timeSignaturesAdded";
            static const Identifier timeSignaturesRemoved = "timeSignaturesRemoved";
            static const Identifier timeSignaturesChanged = "timeSignaturesChanged";
        }

        namespace KeySignatureDeltas
        {
            static const Identifier keySignaturesAdded = "keySignaturesAdded";
            static const Identifier keySignaturesRemoved = "keySignaturesRemoved";
            static const Identifier keySignaturesChanged = "keySignaturesChanged";
        }
    } // namespace VCS

    namespace Undo
    {
        static const Identifier undoStack = "undoStack";
        static const Identifier transaction = "transaction";

        static const Identifier name = "name";
        static const Identifier path = "path";
        static const Identifier trackId = "trackId";
        static const Identifier group = "group";

        static const Identifier treePathBefore = "pathBefore";
        static const Identifier treePathAfter = "pathAfter";
        static const Identifier colourBefore = "colourBefore";
        static const Identifier colourAfter = "colourAfter";
        static const Identifier channelBefore = "channelBefore";
        static const Identifier channelAfter = "channelAfter";
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
        static const Identifier midiTrackChangeChannelAction = "midiTrackChangeChannel";
        static const Identifier midiTrackChangeInstrumentAction = "midiTrackChangeInstrument";
        static const Identifier midiTrackChangeTimeSignatureAction = "midiTrackChangeTimeSignature";
        
        static const Identifier clipInsertAction = "patternClipInsert";
        static const Identifier clipRemoveAction = "patternClipRemove";
        static const Identifier clipChangeAction = "patternClipChange";
        static const Identifier clipsGroupInsertAction = "patternClipsInsert";
        static const Identifier clipsGroupRemoveAction = "patternClipsRemove";
        static const Identifier clipsGroupChangeAction = "patternClipsChange";

        static const Identifier noteInsertAction = "noteInsert";
        static const Identifier noteRemoveAction = "noteRemove";
        static const Identifier noteChangeAction = "noteChange";
        static const Identifier notesGroupInsertAction = "notesInsert";
        static const Identifier notesGroupRemoveAction = "notesRemove";
        static const Identifier notesGroupChangeAction = "notesChange";

        static const Identifier automationEventInsertAction = "automationEventInsert";
        static const Identifier automationEventRemoveAction = "automationEventRemove";
        static const Identifier automationEventChangeAction = "automationEventChange";
        static const Identifier automationEventsGroupInsertAction = "automationEventsInsert";
        static const Identifier automationEventsGroupRemoveAction = "automationEventsRemove";
        static const Identifier automationEventsGroupChangeAction = "automationEventsChange";
        
        static const Identifier annotationEventInsertAction = "annotationInsert";
        static const Identifier annotationEventRemoveAction = "annotationRemove";
        static const Identifier annotationEventChangeAction = "annotationChange";

        static const Identifier timeSignatureEventInsertAction = "timeSignatureInsert";
        static const Identifier timeSignatureEventRemoveAction = "timeSignatureRemove";
        static const Identifier timeSignatureEventChangeAction = "timeSignatureChange";

        static const Identifier keySignatureEventInsertAction = "keySignatureInsert";
        static const Identifier keySignatureEventRemoveAction = "keySignatureRemove";
        static const Identifier keySignatureEventChangeAction = "keySignatureChange";

        static const Identifier projectTemperamentChangeAction = "temperamentChange";
    } // namespace Undo
}  // namespace Serialization
