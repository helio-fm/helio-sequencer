# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

cmake_minimum_required(VERSION 3.4.1)

set(BINARY_NAME "juce_jni")

add_library("cpufeatures" STATIC "${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c")
set_source_files_properties("${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c" PROPERTIES COMPILE_FLAGS "-Wno-sign-conversion -Wno-gnu-statement-expression")

add_definitions("-DJUCE_ANDROID=1" "-DJUCE_ANDROID_API_VERSION=21" "-DJUCE_ANDROID_GL_ES_VERSION_3_0=1" "-DJUCER_ANDROIDSTUDIO_6AB8437A=1" "-DJUCE_APP_VERSION=3.6" "-DJUCE_APP_VERSION_HEX=0x30600")

include_directories( AFTER
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
    "../../Projucer/JuceLibraryCode"
    "../../../ThirdParty/JUCE/modules"
    "${ANDROID_NDK}/sources/android/cpufeatures"
)

enable_language(ASM)

if(JUCE_BUILD_CONFIGURATION MATCHES "DEBUG")
    add_definitions("-DJUCER_ANDROIDSTUDIO_6AB8437A=1" "-DJUCE_APP_VERSION=3.6" "-DJUCE_APP_VERSION_HEX=0x30600" "-DDEBUG=1" "-D_DEBUG=1")
    include_directories( AFTER
        "../../Projucer/JuceLibraryCode"
        "../../../ThirdParty/JUCE/modules"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
        "../../../ThirdParty/ASIO/common"
        "../../../Source"
        "../../../Source/Core"
        "../../../Source/Core/Audio"
        "../../../Source/Core/Audio/BuiltIn"
        "../../../Source/Core/Audio/Instruments"
        "../../../Source/Core/Audio/Monitoring"
        "../../../Source/Core/Audio/Transport"
        "../../../Source/Core/Configuration"
        "../../../Source/Core/Configuration/Models"
        "../../../Source/Core/Configuration/ResourceManagers"
        "../../../Source/Core/CommandPalette"
        "../../../Source/Core/Midi"
        "../../../Source/Core/Midi/Patterns"
        "../../../Source/Core/Midi/Sequences"
        "../../../Source/Core/Midi/Sequences/Events"
        "../../../Source/Core/Network"
        "../../../Source/Core/Network/Models"
        "../../../Source/Core/Network/Requests"
        "../../../Source/Core/Network/Services"
        "../../../Source/Core/Serialization"
        "../../../Source/Core/Tree"
        "../../../Source/Core/Undo"
        "../../../Source/Core/Undo/Actions"
        "../../../Source/Core/VCS"
        "../../../Source/Core/VCS/DiffLogic"
        "../../../Source/Core/Workspace"
        "../../../Source/UI"
        "../../../Source/UI/Common"
        "../../../Source/UI/Common/AudioMonitors"
        "../../../Source/UI/Common/Origami"
        "../../../Source/UI/Dialogs"
        "../../../Source/UI/Headline"
        "../../../Source/UI/Input"
        "../../../Source/UI/Menus"
        "../../../Source/UI/Menus/Base"
        "../../../Source/UI/Menus/SelectionMenus"
        "../../../Source/UI/Pages/Instruments"
        "../../../Source/UI/Pages/Instruments/Editor"
        "../../../Source/UI/Pages/Project"
        "../../../Source/UI/Pages/Settings"
        "../../../Source/UI/Pages/VCS"
        "../../../Source/UI/Pages/Dashboard"
        "../../../Source/UI/Pages/Dashboard/Menu"
        "../../../Source/UI/Popups"
        "../../../Source/UI/Sequencer"
        "../../../Source/UI/Sequencer/Header"
        "../../../Source/UI/Sequencer/Helpers"
        "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap"
        "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap"
        "../../../Source/UI/Sequencer/MiniMaps/LevelsMap"
        "../../../Source/UI/Sequencer/MiniMaps/PianoMap"
        "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap"
        "../../../Source/UI/Sequencer/PatternRoll"
        "../../../Source/UI/Sequencer/PatternRoll/ClipComponents"
        "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip"
        "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip"
        "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip"
        "../../../Source/UI/Sequencer/PianoRoll"
        "../../../Source/UI/Sequencer/Sidebars"
        "../../../Source/UI/Themes"
    )

elseif(JUCE_BUILD_CONFIGURATION MATCHES "RELEASE")
    add_definitions("-DJUCER_ANDROIDSTUDIO_6AB8437A=1" "-DJUCE_APP_VERSION=3.6" "-DJUCE_APP_VERSION_HEX=0x30600" "-DNDEBUG=1")
    include_directories( AFTER
        "../../Projucer/JuceLibraryCode"
        "../../../ThirdParty/JUCE/modules"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
        "../../../ThirdParty/ASIO/common"
        "../../../Source"
        "../../../Source/Core"
        "../../../Source/Core/Audio"
        "../../../Source/Core/Audio/BuiltIn"
        "../../../Source/Core/Audio/Instruments"
        "../../../Source/Core/Audio/Monitoring"
        "../../../Source/Core/Audio/Transport"
        "../../../Source/Core/Configuration"
        "../../../Source/Core/Configuration/Models"
        "../../../Source/Core/Configuration/ResourceManagers"
        "../../../Source/Core/CommandPalette"
        "../../../Source/Core/Midi"
        "../../../Source/Core/Midi/Patterns"
        "../../../Source/Core/Midi/Sequences"
        "../../../Source/Core/Midi/Sequences/Events"
        "../../../Source/Core/Network"
        "../../../Source/Core/Network/Models"
        "../../../Source/Core/Network/Requests"
        "../../../Source/Core/Network/Services"
        "../../../Source/Core/Serialization"
        "../../../Source/Core/Tree"
        "../../../Source/Core/Undo"
        "../../../Source/Core/Undo/Actions"
        "../../../Source/Core/VCS"
        "../../../Source/Core/VCS/DiffLogic"
        "../../../Source/Core/Workspace"
        "../../../Source/UI"
        "../../../Source/UI/Common"
        "../../../Source/UI/Common/AudioMonitors"
        "../../../Source/UI/Common/Origami"
        "../../../Source/UI/Dialogs"
        "../../../Source/UI/Headline"
        "../../../Source/UI/Input"
        "../../../Source/UI/Menus"
        "../../../Source/UI/Menus/Base"
        "../../../Source/UI/Menus/SelectionMenus"
        "../../../Source/UI/Pages/Instruments"
        "../../../Source/UI/Pages/Instruments/Editor"
        "../../../Source/UI/Pages/Project"
        "../../../Source/UI/Pages/Settings"
        "../../../Source/UI/Pages/VCS"
        "../../../Source/UI/Pages/Dashboard"
        "../../../Source/UI/Pages/Dashboard/Menu"
        "../../../Source/UI/Popups"
        "../../../Source/UI/Sequencer"
        "../../../Source/UI/Sequencer/Header"
        "../../../Source/UI/Sequencer/Helpers"
        "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap"
        "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap"
        "../../../Source/UI/Sequencer/MiniMaps/LevelsMap"
        "../../../Source/UI/Sequencer/MiniMaps/PianoMap"
        "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap"
        "../../../Source/UI/Sequencer/PatternRoll"
        "../../../Source/UI/Sequencer/PatternRoll/ClipComponents"
        "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip"
        "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip"
        "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip"
        "../../../Source/UI/Sequencer/PianoRoll"
        "../../../Source/UI/Sequencer/Sidebars"
        "../../../Source/UI/Themes"
    )

else()
    message( FATAL_ERROR "No matching build-configuration found." )
endif()

