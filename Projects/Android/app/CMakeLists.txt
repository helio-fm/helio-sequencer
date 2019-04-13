# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

cmake_minimum_required(VERSION 3.4.1)

SET(BINARY_NAME "juce_jni")

add_library("cpufeatures" STATIC "${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c")
set_source_files_properties("${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c" PROPERTIES COMPILE_FLAGS "-Wno-sign-conversion -Wno-gnu-statement-expression")

add_definitions("-DJUCE_ANDROID=1" "-DJUCE_ANDROID_API_VERSION=21" "-DJUCE_PUSH_NOTIFICATIONS=1" "-DJUCE_ANDROID_GL_ES_VERSION_3_0=1" "-DJUCE_DONT_DECLARE_PROJECTINFO=1" "-DJUCER_ANDROIDSTUDIO_6AB8437A=1" "-DJUCE_APP_VERSION=2.0" "-DJUCE_APP_VERSION_HEX=0x20000")

include_directories( AFTER
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
    "../../Projucer/JuceLibraryCode"
    "../../../ThirdParty/JUCE/modules"
    "${ANDROID_NDK}/sources/android/cpufeatures"
)

enable_language(ASM)

IF(JUCE_BUILD_CONFIGURATION MATCHES "DEBUG")
    add_definitions("-DDEBUG=1" "-D_DEBUG=1")
    include_directories( AFTER
        "../../Projucer/JuceLibraryCode"
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
        "../../../Source/Core/Midi"
        "../../../Source/Core/Midi/Patterns"
        "../../../Source/Core/Midi/Sequences"
        "../../../Source/Core/Midi/Sequences/Events"
        "../../../Source/Core/Network"
        "../../../Source/Core/Network/Models"
        "../../../Source/Core/Network/Requests"
        "../../../Source/Core/Network/Services"
        "../../../Source/Core/Scripting"
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

ELSEIF(JUCE_BUILD_CONFIGURATION MATCHES "RELEASE")
    add_definitions("-DNDEBUG=1")
    include_directories( AFTER
        "../../Projucer/JuceLibraryCode"
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
        "../../../Source/Core/Midi"
        "../../../Source/Core/Midi/Patterns"
        "../../../Source/Core/Midi/Sequences"
        "../../../Source/Core/Midi/Sequences/Events"
        "../../../Source/Core/Network"
        "../../../Source/Core/Network/Models"
        "../../../Source/Core/Network/Requests"
        "../../../Source/Core/Network/Services"
        "../../../Source/Core/Scripting"
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

ELSE(JUCE_BUILD_CONFIGURATION MATCHES "DEBUG")
    MESSAGE( FATAL_ERROR "No matching build-configuration found." )
ENDIF(JUCE_BUILD_CONFIGURATION MATCHES "DEBUG")

add_library( ${BINARY_NAME}

    SHARED

    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthAudioPlugin.cpp"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthAudioPlugin.h"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthFormat.cpp"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthFormat.h"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthPiano.cpp"
    "../../../Source/Core/Audio/BuiltIn/BuiltInSynthPiano.h"
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
    "../../../Source/Core/Audio/Transport/PlayerThread.cpp"
    "../../../Source/Core/Audio/Transport/PlayerThread.h"
    "../../../Source/Core/Audio/Transport/PlayerThreadPool.h"
    "../../../Source/Core/Audio/Transport/ProjectSequencesWrapper.h"
    "../../../Source/Core/Audio/Transport/RendererThread.cpp"
    "../../../Source/Core/Audio/Transport/RendererThread.h"
    "../../../Source/Core/Audio/Transport/Transport.cpp"
    "../../../Source/Core/Audio/Transport/Transport.h"
    "../../../Source/Core/Audio/Transport/TransportListener.h"
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
    "../../../Source/Core/Configuration/Models/Script.cpp"
    "../../../Source/Core/Configuration/Models/Script.h"
    "../../../Source/Core/Configuration/Models/Translation.cpp"
    "../../../Source/Core/Configuration/Models/Translation.h"
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
    "../../../Source/Core/Configuration/ResourceManagers/ScriptsManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/ScriptsManager.h"
    "../../../Source/Core/Configuration/ResourceManagers/TranslationsManager.cpp"
    "../../../Source/Core/Configuration/ResourceManagers/TranslationsManager.h"
    "../../../Source/Core/Configuration/Config.cpp"
    "../../../Source/Core/Configuration/Config.h"
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
    "../../../Source/Core/Serialization/ResourceCache.h"
    "../../../Source/Core/Serialization/Serializable.h"
    "../../../Source/Core/Serialization/SerializationKeys.h"
    "../../../Source/Core/Serialization/Serializer.h"
    "../../../Source/Core/Serialization/BinarySerializer.cpp"
    "../../../Source/Core/Serialization/BinarySerializer.h"
    "../../../Source/Core/Serialization/JsonSerializer.cpp"
    "../../../Source/Core/Serialization/JsonSerializer.h"
    "../../../Source/Core/Serialization/LegacySerializer.cpp"
    "../../../Source/Core/Serialization/LegacySerializer.h"
    "../../../Source/Core/Serialization/XmlSerializer.cpp"
    "../../../Source/Core/Serialization/XmlSerializer.h"
    "../../../Source/Core/Tree/AudioPluginNode.cpp"
    "../../../Source/Core/Tree/AudioPluginNode.h"
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
    "../../../Source/Core/Tree/ProjectInfo.cpp"
    "../../../Source/Core/Tree/ProjectInfo.h"
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
    "../../../Source/Core/Undo/Actions/TimeSignatureEventActions.cpp"
    "../../../Source/Core/Undo/Actions/TimeSignatureEventActions.h"
    "../../../Source/Core/Undo/Actions/UndoAction.h"
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
    "../../../Source/UI/Common/AudioMonitors/GenericAudioMonitorComponent.cpp"
    "../../../Source/UI/Common/AudioMonitors/GenericAudioMonitorComponent.h"
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
    "../../../Source/UI/Common/MenuButton.cpp"
    "../../../Source/UI/Common/MenuButton.h"
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
    "../../../Source/UI/Common/ViewportFitProxyComponent.cpp"
    "../../../Source/UI/Common/ViewportFitProxyComponent.h"
    "../../../Source/UI/Dialogs/AnnotationDialog.cpp"
    "../../../Source/UI/Dialogs/AnnotationDialog.h"
    "../../../Source/UI/Dialogs/FadingDialog.cpp"
    "../../../Source/UI/Dialogs/FadingDialog.h"
    "../../../Source/UI/Dialogs/KeySignatureDialog.cpp"
    "../../../Source/UI/Dialogs/KeySignatureDialog.h"
    "../../../Source/UI/Dialogs/ModalDialogConfirmation.cpp"
    "../../../Source/UI/Dialogs/ModalDialogConfirmation.h"
    "../../../Source/UI/Dialogs/ModalDialogInput.cpp"
    "../../../Source/UI/Dialogs/ModalDialogInput.h"
    "../../../Source/UI/Dialogs/RenderDialog.cpp"
    "../../../Source/UI/Dialogs/RenderDialog.h"
    "../../../Source/UI/Dialogs/TimeSignatureDialog.cpp"
    "../../../Source/UI/Dialogs/TimeSignatureDialog.h"
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
    "../../../Source/UI/Menus/Base/MenuItemComponentMarker.cpp"
    "../../../Source/UI/Menus/Base/MenuItemComponentMarker.h"
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
    "../../../Source/UI/Menus/AnnotationMenu.cpp"
    "../../../Source/UI/Menus/AnnotationMenu.h"
    "../../../Source/UI/Menus/InstrumentMenu.cpp"
    "../../../Source/UI/Menus/InstrumentMenu.h"
    "../../../Source/UI/Menus/MidiTrackMenu.cpp"
    "../../../Source/UI/Menus/MidiTrackMenu.h"
    "../../../Source/UI/Menus/NotesTuningPanel.cpp"
    "../../../Source/UI/Menus/NotesTuningPanel.h"
    "../../../Source/UI/Menus/OrchestraPitMenu.cpp"
    "../../../Source/UI/Menus/OrchestraPitMenu.h"
    "../../../Source/UI/Menus/PatternsMenu.cpp"
    "../../../Source/UI/Menus/PatternsMenu.h"
    "../../../Source/UI/Menus/ProjectMenu.cpp"
    "../../../Source/UI/Menus/ProjectMenu.h"
    "../../../Source/UI/Menus/TimelineMenu.cpp"
    "../../../Source/UI/Menus/TimelineMenu.h"
    "../../../Source/UI/Menus/TimeSignatureMenu.h"
    "../../../Source/UI/Menus/TimeSignatureMenu.cpp"
    "../../../Source/UI/Menus/VersionControlMenu.cpp"
    "../../../Source/UI/Menus/VersionControlMenu.h"
    "../../../Source/UI/Menus/WorkspaceMenu.cpp"
    "../../../Source/UI/Menus/WorkspaceMenu.h"
    "../../../Source/UI/Pages/Dashboard/Menu/DashboardMenu.cpp"
    "../../../Source/UI/Pages/Dashboard/Menu/DashboardMenu.h"
    "../../../Source/UI/Pages/Dashboard/Menu/RecentProjectRow.cpp"
    "../../../Source/UI/Pages/Dashboard/Menu/RecentProjectRow.h"
    "../../../Source/UI/Pages/Dashboard/Menu/LoginButton.cpp"
    "../../../Source/UI/Pages/Dashboard/Menu/LoginButton.h"
    "../../../Source/UI/Pages/Dashboard/Menu/CreateProjectButton.cpp"
    "../../../Source/UI/Pages/Dashboard/Menu/CreateProjectButton.h"
    "../../../Source/UI/Pages/Dashboard/Menu/OpenProjectButton.cpp"
    "../../../Source/UI/Pages/Dashboard/Menu/OpenProjectButton.h"
    "../../../Source/UI/Pages/Dashboard/Dashboard.cpp"
    "../../../Source/UI/Pages/Dashboard/Dashboard.h"
    "../../../Source/UI/Pages/Dashboard/InitScreen.cpp"
    "../../../Source/UI/Pages/Dashboard/InitScreen.h"
    "../../../Source/UI/Pages/Dashboard/LogoFader.cpp"
    "../../../Source/UI/Pages/Dashboard/LogoFader.h"
    "../../../Source/UI/Pages/Dashboard/UpdatesInfoComponent.cpp"
    "../../../Source/UI/Pages/Dashboard/UpdatesInfoComponent.h"
    "../../../Source/UI/Pages/Dashboard/UserProfileComponent.cpp"
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
    "../../../Source/UI/Pages/Settings/LabeledSettingsWrapper.cpp"
    "../../../Source/UI/Pages/Settings/LabeledSettingsWrapper.h"
    "../../../Source/UI/Pages/Settings/SimpleSettingsWrapper.cpp"
    "../../../Source/UI/Pages/Settings/SimpleSettingsWrapper.h"
    "../../../Source/UI/Pages/Settings/SettingsListItemHighlighter.cpp"
    "../../../Source/UI/Pages/Settings/SettingsListItemHighlighter.h"
    "../../../Source/UI/Pages/Settings/SettingsListItemSelection.cpp"
    "../../../Source/UI/Pages/Settings/SettingsListItemSelection.h"
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
    "../../../Source/UI/Popups/ArpPreviewTool.cpp"
    "../../../Source/UI/Popups/ArpPreviewTool.h"
    "../../../Source/UI/Popups/ChordPreviewTool.cpp"
    "../../../Source/UI/Popups/ChordPreviewTool.h"
    "../../../Source/UI/Popups/ScalePreviewTool.cpp"
    "../../../Source/UI/Popups/ScalePreviewTool.h"
    "../../../Source/UI/Popups/RescalePreviewTool.cpp"
    "../../../Source/UI/Popups/RescalePreviewTool.h"
    "../../../Source/UI/Popups/CenteredTooltipComponent.h"
    "../../../Source/UI/Popups/FailTooltip.cpp"
    "../../../Source/UI/Popups/FailTooltip.h"
    "../../../Source/UI/Popups/GenericTooltip.h"
    "../../../Source/UI/Popups/HelioCallout.cpp"
    "../../../Source/UI/Popups/HelioCallout.h"
    "../../../Source/UI/Popups/PopupButton.cpp"
    "../../../Source/UI/Popups/PopupButton.h"
    "../../../Source/UI/Popups/PopupButtonOwner.h"
    "../../../Source/UI/Popups/PopupCustomButton.cpp"
    "../../../Source/UI/Popups/PopupCustomButton.h"
    "../../../Source/UI/Popups/PopupMenuComponent.h"
    "../../../Source/UI/Popups/ProgressIndicator.h"
    "../../../Source/UI/Popups/ProgressTooltip.cpp"
    "../../../Source/UI/Popups/ProgressTooltip.h"
    "../../../Source/UI/Popups/SuccessTooltip.cpp"
    "../../../Source/UI/Popups/SuccessTooltip.h"
    "../../../Source/UI/Popups/TooltipContainer.cpp"
    "../../../Source/UI/Popups/TooltipContainer.h"
    "../../../Source/UI/Sequencer/Header/ClipRangeIndicator.cpp"
    "../../../Source/UI/Sequencer/Header/ClipRangeIndicator.h"
    "../../../Source/UI/Sequencer/Header/HeaderSelectionIndicator.cpp"
    "../../../Source/UI/Sequencer/Header/HeaderSelectionIndicator.h"
    "../../../Source/UI/Sequencer/Header/HybridRollHeader.cpp"
    "../../../Source/UI/Sequencer/Header/HybridRollHeader.h"
    "../../../Source/UI/Sequencer/Header/Playhead.cpp"
    "../../../Source/UI/Sequencer/Header/Playhead.h"
    "../../../Source/UI/Sequencer/Header/SoundProbeIndicator.cpp"
    "../../../Source/UI/Sequencer/Header/SoundProbeIndicator.h"
    "../../../Source/UI/Sequencer/Header/TimeDistanceIndicator.cpp"
    "../../../Source/UI/Sequencer/Header/TimeDistanceIndicator.h"
    "../../../Source/UI/Sequencer/Header/TrackEndIndicator.cpp"
    "../../../Source/UI/Sequencer/Header/TrackEndIndicator.h"
    "../../../Source/UI/Sequencer/Header/TrackStartIndicator.cpp"
    "../../../Source/UI/Sequencer/Header/TrackStartIndicator.h"
    "../../../Source/UI/Sequencer/Helpers/CutPointMark.cpp"
    "../../../Source/UI/Sequencer/Helpers/CutPointMark.h"
    "../../../Source/UI/Sequencer/Helpers/HybridRollExpandMark.cpp"
    "../../../Source/UI/Sequencer/Helpers/HybridRollExpandMark.h"
    "../../../Source/UI/Sequencer/Helpers/KnifeToolHelper.cpp"
    "../../../Source/UI/Sequencer/Helpers/KnifeToolHelper.h"
    "../../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.cpp"
    "../../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.h"
    "../../../Source/UI/Sequencer/Helpers/PatternOperations.cpp"
    "../../../Source/UI/Sequencer/Helpers/PatternOperations.h"
    "../../../Source/UI/Sequencer/Helpers/SequencerOperations.cpp"
    "../../../Source/UI/Sequencer/Helpers/SequencerOperations.h"
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
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip/PianoSequenceMapNoteComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/ClipComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/ClipComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/DummyClipComponent.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/ClipComponents/DummyClipComponent.h"
    "../../../Source/UI/Sequencer/PatternRoll/PatternRoll.cpp"
    "../../../Source/UI/Sequencer/PatternRoll/PatternRoll.h"
    "../../../Source/UI/Sequencer/PianoRoll/NoteComponent.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/NoteComponent.h"
    "../../../Source/UI/Sequencer/PianoRoll/NoteResizerLeft.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/NoteResizerRight.cpp"
    "../../../Source/UI/Sequencer/PianoRoll/NoteResizerLeft.h"
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
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/PianoProjectMap.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/PianoProjectMap.h"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapNoteComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/TrackScroller.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/TrackScroller.h"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/TrackScrollerScreen.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/PianoMap/TrackScrollerScreen.h"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureLargeComponent.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureLargeComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureSmallComponent.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureSmallComponent.h"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignaturesProjectMap.cpp"
    "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignaturesProjectMap.h"
    "../../../Source/UI/Sequencer/HybridRoll.cpp"
    "../../../Source/UI/Sequencer/HybridRoll.h"
    "../../../Source/UI/Sequencer/HybridRollEditMode.cpp"
    "../../../Source/UI/Sequencer/HybridRollEditMode.h"
    "../../../Source/UI/Sequencer/HybridRollListener.h"
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
    "../../../Source/UI/Themes/ComponentFader.cpp"
    "../../../Source/UI/Themes/ComponentFader.h"
    "../../../Source/UI/Themes/DialogBackground.h"
    "../../../Source/UI/Themes/HelioTheme.cpp"
    "../../../Source/UI/Themes/HelioTheme.h"
    "../../../Source/UI/Themes/Icons.cpp"
    "../../../Source/UI/Themes/Icons.h"
    "../../../Source/UI/Themes/FramePanel.cpp"
    "../../../Source/UI/Themes/FramePanel.h"
    "../../../Source/UI/Themes/DialogPanel.cpp"
    "../../../Source/UI/Themes/DialogPanel.h"
    "../../../Source/UI/Themes/PanelBackgroundA.cpp"
    "../../../Source/UI/Themes/PanelBackgroundA.h"
    "../../../Source/UI/Themes/PanelBackgroundB.cpp"
    "../../../Source/UI/Themes/PanelBackgroundB.h"
    "../../../Source/UI/Themes/PanelBackgroundC.cpp"
    "../../../Source/UI/Themes/PanelBackgroundC.h"
    "../../../Source/UI/Themes/SeparatorHorizontal.cpp"
    "../../../Source/UI/Themes/SeparatorHorizontal.h"
    "../../../Source/UI/Themes/SeparatorHorizontalReversed.cpp"
    "../../../Source/UI/Themes/SeparatorHorizontalReversed.h"
    "../../../Source/UI/Themes/SeparatorHorizontalFading.cpp"
    "../../../Source/UI/Themes/SeparatorHorizontalFading.h"
    "../../../Source/UI/Themes/SeparatorHorizontalFadingReversed.cpp"
    "../../../Source/UI/Themes/SeparatorHorizontalFadingReversed.h"
    "../../../Source/UI/Themes/SeparatorVertical.cpp"
    "../../../Source/UI/Themes/SeparatorVertical.h"
    "../../../Source/UI/Themes/SeparatorVerticalSkew.cpp"
    "../../../Source/UI/Themes/SeparatorVerticalSkew.h"
    "../../../Source/UI/Themes/ShadowComponent.h"
    "../../../Source/UI/Themes/ShadowDownwards.cpp"
    "../../../Source/UI/Themes/ShadowDownwards.h"
    "../../../Source/UI/Themes/ShadowUpwards.cpp"
    "../../../Source/UI/Themes/ShadowUpwards.h"
    "../../../Source/UI/Themes/ShadowLeftwards.cpp"
    "../../../Source/UI/Themes/ShadowLeftwards.h"
    "../../../Source/UI/Themes/ShadowRightwards.cpp"
    "../../../Source/UI/Themes/ShadowRightwards.h"
    "../../../Source/UI/Themes/ViewportKineticSlider.cpp"
    "../../../Source/UI/Themes/ViewportKineticSlider.h"
    "../../../Source/UI/MainLayout.cpp"
    "../../../Source/UI/MainLayout.h"
    "../../../Source/Common.cpp"
    "../../../Source/Common.h"
    "../../Projucer/JuceLibraryCode/AppConfig.h"
    "../../Projucer/JuceLibraryCode/BinaryData.cpp"
    "../../Projucer/JuceLibraryCode/BinaryData.h"
    "../../Projucer/JuceLibraryCode/BinaryData2.cpp"
    "../../Projucer/JuceLibraryCode/BinaryData3.cpp"
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
    "../../Projucer/JuceLibraryCode/UnityBuild.cpp"
)

set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynthAudioPlugin.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynthFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynthPiano.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/InternalPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/Instrument.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/OrchestraListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/OrchestraPit.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/PluginScanner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/SerializablePluginDescription.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Monitoring/AudioMonitor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/PlayerThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/PlayerThreadPool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/ProjectSequencesWrapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/RendererThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/Transport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/TransportListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/AudioCore.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/BaseResource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Arpeggiator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Chord.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/ColourScheme.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/HotkeyScheme.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Scale.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Script.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Models/Translation.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ResourceManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ArpeggiatorsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ChordsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ColourSchemesManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/HotkeySchemesManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ScalesManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/ScriptsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/ResourceManagers/TranslationsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Configuration/Config.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../Source/Core/Serialization/ResourceCache.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Serializable.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/SerializationKeys.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Serializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/BinarySerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/JsonSerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/LegacySerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/XmlSerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/AudioPluginNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/AutomationTrackNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/InstrumentNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/MidiTrackNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/MidiTrackSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/OrchestraPitNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/PatternEditorNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/PianoTrackNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectInfo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../Source/Core/Undo/Actions/TimeSignatureEventActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/UndoAction.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../Source/UI/Common/AudioMonitors/GenericAudioMonitorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../Source/UI/Common/MenuButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/MobileComboBox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ModeIndicatorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/OverlayButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/PlayButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/PluginWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/RadioButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ScaleEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/SpectralLogo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ViewportFitProxyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/AnnotationDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/FadingDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/KeySignatureDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/ModalDialogConfirmation.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/ModalDialogInput.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/RenderDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/TimeSignatureDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/Headline.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineDropdown.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineItemArrow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineItemHighlighter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineItemDataSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Headline/HeadlineNavigationPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/LongTapController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/LongTapListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/MultiTouchController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/MultiTouchListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothPanController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothPanListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothZoomController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothZoomListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/Base/MenuItemComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/Base/MenuItemComponentMarker.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/Base/MenuPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/AudioPluginSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/InstrumentNodeSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/PatternRollSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/PianoRollSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/VersionControlHistorySelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/SelectionMenus/VersionControlStageSelectionMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/AnnotationMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/InstrumentMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/MidiTrackMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/NotesTuningPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/OrchestraPitMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/PatternsMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/ProjectMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/TimelineMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/TimeSignatureMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/VersionControlMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Menus/WorkspaceMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/DashboardMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/RecentProjectRow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/LoginButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/CreateProjectButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Menu/OpenProjectButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/Dashboard.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/InitScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/LogoFader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/UpdatesInfoComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Dashboard/UserProfileComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/AudioPluginEditorPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/InstrumentEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/InstrumentEditorConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/InstrumentComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/Editor/InstrumentEditorPin.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/AudioPluginsListComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/InstrumentsListComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Instruments/OrchestraPitPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Project/ProjectPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/AudioSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/ComponentsList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/LabeledSettingsWrapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/SimpleSettingsWrapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/SettingsListItemHighlighter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Pages/Settings/SettingsListItemSelection.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../Source/UI/Popups/ArpPreviewTool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ChordPreviewTool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ScalePreviewTool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/RescalePreviewTool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/CenteredTooltipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/FailTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/GenericTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/HelioCallout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupButtonOwner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupCustomButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupMenuComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ProgressIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ProgressTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/SuccessTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/TooltipContainer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/ClipRangeIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/HeaderSelectionIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/HybridRollHeader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/Playhead.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/SoundProbeIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/TimeDistanceIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/TrackEndIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Header/TrackStartIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/CutPointMark.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/HybridRollExpandMark.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/KnifeToolHelper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/PatternOperations.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/Helpers/SequencerOperations.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveHelper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventsConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepsClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventsConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip/PianoClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip/PianoSequenceMapNoteComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/ClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/ClipComponents/DummyClipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PatternRoll/PatternRoll.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/PianoRoll/NoteComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/PianoMap/PianoProjectMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapNoteComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/PianoMap/TrackScroller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/PianoMap/TrackScrollerScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureLargeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureSmallComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignaturesProjectMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/HybridRoll.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/HybridRollEditMode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Sequencer/HybridRollListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../Source/UI/Themes/DialogPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../Projucer/JuceLibraryCode/AppConfig.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/BinaryData.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/JuceHeader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/UnityBuild.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)

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