add_library( ${BINARY_NAME}

    SHARED

    "../../../Resources/Icons/annotation.svg"
    "../../../Resources/Icons/apply.svg"
    "../../../Resources/Icons/arpeggiate.svg"
    "../../../Resources/Icons/audioPlugin.svg"
    "../../../Resources/Icons/automationTrack.svg"
    "../../../Resources/Icons/back.svg"
    "../../../Resources/Icons/bottomBar.svg"
    "../../../Resources/Icons/browse.svg"
    "../../../Resources/Icons/chordBuilder.svg"
    "../../../Resources/Icons/close.svg"
    "../../../Resources/Icons/colour.svg"
    "../../../Resources/Icons/commit.svg"
    "../../../Resources/Icons/console.svg"
    "../../../Resources/Icons/copy.svg"
    "../../../Resources/Icons/create.svg"
    "../../../Resources/Icons/cropTool.svg"
    "../../../Resources/Icons/cursorTool.svg"
    "../../../Resources/Icons/cut.svg"
    "../../../Resources/Icons/cutterTool.svg"
    "../../../Resources/Icons/down.svg"
    "../../../Resources/Icons/dragTool.svg"
    "../../../Resources/Icons/drawTool.svg"
    "../../../Resources/Icons/ellipsis.svg"
    "../../../Resources/Icons/expand.svg"
    "../../../Resources/Icons/fail.svg"
    "../../../Resources/Icons/forward.svg"
    "../../../Resources/Icons/github.svg"
    "../../../Resources/Icons/helio.svg"
    "../../../Resources/Icons/instrument.svg"
    "../../../Resources/Icons/instrumentNode.svg"
    "../../../Resources/Icons/inverseDown.svg"
    "../../../Resources/Icons/inverseUp.svg"
    "../../../Resources/Icons/list.svg"
    "../../../Resources/Icons/local.svg"
    "../../../Resources/Icons/mute.svg"
    "../../../Resources/Icons/orchestraPit.svg"
    "../../../Resources/Icons/paint.svg"
    "../../../Resources/Icons/paste.svg"
    "../../../Resources/Icons/patterns.svg"
    "../../../Resources/Icons/pause.svg"
    "../../../Resources/Icons/piano.svg"
    "../../../Resources/Icons/pianoTrack.svg"
    "../../../Resources/Icons/play.svg"
    "../../../Resources/Icons/progressIndicator.svg"
    "../../../Resources/Icons/project.svg"
    "../../../Resources/Icons/pull.svg"
    "../../../Resources/Icons/push.svg"
    "../../../Resources/Icons/record.svg"
    "../../../Resources/Icons/redo.svg"
    "../../../Resources/Icons/refactor.svg"
    "../../../Resources/Icons/remote.svg"
    "../../../Resources/Icons/remove.svg"
    "../../../Resources/Icons/render.svg"
    "../../../Resources/Icons/reprise.svg"
    "../../../Resources/Icons/reset.svg"
    "../../../Resources/Icons/revision.svg"
    "../../../Resources/Icons/routing.svg"
    "../../../Resources/Icons/selectionTool.svg"
    "../../../Resources/Icons/selection.svg"
    "../../../Resources/Icons/selectAll.svg"
    "../../../Resources/Icons/selectNone.svg"
    "../../../Resources/Icons/settings.svg"
    "../../../Resources/Icons/stop.svg"
    "../../../Resources/Icons/stretchLeft.svg"
    "../../../Resources/Icons/stretchRight.svg"
    "../../../Resources/Icons/submenu.svg"
    "../../../Resources/Icons/success.svg"
    "../../../Resources/Icons/tag.svg"
    "../../../Resources/Icons/timelineNext.svg"
    "../../../Resources/Icons/timelinePrevious.svg"
    "../../../Resources/Icons/toggleOff.svg"
    "../../../Resources/Icons/toggleOn.svg"
    "../../../Resources/Icons/trackGroup.svg"
    "../../../Resources/Icons/undo.svg"
    "../../../Resources/Icons/unmute.svg"
    "../../../Resources/Icons/up.svg"
    "../../../Resources/Icons/versionControl.svg"
    "../../../Resources/Icons/volume.svg"
    "../../../Resources/Icons/volumePanel.svg"
    "../../../Resources/Icons/zoomIn.svg"
    "../../../Resources/Icons/zoomOut.svg"
    "../../../Resources/Icons/zoomToFit.svg"
    "../../../Resources/Icons/noise.png"
    "../../../Resources/Templates/emptyProject.json"
    "../../../Resources/Templates/exampleProject.json"
    "../../../Resources/logo-v2.png"
    "../../../Resources/arpeggiators.json"
    "../../../Resources/chords.json"
    "../../../Resources/colourSchemes.json"
    "../../../Resources/hotkeySchemes.json"
    "../../../Resources/scales.json"
    "../../../Resources/temperaments.json"
    "../../../Resources/translations.json"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthAudioPlugin.cpp"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthAudioPlugin.h"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthFormat.cpp"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthFormat.h"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynth.cpp"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynth.h"
    "../../../Source/Core/Audio/BuiltIn/InternalPluginFormat.cpp"
    "../../../Source/Core/Audio/BuiltIn/InternalPluginFormat.h"
    "../../../Source/Core/Audio/Instruments/Instrument.cpp"
    "../../../Source/Core/Audio/Instruments/Instrument.h"
    "../../../Source/Core/Audio/Instruments/OrchestraListener.h"
    "../../../Source/Core/Audio/Instruments/OrchestraPit.cpp"
    "../../../Source/Core/Audio/Instruments/OrchestraPit.h"
    "../../../Source/Core/Audio/Instruments/PluginScanner.cpp"
    "../../../Source/Core/Audio/Instruments/PluginScanner.h"
    "../../../Source/Core/Audio/Instruments/SerializablePluginDescription.cpp"
    "../../../Source/Core/Audio/Instruments/SerializablePluginDescription.h"
    "../../../Source/Core/Audio/Monitoring/AudioMonitor.cpp"
    "../../../Source/Core/Audio/Monitoring/AudioMonitor.h"
    "../../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.cpp"
    "../../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.h"
    "../../../Source/Core/Audio/Transport/MidiRecorder.cpp"
    "../../../Source/Core/Audio/Transport/MidiRecorder.h"
    "../../../Source/Core/Audio/Transport/PlayerThread.cpp"
    "../../../Source/Core/Audio/Transport/PlayerThread.h"
    "../../../Source/Core/Audio/Transport/PlayerThreadPool.h"
    "../../../Source/Core/Audio/Transport/RendererThread.cpp"
    "../../../Source/Core/Audio/Transport/RendererThread.h"
    "../../../Source/Core/Audio/Transport/RenderFormat.h"
    "../../../Source/Core/Audio/Transport/Transport.cpp"
    "../../../Source/Core/Audio/Transport/Transport.h"
    "../../../Source/Core/Audio/Transport/TransportListener.h"
    "../../../Source/Core/Audio/Transport/TransportPlaybackCache.h"
    "../../../Source/Core/Audio/AudioCore.cpp"
    "../../../Source/Core/Audio/AudioCore.h"
    "../../../Source/Core/Configuration/Models/BaseResource.h"
    "../../../Source/Core/Configuration/Models/Arpeggiator.cpp"
    "../../../Source/Core/Configuration/Models/Arpeggiator.h"
    "../../../Source/Core/Configuration/Models/Chord.cpp"
    "../../../Source/Core/Configuration/Models/Chord.h"
    "../../../Source/Core/Configuration/Models/ColourScheme.cpp"
    "../../../Source/Core/Configuration/Models/ColourScheme.h"
    "../../../Source/Core/Configuration/Models/HotkeyScheme.cpp"
    "../../../Source/Core/Configuration/Models/HotkeyScheme.h"
    "../../../Source/Core/Configuration/Models/Scale.cpp"
    "../../../Source/Core/Configuration/Models/Scale.h"
    "../../../Source/Core/Configuration/Models/Temperament.cpp"
    "../../../Source/Core/Configuration/Models/Temperament.h"
    "../../../Source/Core/Configuration/Models/Translation.cpp"
    "../../../Source/Core/Configuration/Models/Translation.h"
    "../../../Source/Core/Configuration/Models/TranslationKeys.h"
    "../../../Source/Core/Configuration/ResourceManagers/ResourceManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/ResourceManager.h"
    "../../../Source/Core/Configuration/ResourceManagers/ArpeggiatorsManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/ArpeggiatorsManager.h"
    "../../../Source/Core/Configuration/ResourceManagers/ChordsManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/ChordsManager.h"
    "../../../Source/Core/Configuration/ResourceManagers/ColourSchemesManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/ColourSchemesManager.h"
    "../../../Source/Core/Configuration/ResourceManagers/HotkeySchemesManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/HotkeySchemesManager.h"
    "../../../Source/Core/Configuration/ResourceManagers/ScalesManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/ScalesManager.h"
    "../../../Source/Core/Configuration/ResourceManagers/TemperamentsManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/TemperamentsManager.h"
    "../../../Source/Core/Configuration/ResourceManagers/TranslationsManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/TranslationsManager.h"
    "../../../Source/Core/Configuration/Config.cpp"
    "../../../Source/Core/Configuration/Config.h"
    "../../../Source/Core/Configuration/UserInterfaceFlags.cpp"
    "../../../Source/Core/Configuration/UserInterfaceFlags.h"
    "../../../Source/Core/CommandPalette/CommandPaletteModel.h"
    "../../../Source/Core/CommandPalette/CommandPaletteActionsProvider.cpp"
    "../../../Source/Core/CommandPalette/CommandPaletteActionsProvider.h"
    "../../../Source/Core/CommandPalette/CommandPaletteCommonActions.cpp"
    "../../../Source/Core/CommandPalette/CommandPaletteCommonActions.h"
    "../../../Source/Core/CommandPalette/CommandPaletteChordConstructor.cpp"
    "../../../Source/Core/CommandPalette/CommandPaletteChordConstructor.h"
    "../../../Source/Core/CommandPalette/CommandPaletteMoveNotesMenu.cpp"
    "../../../Source/Core/CommandPalette/CommandPaletteMoveNotesMenu.h"
    "../../../Source/Core/CommandPalette/CommandPaletteProjectsList.cpp"
    "../../../Source/Core/CommandPalette/CommandPaletteProjectsList.h"
    "../../../Source/Core/CommandPalette/CommandPaletteTimelineEvents.cpp"
    "../../../Source/Core/CommandPalette/CommandPaletteTimelineEvents.h"
    "../../../Source/Core/Midi/Patterns/Clip.cpp"
    "../../../Source/Core/Midi/Patterns/Clip.h"
    "../../../Source/Core/Midi/Patterns/Pattern.cpp"
    "../../../Source/Core/Midi/Patterns/Pattern.h"
    "../../../Source/Core/Midi/Sequences/Events/AnnotationEvent.cpp"
    "../../../Source/Core/Midi/Sequences/Events/AnnotationEvent.h"
    "../../../Source/Core/Midi/Sequences/Events/AutomationEvent.cpp"
    "../../../Source/Core/Midi/Sequences/Events/AutomationEvent.h"
    "../../../Source/Core/Midi/Sequences/Events/KeySignatureEvent.cpp"
    "../../../Source/Core/Midi/Sequences/Events/KeySignatureEvent.h"
    "../../../Source/Core/Midi/Sequences/Events/MidiEvent.cpp"
    "../../../Source/Core/Midi/Sequences/Events/MidiEvent.h"
    "../../../Source/Core/Midi/Sequences/Events/Note.cpp"
    "../../../Source/Core/Midi/Sequences/Events/Note.h"
    "../../../Source/Core/Midi/Sequences/Events/TimeSignatureEvent.cpp"
    "../../../Source/Core/Midi/Sequences/Events/TimeSignatureEvent.h"
    "../../../Source/Core/Midi/Sequences/AnnotationsSequence.cpp"
    "../../../Source/Core/Midi/Sequences/AnnotationsSequence.h"
    "../../../Source/Core/Midi/Sequences/AutomationSequence.cpp"
    "../../../Source/Core/Midi/Sequences/AutomationSequence.h"
    "../../../Source/Core/Midi/Sequences/KeySignaturesSequence.cpp"
    "../../../Source/Core/Midi/Sequences/KeySignaturesSequence.h"
    "../../../Source/Core/Midi/Sequences/MidiSequence.cpp"
    "../../../Source/Core/Midi/Sequences/MidiSequence.h"
    "../../../Source/Core/Midi/Sequences/PianoSequence.cpp"
    "../../../Source/Core/Midi/Sequences/PianoSequence.h"
    "../../../Source/Core/Midi/Sequences/TimeSignaturesSequence.cpp"
    "../../../Source/Core/Midi/Sequences/TimeSignaturesSequence.h"
    "../../../Source/Core/Midi/KeyboardMapping.cpp"
    "../../../Source/Core/Midi/KeyboardMapping.h"
    "../../../Source/Core/Midi/MidiTrack.cpp"
    "../../../Source/Core/Midi/MidiTrack.h"
    "../../../Source/Core/Network/Models/ApiModel.h"
    "../../../Source/Core/Network/Models/AppInfoDto.h"
    "../../../Source/Core/Network/Models/AppResourceDto.h"
    "../../../Source/Core/Network/Models/AppVersionDto.h"
    "../../../Source/Core/Network/Models/AuthSessionDto.h"
    "../../../Source/Core/Network/Models/ProjectDto.h"
    "../../../Source/Core/Network/Models/ProjectsListDto.h"
    "../../../Source/Core/Network/Models/RevisionDto.h"
    "../../../Source/Core/Network/Models/UserProfileDto.h"
    "../../../Source/Core/Network/Models/UserResourceDto.h"
    "../../../Source/Core/Network/Models/UserSessionDto.h"
    "../../../Source/Core/Network/Requests/BackendRequest.cpp"
    "../../../Source/Core/Network/Requests/BackendRequest.h"
    "../../../Source/Core/Network/Requests/AuthThread.h"
    "../../../Source/Core/Network/Requests/BaseConfigSyncThread.h"
    "../../../Source/Core/Network/Requests/UserConfigSyncThread.cpp"
    "../../../Source/Core/Network/Requests/UserConfigSyncThread.h"
    "../../../Source/Core/Network/Requests/ProjectCloneThread.cpp"
    "../../../Source/Core/Network/Requests/ProjectCloneThread.h"
    "../../../Source/Core/Network/Requests/ProjectDeleteThread.cpp"
    "../../../Source/Core/Network/Requests/ProjectDeleteThread.h"
    "../../../Source/Core/Network/Requests/RevisionsSyncHelpers.cpp"
    "../../../Source/Core/Network/Requests/RevisionsSyncHelpers.h"
    "../../../Source/Core/Network/Requests/RevisionsSyncThread.cpp"
    "../../../Source/Core/Network/Requests/RevisionsSyncThread.h"
    "../../../Source/Core/Network/Requests/RequestUserProfileThread.h"
    "../../../Source/Core/Network/Requests/TokenCheckThread.h"
    "../../../Source/Core/Network/Requests/TokenUpdateThread.h"
    "../../../Source/Core/Network/Requests/UpdatesCheckThread.h"
    "../../../Source/Core/Network/Services/BackendService.h"
    "../../../Source/Core/Network/Services/ProjectSyncService.cpp"
    "../../../Source/Core/Network/Services/ProjectSyncService.h"
    "../../../Source/Core/Network/Services/ResourceSyncService.cpp"
    "../../../Source/Core/Network/Services/ResourceSyncService.h"
    "../../../Source/Core/Network/Services/SessionService.cpp"
    "../../../Source/Core/Network/Services/SessionService.h"
    "../../../Source/Core/Network/Network.cpp"
    "../../../Source/Core/Network/Network.h"
    "../../../Source/Core/Serialization/Autosaver.cpp"
    "../../../Source/Core/Serialization/Autosaver.h"
    "../../../Source/Core/Serialization/Document.cpp"
    "../../../Source/Core/Serialization/Document.h"
    "../../../Source/Core/Serialization/DocumentHelpers.cpp"
    "../../../Source/Core/Serialization/DocumentHelpers.h"
    "../../../Source/Core/Serialization/DocumentOwner.h"
    "../../../Source/Core/Serialization/Serializable.h"
    "../../../Source/Core/Serialization/SerializationKeys.h"
    "../../../Source/Core/Serialization/SerializedData.cpp"
    "../../../Source/Core/Serialization/SerializedData.h"
    "../../../Source/Core/Serialization/Serializer.h"
    "../../../Source/Core/Serialization/BinarySerializer.cpp"
    "../../../Source/Core/Serialization/BinarySerializer.h"
    "../../../Source/Core/Serialization/JsonSerializer.cpp"
    "../../../Source/Core/Serialization/JsonSerializer.h"
    "../../../Source/Core/Serialization/XmlSerializer.cpp"
    "../../../Source/Core/Serialization/XmlSerializer.h"
    "../../../Source/Core/Tree/AutomationTrackNode.cpp"
    "../../../Source/Core/Tree/AutomationTrackNode.h"
    "../../../Source/Core/Tree/InstrumentNode.cpp"
    "../../../Source/Core/Tree/InstrumentNode.h"
    "../../../Source/Core/Tree/MidiTrackNode.cpp"
    "../../../Source/Core/Tree/MidiTrackNode.h"
    "../../../Source/Core/Tree/MidiTrackSource.h"
    "../../../Source/Core/Tree/OrchestraPitNode.cpp"
    "../../../Source/Core/Tree/OrchestraPitNode.h"
    "../../../Source/Core/Tree/PatternEditorNode.cpp"
    "../../../Source/Core/Tree/PatternEditorNode.h"
    "../../../Source/Core/Tree/PianoTrackNode.cpp"
    "../../../Source/Core/Tree/PianoTrackNode.h"
    "../../../Source/Core/Tree/ProjectMetadata.cpp"
    "../../../Source/Core/Tree/ProjectMetadata.h"
    "../../../Source/Core/Tree/ProjectTimeline.cpp"
    "../../../Source/Core/Tree/ProjectTimeline.h"
    "../../../Source/Core/Tree/ProjectEventDispatcher.h"
    "../../../Source/Core/Tree/ProjectListener.h"
    "../../../Source/Core/Tree/ProjectNode.cpp"
    "../../../Source/Core/Tree/ProjectNode.h"
    "../../../Source/Core/Tree/RootNode.cpp"
    "../../../Source/Core/Tree/RootNode.h"
    "../../../Source/Core/Tree/SettingsNode.cpp"
    "../../../Source/Core/Tree/SettingsNode.h"
    "../../../Source/Core/Tree/TrackGroupNode.cpp"
    "../../../Source/Core/Tree/TrackGroupNode.h"
    "../../../Source/Core/Tree/TreeNode.cpp"
    "../../../Source/Core/Tree/TreeNode.h"
    "../../../Source/Core/Tree/TreeNodeSerializer.cpp"
    "../../../Source/Core/Tree/TreeNodeSerializer.h"
    "../../../Source/Core/Tree/VersionControlNode.cpp"
    "../../../Source/Core/Tree/VersionControlNode.h"
    "../../../Source/Core/Undo/Actions/AnnotationEventActions.cpp"
    "../../../Source/Core/Undo/Actions/AnnotationEventActions.h"
    "../../../Source/Core/Undo/Actions/AutomationTrackActions.cpp"
    "../../../Source/Core/Undo/Actions/AutomationTrackActions.h"
    "../../../Source/Core/Undo/Actions/AutomationEventActions.cpp"
    "../../../Source/Core/Undo/Actions/AutomationEventActions.h"
    "../../../Source/Core/Undo/Actions/KeySignatureEventActions.cpp"
    "../../../Source/Core/Undo/Actions/KeySignatureEventActions.h"
    "../../../Source/Core/Undo/Actions/MidiTrackActions.cpp"
    "../../../Source/Core/Undo/Actions/MidiTrackActions.h"
    "../../../Source/Core/Undo/Actions/NoteActions.cpp"
    "../../../Source/Core/Undo/Actions/NoteActions.h"
    "../../../Source/Core/Undo/Actions/PatternActions.cpp"
    "../../../Source/Core/Undo/Actions/PatternActions.h"
    "../../../Source/Core/Undo/Actions/PianoTrackActions.cpp"
    "../../../Source/Core/Undo/Actions/PianoTrackActions.h"
    "../../../Source/Core/Undo/Actions/ProjectMetadataActions.cpp"
    "../../../Source/Core/Undo/Actions/ProjectMetadataActions.h"
    "../../../Source/Core/Undo/Actions/TimeSignatureEventActions.cpp"
    "../../../Source/Core/Undo/Actions/TimeSignatureEventActions.h"
    "../../../Source/Core/Undo/Actions/UndoAction.h"
    "../../../Source/Core/Undo/UndoActionIDs.h"
    "../../../Source/Core/Undo/UndoStack.cpp"
    "../../../Source/Core/Undo/UndoStack.h"
    "../../../Source/Core/VCS/DiffLogic/AutomationTrackDiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/AutomationTrackDiffLogic.h"
    "../../../Source/Core/VCS/DiffLogic/DiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/DiffLogic.h"
    "../../../Source/Core/VCS/DiffLogic/PatternDiffHelpers.cpp"
    "../../../Source/Core/VCS/DiffLogic/PatternDiffHelpers.h"
    "../../../Source/Core/VCS/DiffLogic/PianoTrackDiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/PianoTrackDiffLogic.h"
    "../../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.h"
    "../../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.h"
    "../../../Source/Core/VCS/Delta.cpp"
    "../../../Source/Core/VCS/Delta.h"
    "../../../Source/Core/VCS/Diff.cpp"
    "../../../Source/Core/VCS/Diff.h"
    "../../../Source/Core/VCS/Head.cpp"
    "../../../Source/Core/VCS/Head.h"
    "../../../Source/Core/VCS/RemoteCache.cpp"
    "../../../Source/Core/VCS/RemoteCache.h"
    "../../../Source/Core/VCS/Revision.cpp"
    "../../../Source/Core/VCS/Revision.h"
    "../../../Source/Core/VCS/RevisionItem.cpp"
    "../../../Source/Core/VCS/RevisionItem.h"
    "../../../Source/Core/VCS/Snapshot.cpp"
    "../../../Source/Core/VCS/Snapshot.h"
    "../../../Source/Core/VCS/StashesRepository.cpp"
    "../../../Source/Core/VCS/StashesRepository.h"
    "../../../Source/Core/VCS/TrackedItem.h"
    "../../../Source/Core/VCS/TrackedItemsSource.h"
    "../../../Source/Core/VCS/VersionControl.cpp"
    "../../../Source/Core/VCS/VersionControl.h"
    "../../../Source/Core/Workspace/NavigationHistory.cpp"
    "../../../Source/Core/Workspace/NavigationHistory.h"
    "../../../Source/Core/Workspace/RecentProjectInfo.cpp"
    "../../../Source/Core/Workspace/RecentProjectInfo.h"
    "../../../Source/Core/Workspace/SyncedConfigurationInfo.cpp"
    "../../../Source/Core/Workspace/SyncedConfigurationInfo.h"
    "../../../Source/Core/Workspace/UserSessionInfo.cpp"
    "../../../Source/Core/Workspace/UserSessionInfo.h"
    "../../../Source/Core/Workspace/UserProfile.cpp"
    "../../../Source/Core/Workspace/UserProfile.h"
    "../../../Source/Core/Workspace/Workspace.cpp"
    "../../../Source/Core/Workspace/Workspace.h"
    "../../../Source/Core/App.cpp"
    "../../../Source/Core/App.h"
    "../../../Source/UI/Common/AudioMonitors/SpectrogramAudioMonitorComponent.cpp"
    "../../../Source/UI/Common/AudioMonitors/SpectrogramAudioMonitorComponent.h"
    "../../../Source/UI/Common/AudioMonitors/WaveformAudioMonitorComponent.cpp"
    "../../../Source/UI/Common/AudioMonitors/WaveformAudioMonitorComponent.h"
    "../../../Source/UI/Common/Origami/Origami.cpp"
    "../../../Source/UI/Common/Origami/Origami.h"
    "../../../Source/UI/Common/Origami/OrigamiHorizontal.cpp"
    "../../../Source/UI/Common/Origami/OrigamiHorizontal.h"
    "../../../Source/UI/Common/Origami/OrigamiVertical.cpp"
    "../../../Source/UI/Common/Origami/OrigamiVertical.h"
    "../../../Source/UI/Common/CachedLabelImage.h"
    "../../../Source/UI/Common/ScaledComponentProxy.h"
    "../../../Source/UI/Common/ColourButton.cpp"
    "../../../Source/UI/Common/ColourButton.h"
    "../../../Source/UI/Common/ColourSwatches.cpp"
    "../../../Source/UI/Common/ColourSwatches.h"
    "../../../Source/UI/Common/CommandIDs.cpp"
    "../../../Source/UI/Common/ColourIDs.cpp"
    "../../../Source/UI/Common/ColourIDs.h"
    "../../../Source/UI/Common/CommandIDs.h"
    "../../../Source/UI/Common/ComponentIDs.h"
    "../../../Source/UI/Common/DraggingListBoxComponent.cpp"
    "../../../Source/UI/Common/DraggingListBoxComponent.h"
    "../../../Source/UI/Common/FineTuningComponentDragger.cpp"
    "../../../Source/UI/Common/FineTuningComponentDragger.h"
    "../../../Source/UI/Common/FineTuningValueIndicator.cpp"
    "../../../Source/UI/Common/FineTuningValueIndicator.h"
    "../../../Source/UI/Common/KeySelector.cpp"
    "../../../Source/UI/Common/KeySelector.h"
    "../../../Source/UI/Common/FloatBoundsComponent.h"
    "../../../Source/UI/Common/HelperRectangle.h"
    "../../../Source/UI/Common/HighlightedComponent.h"
    "../../../Source/UI/Common/IconButton.h"
    "../../../Source/UI/Common/IconComponent.h"
    "../../../Source/UI/Common/MobileComboBox.cpp"
    "../../../Source/UI/Common/MobileComboBox.h"
    "../../../Source/UI/Common/ModeIndicatorComponent.cpp"
    "../../../Source/UI/Common/ModeIndicatorComponent.h"
    "../../../Source/UI/Common/OverlayButton.cpp"
    "../../../Source/UI/Common/OverlayButton.h"
    "../../../Source/UI/Common/PlayButton.cpp"
    "../../../Source/UI/Common/PlayButton.h"
    "../../../Source/UI/Common/PluginWindow.cpp"
    "../../../Source/UI/Common/PluginWindow.h"
    "../../../Source/UI/Common/RadioButton.cpp"
    "../../../Source/UI/Common/RadioButton.h"
    "../../../Source/UI/Common/ScaleEditor.cpp"
    "../../../Source/UI/Common/ScaleEditor.h"
    "../../../Source/UI/Common/SpectralLogo.cpp"
    "../../../Source/UI/Common/SpectralLogo.h"
    "../../../Source/UI/Common/TransportControlComponent.cpp"
    "../../../Source/UI/Common/TransportControlComponent.h"
    "../../../Source/UI/Common/ViewportFitProxyComponent.cpp"
    "../../../Source/UI/Common/ViewportFitProxyComponent.h"
    "../../../Source/UI/Dialogs/DialogBase.cpp"
    "../../../Source/UI/Dialogs/DialogBase.h"
    "../../../Source/UI/Dialogs/AnnotationDialog.cpp"
    "../../../Source/UI/Dialogs/AnnotationDialog.h"
    "../../../Source/UI/Dialogs/KeySignatureDialog.cpp"
    "../../../Source/UI/Dialogs/KeySignatureDialog.h"
    "../../../Source/UI/Dialogs/ModalDialogConfirmation.cpp"
    "../../../Source/UI/Dialogs/ModalDialogConfirmation.h"
    "../../../Source/UI/Dialogs/ModalDialogInput.cpp"
    "../../../Source/UI/Dialogs/ModalDialogInput.h"
    "../../../Source/UI/Dialogs/RenderDialog.cpp"
    "../../../Source/UI/Dialogs/RenderDialog.h"
    "../../../Source/UI/Dialogs/TempoDialog.cpp"
    "../../../Source/UI/Dialogs/TempoDialog.h"
    "../../../Source/UI/Dialogs/TimeSignatureDialog.cpp"
    "../../../Source/UI/Dialogs/TimeSignatureDialog.h"
    "../../../Source/UI/Dialogs/TrackPropertiesDialog.cpp"
    "../../../Source/UI/Dialogs/TrackPropertiesDialog.h"
    "../../../Source/UI/Headline/Headline.cpp"
    "../../../Source/UI/Headline/Headline.h"
    "../../../Source/UI/Headline/HeadlineDropdown.cpp"
    "../../../Source/UI/Headline/HeadlineDropdown.h"
    "../../../Source/UI/Headline/HeadlineItem.cpp"
    "../../../Source/UI/Headline/HeadlineItem.h"
    "../../../Source/UI/Headline/HeadlineItemArrow.cpp"
    "../../../Source/UI/Headline/HeadlineItemArrow.h"
    "../../../Source/UI/Headline/HeadlineItemHighlighter.cpp"
    "../../../Source/UI/Headline/HeadlineItemHighlighter.h"
    "../../../Source/UI/Headline/HeadlineItemDataSource.h"
    "../../../Source/UI/Headline/HeadlineNavigationPanel.cpp"
    "../../../Source/UI/Headline/HeadlineNavigationPanel.h"
    "../../../Source/UI/Headline/HeadlineContextMenuController.cpp"
    "../../../Source/UI/Headline/HeadlineContextMenuController.h"
    "../../../Source/UI/Input/LongTapController.h"
    "../../../Source/UI/Input/LongTapListener.h"
    "../../../Source/UI/Input/MultiTouchController.cpp"
    "../../../Source/UI/Input/MultiTouchController.h"
    "../../../Source/UI/Input/MultiTouchListener.h"
    "../../../Source/UI/Input/SmoothPanController.h"
    "../../../Source/UI/Input/SmoothPanListener.h"
    "../../../Source/UI/Input/SmoothZoomController.h"
    "../../../Source/UI/Input/SmoothZoomListener.h"
    "../../../Source/UI/Menus/Base/MenuItemComponent.cpp"
    "../../../Source/UI/Menus/Base/MenuItemComponent.h"
    "../../../Source/UI/Menus/Base/MenuPanel.cpp"
    "../../../Source/UI/Menus/Base/MenuPanel.h"
    "../../../Source/UI/Menus/SelectionMenus/AudioPluginSelectionMenu.cpp"
    "../../../Source/UI/Menus/SelectionMenus/AudioPluginSelectionMenu.h"
    "../../../Source/UI/Menus/SelectionMenus/InstrumentNodeSelectionMenu.cpp"
    "../../../Source/UI/Menus/SelectionMenus/InstrumentNodeSelectionMenu.h"
    "../../../Source/UI/Menus/SelectionMenus/PatternRollSelectionMenu.cpp"
    "../../../Source/UI/Menus/SelectionMenus/PatternRollSelectionMenu.h"
    "../../../Source/UI/Menus/SelectionMenus/PianoRollSelectionMenu.cpp"
    "../../../Source/UI/Menus/SelectionMenus/PianoRollSelectionMenu.h"
    "../../../Source/UI/Menus/SelectionMenus/VersionControlHistorySelectionMenu.cpp"
    "../../../Source/UI/Menus/SelectionMenus/VersionControlStageSelectionMenu.cpp"
    "../../../Source/UI/Menus/SelectionMenus/VersionControlHistorySelectionMenu.h"
    "../../../Source/UI/Menus/SelectionMenus/VersionControlStageSelectionMenu.h"
    "../../../Source/UI/Menus/InstrumentMenu.cpp"
    "../../../Source/UI/Menus/InstrumentMenu.h"
    "../../../Source/UI/Menus/MidiTrackMenu.cpp"
    "../../../Source/UI/Menus/MidiTrackMenu.h"
    "../../../Source/UI/Menus/OrchestraPitMenu.cpp"
    "../../../Source/UI/Menus/OrchestraPitMenu.h"
    "../../../Source/UI/Menus/PatternsMenu.cpp"
    "../../../Source/UI/Menus/PatternsMenu.h"
    "../../../Source/UI/Menus/ProjectMenu.cpp"
    "../../../Source/UI/Menus/ProjectMenu.h"
    "../../../Source/UI/Menus/TimelineMenu.cpp"
    "../../../Source/UI/Menus/TimelineMenu.h"
    "../../../Source/UI/Menus/VersionControlMenu.cpp"
    "../../../Source/UI/Menus/VersionControlMenu.h"
    "../../../Source/UI/Menus/WorkspaceMenu.cpp"
    "../../../Source/UI/Menus/WorkspaceMenu.h"
    "../../../Source/UI/Pages/Dashboard/Menu/DashboardMenu.cpp"
    "../../../Source/UI/Pages/Dashboard/Menu/DashboardMenu.h"
    "../../../Source/UI/Pages/Dashboard/Menu/RecentProjectRow.cpp"
    "../../../Source/UI/Pages/Dashboard/Menu/RecentProjectRow.h"
    "../../../Source/UI/Pages/Dashboard/Menu/LoginButton.h"
    "../../../Source/UI/Pages/Dashboard/Menu/CreateProjectButton.h"
    "../../../Source/UI/Pages/Dashboard/Menu/OpenProjectButton.h"
    "../../../Source/UI/Pages/Dashboard/Dashboard.cpp"
    "../../../Source/UI/Pages/Dashboard/Dashboard.h"
    "../../../Source/UI/Pages/Dashboard/UpdatesInfoComponent.cpp"
    "../../../Source/UI/Pages/Dashboard/UpdatesInfoComponent.h"
    "../../../Source/UI/Pages/Dashboard/UserProfileComponent.h"
    "../../../Source/UI/Pages/Instruments/Editor/AudioPluginEditorPage.cpp"
    "../../../Source/UI/Pages/Instruments/Editor/AudioPluginEditorPage.h"
    "../../../Source/UI/Pages/Instruments/Editor/InstrumentEditor.cpp"
    "../../../Source/UI/Pages/Instruments/Editor/InstrumentEditor.h"
    "../../../Source/UI/Pages/Instruments/Editor/InstrumentEditorConnector.cpp"
    "../../../Source/UI/Pages/Instruments/Editor/InstrumentEditorConnector.h"
    "../../../Source/UI/Pages/Instruments/Editor/InstrumentComponent.cpp"
    "../../../Source/UI/Pages/Instruments/Editor/InstrumentComponent.h"
    "../../../Source/UI/Pages/Instruments/Editor/InstrumentEditorPin.cpp"
    "../../../Source/UI/Pages/Instruments/Editor/InstrumentEditorPin.h"
    "../../../Source/UI/Pages/Instruments/Editor/KeyboardMappingPage.cpp"
    "../../../Source/UI/Pages/Instruments/Editor/KeyboardMappingPage.h"
    "../../../Source/UI/Pages/Instruments/AudioPluginsListComponent.cpp"
    "../../../Source/UI/Pages/Instruments/AudioPluginsListComponent.h"
    "../../../Source/UI/Pages/Instruments/InstrumentsListComponent.cpp"
    "../../../Source/UI/Pages/Instruments/InstrumentsListComponent.h"
    "../../../Source/UI/Pages/Instruments/OrchestraPitPage.cpp"
    "../../../Source/UI/Pages/Instruments/OrchestraPitPage.h"
    "../../../Source/UI/Pages/Project/ProjectPage.cpp"
    "../../../Source/UI/Pages/Project/ProjectPage.h"
    "../../../Source/UI/Pages/Settings/AudioSettings.cpp"
    "../../../Source/UI/Pages/Settings/AudioSettings.h"
    "../../../Source/UI/Pages/Settings/ComponentsList.cpp"
    "../../../Source/UI/Pages/Settings/ComponentsList.h"
    "../../../Source/UI/Pages/Settings/SettingsFrameWrapper.cpp"
    "../../../Source/UI/Pages/Settings/SettingsFrameWrapper.h"
    "../../../Source/UI/Pages/Settings/SettingsPage.cpp"
    "../../../Source/UI/Pages/Settings/SettingsPage.h"
    "../../../Source/UI/Pages/Settings/SyncSettings.cpp"
    "../../../Source/UI/Pages/Settings/SyncSettings.h"
    "../../../Source/UI/Pages/Settings/SyncSettingsItem.cpp"
    "../../../Source/UI/Pages/Settings/SyncSettingsItem.h"
    "../../../Source/UI/Pages/Settings/ThemeSettings.cpp"
    "../../../Source/UI/Pages/Settings/ThemeSettings.h"
    "../../../Source/UI/Pages/Settings/ThemeSettingsItem.cpp"
    "../../../Source/UI/Pages/Settings/ThemeSettingsItem.h"
    "../../../Source/UI/Pages/Settings/TranslationSettings.cpp"
    "../../../Source/UI/Pages/Settings/TranslationSettings.h"
    "../../../Source/UI/Pages/Settings/TranslationSettingsItem.cpp"
    "../../../Source/UI/Pages/Settings/TranslationSettingsItem.h"
    "../../../Source/UI/Pages/Settings/UserInterfaceSettings.cpp"
    "../../../Source/UI/Pages/Settings/UserInterfaceSettings.h"
    "../../../Source/UI/Pages/VCS/HistoryComponent.cpp"
    "../../../Source/UI/Pages/VCS/HistoryComponent.h"
    "../../../Source/UI/Pages/VCS/RevisionComponent.cpp"
    "../../../Source/UI/Pages/VCS/RevisionComponent.h"
    "../../../Source/UI/Pages/VCS/RevisionConnectorComponent.cpp"
    "../../../Source/UI/Pages/VCS/RevisionConnectorComponent.h"
    "../../../Source/UI/Pages/VCS/RevisionItemComponent.cpp"
    "../../../Source/UI/Pages/VCS/RevisionItemComponent.h"
    "../../../Source/UI/Pages/VCS/RevisionTooltipComponent.cpp"
    "../../../Source/UI/Pages/VCS/RevisionTooltipComponent.h"
    "../../../Source/UI/Pages/VCS/RevisionTreeComponent.cpp"
    "../../../Source/UI/Pages/VCS/RevisionTreeComponent.h"
    "../../../Source/UI/Pages/VCS/StageComponent.cpp"
    "../../../Source/UI/Pages/VCS/StageComponent.h"
    "../../../Source/UI/Pages/VCS/VersionControlEditor.cpp"
    "../../../Source/UI/Pages/VCS/VersionControlEditor.h"
    "../../../Source/UI/Popups/CommandPalette.cpp"
    "../../../Source/UI/Popups/CommandPalette.h"
    "../../../Source/UI/Popups/NotesTuningPanel.cpp"
    "../../../Source/UI/Popups/NotesTuningPanel.h"
    "../../../Source/UI/Popups/ArpPreviewTool.cpp"
    "../../../Source/UI/Popups/ArpPreviewTool.h"
    "../../../Source/UI/Popups/ChordPreviewTool.cpp"
    "../../../Source/UI/Popups/ChordPreviewTool.h"
    "../../../Source/UI/Popups/RescalePreviewTool.cpp"
    "../../../Source/UI/Popups/RescalePreviewTool.h"
    "../../../Source/UI/Popups/ScalePreviewTool.cpp"
    "../../../Source/UI/Popups/ScalePreviewTool.h"
    "../../../Source/UI/Popups/HelioCallout.cpp"
    "../../../Source/UI/Popups/HelioCallout.h"
    "../../../Source/UI/Popups/PopupButton.cpp"
    "../../../Source/UI/Popups/PopupButton.h"
    "../../../Source/UI/Popups/PopupButtonOwner.h"
    "../../../Source/UI/Popups/PopupCustomButton.cpp"
    "../../../Source/UI/Popups/PopupCustomButton.h"
    "../../../Source/UI/Popups/PopupMenuComponent.h"
    "../../../Source/UI/Popups/CenteredTooltipComponent.h"
    "../../../Source/UI/Popups/FailTooltip.h"
    "../../../Source/UI/Popups/GenericTooltip.h"
    "../../../Source/UI/Popups/SuccessTooltip.h"
    "../../../Source/UI/Popups/ProgressIndicator.h"
    "../../../Source/UI/Popups/ProgressTooltip.cpp"
    "../../../Source/UI/Popups/ProgressTooltip.h"
    "../../../Source/UI/Popups/TooltipContainer.cpp"
    "../../../Source/UI/Popups/TooltipContainer.h"
    "../../../Source/UI/Sequencer/Header/ClipRangeIndicator.h"
    "../../../Source/UI/Sequencer/Header/HeaderSelectionIndicator.cpp"
    "../../../Source/UI/Sequencer/Header/HeaderSelectionIndicator.h"
    "../../../Source/UI/Sequencer/Header/RollHeader.cpp"
    "../../../Source/UI/Sequencer/Header/RollHeader.h"
    "../../../Source/UI/Sequencer/Header/Playhead.cpp"
    "../../../Source/UI/Sequencer/Header/Playhead.h"
    "../../../Source/UI/Sequencer/Header/SoundProbeIndicator.h"
    "../../../Source/UI/Sequencer/Header/TimeDistanceIndicator.h"
    "../../../Source/UI/Sequencer/Header/TrackEndIndicator.h"
    "../../../Source/UI/Sequencer/Header/TrackStartIndicator.h"
    "../../../Source/UI/Sequencer/Helpers/CutPointMark.cpp"
    "../../../Source/UI/Sequencer/Helpers/CutPointMark.h"
    "../../../Source/UI/Sequencer/Helpers/RollExpandMark.cpp"
    "../../../Source/UI/Sequencer/Helpers/RollExpandMark.h"
    "../../../Source/UI/Sequencer/Helpers/KnifeToolHelper.cpp"
    "../../../Source/UI/Sequencer/Helpers/KnifeToolHelper.h"
    "../../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.cpp"
    "../../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.h"
    "../../../Source/UI/Sequencer/Helpers/PatternOperations.cpp"
    "../../../Source/UI/Sequencer/Helpers/PatternOperations.h"
    "../../../Source/UI/Sequencer/Helpers/SequencerOperations.cpp"
    "../../../Source/UI/Sequencer/Helpers/SequencerOperations.h"
    "../../../Source/UI/Sequencer/Helpers/InteractiveActions.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveClipComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveClipComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveHelper.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveHelper.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventsConnector.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventsConnector.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepsClipComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepsClipComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventsConnector.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventsConnector.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip/PianoClipComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip/PianoClipComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/ClipComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/ClipComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/DummyClipComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/DummyClipComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/PatternRoll.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/PatternRoll.h"
    "../../../Source/UI/Sequencer/PianoRoll/HighlightingScheme.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/HighlightingScheme.h"
    "../../../Source/UI/Sequencer/PianoRoll/NoteComponent.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/NoteComponent.h"
    "../../../Source/UI/Sequencer/PianoRoll/NoteNameGuide.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/NoteNameGuide.h"
    "../../../Source/UI/Sequencer/PianoRoll/NoteNameGuidesBar.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/NoteNameGuidesBar.h"
    "../../../Source/UI/Sequencer/PianoRoll/NoteResizerLeft.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/NoteResizerLeft.h"
    "../../../Source/UI/Sequencer/PianoRoll/NoteResizerRight.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/NoteResizerRight.h"
    "../../../Source/UI/Sequencer/PianoRoll/PianoRoll.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/PianoRoll.h"
    "../../../Source/UI/Sequencer/Sidebars/SequencerSidebarLeft.cpp"
    "../../../Source/UI/Sequencer/Sidebars/SequencerSidebarLeft.h"
    "../../../Source/UI/Sequencer/Sidebars/SequencerSidebarRight.cpp"
    "../../../Source/UI/Sequencer/Sidebars/SequencerSidebarRight.h"
    "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationLargeComponent.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationLargeComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationSmallComponent.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationSmallComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationsProjectMap.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationsProjectMap.h"
    "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureLargeComponent.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureLargeComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureSmallComponent.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureSmallComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignaturesProjectMap.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignaturesProjectMap.h"
    "../../../Source/UI/Sequencer/MiniMaps/LevelsMap/LevelsMapScroller.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/LevelsMap/LevelsMapScroller.h"
    "../../../Source/UI/Sequencer/MiniMaps/LevelsMap/VelocityProjectMap.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/LevelsMap/VelocityProjectMap.h"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/PianoProjectMap.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/PianoProjectMap.h"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapScroller.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapScroller.h"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapScrollerScreen.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapScrollerScreen.h"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureLargeComponent.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureLargeComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureSmallComponent.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureSmallComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignaturesProjectMap.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignaturesProjectMap.h"
    "../../../Source/UI/Sequencer/RollBase.cpp"
    "../../../Source/UI/Sequencer/RollBase.h"
    "../../../Source/UI/Sequencer/RollEditMode.cpp"
    "../../../Source/UI/Sequencer/RollEditMode.h"
    "../../../Source/UI/Sequencer/RollListener.h"
    "../../../Source/UI/Sequencer/Lasso.cpp"
    "../../../Source/UI/Sequencer/Lasso.h"
    "../../../Source/UI/Sequencer/LassoListeners.cpp"
    "../../../Source/UI/Sequencer/LassoListeners.h"
    "../../../Source/UI/Sequencer/MidiEventComponent.cpp"
    "../../../Source/UI/Sequencer/MidiEventComponent.h"
    "../../../Source/UI/Sequencer/SelectableComponent.h"
    "../../../Source/UI/Sequencer/SelectionComponent.cpp"
    "../../../Source/UI/Sequencer/SelectionComponent.h"
    "../../../Source/UI/Sequencer/SequencerLayout.cpp"
    "../../../Source/UI/Sequencer/SequencerLayout.h"
    "../../../Source/UI/Themes/ComponentFader.h"
    "../../../Source/UI/Themes/DialogBackground.h"
    "../../../Source/UI/Themes/HelioTheme.cpp"
    "../../../Source/UI/Themes/HelioTheme.h"
    "../../../Source/UI/Themes/Icons.cpp"
    "../../../Source/UI/Themes/Icons.h"
    "../../../Source/UI/Themes/FramePanel.h"
    "../../../Source/UI/Themes/PanelBackgroundA.h"
    "../../../Source/UI/Themes/PanelBackgroundB.h"
    "../../../Source/UI/Themes/PanelBackgroundC.h"
    "../../../Source/UI/Themes/SeparatorHorizontal.h"
    "../../../Source/UI/Themes/SeparatorHorizontalReversed.h"
    "../../../Source/UI/Themes/SeparatorHorizontalFading.h"
    "../../../Source/UI/Themes/SeparatorHorizontalFadingReversed.h"
    "../../../Source/UI/Themes/SeparatorVertical.h"
    "../../../Source/UI/Themes/SeparatorVerticalSkew.h"
    "../../../Source/UI/Themes/ShadowComponent.h"
    "../../../Source/UI/Themes/ShadowDownwards.h"
    "../../../Source/UI/Themes/ShadowUpwards.h"
    "../../../Source/UI/Themes/ShadowLeftwards.h"
    "../../../Source/UI/Themes/ShadowRightwards.h"
    "../../../Source/UI/Themes/ViewportKineticSlider.cpp"
    "../../../Source/UI/Themes/ViewportKineticSlider.h"
    "../../../Source/UI/MainLayout.cpp"
    "../../../Source/UI/MainLayout.h"
    "../../../Source/Common.cpp"
    "../../../Source/Common.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/audio_play_head/juce_AudioPlayHead.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioChannelSet.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioChannelSet.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioDataConverters.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioDataConverters.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioProcessLoadMeasurer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioProcessLoadMeasurer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioSampleBuffer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_FloatVectorOperations.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_FloatVectorOperations.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiBuffer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiBuffer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiFile.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiFile.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiKeyboardState.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiKeyboardState.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiMessage.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiMessage.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiMessageSequence.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiMessageSequence.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiRPN.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiRPN.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEInstrument.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEInstrument.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEMessages.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEMessages.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPENote.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPENote.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiser.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiserBase.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiserBase.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiserVoice.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiserVoice.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEUtils.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEUtils.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEValue.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEValue.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZoneLayout.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZoneLayout.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/native/juce_mac_CoreAudioLayouts.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_AudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_BufferingAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_BufferingAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ChannelRemappingAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ChannelRemappingAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_IIRFilterAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_IIRFilterAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_MemoryAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_MemoryAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_MixerAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_MixerAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_PositionableAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ResamplingAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ResamplingAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ReverbAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ReverbAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ToneGeneratorAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ToneGeneratorAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/synthesisers/juce_Synthesiser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/synthesisers/juce_Synthesiser.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_ADSR.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_ADSR_test.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_Decibels.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_GenericInterpolator.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_IIRFilter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_IIRFilter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_Interpolators.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_Interpolators.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_LagrangeInterpolator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_Reverb.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_SmoothedValue.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_SmoothedValue.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_WindowedSincInterpolator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioDeviceManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioDeviceManager.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODevice.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODevice.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODeviceType.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODeviceType.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_SystemAudioVolume.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPacket.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPackets.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPBytestreamInputHandler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPConversion.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPConverters.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPDispatcher.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPFactory.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPIterator.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPMidi1ToBytestreamTranslator.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPMidi1ToMidi2DefaultTranslator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPMidi1ToMidi2DefaultTranslator.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPProtocols.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPReceiver.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPSysEx7.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPSysEx7.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPTests.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPU32InputHandler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPUtils.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPUtils.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPView.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPView.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiDevices.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiDevices.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiMessageCollector.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiMessageCollector.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/AudioStream.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/AudioStreamBase.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/AudioStreamBuilder.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/AudioStreamCallback.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/Definitions.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/LatencyTuner.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/Oboe.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/ResultWithValue.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/StabilizedCallback.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/Utilities.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/Version.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/aaudio/AAudioLoader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/aaudio/AAudioLoader.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/aaudio/AudioStreamAAudio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/aaudio/AudioStreamAAudio.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioClock.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioSourceCaller.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioSourceCaller.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioStreamBuilder.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/DataConversionFlowGraph.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/DataConversionFlowGraph.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FilterAudioStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FilterAudioStream.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockAdapter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockAdapter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockReader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockReader.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockWriter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockWriter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/LatencyTuner.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/MonotonicCounter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/OboeDebug.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/QuirksManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/QuirksManager.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/SourceFloatCaller.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/SourceFloatCaller.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/SourceI16Caller.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/SourceI16Caller.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/StabilizedCallback.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/Trace.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/Trace.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/Utilities.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/Version.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoBuffer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoBuffer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoController.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoController.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoControllerBase.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoControllerBase.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoControllerIndirect.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoControllerIndirect.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/HyperbolicCosineWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/IntegerRatio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/IntegerRatio.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/KaiserWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/LinearResampler.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/LinearResampler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/MultiChannelResampler.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/MultiChannelResampler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResampler.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResampler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResamplerMono.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResamplerMono.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResamplerStereo.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResamplerStereo.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/SincResampler.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/SincResampler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/SincResamplerStereo.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/SincResamplerStereo.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ChannelCountConverter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ChannelCountConverter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ClipToRange.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ClipToRange.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/FlowGraphNode.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/FlowGraphNode.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ManyToMultiConverter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ManyToMultiConverter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/MonoToMultiConverter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/MonoToMultiConverter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/MultiToMonoConverter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/MultiToMonoConverter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/RampLinear.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/RampLinear.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SampleRateConverter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SampleRateConverter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkFloat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkFloat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkI16.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkI16.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkI24.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkI24.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceFloat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceFloat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceI16.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceI16.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceI24.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceI24.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioInputStreamOpenSLES.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioInputStreamOpenSLES.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioOutputStreamOpenSLES.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioOutputStreamOpenSLES.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioStreamBuffered.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioStreamBuffered.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioStreamOpenSLES.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioStreamOpenSLES.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/EngineOpenSLES.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/EngineOpenSLES.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/OpenSLESUtilities.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/OpenSLESUtilities.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/OutputMixerOpenSLES.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/OutputMixerOpenSLES.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/CMakeLists.txt"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/readme.md"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Audio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_HighPerformanceAudioHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Midi.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Oboe.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_OpenSL.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_ios_Audio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_ios_Audio.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_ALSA.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_Bela.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_JackAudio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_Midi.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_mac_CoreAudio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_mac_CoreMidi.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_MidiDataConcatenator.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_win32_ASIO.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_win32_DirectSound.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_win32_Midi.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_win32_WASAPI.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/sources/juce_AudioSourcePlayer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/sources/juce_AudioSourcePlayer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/sources/juce_AudioTransportSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/sources/juce_AudioTransportSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/juce_audio_devices.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/juce_audio_devices.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/juce_audio_devices.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/all.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/bitmath.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/bitreader.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/bitwriter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/cpu.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/crc.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/fixed.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/float.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/format.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/lpc.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/md5.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/memory.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/metadata.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/stream_encoder.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/stream_encoder_framing.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/window.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/protected/all.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/protected/stream_decoder.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/protected/stream_encoder.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/bitmath.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/bitreader.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/bitwriter.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/cpu.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/crc.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/fixed.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/float.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/format.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/lpc_flac.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/md5.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/memory.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/stream_decoder.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/stream_encoder.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/stream_encoder_framing.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/window_flac.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/all.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/alloc.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/assert.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/callback.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/compat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/endswap.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/export.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/Flac Licence.txt"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/format.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/metadata.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/ordinals.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/stream_decoder.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/stream_encoder.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/win_utf8_io.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/books/coupled/res_books_51.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/books/coupled/res_books_stereo.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/books/floor/floor_books.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/books/uncoupled/res_books_uncoupled.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/floor_all.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/psych_8.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/psych_11.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/psych_16.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/psych_44.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_8.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_16.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_44.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_44p51.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_44u.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_8.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_11.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_16.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_22.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_32.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_44.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_44p51.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_44u.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_X.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/analysis.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/backends.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/bitrate.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/bitrate.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/block.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/codebook.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/codebook.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/codec_internal.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/envelope.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/envelope.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/floor0.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/floor1.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/highlevel.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/info.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lookup.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lookup.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lookup_data.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lpc.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lpc.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lsp.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lsp.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/mapping0.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/masking.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/mdct.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/mdct.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/misc.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/misc.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/os.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/psy.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/psy.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/registry.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/registry.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/res0.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/scales.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/sharedbook.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/smallft.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/smallft.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/synthesis.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/vorbisenc.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/vorbisfile.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/window.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/window.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/README.md"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/bitwise.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/codec.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/config_types.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/crctable.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/framing.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/Ogg Vorbis Licence.txt"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/ogg.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/os_types.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/vorbisenc.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/vorbisfile.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_AiffAudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_AiffAudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_CoreAudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_CoreAudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_FlacAudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_FlacAudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_LAMEEncoderAudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_LAMEEncoderAudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_MP3AudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_MP3AudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_OggVorbisAudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_OggVorbisAudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_WavAudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_WavAudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_WindowsMediaAudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_WindowsMediaAudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatManager.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatReader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatReader.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatReaderSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatReaderSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatWriter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatWriter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioSubsectionReader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioSubsectionReader.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_BufferingAudioFormatReader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_BufferingAudioFormatReader.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_MemoryMappedAudioFormatReader.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/sampler/juce_Sampler.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/sampler/juce_Sampler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormatManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormatManager.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/baseiids.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/classfactoryhelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fbuffer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fbuffer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fdebug.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fdebug.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fobject.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fobject.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fstreamer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fstreamer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fstring.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fstring.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/updatehandler.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/updatehandler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/thread/include/flock.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/thread/source/flock.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/LICENSE.txt"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/README.md"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/conststringtable.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/conststringtable.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/coreiids.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/falignpop.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/falignpush.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/fplatform.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/fstrdefs.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ftypes.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/funknown.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/funknown.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/futils.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/fvariant.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ibstream.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/icloneable.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ipersistent.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ipluginbase.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/istringresult.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/iupdatehandler.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/smartpointer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/typesizecheck.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ustring.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ustring.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/gui/iplugview.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/gui/iplugviewcontentscalesupport.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstattributes.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstaudioprocessor.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstautomationstate.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstchannelcontextinfo.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstcomponent.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstcontextmenu.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivsteditcontroller.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstevents.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivsthostapplication.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstinterappaudio.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstmessage.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstmidicontrollers.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstmidilearn.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstnoteexpression.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstparameterchanges.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstphysicalui.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstpluginterfacesupport.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstplugview.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstprefetchablesupport.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstprocesscontext.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstrepresentation.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivsttestplugprovider.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstunits.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/vstpshpack4.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/vstspeaker.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/vsttypes.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/LICENSE.txt"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/README.md"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/common/memorystream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/common/memorystream.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/common/pluginview.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/common/pluginview.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/hosting/hostclasses.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/hosting/hostclasses.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/hosting/pluginterfacesupport.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/hosting/pluginterfacesupport.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstbus.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstbus.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstcomponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstcomponent.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstcomponentbase.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstcomponentbase.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vsteditcontroller.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vsteditcontroller.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstinitiids.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstparameters.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstparameters.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstpresetfile.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstpresetfile.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/LICENSE.txt"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/README.md"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/vestige/vestige.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/JUCE_README.txt"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/LICENSE.txt"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/README.md"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AU_Shared.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AudioUnitPluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AudioUnitPluginFormat.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LADSPAPluginFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LADSPAPluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LegacyAudioParameter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3Common.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3Headers.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3PluginFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3PluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTCommon.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTMidiEventList.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTPluginFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTPluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioPluginInstance.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioPluginInstance.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorEditor.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorEditor.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorGraph.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorGraph.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorListener.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorParameter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorParameterGroup.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorParameterGroup.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_GenericAudioProcessorEditor.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_GenericAudioProcessorEditor.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_PluginDescription.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_PluginDescription.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_KnownPluginList.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_KnownPluginList.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_PluginDirectoryScanner.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_PluginDirectoryScanner.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_PluginListComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_PluginListComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterBool.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterBool.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterChoice.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterChoice.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterFloat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterFloat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterInt.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterInt.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorParameterWithID.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorParameterWithID.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_ParameterAttachments.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_ParameterAttachments.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_PluginHostType.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_PluginHostType.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_RangedAudioParameter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_RangedAudioParameter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_AbstractFifo.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_AbstractFifo.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_Array.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ArrayAllocationBase.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ArrayBase.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ArrayBase.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_DynamicObject.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_DynamicObject.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ElementComparator.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_HashMap.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_HashMap_test.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_LinkedListPointer.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ListenerList.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_NamedValueSet.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_NamedValueSet.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_OwnedArray.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_OwnedArray.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_PropertySet.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_PropertySet.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ReferenceCountedArray.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ReferenceCountedArray.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ScopedValueSetter.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_SortedSet.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_SparseSet.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_SparseSet.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_Variant.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_Variant.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_DirectoryIterator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_DirectoryIterator.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_File.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_File.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileFilter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileFilter.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileInputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileInputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileOutputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileOutputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileSearchPath.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileSearchPath.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_MemoryMappedFile.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_RangedDirectoryIterator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_RangedDirectoryIterator.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_TemporaryFile.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_TemporaryFile.h"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_WildcardFileFilter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/files/juce_WildcardFileFilter.h"
    "../../../ThirdParty/JUCE/modules/juce_core/javascript/juce_Javascript.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/javascript/juce_Javascript.h"
    "../../../ThirdParty/JUCE/modules/juce_core/javascript/juce_JSON.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/javascript/juce_JSON.h"
    "../../../ThirdParty/JUCE/modules/juce_core/logging/juce_FileLogger.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/logging/juce_FileLogger.h"
    "../../../ThirdParty/JUCE/modules/juce_core/logging/juce_Logger.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/logging/juce_Logger.h"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_BigInteger.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_BigInteger.h"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Expression.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Expression.h"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_MathsFunctions.h"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_NormalisableRange.h"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Random.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Random.h"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Range.h"
    "../../../ThirdParty/JUCE/modules/juce_core/maths/juce_StatisticsAccumulator.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_AllocationHooks.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_AllocationHooks.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_Atomic.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ByteOrder.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ContainerDeletePolicy.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_HeapBlock.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_HeavyweightLeakedObjectDetector.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_LeakedObjectDetector.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_Memory.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_MemoryBlock.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_MemoryBlock.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_OptionalScopedPointer.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ReferenceCountedObject.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ScopedPointer.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_SharedResourcePointer.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_Singleton.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_WeakReference.h"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_ConsoleApplication.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_ConsoleApplication.h"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Result.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Result.h"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_RuntimePermissions.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_RuntimePermissions.h"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Uuid.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Uuid.h"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_WindowsRegistry.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/java/README.txt"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Files.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_JNIHelpers.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_JNIHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Misc.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Network.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_RuntimePermissions.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_SystemStats.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Threads.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_BasicNativeHeaders.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_curl_Network.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_intel_SharedCode.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_CommonFile.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Files.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Network.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_SystemStats.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Threads.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_CFHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_ClangBugWorkaround.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Files.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Network.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_ObjCHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Strings.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_SystemStats.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Threads.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_IPAddress.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_NamedPipe.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_SharedCode.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_wasm_SystemStats.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_ComSmartPtr.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_Files.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_Network.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_Registry.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_SystemStats.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_Threads.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_IPAddress.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_IPAddress.h"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_MACAddress.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_MACAddress.h"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_NamedPipe.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_NamedPipe.h"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_Socket.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_Socket.h"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_URL.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_URL.h"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_WebInputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/network/juce_WebInputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_BufferedInputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_BufferedInputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_FileInputSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_FileInputSource.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_InputSource.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_InputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_InputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_MemoryInputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_MemoryInputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_MemoryOutputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_MemoryOutputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_OutputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_OutputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_SubregionStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_SubregionStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_URLInputSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/streams/juce_URLInputSource.h"
    "../../../ThirdParty/JUCE/modules/juce_core/system/juce_CompilerSupport.h"
    "../../../ThirdParty/JUCE/modules/juce_core/system/juce_CompilerWarnings.h"
    "../../../ThirdParty/JUCE/modules/juce_core/system/juce_PlatformDefs.h"
    "../../../ThirdParty/JUCE/modules/juce_core/system/juce_StandardHeader.h"
    "../../../ThirdParty/JUCE/modules/juce_core/system/juce_SystemStats.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/system/juce_SystemStats.h"
    "../../../ThirdParty/JUCE/modules/juce_core/system/juce_TargetPlatform.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_Base64.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_Base64.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharacterFunctions.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharacterFunctions.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharPointer_ASCII.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharPointer_UTF8.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharPointer_UTF16.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharPointer_UTF32.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_Identifier.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_Identifier.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_LocalisedStrings.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_LocalisedStrings.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_NewLine.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_String.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_String.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringArray.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringArray.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringPairArray.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringPairArray.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringPool.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringPool.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringRef.h"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_TextDiff.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/text/juce_TextDiff.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ChildProcess.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ChildProcess.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_CriticalSection.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_DynamicLibrary.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_HighResolutionTimer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_HighResolutionTimer.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_InterProcessLock.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_Process.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ReadWriteLock.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ReadWriteLock.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ScopedLock.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ScopedReadLock.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ScopedWriteLock.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_SpinLock.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_Thread.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_Thread.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ThreadLocalValue.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ThreadPool.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ThreadPool.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_TimeSliceThread.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_TimeSliceThread.h"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_WaitableEvent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_WaitableEvent.h"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_PerformanceCounter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_PerformanceCounter.h"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_RelativeTime.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_RelativeTime.h"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_Time.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_Time.h"
    "../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTest.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTest.h"
    "../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTestCategories.h"
    "../../../ThirdParty/JUCE/modules/juce_core/xml/juce_XmlDocument.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/xml/juce_XmlDocument.h"
    "../../../ThirdParty/JUCE/modules/juce_core/xml/juce_XmlElement.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/xml/juce_XmlElement.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/adler32.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/compress.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/crc32.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/crc32.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/deflate.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/deflate.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/infback.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inffast.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inffast.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inffixed.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inflate.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inflate.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inftrees.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inftrees.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/trees.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/trees.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/uncompr.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zconf.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zconf.in.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zlib.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zutil.c"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zutil.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/juce_GZIPCompressorOutputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/juce_GZIPCompressorOutputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/juce_GZIPDecompressorInputStream.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/juce_GZIPDecompressorInputStream.h"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/juce_ZipFile.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/zip/juce_ZipFile.h"
    "../../../ThirdParty/JUCE/modules/juce_core/juce_core.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/juce_core.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/juce_core.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/app_properties/juce_ApplicationProperties.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/app_properties/juce_ApplicationProperties.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/app_properties/juce_PropertiesFile.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/app_properties/juce_PropertiesFile.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/undomanager/juce_UndoableAction.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/undomanager/juce_UndoManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/undomanager/juce_UndoManager.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_CachedValue.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_CachedValue.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_Value.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_Value.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueTree.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueTree.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueTreeSynchroniser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueTreeSynchroniser.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueWithDefault.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueWithDefault.h"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.mm"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.h"
    "../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ActionBroadcaster.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ActionBroadcaster.h"
    "../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ActionListener.h"
    "../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_AsyncUpdater.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_AsyncUpdater.h"
    "../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ChangeBroadcaster.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ChangeBroadcaster.h"
    "../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ChangeListener.h"
    "../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_ConnectedChildProcess.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_ConnectedChildProcess.h"
    "../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_InterprocessConnection.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_InterprocessConnection.h"
    "../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_InterprocessConnectionServer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_InterprocessConnectionServer.h"
    "../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_NetworkServiceDiscovery.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_NetworkServiceDiscovery.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_ApplicationBase.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_ApplicationBase.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_CallbackMessage.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_DeletedAtShutdown.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_DeletedAtShutdown.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_Initialisation.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_Message.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MessageListener.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MessageListener.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MessageManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MessageManager.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MountedVolumeListChangeDetector.h"
    "../../../ThirdParty/JUCE/modules/juce_events/messages/juce_NotificationType.h"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_android_Messaging.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_ios_MessageManager.mm"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_linux_EventLoop.h"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_linux_Messaging.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_mac_MessageManager.mm"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_osx_MessageQueue.h"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_win32_HiddenMessageWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_win32_Messaging.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_win32_WinRTWrapper.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/native/juce_win32_WinRTWrapper.h"
    "../../../ThirdParty/JUCE/modules/juce_events/timers/juce_MultiTimer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/timers/juce_MultiTimer.h"
    "../../../ThirdParty/JUCE/modules/juce_events/timers/juce_Timer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/timers/juce_Timer.h"
    "../../../ThirdParty/JUCE/modules/juce_events/juce_events.cpp"
    "../../../ThirdParty/JUCE/modules/juce_events/juce_events.mm"
    "../../../ThirdParty/JUCE/modules/juce_events/juce_events.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_Colour.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_Colour.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_ColourGradient.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_ColourGradient.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_Colours.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_Colours.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_FillType.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_FillType.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_PixelFormats.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_GraphicsContext.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_GraphicsContext.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsContext.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsPostScriptRenderer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsPostScriptRenderer.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsSoftwareRenderer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsSoftwareRenderer.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_DropShadowEffect.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_DropShadowEffect.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_GlowEffect.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_GlowEffect.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_ImageEffectFilter.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_AttributedString.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_AttributedString.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_CustomTypeface.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_CustomTypeface.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_Font.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_Font.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_GlyphArrangement.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_GlyphArrangement.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_TextLayout.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_TextLayout.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_Typeface.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_Typeface.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_AffineTransform.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_AffineTransform.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_BorderSize.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_EdgeTable.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_EdgeTable.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Line.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Parallelogram.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Path.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Path.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_PathIterator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_PathIterator.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_PathStrokeType.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_PathStrokeType.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Point.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Rectangle.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_RectangleList.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/cderror.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/changes to libjpeg for JUCE.txt"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcapimin.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcapistd.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jccoefct.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jccolor.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcdctmgr.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jchuff.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jchuff.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcinit.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcmainct.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcmarker.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcmaster.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcomapi.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jconfig.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcparam.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcphuff.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcprepct.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcsample.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jctrans.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdapimin.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdapistd.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdatasrc.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdcoefct.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdcolor.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdct.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jddctmgr.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdhuff.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdhuff.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdinput.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdmainct.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdmarker.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdmaster.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdmerge.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdphuff.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdpostct.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdsample.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdtrans.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jerror.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jerror.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jfdctflt.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jfdctfst.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jfdctint.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jidctflt.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jidctfst.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jidctint.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jidctred.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jinclude.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jmemmgr.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jmemnobs.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jmemsys.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jmorecfg.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jpegint.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jpeglib.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jquant1.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jquant2.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jutils.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jversion.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/transupp.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/transupp.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/libpng_readme.txt"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/png.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/png.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngconf.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngdebug.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngerror.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngget.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pnginfo.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngmem.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngpread.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngpriv.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngread.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngrio.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngrtran.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngrutil.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngset.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngstruct.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngtrans.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngwio.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngwrite.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngwtran.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngwutil.c"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/juce_GIFLoader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/juce_JPEGLoader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/juce_PNGLoader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_Image.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_Image.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageCache.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageCache.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageConvolutionKernel.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageConvolutionKernel.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageFileFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageFileFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_android_Fonts.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_android_GraphicsContext.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_android_IconHelpers.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_freetype_Fonts.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_linux_Fonts.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_linux_IconHelpers.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_CoreGraphicsContext.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_CoreGraphicsContext.mm"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_CoreGraphicsHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_Fonts.mm"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_IconHelpers.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_RenderingHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_Direct2DGraphicsContext.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_Direct2DGraphicsContext.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_DirectWriteTypeface.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_DirectWriteTypeLayout.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_Fonts.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_IconHelpers.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_Justification.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_RectanglePlacement.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_RectanglePlacement.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.mm"
    "../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/application/juce_Application.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/application/juce_Application.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ArrowButton.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ArrowButton.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_Button.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_Button.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_DrawableButton.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_DrawableButton.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_HyperlinkButton.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_HyperlinkButton.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ImageButton.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ImageButton.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ShapeButton.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ShapeButton.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_TextButton.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_TextButton.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ToggleButton.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ToggleButton.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ToolbarButton.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ToolbarButton.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandID.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandInfo.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandInfo.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandManager.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandTarget.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandTarget.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_KeyPressMappingSet.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_KeyPressMappingSet.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_CachedComponentImage.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_Component.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_Component.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ComponentListener.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ComponentListener.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ModalComponentManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ModalComponentManager.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/desktop/juce_Desktop.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/desktop/juce_Desktop.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/desktop/juce_Displays.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/desktop/juce_Displays.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_Drawable.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_Drawable.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableComposite.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableComposite.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableImage.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableImage.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawablePath.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawablePath.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableRectangle.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableRectangle.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableShape.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableShape.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableText.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableText.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_SVGParser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_ContentSharer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_ContentSharer.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsDisplayComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsDisplayComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsList.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsList.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileBrowserComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileBrowserComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileBrowserListener.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileChooser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileChooser.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileChooserDialogBox.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileChooserDialogBox.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileListComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileListComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FilenameComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FilenameComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FilePreviewComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileSearchPathListComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileSearchPathListComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileTreeComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileTreeComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_ImagePreviewComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_ImagePreviewComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_CaretComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_CaretComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyboardFocusTraverser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyboardFocusTraverser.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyListener.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyListener.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyPress.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyPress.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_ModifierKeys.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_ModifierKeys.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_SystemClipboard.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_TextEditorKeyMapper.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_TextInputTarget.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_AnimatedPosition.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_AnimatedPositionBehaviours.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentAnimator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentAnimator.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentBoundsConstrainer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentBoundsConstrainer.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentBuilder.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentBuilder.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentMovementWatcher.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentMovementWatcher.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ConcertinaPanel.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ConcertinaPanel.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_FlexBox.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_FlexBox.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_FlexItem.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_Grid.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_Grid.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_GridItem.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_GridItem.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_GroupComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_GroupComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_MultiDocumentPanel.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_MultiDocumentPanel.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableBorderComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableBorderComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableCornerComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableCornerComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableEdgeComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableEdgeComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ScrollBar.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ScrollBar.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_SidePanel.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_SidePanel.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableLayoutManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableLayoutManager.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableLayoutResizerBar.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableLayoutResizerBar.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableObjectResizer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableObjectResizer.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_TabbedButtonBar.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_TabbedButtonBar.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_TabbedComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_TabbedComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_Viewport.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_Viewport.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V1.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V1.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V2.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V2.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V3.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V3.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V4.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V4.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_BurgerMenuComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_BurgerMenuComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_MenuBarComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_MenuBarComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_MenuBarModel.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_MenuBarModel.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_PopupMenu.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_PopupMenu.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_BubbleComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_BubbleComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_DropShadower.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_DropShadower.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_JUCESplashScreen.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_JUCESplashScreen.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_ComponentDragger.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_ComponentDragger.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_DragAndDropContainer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_DragAndDropContainer.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_DragAndDropTarget.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_FileDragAndDropTarget.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_LassoComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseCursor.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseCursor.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseEvent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseEvent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseInactivityDetector.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseInactivityDetector.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseInputSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseInputSource.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseListener.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseListener.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_SelectedItemSet.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_TextDragAndDropTarget.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_TooltipClient.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_X11_DragAndDrop.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_X11_Symbols.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_X11_Symbols.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_XWindowSystem.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_XWindowSystem.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_ContentSharer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_FileChooser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_Windowing.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_common_MimeTypes.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_ContentSharer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_FileChooser.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_UIViewComponentPeer.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_Windowing.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_FileChooser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_Windowing.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_FileChooser.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_MainMenu.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_MouseCursor.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_NSViewComponentPeer.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_Windowing.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_MultiTouchMapper.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_DragAndDrop.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_FileChooser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_ScopedThreadDPIAwarenessSetter.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_Windowing.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_MarkerList.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_MarkerList.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeCoordinate.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeCoordinate.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeCoordinatePositioner.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeCoordinatePositioner.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeParallelogram.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeParallelogram.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativePoint.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativePoint.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativePointPath.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativePointPath.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeRectangle.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeRectangle.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_BooleanPropertyComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_BooleanPropertyComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_ButtonPropertyComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_ButtonPropertyComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_ChoicePropertyComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_ChoicePropertyComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_MultiChoicePropertyComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_MultiChoicePropertyComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_PropertyComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_PropertyComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_PropertyPanel.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_PropertyPanel.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_SliderPropertyComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_SliderPropertyComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_TextPropertyComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_TextPropertyComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ComboBox.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ComboBox.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ImageComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ImageComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Label.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Label.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ListBox.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ListBox.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ProgressBar.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ProgressBar.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Slider.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Slider.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TableHeaderComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TableHeaderComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TableListBox.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TableListBox.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TextEditor.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TextEditor.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Toolbar.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Toolbar.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemFactory.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemPalette.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemPalette.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TreeView.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TreeView.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_AlertWindow.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_AlertWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_CallOutBox.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_CallOutBox.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ComponentPeer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ComponentPeer.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_DialogWindow.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_DialogWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_DocumentWindow.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_DocumentWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_NativeMessageBox.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ResizableWindow.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ResizableWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ThreadWithProgressWindow.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ThreadWithProgressWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_TooltipWindow.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_TooltipWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_TopLevelWindow.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_TopLevelWindow.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeDocument.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeDocument.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeEditorComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeEditorComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeTokeniser.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CPlusPlusCodeTokeniser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CPlusPlusCodeTokeniser.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CPlusPlusCodeTokeniserFunctions.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_LuaCodeTokeniser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_LuaCodeTokeniser.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_XMLCodeTokeniser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_XMLCodeTokeniser.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/documents/juce_FileBasedDocument.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/documents/juce_FileBasedDocument.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_ActiveXControlComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_AndroidViewComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_HWNDComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_NSViewComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_ScopedDPIAwarenessDisabler.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_UIViewComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_XEmbedComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_AnimatedAppComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_AnimatedAppComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_AppleRemote.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_BubbleMessageComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_BubbleMessageComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_ColourSelector.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_ColourSelector.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_KeyMappingEditorComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_KeyMappingEditorComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_LiveConstantEditor.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_LiveConstantEditor.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_PreferencesPanel.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_PreferencesPanel.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_PushNotifications.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_PushNotifications.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SplashScreen.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SplashScreen.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SystemTrayIconComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SystemTrayIconComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_WebBrowserComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_android_PushNotifications.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_android_WebBrowserComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_AndroidViewComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_ios_PushNotifications.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_ios_UIViewComponent.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_X11_SystemTrayIcon.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_X11_WebBrowserComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_XEmbedComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_AppleRemote.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_CarbonViewWrapperComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_NSViewComponent.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_PushNotifications.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_SystemTrayIcon.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_WebBrowserComponent.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_ActiveXComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_HWNDComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_SystemTrayIcon.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_WebBrowserComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/geometry/juce_Draggable3DOrientation.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/geometry/juce_Matrix3D.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/geometry/juce_Quaternion.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/geometry/juce_Vector3D.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_MissingGLDefinitions.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_android.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_ios.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_linux_X11.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_osx.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_win32.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGLExtensions.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLContext.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLContext.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLFrameBuffer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLFrameBuffer.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLGraphicsContext.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLGraphicsContext.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLHelpers.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLImage.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLImage.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLPixelFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLPixelFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLRenderer.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLShaderProgram.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLShaderProgram.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLTexture.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLTexture.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/utils/juce_OpenGLAppComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/utils/juce_OpenGLAppComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.cpp"
    "../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.mm"
    "../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.h"
    "../../Projucer/JuceLibraryCode/AppConfig.h"
    "../../Projucer/JuceLibraryCode/BinaryData.cpp"
    "../../Projucer/JuceLibraryCode/BinaryData.h"
    "../../Projucer/JuceLibraryCode/include_juce_audio_basics.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_audio_devices.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_audio_formats.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_audio_processors.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_core.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_data_structures.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_events.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_graphics.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_gui_basics.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_gui_extra.cpp"
    "../../Projucer/JuceLibraryCode/include_juce_opengl.cpp"
    "../../Projucer/JuceLibraryCode/JuceHeader.h"
)

set_source_files_properties("../../../Resources/Icons/annotation.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/apply.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/arpeggiate.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/audioPlugin.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/automationTrack.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/back.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/bottomBar.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/browse.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/chordBuilder.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/close.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/colour.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/commit.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/console.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/copy.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/create.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cropTool.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cursorTool.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cut.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cutterTool.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/down.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/dragTool.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/drawTool.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/ellipsis.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/expand.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/fail.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/forward.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/github.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/helio.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/instrument.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/instrumentNode.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/inverseDown.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/inverseUp.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/list.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/local.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/mute.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/orchestraPit.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/paint.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/paste.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/patterns.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/pause.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/piano.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/pianoTrack.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/play.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/progressIndicator.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/project.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/pull.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/push.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/record.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/redo.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/refactor.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/remote.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/remove.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/render.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/reprise.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/reset.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/revision.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/routing.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/selectionTool.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/selection.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/selectAll.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/selectNone.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/settings.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/stop.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/stretchLeft.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/stretchRight.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/submenu.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/success.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/tag.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/timelineNext.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/timelinePrevious.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/toggleOff.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/toggleOn.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/trackGroup.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/undo.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/unmute.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/up.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/versionControl.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/volume.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/volumePanel.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/zoomIn.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/zoomOut.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/zoomToFit.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/noise.png" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Templates/emptyProject.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Templates/exampleProject.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/logo-v2.png" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/arpeggiators.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/chords.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/colourSchemes.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/hotkeySchemes.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/scales.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/temperaments.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/translations.json" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynthAudioPlugin.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynthFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynth.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/InternalPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/Instrument.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/OrchestraListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/OrchestraPit.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/PluginScanner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/SerializablePluginDescription.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Monitoring/AudioMonitor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/MidiRecorder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/PlayerThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/PlayerThreadPool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/RendererThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/RenderFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/Transport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/TransportListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/TransportPlaybackCache.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/AudioCore.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/BaseResource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Arpeggiator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Chord.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/ColourScheme.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/HotkeyScheme.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Scale.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Temperament.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Translation.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/TranslationKeys.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ResourceManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ArpeggiatorsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ChordsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ColourSchemesManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/HotkeySchemesManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ScalesManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/TemperamentsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/TranslationsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Config.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/UserInterfaceFlags.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/CommandPalette/CommandPaletteModel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/CommandPalette/CommandPaletteActionsProvider.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/CommandPalette/CommandPaletteCommonActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/CommandPalette/CommandPaletteChordConstructor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/CommandPalette/CommandPaletteMoveNotesMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/CommandPalette/CommandPaletteProjectsList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/CommandPalette/CommandPaletteTimelineEvents.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Patterns/Clip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Patterns/Pattern.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/Events/AnnotationEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/Events/AutomationEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/Events/KeySignatureEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/Events/MidiEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/Events/Note.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/Events/TimeSignatureEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/AnnotationsSequence.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/AutomationSequence.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/KeySignaturesSequence.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/MidiSequence.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/PianoSequence.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/Sequences/TimeSignaturesSequence.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/KeyboardMapping.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Midi/MidiTrack.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/ApiModel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/AppInfoDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/AppResourceDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/AppVersionDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/AuthSessionDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/ProjectDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/ProjectsListDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/RevisionDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/UserProfileDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/UserResourceDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Models/UserSessionDto.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/BackendRequest.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/AuthThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/BaseConfigSyncThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/UserConfigSyncThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/ProjectCloneThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/ProjectDeleteThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/RevisionsSyncHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/RevisionsSyncThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/RequestUserProfileThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/TokenCheckThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/TokenUpdateThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Requests/UpdatesCheckThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Services/BackendService.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Services/ProjectSyncService.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Services/ResourceSyncService.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Services/SessionService.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/Network.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Autosaver.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Document.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/DocumentHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/DocumentOwner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Serializable.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/SerializationKeys.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/SerializedData.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Serializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/BinarySerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/JsonSerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/XmlSerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/AutomationTrackNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/InstrumentNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/MidiTrackNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/MidiTrackSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/OrchestraPitNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/PatternEditorNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/PianoTrackNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectMetadata.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectTimeline.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectEventDispatcher.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/RootNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/SettingsNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/TrackGroupNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/TreeNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/TreeNodeSerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/VersionControlNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/AnnotationEventActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/AutomationTrackActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/AutomationEventActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/KeySignatureEventActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/MidiTrackActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/NoteActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/PatternActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/PianoTrackActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/ProjectMetadataActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/TimeSignatureEventActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/UndoAction.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/UndoActionIDs.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/UndoStack.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/AutomationTrackDiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/DiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/PatternDiffHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/PianoTrackDiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Delta.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Diff.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Head.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/RemoteCache.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Revision.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/RevisionItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Snapshot.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/StashesRepository.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/TrackedItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/TrackedItemsSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/VersionControl.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Workspace/NavigationHistory.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Workspace/RecentProjectInfo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Workspace/SyncedConfigurationInfo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Workspace/UserSessionInfo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Workspace/UserProfile.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Workspace/Workspace.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/App.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/AudioMonitors/SpectrogramAudioMonitorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/AudioMonitors/WaveformAudioMonitorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/Origami/Origami.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/Origami/OrigamiHorizontal.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/Origami/OrigamiVertical.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/CachedLabelImage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ScaledComponentProxy.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ColourButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ColourSwatches.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ColourIDs.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/CommandIDs.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ComponentIDs.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/DraggingListBoxComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/FineTuningComponentDragger.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/FineTuningValueIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/KeySelector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/FloatBoundsComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/HelperRectangle.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/HighlightedComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/IconButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/IconComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/MobileComboBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ModeIndicatorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/OverlayButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/PlayButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/PluginWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/RadioButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ScaleEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/SpectralLogo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/TransportControlComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ViewportFitProxyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/DialogBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/AnnotationDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/KeySignatureDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/ModalDialogConfirmation.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/ModalDialogInput.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/RenderDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/TempoDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/TimeSignatureDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/TrackPropertiesDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/Headline.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineDropdown.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineItemArrow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineItemHighlighter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineItemDataSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineNavigationPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineContextMenuController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/LongTapController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/LongTapListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/MultiTouchController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/MultiTouchListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothPanController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothPanListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothZoomController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothZoomListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/Base/MenuItemComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/Base/MenuPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/AudioPluginSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/InstrumentNodeSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/PatternRollSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/PianoRollSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/VersionControlHistorySelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/VersionControlStageSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/InstrumentMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/MidiTrackMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/OrchestraPitMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/PatternsMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/ProjectMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/TimelineMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/VersionControlMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/WorkspaceMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/DashboardMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/RecentProjectRow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/LoginButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/CreateProjectButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/OpenProjectButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Dashboard.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/UpdatesInfoComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/UserProfileComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/AudioPluginEditorPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/InstrumentEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/InstrumentEditorConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/InstrumentComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/InstrumentEditorPin.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/KeyboardMappingPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/AudioPluginsListComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/InstrumentsListComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/OrchestraPitPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Project/ProjectPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/AudioSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/ComponentsList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/SettingsFrameWrapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/SettingsPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/SyncSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/SyncSettingsItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/ThemeSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/ThemeSettingsItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/TranslationSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/TranslationSettingsItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/UserInterfaceSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/VCS/HistoryComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/VCS/RevisionComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/VCS/RevisionConnectorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/VCS/RevisionItemComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/VCS/RevisionTooltipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/VCS/RevisionTreeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/VCS/StageComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/VCS/VersionControlEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/CommandPalette.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/NotesTuningPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ArpPreviewTool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ChordPreviewTool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/RescalePreviewTool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ScalePreviewTool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/HelioCallout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupButtonOwner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupCustomButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupMenuComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/CenteredTooltipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/FailTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/GenericTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/SuccessTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ProgressIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ProgressTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/TooltipContainer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/ClipRangeIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/HeaderSelectionIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/RollHeader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/Playhead.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/SoundProbeIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/TimeDistanceIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/TrackEndIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/TrackStartIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/CutPointMark.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/RollExpandMark.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/KnifeToolHelper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/PatternOperations.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/SequencerOperations.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/InteractiveActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveHelper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventsConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepsClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventsConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip/PianoClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/ClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/DummyClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/PatternRoll.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PianoRoll/HighlightingScheme.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PianoRoll/NoteComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PianoRoll/NoteNameGuide.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PianoRoll/NoteNameGuidesBar.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PianoRoll/NoteResizerLeft.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PianoRoll/NoteResizerRight.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PianoRoll/PianoRoll.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Sidebars/SequencerSidebarLeft.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Sidebars/SequencerSidebarRight.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationLargeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationSmallComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationsProjectMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureLargeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureSmallComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignaturesProjectMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/LevelsMap/LevelsMapScroller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/LevelsMap/VelocityProjectMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/PianoMap/PianoProjectMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapScroller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapScrollerScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureLargeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureSmallComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignaturesProjectMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/RollBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/RollEditMode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/RollListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Lasso.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/LassoListeners.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MidiEventComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/SelectableComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/SelectionComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/SequencerLayout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ComponentFader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/DialogBackground.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/HelioTheme.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/Icons.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/FramePanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelBackgroundA.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelBackgroundB.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelBackgroundC.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorHorizontal.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorHorizontalReversed.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorHorizontalFading.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorHorizontalFadingReversed.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorVertical.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorVerticalSkew.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowDownwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowUpwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowLeftwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowRightwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ViewportKineticSlider.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MainLayout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Common.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/audio_play_head/juce_AudioPlayHead.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioChannelSet.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioChannelSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioDataConverters.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioDataConverters.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioProcessLoadMeasurer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioProcessLoadMeasurer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioSampleBuffer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_FloatVectorOperations.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_FloatVectorOperations.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiBuffer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiBuffer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiFile.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiFile.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiKeyboardState.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiKeyboardState.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiMessage.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiMessage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiMessageSequence.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiMessageSequence.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiRPN.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/midi/juce_MidiRPN.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEInstrument.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEInstrument.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEMessages.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEMessages.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPENote.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPENote.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiserBase.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiserBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiserVoice.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPESynthesiserVoice.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEUtils.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEUtils.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEValue.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEValue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZoneLayout.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZoneLayout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/native/juce_mac_CoreAudioLayouts.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_AudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_BufferingAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_BufferingAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ChannelRemappingAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ChannelRemappingAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_IIRFilterAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_IIRFilterAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_MemoryAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_MemoryAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_MixerAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_MixerAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_PositionableAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ResamplingAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ResamplingAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ReverbAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ReverbAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ToneGeneratorAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ToneGeneratorAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/synthesisers/juce_Synthesiser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/synthesisers/juce_Synthesiser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_ADSR.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_ADSR_test.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_Decibels.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_GenericInterpolator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_IIRFilter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_IIRFilter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_Interpolators.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_Interpolators.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_LagrangeInterpolator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_Reverb.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_SmoothedValue.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_SmoothedValue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/utilities/juce_WindowedSincInterpolator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioDeviceManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioDeviceManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODevice.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODevice.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODeviceType.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODeviceType.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_SystemAudioVolume.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPacket.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPackets.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPBytestreamInputHandler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPConversion.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPConverters.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPDispatcher.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPFactory.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPIterator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPMidi1ToBytestreamTranslator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPMidi1ToMidi2DefaultTranslator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPMidi1ToMidi2DefaultTranslator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPProtocols.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPReceiver.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPSysEx7.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPSysEx7.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPTests.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPU32InputHandler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPUtils.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPUtils.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPView.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/ump/juce_UMPView.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiDevices.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiDevices.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiMessageCollector.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiMessageCollector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/AudioStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/AudioStreamBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/AudioStreamBuilder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/AudioStreamCallback.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/Definitions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/LatencyTuner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/Oboe.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/ResultWithValue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/StabilizedCallback.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/Utilities.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/include/oboe/Version.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/aaudio/AAudioLoader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/aaudio/AAudioLoader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/aaudio/AudioStreamAAudio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/aaudio/AudioStreamAAudio.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioClock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioSourceCaller.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioSourceCaller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/AudioStreamBuilder.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/DataConversionFlowGraph.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/DataConversionFlowGraph.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FilterAudioStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FilterAudioStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockAdapter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockAdapter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockReader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockReader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockWriter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/FixedBlockWriter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/LatencyTuner.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/MonotonicCounter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/OboeDebug.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/QuirksManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/QuirksManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/SourceFloatCaller.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/SourceFloatCaller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/SourceI16Caller.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/SourceI16Caller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/StabilizedCallback.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/Trace.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/Trace.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/Utilities.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/common/Version.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoBuffer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoBuffer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoController.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoControllerBase.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoControllerBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoControllerIndirect.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/fifo/FifoControllerIndirect.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/HyperbolicCosineWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/IntegerRatio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/IntegerRatio.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/KaiserWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/LinearResampler.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/LinearResampler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/MultiChannelResampler.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/MultiChannelResampler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResampler.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResampler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResamplerMono.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResamplerMono.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResamplerStereo.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/PolyphaseResamplerStereo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/SincResampler.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/SincResampler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/SincResamplerStereo.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/resampler/SincResamplerStereo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ChannelCountConverter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ChannelCountConverter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ClipToRange.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ClipToRange.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/FlowGraphNode.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/FlowGraphNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ManyToMultiConverter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/ManyToMultiConverter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/MonoToMultiConverter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/MonoToMultiConverter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/MultiToMonoConverter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/MultiToMonoConverter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/RampLinear.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/RampLinear.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SampleRateConverter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SampleRateConverter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkFloat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkFloat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkI16.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkI16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkI24.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SinkI24.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceFloat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceFloat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceI16.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceI16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceI24.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/flowgraph/SourceI24.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioInputStreamOpenSLES.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioInputStreamOpenSLES.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioOutputStreamOpenSLES.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioOutputStreamOpenSLES.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioStreamBuffered.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioStreamBuffered.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioStreamOpenSLES.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/AudioStreamOpenSLES.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/EngineOpenSLES.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/EngineOpenSLES.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/OpenSLESUtilities.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/OpenSLESUtilities.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/OutputMixerOpenSLES.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/src/opensles/OutputMixerOpenSLES.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/CMakeLists.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/oboe/readme.md" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Audio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_HighPerformanceAudioHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Midi.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Oboe.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_OpenSL.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_ios_Audio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_ios_Audio.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_ALSA.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_Bela.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_JackAudio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_Midi.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_mac_CoreAudio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_mac_CoreMidi.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_MidiDataConcatenator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_win32_ASIO.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_win32_DirectSound.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_win32_Midi.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_win32_WASAPI.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/sources/juce_AudioSourcePlayer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/sources/juce_AudioSourcePlayer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/sources/juce_AudioTransportSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/sources/juce_AudioTransportSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/juce_audio_devices.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/juce_audio_devices.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/juce_audio_devices.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/all.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/bitmath.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/bitreader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/bitwriter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/cpu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/crc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/fixed.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/float.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/format.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/lpc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/md5.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/memory.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/metadata.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/stream_encoder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/stream_encoder_framing.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/private/window.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/protected/all.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/protected/stream_decoder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/include/protected/stream_encoder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/bitmath.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/bitreader.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/bitwriter.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/cpu.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/crc.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/fixed.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/float.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/format.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/lpc_flac.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/md5.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/memory.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/stream_decoder.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/stream_encoder.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/stream_encoder_framing.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/libFLAC/window_flac.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/all.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/alloc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/assert.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/callback.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/compat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/endswap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/export.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/Flac Licence.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/format.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/metadata.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/ordinals.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/stream_decoder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/stream_encoder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/flac/win_utf8_io.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/books/coupled/res_books_51.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/books/coupled/res_books_stereo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/books/floor/floor_books.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/books/uncoupled/res_books_uncoupled.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/floor_all.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/psych_8.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/psych_11.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/psych_16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/psych_44.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_8.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_44.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_44p51.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/residue_44u.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_8.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_11.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_22.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_32.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_44.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_44p51.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_44u.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/modes/setup_X.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/analysis.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/backends.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/bitrate.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/bitrate.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/block.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/codebook.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/codebook.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/codec_internal.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/envelope.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/envelope.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/floor0.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/floor1.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/highlevel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/info.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lookup.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lookup.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lookup_data.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lpc.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lpc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lsp.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/lsp.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/mapping0.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/masking.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/mdct.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/mdct.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/misc.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/misc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/os.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/psy.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/psy.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/registry.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/registry.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/res0.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/scales.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/sharedbook.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/smallft.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/smallft.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/synthesis.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/vorbisenc.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/vorbisfile.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/window.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/lib/window.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.7/README.md" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/bitwise.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/codec.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/config_types.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/crctable.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/framing.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/Ogg Vorbis Licence.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/ogg.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/os_types.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/vorbisenc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/vorbisfile.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_AiffAudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_AiffAudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_CoreAudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_CoreAudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_FlacAudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_FlacAudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_LAMEEncoderAudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_LAMEEncoderAudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_MP3AudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_MP3AudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_OggVorbisAudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_OggVorbisAudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_WavAudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_WavAudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_WindowsMediaAudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_WindowsMediaAudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatReader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatReader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatReaderSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatReaderSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatWriter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioFormatWriter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioSubsectionReader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_AudioSubsectionReader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_BufferingAudioFormatReader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_BufferingAudioFormatReader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/format/juce_MemoryMappedAudioFormatReader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/sampler/juce_Sampler.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/sampler/juce_Sampler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormatManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormatManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/baseiids.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/classfactoryhelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fbuffer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fbuffer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fdebug.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fdebug.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fobject.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fobject.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fstreamer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fstreamer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fstring.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/fstring.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/updatehandler.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/source/updatehandler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/thread/include/flock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/thread/source/flock.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/LICENSE.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/base/README.md" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/conststringtable.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/conststringtable.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/coreiids.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/falignpop.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/falignpush.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/fplatform.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/fstrdefs.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ftypes.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/funknown.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/funknown.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/futils.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/fvariant.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ibstream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/icloneable.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ipersistent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ipluginbase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/istringresult.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/iupdatehandler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/smartpointer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/typesizecheck.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ustring.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/base/ustring.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/gui/iplugview.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/gui/iplugviewcontentscalesupport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstattributes.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstaudioprocessor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstautomationstate.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstchannelcontextinfo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstcomponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstcontextmenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivsteditcontroller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstevents.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivsthostapplication.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstinterappaudio.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstmessage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstmidicontrollers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstmidilearn.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstnoteexpression.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstparameterchanges.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstphysicalui.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstpluginterfacesupport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstplugview.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstprefetchablesupport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstprocesscontext.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstrepresentation.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivsttestplugprovider.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstunits.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/vstpshpack4.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/vstspeaker.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/vsttypes.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/LICENSE.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/README.md" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/common/memorystream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/common/memorystream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/common/pluginview.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/common/pluginview.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/hosting/hostclasses.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/hosting/hostclasses.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/hosting/pluginterfacesupport.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/hosting/pluginterfacesupport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstbus.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstbus.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstcomponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstcomponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstcomponentbase.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstcomponentbase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vsteditcontroller.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vsteditcontroller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstinitiids.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstparameters.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstparameters.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstpresetfile.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/source/vst/vstpresetfile.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/LICENSE.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/public.sdk/README.md" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/vestige/vestige.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/JUCE_README.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/LICENSE.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK/README.md" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AU_Shared.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AudioUnitPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AudioUnitPluginFormat.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LADSPAPluginFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LADSPAPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LegacyAudioParameter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3Common.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3Headers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3PluginFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3PluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTCommon.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTMidiEventList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTPluginFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioPluginInstance.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioPluginInstance.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorEditor.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorGraph.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorGraph.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorParameter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorParameterGroup.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorParameterGroup.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_GenericAudioProcessorEditor.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_GenericAudioProcessorEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_PluginDescription.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_PluginDescription.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_KnownPluginList.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_KnownPluginList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_PluginDirectoryScanner.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_PluginDirectoryScanner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_PluginListComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/scanning/juce_PluginListComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterBool.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterBool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterChoice.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterChoice.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterFloat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterFloat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterInt.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterInt.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorParameterWithID.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorParameterWithID.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_ParameterAttachments.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_ParameterAttachments.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_PluginHostType.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_PluginHostType.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_RangedAudioParameter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_RangedAudioParameter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_AbstractFifo.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_AbstractFifo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_Array.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ArrayAllocationBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ArrayBase.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ArrayBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_DynamicObject.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_DynamicObject.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ElementComparator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_HashMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_HashMap_test.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_LinkedListPointer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ListenerList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_NamedValueSet.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_NamedValueSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_OwnedArray.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_OwnedArray.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_PropertySet.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_PropertySet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ReferenceCountedArray.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ReferenceCountedArray.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ScopedValueSetter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_SortedSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_SparseSet.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_SparseSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_Variant.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_Variant.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_DirectoryIterator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_DirectoryIterator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_File.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_File.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileFilter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileFilter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileInputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileInputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileOutputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileOutputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileSearchPath.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_FileSearchPath.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_MemoryMappedFile.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_RangedDirectoryIterator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_RangedDirectoryIterator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_TemporaryFile.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_TemporaryFile.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_WildcardFileFilter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/files/juce_WildcardFileFilter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/javascript/juce_Javascript.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/javascript/juce_Javascript.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/javascript/juce_JSON.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/javascript/juce_JSON.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/logging/juce_FileLogger.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/logging/juce_FileLogger.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/logging/juce_Logger.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/logging/juce_Logger.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_BigInteger.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_BigInteger.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Expression.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Expression.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_MathsFunctions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_NormalisableRange.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Random.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Random.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_Range.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/maths/juce_StatisticsAccumulator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_AllocationHooks.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_AllocationHooks.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_Atomic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ByteOrder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ContainerDeletePolicy.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_HeapBlock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_HeavyweightLeakedObjectDetector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_LeakedObjectDetector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_Memory.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_MemoryBlock.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_MemoryBlock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_OptionalScopedPointer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ReferenceCountedObject.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ScopedPointer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_SharedResourcePointer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_Singleton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_WeakReference.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_ConsoleApplication.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_ConsoleApplication.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Result.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Result.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_RuntimePermissions.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_RuntimePermissions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Uuid.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Uuid.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_WindowsRegistry.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/java/README.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Files.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_JNIHelpers.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_JNIHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Misc.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Network.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_RuntimePermissions.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_SystemStats.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Threads.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_BasicNativeHeaders.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_curl_Network.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_intel_SharedCode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_CommonFile.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Files.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Network.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_SystemStats.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Threads.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_CFHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_ClangBugWorkaround.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Files.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Network.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_ObjCHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Strings.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_SystemStats.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Threads.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_IPAddress.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_NamedPipe.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_SharedCode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_wasm_SystemStats.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_ComSmartPtr.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_Files.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_Network.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_Registry.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_SystemStats.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_win32_Threads.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_IPAddress.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_IPAddress.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_MACAddress.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_MACAddress.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_NamedPipe.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_NamedPipe.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_Socket.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_Socket.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_URL.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_URL.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_WebInputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/network/juce_WebInputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_BufferedInputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_BufferedInputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_FileInputSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_FileInputSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_InputSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_InputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_InputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_MemoryInputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_MemoryInputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_MemoryOutputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_MemoryOutputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_OutputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_OutputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_SubregionStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_SubregionStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_URLInputSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/streams/juce_URLInputSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/system/juce_CompilerSupport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/system/juce_CompilerWarnings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/system/juce_PlatformDefs.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/system/juce_StandardHeader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/system/juce_SystemStats.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/system/juce_SystemStats.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/system/juce_TargetPlatform.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_Base64.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_Base64.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharacterFunctions.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharacterFunctions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharPointer_ASCII.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharPointer_UTF8.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharPointer_UTF16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_CharPointer_UTF32.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_Identifier.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_Identifier.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_LocalisedStrings.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_LocalisedStrings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_NewLine.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_String.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_String.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringArray.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringArray.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringPairArray.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringPairArray.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringPool.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringPool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_StringRef.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_TextDiff.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/text/juce_TextDiff.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ChildProcess.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ChildProcess.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_CriticalSection.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_DynamicLibrary.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_HighResolutionTimer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_HighResolutionTimer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_InterProcessLock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_Process.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ReadWriteLock.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ReadWriteLock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ScopedLock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ScopedReadLock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ScopedWriteLock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_SpinLock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_Thread.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_Thread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ThreadLocalValue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ThreadPool.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_ThreadPool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_TimeSliceThread.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_TimeSliceThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_WaitableEvent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_WaitableEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_PerformanceCounter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_PerformanceCounter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_RelativeTime.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_RelativeTime.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_Time.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_Time.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTest.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTest.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTestCategories.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/xml/juce_XmlDocument.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/xml/juce_XmlDocument.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/xml/juce_XmlElement.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/xml/juce_XmlElement.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/adler32.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/compress.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/crc32.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/crc32.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/deflate.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/deflate.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/infback.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inffast.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inffast.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inffixed.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inflate.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inflate.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inftrees.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/inftrees.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/trees.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/trees.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/uncompr.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zconf.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zconf.in.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zlib.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zutil.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/zlib/zutil.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/juce_GZIPCompressorOutputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/juce_GZIPCompressorOutputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/juce_GZIPDecompressorInputStream.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/juce_GZIPDecompressorInputStream.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/juce_ZipFile.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/zip/juce_ZipFile.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/juce_core.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/juce_core.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/juce_core.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/app_properties/juce_ApplicationProperties.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/app_properties/juce_ApplicationProperties.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/app_properties/juce_PropertiesFile.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/app_properties/juce_PropertiesFile.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/undomanager/juce_UndoableAction.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/undomanager/juce_UndoManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/undomanager/juce_UndoManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_CachedValue.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_CachedValue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_Value.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_Value.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueTree.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueTree.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueTreeSynchroniser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueTreeSynchroniser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueWithDefault.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/values/juce_ValueWithDefault.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ActionBroadcaster.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ActionBroadcaster.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ActionListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_AsyncUpdater.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_AsyncUpdater.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ChangeBroadcaster.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ChangeBroadcaster.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/broadcasters/juce_ChangeListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_ConnectedChildProcess.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_ConnectedChildProcess.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_InterprocessConnection.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_InterprocessConnection.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_InterprocessConnectionServer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_InterprocessConnectionServer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_NetworkServiceDiscovery.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/interprocess/juce_NetworkServiceDiscovery.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_ApplicationBase.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_ApplicationBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_CallbackMessage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_DeletedAtShutdown.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_DeletedAtShutdown.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_Initialisation.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_Message.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MessageListener.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MessageListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MessageManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MessageManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_MountedVolumeListChangeDetector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/messages/juce_NotificationType.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_android_Messaging.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_ios_MessageManager.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_linux_EventLoop.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_linux_Messaging.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_mac_MessageManager.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_osx_MessageQueue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_win32_HiddenMessageWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_win32_Messaging.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_win32_WinRTWrapper.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/native/juce_win32_WinRTWrapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/timers/juce_MultiTimer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/timers/juce_MultiTimer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/timers/juce_Timer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/timers/juce_Timer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/juce_events.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/juce_events.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/juce_events.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_Colour.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_Colour.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_ColourGradient.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_ColourGradient.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_Colours.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_Colours.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_FillType.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_FillType.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/colour/juce_PixelFormats.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_GraphicsContext.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_GraphicsContext.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsContext.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsPostScriptRenderer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsPostScriptRenderer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsSoftwareRenderer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/contexts/juce_LowLevelGraphicsSoftwareRenderer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_DropShadowEffect.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_DropShadowEffect.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_GlowEffect.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_GlowEffect.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/effects/juce_ImageEffectFilter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_AttributedString.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_AttributedString.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_CustomTypeface.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_CustomTypeface.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_Font.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_Font.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_GlyphArrangement.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_GlyphArrangement.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_TextLayout.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_TextLayout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_Typeface.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/fonts/juce_Typeface.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_AffineTransform.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_AffineTransform.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_BorderSize.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_EdgeTable.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_EdgeTable.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Line.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Parallelogram.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Path.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Path.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_PathIterator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_PathIterator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_PathStrokeType.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_PathStrokeType.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Point.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_Rectangle.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/geometry/juce_RectangleList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/cderror.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/changes to libjpeg for JUCE.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcapimin.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcapistd.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jccoefct.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jccolor.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcdctmgr.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jchuff.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jchuff.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcinit.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcmainct.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcmarker.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcmaster.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcomapi.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jconfig.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcparam.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcphuff.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcprepct.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jcsample.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jctrans.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdapimin.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdapistd.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdatasrc.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdcoefct.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdcolor.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdct.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jddctmgr.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdhuff.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdhuff.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdinput.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdmainct.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdmarker.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdmaster.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdmerge.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdphuff.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdpostct.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdsample.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jdtrans.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jerror.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jerror.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jfdctflt.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jfdctfst.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jfdctint.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jidctflt.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jidctfst.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jidctint.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jidctred.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jinclude.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jmemmgr.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jmemnobs.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jmemsys.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jmorecfg.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jpegint.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jpeglib.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jquant1.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jquant2.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jutils.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/jversion.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/transupp.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/jpglib/transupp.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/libpng_readme.txt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/png.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/png.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngconf.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngdebug.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngerror.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngget.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pnginfo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngmem.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngpread.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngpriv.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngread.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngrio.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngrtran.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngrutil.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngset.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngstruct.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngtrans.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngwio.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngwrite.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngwtran.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/pnglib/pngwutil.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/juce_GIFLoader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/juce_JPEGLoader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/image_formats/juce_PNGLoader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_Image.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_Image.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageCache.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageCache.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageConvolutionKernel.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageConvolutionKernel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageFileFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/images/juce_ImageFileFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_android_Fonts.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_android_GraphicsContext.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_android_IconHelpers.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_freetype_Fonts.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_linux_Fonts.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_linux_IconHelpers.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_CoreGraphicsContext.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_CoreGraphicsContext.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_CoreGraphicsHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_Fonts.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_mac_IconHelpers.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_RenderingHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_Direct2DGraphicsContext.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_Direct2DGraphicsContext.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_DirectWriteTypeface.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_DirectWriteTypeLayout.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_Fonts.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_IconHelpers.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_Justification.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_RectanglePlacement.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_RectanglePlacement.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/application/juce_Application.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/application/juce_Application.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ArrowButton.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ArrowButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_Button.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_Button.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_DrawableButton.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_DrawableButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_HyperlinkButton.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_HyperlinkButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ImageButton.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ImageButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ShapeButton.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ShapeButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_TextButton.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_TextButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ToggleButton.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ToggleButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ToolbarButton.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/buttons/juce_ToolbarButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandID.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandInfo.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandInfo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandTarget.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_ApplicationCommandTarget.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_KeyPressMappingSet.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/commands/juce_KeyPressMappingSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_CachedComponentImage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_Component.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_Component.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ComponentListener.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ComponentListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ModalComponentManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ModalComponentManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/desktop/juce_Desktop.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/desktop/juce_Desktop.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/desktop/juce_Displays.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/desktop/juce_Displays.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_Drawable.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_Drawable.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableComposite.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableComposite.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableImage.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableImage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawablePath.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawablePath.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableRectangle.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableRectangle.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableShape.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableShape.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableText.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_DrawableText.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/drawables/juce_SVGParser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_ContentSharer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_ContentSharer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsDisplayComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsDisplayComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsList.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_DirectoryContentsList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileBrowserComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileBrowserComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileBrowserListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileChooser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileChooser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileChooserDialogBox.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileChooserDialogBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileListComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileListComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FilenameComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FilenameComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FilePreviewComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileSearchPathListComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileSearchPathListComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileTreeComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_FileTreeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_ImagePreviewComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/filebrowser/juce_ImagePreviewComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_CaretComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_CaretComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyboardFocusTraverser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyboardFocusTraverser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyListener.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyPress.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_KeyPress.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_ModifierKeys.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_ModifierKeys.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_SystemClipboard.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_TextEditorKeyMapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/keyboard/juce_TextInputTarget.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_AnimatedPosition.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_AnimatedPositionBehaviours.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentAnimator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentAnimator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentBoundsConstrainer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentBoundsConstrainer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentBuilder.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentBuilder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentMovementWatcher.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ComponentMovementWatcher.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ConcertinaPanel.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ConcertinaPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_FlexBox.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_FlexBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_FlexItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_Grid.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_Grid.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_GridItem.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_GridItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_GroupComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_GroupComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_MultiDocumentPanel.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_MultiDocumentPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableBorderComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableBorderComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableCornerComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableCornerComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableEdgeComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ResizableEdgeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ScrollBar.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_ScrollBar.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_SidePanel.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_SidePanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableLayoutManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableLayoutManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableLayoutResizerBar.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableLayoutResizerBar.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableObjectResizer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_StretchableObjectResizer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_TabbedButtonBar.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_TabbedButtonBar.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_TabbedComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_TabbedComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_Viewport.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/layout/juce_Viewport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V1.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V1.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V2.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V2.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V3.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V3.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V4.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/lookandfeel/juce_LookAndFeel_V4.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_BurgerMenuComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_BurgerMenuComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_MenuBarComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_MenuBarComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_MenuBarModel.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_MenuBarModel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_PopupMenu.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/menus/juce_PopupMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_BubbleComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_BubbleComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_DropShadower.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_DropShadower.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_JUCESplashScreen.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/misc/juce_JUCESplashScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_ComponentDragger.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_ComponentDragger.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_DragAndDropContainer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_DragAndDropContainer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_DragAndDropTarget.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_FileDragAndDropTarget.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_LassoComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseCursor.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseCursor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseEvent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseInactivityDetector.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseInactivityDetector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseInputSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseInputSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseListener.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_MouseListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_SelectedItemSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_TextDragAndDropTarget.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/mouse/juce_TooltipClient.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_X11_DragAndDrop.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_X11_Symbols.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_X11_Symbols.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_XWindowSystem.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/x11/juce_linux_XWindowSystem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_ContentSharer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_FileChooser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_Windowing.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_common_MimeTypes.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_ContentSharer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_FileChooser.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_UIViewComponentPeer.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_Windowing.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_FileChooser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_Windowing.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_FileChooser.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_MainMenu.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_MouseCursor.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_NSViewComponentPeer.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_Windowing.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_MultiTouchMapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_DragAndDrop.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_FileChooser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_ScopedThreadDPIAwarenessSetter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_Windowing.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_MarkerList.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_MarkerList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeCoordinate.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeCoordinate.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeCoordinatePositioner.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeCoordinatePositioner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeParallelogram.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeParallelogram.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativePoint.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativePoint.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativePointPath.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativePointPath.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeRectangle.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/positioning/juce_RelativeRectangle.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_BooleanPropertyComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_BooleanPropertyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_ButtonPropertyComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_ButtonPropertyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_ChoicePropertyComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_ChoicePropertyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_MultiChoicePropertyComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_MultiChoicePropertyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_PropertyComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_PropertyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_PropertyPanel.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_PropertyPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_SliderPropertyComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_SliderPropertyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_TextPropertyComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/properties/juce_TextPropertyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ComboBox.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ComboBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ImageComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ImageComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Label.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Label.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ListBox.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ListBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ProgressBar.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ProgressBar.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Slider.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Slider.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TableHeaderComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TableHeaderComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TableListBox.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TableListBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TextEditor.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TextEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Toolbar.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_Toolbar.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemFactory.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemPalette.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_ToolbarItemPalette.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TreeView.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/widgets/juce_TreeView.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_AlertWindow.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_AlertWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_CallOutBox.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_CallOutBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ComponentPeer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ComponentPeer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_DialogWindow.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_DialogWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_DocumentWindow.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_DocumentWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_NativeMessageBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ResizableWindow.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ResizableWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ThreadWithProgressWindow.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_ThreadWithProgressWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_TooltipWindow.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_TooltipWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_TopLevelWindow.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/windows/juce_TopLevelWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeDocument.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeDocument.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeEditorComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeEditorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CodeTokeniser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CPlusPlusCodeTokeniser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CPlusPlusCodeTokeniser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_CPlusPlusCodeTokeniserFunctions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_LuaCodeTokeniser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_LuaCodeTokeniser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_XMLCodeTokeniser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/code_editor/juce_XMLCodeTokeniser.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/documents/juce_FileBasedDocument.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/documents/juce_FileBasedDocument.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_ActiveXControlComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_AndroidViewComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_HWNDComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_NSViewComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_ScopedDPIAwarenessDisabler.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_UIViewComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_XEmbedComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_AnimatedAppComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_AnimatedAppComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_AppleRemote.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_BubbleMessageComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_BubbleMessageComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_ColourSelector.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_ColourSelector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_KeyMappingEditorComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_KeyMappingEditorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_LiveConstantEditor.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_LiveConstantEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_PreferencesPanel.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_PreferencesPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_PushNotifications.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_PushNotifications.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SplashScreen.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SplashScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SystemTrayIconComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SystemTrayIconComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_WebBrowserComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_android_PushNotifications.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_android_WebBrowserComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_AndroidViewComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_ios_PushNotifications.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_ios_UIViewComponent.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_X11_SystemTrayIcon.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_X11_WebBrowserComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_XEmbedComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_AppleRemote.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_CarbonViewWrapperComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_NSViewComponent.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_PushNotifications.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_SystemTrayIcon.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_WebBrowserComponent.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_ActiveXComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_HWNDComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_SystemTrayIcon.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_WebBrowserComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/geometry/juce_Draggable3DOrientation.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/geometry/juce_Matrix3D.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/geometry/juce_Quaternion.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/geometry/juce_Vector3D.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_MissingGLDefinitions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_android.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_ios.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_linux_X11.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_osx.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGL_win32.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/native/juce_OpenGLExtensions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLContext.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLContext.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLFrameBuffer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLFrameBuffer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLGraphicsContext.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLGraphicsContext.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLHelpers.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLImage.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLImage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLPixelFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLPixelFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLRenderer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLShaderProgram.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLShaderProgram.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLTexture.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/opengl/juce_OpenGLTexture.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/utils/juce_OpenGLAppComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/utils/juce_OpenGLAppComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/AppConfig.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/BinaryData.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/JuceHeader.h" PROPERTIES HEADER_FILE_ONLY TRUE)

target_compile_options( ${BINARY_NAME} PRIVATE "-fsigned-char" "-Wno-unused-command-line-argument" )

if( JUCE_BUILD_CONFIGURATION MATCHES "DEBUG" )
    target_compile_options( ${BINARY_NAME} PRIVATE)
endif()

if( JUCE_BUILD_CONFIGURATION MATCHES "RELEASE" )
    target_compile_options( ${BINARY_NAME} PRIVATE)
endif()

find_library(log "log")
find_library(android "android")
find_library(glesv3 "GLESv3")
find_library(egl "EGL")

target_link_libraries( ${BINARY_NAME}

    ${log}
    ${android}
    ${glesv3}
    ${egl}
    "cpufeatures"
)
