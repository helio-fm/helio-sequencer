# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

cmake_minimum_required(VERSION 3.4.1)

SET(BINARY_NAME "juce_jni")

add_definitions("-DJUCE_ANDROID=1" "-DJUCE_ANDROID_API_VERSION=11" "-DJUCE_ANDROID_ACTIVITY_CLASSNAME=com_peterrudenko_Helio" "-DJUCE_ANDROID_ACTIVITY_CLASSPATH=\"com/peterrudenko/Helio\"" "-DJUCE_DONT_DECLARE_PROJECTINFO=1")

include_directories( AFTER
    "../../../ThirdParty/VST_SDK/VST3_SDK"
    "../../Projucer/JuceLibraryCode"
    "../../../ThirdParty/JUCE/modules"
)

IF(JUCE_BUILD_CONFIGFURATION MATCHES "DEBUG")
    add_definitions("-DDEBUG=1" "-D_DEBUG=1")
    include_directories( AFTER
        "../../../ThirdParty/JUCE/modules/juce_audio_basics"
        "../../../ThirdParty/JUCE/modules/juce_audio_devices"
        "../../../ThirdParty/JUCE/modules/juce_audio_formats"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors"
        "../../../ThirdParty/JUCE/modules/juce_audio_utils"
        "../../../ThirdParty/JUCE/modules/juce_core"
        "../../../ThirdParty/JUCE/modules/juce_cryptography"
        "../../../ThirdParty/JUCE/modules/juce_data_structures"
        "../../../ThirdParty/JUCE/modules/juce_events"
        "../../../ThirdParty/JUCE/modules/juce_graphics"
        "../../../ThirdParty/JUCE/modules/juce_gui_basics"
        "../../../ThirdParty/JUCE/modules/juce_gui_extra"
        "../../../ThirdParty/JUCE/modules/juce_opengl"
        "../../../ThirdParty/JUCE/modules/juce_osc"
        "../../../Source"
        "../../../Source/Core"
        "../../../Source/Core/App"
        "../../../Source/Core/Audio"
        "../../../Source/Core/Audio/BuiltIn"
        "../../../Source/Core/Audio/Instruments"
        "../../../Source/Core/Audio/Monitoring"
        "../../../Source/Core/Audio/Transport"
        "../../../Source/Core/Audio/Waveform"
        "../../../Source/Core/Clipboard"
        "../../../Source/Core/Events"
        "../../../Source/Core/Layers"
        "../../../Source/Core/Network"
        "../../../Source/Core/Serialization"
        "../../../Source/Core/Supervisor"
        "../../../Source/Core/Tools"
        "../../../Source/Core/Translation"
        "../../../Source/Core/Tree"
        "../../../Source/Core/Tutorial"
        "../../../Source/Core/Undo"
        "../../../Source/Core/Undo/Actions"
        "../../../Source/Core/VCS"
        "../../../Source/Core/VCS/DiffLogic"
        "../../../Source/Core/VCS/Network"
        "../../../Source/UI"
        "../../../Source/UI/CodeEditor"
        "../../../Source/UI/CommandPanels"
        "../../../Source/UI/CommandPanels/Base"
        "../../../Source/UI/Common"
        "../../../Source/UI/Common/Meters"
        "../../../Source/UI/Common/Origami"
        "../../../Source/UI/Console"
        "../../../Source/UI/Dialogs"
        "../../../Source/UI/Input"
        "../../../Source/UI/InstrumentsPage"
        "../../../Source/UI/InstrumentsPage/Editor"
        "../../../Source/UI/Intro"
        "../../../Source/UI/MidiEditor"
        "../../../Source/UI/MidiEditor/AnnotationsMap"
        "../../../Source/UI/MidiEditor/AutomationMap"
        "../../../Source/UI/MidiEditor/Header"
        "../../../Source/UI/MidiEditor/Helpers"
        "../../../Source/UI/MidiEditor/TimeSignaturesMap"
        "../../../Source/UI/MidiEditor/TrackMap"
        "../../../Source/UI/MidiEditor/TriggersMap"
        "../../../Source/UI/MidiEditor/WaveformMap"
        "../../../Source/UI/Popups"
        "../../../Source/UI/ProjectPage"
        "../../../Source/UI/Rollovers"
        "../../../Source/UI/SettingsPage"
        "../../../Source/UI/Themes"
        "../../../Source/UI/Tree"
        "../../../Source/UI/VCSPage"
        "../../../Source/UI/WorkspacePage"
        "../../../Source/UI/WorkspacePage/Menu"
        "../../../Source/UI/WorkspacePage/Scroller"
    )

ELSEIF(JUCE_BUILD_CONFIGFURATION MATCHES "RELEASE")
    add_definitions("-DNDEBUG=1")
    include_directories( AFTER
        "../../../ThirdParty/JUCE/modules/juce_audio_basics"
        "../../../ThirdParty/JUCE/modules/juce_audio_devices"
        "../../../ThirdParty/JUCE/modules/juce_audio_formats"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors"
        "../../../ThirdParty/JUCE/modules/juce_audio_utils"
        "../../../ThirdParty/JUCE/modules/juce_core"
        "../../../ThirdParty/JUCE/modules/juce_cryptography"
        "../../../ThirdParty/JUCE/modules/juce_data_structures"
        "../../../ThirdParty/JUCE/modules/juce_events"
        "../../../ThirdParty/JUCE/modules/juce_graphics"
        "../../../ThirdParty/JUCE/modules/juce_gui_basics"
        "../../../ThirdParty/JUCE/modules/juce_gui_extra"
        "../../../ThirdParty/JUCE/modules/juce_opengl"
        "../../../ThirdParty/JUCE/modules/juce_osc"
        "../../../Source"
        "../../../Source/Core"
        "../../../Source/Core/App"
        "../../../Source/Core/Audio"
        "../../../Source/Core/Audio/BuiltIn"
        "../../../Source/Core/Audio/Instruments"
        "../../../Source/Core/Audio/Monitoring"
        "../../../Source/Core/Audio/Transport"
        "../../../Source/Core/Audio/Waveform"
        "../../../Source/Core/Clipboard"
        "../../../Source/Core/Events"
        "../../../Source/Core/Layers"
        "../../../Source/Core/Network"
        "../../../Source/Core/Serialization"
        "../../../Source/Core/Supervisor"
        "../../../Source/Core/Tools"
        "../../../Source/Core/Translation"
        "../../../Source/Core/Tree"
        "../../../Source/Core/Tutorial"
        "../../../Source/Core/Undo"
        "../../../Source/Core/Undo/Actions"
        "../../../Source/Core/VCS"
        "../../../Source/Core/VCS/DiffLogic"
        "../../../Source/Core/VCS/Network"
        "../../../Source/UI"
        "../../../Source/UI/CodeEditor"
        "../../../Source/UI/CommandPanels"
        "../../../Source/UI/CommandPanels/Base"
        "../../../Source/UI/Common"
        "../../../Source/UI/Common/Meters"
        "../../../Source/UI/Common/Origami"
        "../../../Source/UI/Console"
        "../../../Source/UI/Dialogs"
        "../../../Source/UI/Input"
        "../../../Source/UI/InstrumentsPage"
        "../../../Source/UI/InstrumentsPage/Editor"
        "../../../Source/UI/Intro"
        "../../../Source/UI/MidiEditor"
        "../../../Source/UI/MidiEditor/AnnotationsMap"
        "../../../Source/UI/MidiEditor/AutomationMap"
        "../../../Source/UI/MidiEditor/Header"
        "../../../Source/UI/MidiEditor/Helpers"
        "../../../Source/UI/MidiEditor/TimeSignaturesMap"
        "../../../Source/UI/MidiEditor/TrackMap"
        "../../../Source/UI/MidiEditor/TriggersMap"
        "../../../Source/UI/MidiEditor/WaveformMap"
        "../../../Source/UI/Popups"
        "../../../Source/UI/ProjectPage"
        "../../../Source/UI/Rollovers"
        "../../../Source/UI/SettingsPage"
        "../../../Source/UI/Themes"
        "../../../Source/UI/Tree"
        "../../../Source/UI/VCSPage"
        "../../../Source/UI/WorkspacePage"
        "../../../Source/UI/WorkspacePage/Menu"
        "../../../Source/UI/WorkspacePage/Scroller"
    )

ELSE(JUCE_BUILD_CONFIGFURATION MATCHES "DEBUG")
    MESSAGE( FATAL_ERROR "No matching build-configuration found." )
ENDIF(JUCE_BUILD_CONFIGFURATION MATCHES "DEBUG")

add_library( ${BINARY_NAME}

    SHARED

    "../../../Resources/Fonts/comfortaa.font"
    "../../../Resources/Fonts/lato.fnt"
    "../../../Resources/Themes/Backgrounds/defaultPattern.png"
    "../../../Resources/Themes/ColourSchemes.xml"
    "../../../Resources/PianoSamples/A0v9.ogg"
    "../../../Resources/PianoSamples/A1v9.ogg"
    "../../../Resources/PianoSamples/A2v9.ogg"
    "../../../Resources/PianoSamples/A3v9.ogg"
    "../../../Resources/PianoSamples/A4v9.ogg"
    "../../../Resources/PianoSamples/A5v9.ogg"
    "../../../Resources/PianoSamples/A6v9.ogg"
    "../../../Resources/PianoSamples/A7v9.ogg"
    "../../../Resources/PianoSamples/C1v9.ogg"
    "../../../Resources/PianoSamples/C2v9.ogg"
    "../../../Resources/PianoSamples/C3v9.ogg"
    "../../../Resources/PianoSamples/C4v9.ogg"
    "../../../Resources/PianoSamples/C5v9.ogg"
    "../../../Resources/PianoSamples/C6v9.ogg"
    "../../../Resources/PianoSamples/C7v9.ogg"
    "../../../Resources/PianoSamples/C8v9.ogg"
    "../../../Resources/PianoSamples/D#1v9.ogg"
    "../../../Resources/PianoSamples/D#2v9.ogg"
    "../../../Resources/PianoSamples/D#3v9.ogg"
    "../../../Resources/PianoSamples/D#4v9.ogg"
    "../../../Resources/PianoSamples/D#5v9.ogg"
    "../../../Resources/PianoSamples/D#6v9.ogg"
    "../../../Resources/PianoSamples/D#7v9.ogg"
    "../../../Resources/PianoSamples/F#1v9.ogg"
    "../../../Resources/PianoSamples/F#2v9.ogg"
    "../../../Resources/PianoSamples/F#3v9.ogg"
    "../../../Resources/PianoSamples/F#4v9.ogg"
    "../../../Resources/PianoSamples/F#5v9.ogg"
    "../../../Resources/PianoSamples/F#6v9.ogg"
    "../../../Resources/PianoSamples/F#7v9.ogg"
    "../../../Resources/Icons/angle-double-down.svg"
    "../../../Resources/Icons/angle-double-left.svg"
    "../../../Resources/Icons/angle-double-right.svg"
    "../../../Resources/Icons/angle-double-up.svg"
    "../../../Resources/Icons/angle-down.svg"
    "../../../Resources/Icons/angle-left.svg"
    "../../../Resources/Icons/angle-right.svg"
    "../../../Resources/Icons/angle-up.svg"
    "../../../Resources/Icons/arpeggiator.svg"
    "../../../Resources/Icons/arrow-back.svg"
    "../../../Resources/Icons/arrow-forward.svg"
    "../../../Resources/Icons/arrow-left2.svg"
    "../../../Resources/Icons/arrow-right2.svg"
    "../../../Resources/Icons/arrows.svg"
    "../../../Resources/Icons/beamed_note.svg"
    "../../../Resources/Icons/bezier.svg"
    "../../../Resources/Icons/brush.svg"
    "../../../Resources/Icons/check.svg"
    "../../../Resources/Icons/chevron-left2.svg"
    "../../../Resources/Icons/chevron-right2.svg"
    "../../../Resources/Icons/clef.svg"
    "../../../Resources/Icons/cloud-download.svg"
    "../../../Resources/Icons/cloud-upload.svg"
    "../../../Resources/Icons/cloud2.svg"
    "../../../Resources/Icons/clouds.svg"
    "../../../Resources/Icons/columns.svg"
    "../../../Resources/Icons/copy.svg"
    "../../../Resources/Icons/crop.svg"
    "../../../Resources/Icons/cup.svg"
    "../../../Resources/Icons/cursor2.svg"
    "../../../Resources/Icons/diskette.svg"
    "../../../Resources/Icons/download2.svg"
    "../../../Resources/Icons/drive.svg"
    "../../../Resources/Icons/eight_note.svg"
    "../../../Resources/Icons/ellipsis-h.svg"
    "../../../Resources/Icons/ellipsis-v.svg"
    "../../../Resources/Icons/folder-open.svg"
    "../../../Resources/Icons/folder.svg"
    "../../../Resources/Icons/folder2.svg"
    "../../../Resources/Icons/heptagram2.svg"
    "../../../Resources/Icons/history.svg"
    "../../../Resources/Icons/hourglass.svg"
    "../../../Resources/Icons/insert-space.svg"
    "../../../Resources/Icons/key.svg"
    "../../../Resources/Icons/knob.svg"
    "../../../Resources/Icons/logo2.svg"
    "../../../Resources/Icons/marquee.svg"
    "../../../Resources/Icons/menu.svg"
    "../../../Resources/Icons/minus2.svg"
    "../../../Resources/Icons/paste.svg"
    "../../../Resources/Icons/pause2.svg"
    "../../../Resources/Icons/pencil.svg"
    "../../../Resources/Icons/pencil4.svg"
    "../../../Resources/Icons/play2.svg"
    "../../../Resources/Icons/plus2.svg"
    "../../../Resources/Icons/poetry.svg"
    "../../../Resources/Icons/quote.svg"
    "../../../Resources/Icons/reroute.svg"
    "../../../Resources/Icons/saxophone.svg"
    "../../../Resources/Icons/scissors.svg"
    "../../../Resources/Icons/settings2.svg"
    "../../../Resources/Icons/switch.svg"
    "../../../Resources/Icons/terminal.svg"
    "../../../Resources/Icons/times.svg"
    "../../../Resources/Icons/toggle-off.svg"
    "../../../Resources/Icons/toggle-on.svg"
    "../../../Resources/Icons/updown.svg"
    "../../../Resources/Icons/volume-off.svg"
    "../../../Resources/Icons/volume-up.svg"
    "../../../Resources/Icons/waveform.svg"
    "../../../Resources/Icons/wipe-space.svg"
    "../../../Resources/Icons/zoom-in.svg"
    "../../../Resources/Icons/zoom-out.svg"
    "../../../Resources/DefaultArps.xml"
    "../../../Resources/DefaultTranslations.xml"
    "../../../Resources/Icon.png"
    "../../../Resources/Logo.png"
    "../../../Source/Core/App/App.cpp"
    "../../../Source/Core/App/App.h"
    "../../../Source/Core/App/Config.cpp"
    "../../../Source/Core/App/Config.h"
    "../../../Source/Core/App/HelioLogger.h"
    "../../../Source/Core/App/Workspace.cpp"
    "../../../Source/Core/App/Workspace.h"
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
    "../../../Source/Core/Audio/Instruments/PluginManager.cpp"
    "../../../Source/Core/Audio/Instruments/PluginManager.h"
    "../../../Source/Core/Audio/Instruments/PluginSmartDescription.cpp"
    "../../../Source/Core/Audio/Instruments/PluginSmartDescription.h"
    "../../../Source/Core/Audio/Monitoring/AudioMonitor.cpp"
    "../../../Source/Core/Audio/Monitoring/AudioMonitor.h"
    "../../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.cpp"
    "../../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.h"
    "../../../Source/Core/Audio/Transport/PlayerThread.cpp"
    "../../../Source/Core/Audio/Transport/PlayerThread.h"
    "../../../Source/Core/Audio/Transport/ProjectSequencesWrapper.h"
    "../../../Source/Core/Audio/Transport/RendererThread.cpp"
    "../../../Source/Core/Audio/Transport/RendererThread.h"
    "../../../Source/Core/Audio/Transport/Transport.cpp"
    "../../../Source/Core/Audio/Transport/Transport.h"
    "../../../Source/Core/Audio/Transport/TransportListener.h"
    "../../../Source/Core/Audio/AudiobusOutput.mm"
    "../../../Source/Core/Audio/AudiobusOutput.h"
    "../../../Source/Core/Audio/AudioCore.cpp"
    "../../../Source/Core/Audio/AudioCore.h"
    "../../../Source/Core/Clipboard/ClipboardOwner.h"
    "../../../Source/Core/Clipboard/InternalClipboard.cpp"
    "../../../Source/Core/Clipboard/InternalClipboard.h"
    "../../../Source/Core/Events/AnnotationEvent.cpp"
    "../../../Source/Core/Events/AnnotationEvent.h"
    "../../../Source/Core/Events/AutomationEvent.cpp"
    "../../../Source/Core/Events/AutomationEvent.h"
    "../../../Source/Core/Events/MidiEvent.cpp"
    "../../../Source/Core/Events/MidiEvent.h"
    "../../../Source/Core/Events/Note.cpp"
    "../../../Source/Core/Events/Note.h"
    "../../../Source/Core/Events/TimeSignatureEvent.cpp"
    "../../../Source/Core/Events/TimeSignatureEvent.h"
    "../../../Source/Core/Layers/AnnotationsLayer.cpp"
    "../../../Source/Core/Layers/AnnotationsLayer.h"
    "../../../Source/Core/Layers/AutomationLayer.cpp"
    "../../../Source/Core/Layers/AutomationLayer.h"
    "../../../Source/Core/Layers/MidiLayer.cpp"
    "../../../Source/Core/Layers/MidiLayer.h"
    "../../../Source/Core/Layers/PianoLayer.cpp"
    "../../../Source/Core/Layers/PianoLayer.h"
    "../../../Source/Core/Layers/TimeSignaturesLayer.cpp"
    "../../../Source/Core/Layers/TimeSignaturesLayer.h"
    "../../../Source/Core/Network/AuthorizationManager.cpp"
    "../../../Source/Core/Network/AuthorizationManager.h"
    "../../../Source/Core/Network/HelioServerDefines.h"
    "../../../Source/Core/Network/LoginThread.cpp"
    "../../../Source/Core/Network/LoginThread.h"
    "../../../Source/Core/Network/LogoutThread.cpp"
    "../../../Source/Core/Network/LogoutThread.h"
    "../../../Source/Core/Network/RequestArpeggiatorsThread.cpp"
    "../../../Source/Core/Network/RequestArpeggiatorsThread.h"
    "../../../Source/Core/Network/RequestColourSchemesThread.cpp"
    "../../../Source/Core/Network/RequestColourSchemesThread.h"
    "../../../Source/Core/Network/RequestProjectsListThread.cpp"
    "../../../Source/Core/Network/RequestProjectsListThread.h"
    "../../../Source/Core/Network/RequestTranslationsThread.cpp"
    "../../../Source/Core/Network/RequestTranslationsThread.h"
    "../../../Source/Core/Network/UpdateManager.cpp"
    "../../../Source/Core/Network/UpdateManager.h"
    "../../../Source/Core/Serialization/Autosaver.cpp"
    "../../../Source/Core/Serialization/Autosaver.h"
    "../../../Source/Core/Serialization/DataEncoder.cpp"
    "../../../Source/Core/Serialization/DataEncoder.h"
    "../../../Source/Core/Serialization/Document.cpp"
    "../../../Source/Core/Serialization/Document.h"
    "../../../Source/Core/Serialization/DocumentOwner.h"
    "../../../Source/Core/Serialization/FileUtils.cpp"
    "../../../Source/Core/Serialization/FileUtils.h"
    "../../../Source/Core/Serialization/Serializable.h"
    "../../../Source/Core/Serialization/SerializationKeys.h"
    "../../../Source/Core/Supervisor/Session.cpp"
    "../../../Source/Core/Supervisor/Session.h"
    "../../../Source/Core/Supervisor/SessionManager.cpp"
    "../../../Source/Core/Supervisor/SessionManager.h"
    "../../../Source/Core/Supervisor/Supervisor.cpp"
    "../../../Source/Core/Supervisor/Supervisor.h"
    "../../../Source/Core/Tools/Arpeggiator.cpp"
    "../../../Source/Core/Tools/Arpeggiator.h"
    "../../../Source/Core/Tools/ArpeggiatorsManager.cpp"
    "../../../Source/Core/Tools/ArpeggiatorsManager.h"
    "../../../Source/Core/Tools/ColourScheme.cpp"
    "../../../Source/Core/Tools/ColourScheme.h"
    "../../../Source/Core/Tools/ColourSchemeManager.cpp"
    "../../../Source/Core/Tools/ColourSchemeManager.h"
    "../../../Source/Core/Tools/MidiRollToolbox.cpp"
    "../../../Source/Core/Tools/MidiRollToolbox.h"
    "../../../Source/Core/Translation/TranslationKeys.h"
    "../../../Source/Core/Translation/TranslationManager.cpp"
    "../../../Source/Core/Translation/TranslationManager.h"
    "../../../Source/Core/Tree/RecentFilesList.cpp"
    "../../../Source/Core/Tree/RecentFilesList.h"
    "../../../Source/Core/Tree/AudioPluginTreeItem.cpp"
    "../../../Source/Core/Tree/AudioPluginTreeItem.h"
    "../../../Source/Core/Tree/AutomationLayerTreeItem.cpp"
    "../../../Source/Core/Tree/AutomationLayerTreeItem.h"
    "../../../Source/Core/Tree/InstrumentsRootTreeItem.cpp"
    "../../../Source/Core/Tree/InstrumentsRootTreeItem.h"
    "../../../Source/Core/Tree/InstrumentTreeItem.cpp"
    "../../../Source/Core/Tree/InstrumentTreeItem.h"
    "../../../Source/Core/Tree/LastShownTreeItems.h"
    "../../../Source/Core/Tree/LayerGroupTreeItem.cpp"
    "../../../Source/Core/Tree/LayerGroupTreeItem.h"
    "../../../Source/Core/Tree/LayerTreeItem.cpp"
    "../../../Source/Core/Tree/LayerTreeItem.h"
    "../../../Source/Core/Tree/PianoLayerTreeItem.cpp"
    "../../../Source/Core/Tree/PianoLayerTreeItem.h"
    "../../../Source/Core/Tree/ProjectInfo.cpp"
    "../../../Source/Core/Tree/ProjectInfo.h"
    "../../../Source/Core/Tree/ProjectTimeline.cpp"
    "../../../Source/Core/Tree/ProjectTimeline.h"
    "../../../Source/Core/Tree/ProjectListener.h"
    "../../../Source/Core/Tree/ProjectTreeItem.cpp"
    "../../../Source/Core/Tree/ProjectTreeItem.h"
    "../../../Source/Core/Tree/RootTreeItem.cpp"
    "../../../Source/Core/Tree/RootTreeItem.h"
    "../../../Source/Core/Tree/SafeTreeItemPointer.h"
    "../../../Source/Core/Tree/SettingsTreeItem.cpp"
    "../../../Source/Core/Tree/SettingsTreeItem.h"
    "../../../Source/Core/Tree/TreeItem.cpp"
    "../../../Source/Core/Tree/TreeItem.h"
    "../../../Source/Core/Tree/TreeItemChildrenSerializer.cpp"
    "../../../Source/Core/Tree/TreeItemChildrenSerializer.h"
    "../../../Source/Core/Tree/VersionControlTreeItem.cpp"
    "../../../Source/Core/Tree/VersionControlTreeItem.h"
    "../../../Source/Core/Undo/Actions/AnnotationEventActions.cpp"
    "../../../Source/Core/Undo/Actions/AnnotationEventActions.h"
    "../../../Source/Core/Undo/Actions/AutoLayerTreeItemActions.cpp"
    "../../../Source/Core/Undo/Actions/AutoLayerTreeItemActions.h"
    "../../../Source/Core/Undo/Actions/AutomationEventActions.cpp"
    "../../../Source/Core/Undo/Actions/AutomationEventActions.h"
    "../../../Source/Core/Undo/Actions/LayerTreeItemActions.cpp"
    "../../../Source/Core/Undo/Actions/LayerTreeItemActions.h"
    "../../../Source/Core/Undo/Actions/MidiLayerActions.cpp"
    "../../../Source/Core/Undo/Actions/MidiLayerActions.h"
    "../../../Source/Core/Undo/Actions/NoteActions.cpp"
    "../../../Source/Core/Undo/Actions/NoteActions.h"
    "../../../Source/Core/Undo/Actions/PianoLayerTreeItemActions.cpp"
    "../../../Source/Core/Undo/Actions/PianoLayerTreeItemActions.h"
    "../../../Source/Core/Undo/Actions/TimeSignatureEventActions.cpp"
    "../../../Source/Core/Undo/Actions/TimeSignatureEventActions.h"
    "../../../Source/Core/Undo/Actions/UndoAction.h"
    "../../../Source/Core/Undo/UndoStack.cpp"
    "../../../Source/Core/Undo/UndoStack.h"
    "../../../Source/Core/VCS/DiffLogic/AutoLayerDeltas.h"
    "../../../Source/Core/VCS/DiffLogic/AutomationLayerDiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/AutomationLayerDiffLogic.h"
    "../../../Source/Core/VCS/DiffLogic/DiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/DiffLogic.h"
    "../../../Source/Core/VCS/DiffLogic/PianoLayerDeltas.h"
    "../../../Source/Core/VCS/DiffLogic/PianoLayerDiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/PianoLayerDiffLogic.h"
    "../../../Source/Core/VCS/DiffLogic/ProjectInfoDeltas.h"
    "../../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.h"
    "../../../Source/Core/VCS/DiffLogic/ProjectTimelineDeltas.h"
    "../../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.cpp"
    "../../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.h"
    "../../../Source/Core/VCS/Network/PullThread.cpp"
    "../../../Source/Core/VCS/Network/PullThread.h"
    "../../../Source/Core/VCS/Network/PushThread.cpp"
    "../../../Source/Core/VCS/Network/PushThread.h"
    "../../../Source/Core/VCS/Network/RemovalThread.cpp"
    "../../../Source/Core/VCS/Network/RemovalThread.h"
    "../../../Source/Core/VCS/Network/SyncMessage.h"
    "../../../Source/Core/VCS/Network/SyncThread.cpp"
    "../../../Source/Core/VCS/Network/SyncThread.h"
    "../../../Source/Core/VCS/Client.cpp"
    "../../../Source/Core/VCS/Client.h"
    "../../../Source/Core/VCS/Delta.cpp"
    "../../../Source/Core/VCS/Delta.h"
    "../../../Source/Core/VCS/Diff.cpp"
    "../../../Source/Core/VCS/Diff.h"
    "../../../Source/Core/VCS/Head.cpp"
    "../../../Source/Core/VCS/Head.h"
    "../../../Source/Core/VCS/HeadState.cpp"
    "../../../Source/Core/VCS/HeadState.h"
    "../../../Source/Core/VCS/Key.h"
    "../../../Source/Core/VCS/Pack.cpp"
    "../../../Source/Core/VCS/Pack.h"
    "../../../Source/Core/VCS/Revision.cpp"
    "../../../Source/Core/VCS/Revision.h"
    "../../../Source/Core/VCS/RevisionItem.cpp"
    "../../../Source/Core/VCS/RevisionItem.h"
    "../../../Source/Core/VCS/StashesRepository.cpp"
    "../../../Source/Core/VCS/StashesRepository.h"
    "../../../Source/Core/VCS/TrackedItem.h"
    "../../../Source/Core/VCS/TrackedItemsSource.h"
    "../../../Source/Core/VCS/VersionControl.cpp"
    "../../../Source/Core/VCS/VersionControl.h"
    "../../../Source/UI/CommandPanels/Base/CommandItemComponent.cpp"
    "../../../Source/UI/CommandPanels/Base/CommandItemComponent.h"
    "../../../Source/UI/CommandPanels/Base/CommandItemComponentMarker.cpp"
    "../../../Source/UI/CommandPanels/Base/CommandItemComponentMarker.h"
    "../../../Source/UI/CommandPanels/Base/CommandPanel.cpp"
    "../../../Source/UI/CommandPanels/Base/CommandPanel.h"
    "../../../Source/UI/CommandPanels/AnnotationCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/AnnotationCommandPanel.h"
    "../../../Source/UI/CommandPanels/AnnotationsCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/AnnotationsCommandPanel.h"
    "../../../Source/UI/CommandPanels/ArpeggiatorEditorPanel.cpp"
    "../../../Source/UI/CommandPanels/ArpeggiatorEditorPanel.h"
    "../../../Source/UI/CommandPanels/ArpeggiatorPanel.cpp"
    "../../../Source/UI/CommandPanels/ArpeggiatorPanel.h"
    "../../../Source/UI/CommandPanels/AutomationsCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/AutomationsCommandPanel.h"
    "../../../Source/UI/CommandPanels/InstrumentCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/InstrumentCommandPanel.h"
    "../../../Source/UI/CommandPanels/InstrumentsCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/InstrumentsCommandPanel.h"
    "../../../Source/UI/CommandPanels/LayerCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/LayerCommandPanel.h"
    "../../../Source/UI/CommandPanels/MidiRollCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/MidiRollCommandPanel.h"
    "../../../Source/UI/CommandPanels/MidiRollCommandPanelDefault.cpp"
    "../../../Source/UI/CommandPanels/MidiRollCommandPanelDefault.h"
    "../../../Source/UI/CommandPanels/MidiRollCommandPanelPhone.cpp"
    "../../../Source/UI/CommandPanels/MidiRollCommandPanelPhone.h"
    "../../../Source/UI/CommandPanels/MoveToLayerCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/MoveToLayerCommandPanel.h"
    "../../../Source/UI/CommandPanels/NotesTuningPanel.cpp"
    "../../../Source/UI/CommandPanels/NotesTuningPanel.h"
    "../../../Source/UI/CommandPanels/ProjectCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/ProjectCommandPanel.h"
    "../../../Source/UI/CommandPanels/RootTreeItemPanelDefault.cpp"
    "../../../Source/UI/CommandPanels/RootTreeItemPanelDefault.h"
    "../../../Source/UI/CommandPanels/RootTreeItemPanelCompact.cpp"
    "../../../Source/UI/CommandPanels/RootTreeItemPanelCompact.h"
    "../../../Source/UI/CommandPanels/VCSCommandPanel.cpp"
    "../../../Source/UI/CommandPanels/VCSCommandPanel.h"
    "../../../Source/UI/Common/Meters/SpectrumComponent.cpp"
    "../../../Source/UI/Common/Meters/SpectrumComponent.h"
    "../../../Source/UI/Common/Meters/VolumeComponent.cpp"
    "../../../Source/UI/Common/Meters/VolumeComponent.h"
    "../../../Source/UI/Common/Origami/Origami.cpp"
    "../../../Source/UI/Common/Origami/Origami.h"
    "../../../Source/UI/Common/Origami/OrigamiHorizontal.cpp"
    "../../../Source/UI/Common/Origami/OrigamiHorizontal.h"
    "../../../Source/UI/Common/Origami/OrigamiVertical.cpp"
    "../../../Source/UI/Common/Origami/OrigamiVertical.h"
    "../../../Source/UI/Common/CommandIDs.h"
    "../../../Source/UI/Common/DraggingListBoxComponent.cpp"
    "../../../Source/UI/Common/DraggingListBoxComponent.h"
    "../../../Source/UI/Common/FatalErrorScreen.cpp"
    "../../../Source/UI/Common/FatalErrorScreen.h"
    "../../../Source/UI/Common/HelperRectangle.h"
    "../../../Source/UI/Common/HighlightedComponent.h"
    "../../../Source/UI/Common/IconButton.h"
    "../../../Source/UI/Common/IconComponent.h"
    "../../../Source/UI/Common/MenuButton.cpp"
    "../../../Source/UI/Common/MenuButton.h"
    "../../../Source/UI/Common/PlayButton.cpp"
    "../../../Source/UI/Common/PlayButton.h"
    "../../../Source/UI/Common/PluginWindow.cpp"
    "../../../Source/UI/Common/PluginWindow.h"
    "../../../Source/UI/Common/ShapeComponent.h"
    "../../../Source/UI/Common/SizeSwitcherComponent.cpp"
    "../../../Source/UI/Common/SizeSwitcherComponent.h"
    "../../../Source/UI/Common/SpectralLogo.cpp"
    "../../../Source/UI/Common/SpectralLogo.h"
    "../../../Source/UI/Common/ViewportFitProxyComponent.cpp"
    "../../../Source/UI/Common/ViewportFitProxyComponent.h"
    "../../../Source/UI/Dialogs/AuthorizationDialog.cpp"
    "../../../Source/UI/Dialogs/AuthorizationDialog.h"
    "../../../Source/UI/Dialogs/FadingDialog.cpp"
    "../../../Source/UI/Dialogs/FadingDialog.h"
    "../../../Source/UI/Dialogs/ModalDialogConfirmation.cpp"
    "../../../Source/UI/Dialogs/ModalDialogConfirmation.h"
    "../../../Source/UI/Dialogs/ModalDialogInput.cpp"
    "../../../Source/UI/Dialogs/ModalDialogInput.h"
    "../../../Source/UI/Dialogs/RenderDialog.cpp"
    "../../../Source/UI/Dialogs/RenderDialog.h"
    "../../../Source/UI/Dialogs/UpdateDialog.cpp"
    "../../../Source/UI/Dialogs/UpdateDialog.h"
    "../../../Source/UI/Input/LongHoldController.h"
    "../../../Source/UI/Input/LongHoldListener.h"
    "../../../Source/UI/Input/LongTapController.h"
    "../../../Source/UI/Input/LongTapListener.h"
    "../../../Source/UI/Input/MultiTouchController.cpp"
    "../../../Source/UI/Input/MultiTouchController.h"
    "../../../Source/UI/Input/MultiTouchListener.h"
    "../../../Source/UI/Input/SmoothPanController.h"
    "../../../Source/UI/Input/SmoothPanListener.h"
    "../../../Source/UI/Input/SmoothZoomController.h"
    "../../../Source/UI/Input/SmoothZoomListener.h"
    "../../../Source/UI/InstrumentsPage/Editor/AudioPluginEditorPage.cpp"
    "../../../Source/UI/InstrumentsPage/Editor/AudioPluginEditorPage.h"
    "../../../Source/UI/InstrumentsPage/Editor/InstrumentEditor.cpp"
    "../../../Source/UI/InstrumentsPage/Editor/InstrumentEditor.h"
    "../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorConnector.cpp"
    "../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorConnector.h"
    "../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorNode.cpp"
    "../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorNode.h"
    "../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorPin.cpp"
    "../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorPin.h"
    "../../../Source/UI/InstrumentsPage/InstrumentRow.cpp"
    "../../../Source/UI/InstrumentsPage/InstrumentRow.h"
    "../../../Source/UI/InstrumentsPage/InstrumentsPage.cpp"
    "../../../Source/UI/InstrumentsPage/InstrumentsPage.h"
    "../../../Source/UI/Intro/InitScreen.cpp"
    "../../../Source/UI/Intro/InitScreen.h"
    "../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationLargeComponent.cpp"
    "../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationLargeComponent.h"
    "../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationSmallComponent.cpp"
    "../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationSmallComponent.h"
    "../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationsTrackMap.cpp"
    "../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationsTrackMap.h"
    "../../../Source/UI/MidiEditor/AutomationMap/AutomationCurveHelper.cpp"
    "../../../Source/UI/MidiEditor/AutomationMap/AutomationCurveHelper.h"
    "../../../Source/UI/MidiEditor/AutomationMap/AutomationEventComponent.cpp"
    "../../../Source/UI/MidiEditor/AutomationMap/AutomationEventComponent.h"
    "../../../Source/UI/MidiEditor/AutomationMap/AutomationEventsConnector.cpp"
    "../../../Source/UI/MidiEditor/AutomationMap/AutomationEventsConnector.h"
    "../../../Source/UI/MidiEditor/AutomationMap/AutomationTrackMap.cpp"
    "../../../Source/UI/MidiEditor/AutomationMap/AutomationTrackMap.h"
    "../../../Source/UI/MidiEditor/Header/HeaderSelectionIndicator.cpp"
    "../../../Source/UI/MidiEditor/Header/HeaderSelectionIndicator.h"
    "../../../Source/UI/MidiEditor/Header/MidiRollHeader.cpp"
    "../../../Source/UI/MidiEditor/Header/MidiRollHeader.h"
    "../../../Source/UI/MidiEditor/Header/SoundProbeIndicator.cpp"
    "../../../Source/UI/MidiEditor/Header/SoundProbeIndicator.h"
    "../../../Source/UI/MidiEditor/Header/TimeDistanceIndicator.cpp"
    "../../../Source/UI/MidiEditor/Header/TimeDistanceIndicator.h"
    "../../../Source/UI/MidiEditor/Header/TrackEndIndicator.cpp"
    "../../../Source/UI/MidiEditor/Header/TrackEndIndicator.h"
    "../../../Source/UI/MidiEditor/Header/TrackStartIndicator.cpp"
    "../../../Source/UI/MidiEditor/Header/TrackStartIndicator.h"
    "../../../Source/UI/MidiEditor/Header/TransportIndicator.cpp"
    "../../../Source/UI/MidiEditor/Header/TransportIndicator.h"
    "../../../Source/UI/MidiEditor/Helpers/TimelineWarningMarker.cpp"
    "../../../Source/UI/MidiEditor/Helpers/TimelineWarningMarker.h"
    "../../../Source/UI/MidiEditor/Helpers/ComponentConnectorCurve.cpp"
    "../../../Source/UI/MidiEditor/Helpers/ComponentConnectorCurve.h"
    "../../../Source/UI/MidiEditor/Helpers/InsertSpaceHelper.cpp"
    "../../../Source/UI/MidiEditor/Helpers/InsertSpaceHelper.h"
    "../../../Source/UI/MidiEditor/Helpers/MidiRollExpandMark.cpp"
    "../../../Source/UI/MidiEditor/Helpers/MidiRollExpandMark.h"
    "../../../Source/UI/MidiEditor/Helpers/NoteResizerLeft.cpp"
    "../../../Source/UI/MidiEditor/Helpers/NoteResizerLeft.h"
    "../../../Source/UI/MidiEditor/Helpers/NoteResizerRight.cpp"
    "../../../Source/UI/MidiEditor/Helpers/NoteResizerRight.h"
    "../../../Source/UI/MidiEditor/Helpers/WipeSpaceHelper.cpp"
    "../../../Source/UI/MidiEditor/Helpers/WipeSpaceHelper.h"
    "../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignatureLargeComponent.cpp"
    "../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignatureLargeComponent.h"
    "../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignatureSmallComponent.cpp"
    "../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignatureSmallComponent.h"
    "../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignaturesTrackMap.cpp"
    "../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignaturesTrackMap.h"
    "../../../Source/UI/MidiEditor/TrackMap/PianoTrackMap.cpp"
    "../../../Source/UI/MidiEditor/TrackMap/PianoTrackMap.h"
    "../../../Source/UI/MidiEditor/TrackMap/TrackScroller.cpp"
    "../../../Source/UI/MidiEditor/TrackMap/TrackScroller.h"
    "../../../Source/UI/MidiEditor/TrackMap/TrackScrollerScreen.cpp"
    "../../../Source/UI/MidiEditor/TrackMap/TrackScrollerScreen.h"
    "../../../Source/UI/MidiEditor/TriggersMap/TriggerEventComponent.cpp"
    "../../../Source/UI/MidiEditor/TriggersMap/TriggerEventComponent.h"
    "../../../Source/UI/MidiEditor/TriggersMap/TriggerEventConnector.cpp"
    "../../../Source/UI/MidiEditor/TriggersMap/TriggerEventConnector.h"
    "../../../Source/UI/MidiEditor/TriggersMap/TriggersTrackMap.cpp"
    "../../../Source/UI/MidiEditor/TriggersMap/TriggersTrackMap.h"
    "../../../Source/UI/MidiEditor/MidiEditor.cpp"
    "../../../Source/UI/MidiEditor/MidiEditor.h"
    "../../../Source/UI/MidiEditor/MidiEventComponent.cpp"
    "../../../Source/UI/MidiEditor/MidiEventComponent.h"
    "../../../Source/UI/MidiEditor/MidiEventComponentLasso.cpp"
    "../../../Source/UI/MidiEditor/MidiEventComponentLasso.h"
    "../../../Source/UI/MidiEditor/MidiEventSelection.h"
    "../../../Source/UI/MidiEditor/MidiLayerOwner.h"
    "../../../Source/UI/MidiEditor/MidiRoll.cpp"
    "../../../Source/UI/MidiEditor/MidiRoll.h"
    "../../../Source/UI/MidiEditor/MidiRollEditMode.cpp"
    "../../../Source/UI/MidiEditor/MidiRollEditMode.h"
    "../../../Source/UI/MidiEditor/MidiRollListener.h"
    "../../../Source/UI/MidiEditor/NoteComponent.cpp"
    "../../../Source/UI/MidiEditor/NoteComponent.h"
    "../../../Source/UI/MidiEditor/PianoRoll.cpp"
    "../../../Source/UI/MidiEditor/PianoRoll.h"
    "../../../Source/UI/Popups/CenteredTooltipComponent.h"
    "../../../Source/UI/Popups/ChordTooltip.cpp"
    "../../../Source/UI/Popups/ChordTooltip.h"
    "../../../Source/UI/Popups/FailTooltip.cpp"
    "../../../Source/UI/Popups/FailTooltip.h"
    "../../../Source/UI/Popups/GenericTooltip.h"
    "../../../Source/UI/Popups/HelioCallout.cpp"
    "../../../Source/UI/Popups/HelioCallout.h"
    "../../../Source/UI/Popups/NoNotesPopup.cpp"
    "../../../Source/UI/Popups/NoNotesPopup.h"
    "../../../Source/UI/Popups/NotePopupListener.h"
    "../../../Source/UI/Popups/PopupButton.cpp"
    "../../../Source/UI/Popups/PopupButton.h"
    "../../../Source/UI/Popups/PopupButtonOwner.h"
    "../../../Source/UI/Popups/PopupCustomButton.cpp"
    "../../../Source/UI/Popups/PopupCustomButton.h"
    "../../../Source/UI/Popups/PopupImageButton.cpp"
    "../../../Source/UI/Popups/PopupImageButton.h"
    "../../../Source/UI/Popups/PopupMenuComponent.h"
    "../../../Source/UI/Popups/ProgressIndicator.h"
    "../../../Source/UI/Popups/ProgressTooltip.cpp"
    "../../../Source/UI/Popups/ProgressTooltip.h"
    "../../../Source/UI/Popups/SuccessTooltip.cpp"
    "../../../Source/UI/Popups/SuccessTooltip.h"
    "../../../Source/UI/Popups/TooltipContainer.cpp"
    "../../../Source/UI/Popups/TooltipContainer.h"
    "../../../Source/UI/Rollovers/RolloverBackButtonLeft.cpp"
    "../../../Source/UI/Rollovers/RolloverBackButtonLeft.h"
    "../../../Source/UI/Rollovers/RolloverBackButtonRight.cpp"
    "../../../Source/UI/Rollovers/RolloverBackButtonRight.h"
    "../../../Source/UI/Rollovers/RolloverHeaderLeft.cpp"
    "../../../Source/UI/Rollovers/RolloverHeaderLeft.h"
    "../../../Source/UI/Rollovers/RolloverHeaderRight.cpp"
    "../../../Source/UI/Rollovers/RolloverHeaderRight.h"
    "../../../Source/UI/Rollovers/RolloverContainer.cpp"
    "../../../Source/UI/Rollovers/RolloverContainer.h"
    "../../../Source/UI/Rollovers/ProjectRollover.cpp"
    "../../../Source/UI/Rollovers/ProjectRollover.h"
    "../../../Source/UI/Rollovers/SettingsRollover.cpp"
    "../../../Source/UI/Rollovers/SettingsRollover.h"
    "../../../Source/UI/ProjectPage/ProjectPage.cpp"
    "../../../Source/UI/ProjectPage/ProjectPage.h"
    "../../../Source/UI/ProjectPage/ProjectPageDefault.cpp"
    "../../../Source/UI/ProjectPage/ProjectPageDefault.h"
    "../../../Source/UI/ProjectPage/ProjectPagePhone.cpp"
    "../../../Source/UI/ProjectPage/ProjectPagePhone.h"
    "../../../Source/UI/SettingsPage/AudioSettings.cpp"
    "../../../Source/UI/SettingsPage/AudioSettings.h"
    "../../../Source/UI/SettingsPage/AuthorizationSettings.cpp"
    "../../../Source/UI/SettingsPage/AuthorizationSettings.h"
    "../../../Source/UI/SettingsPage/ColourChangeButton.h"
    "../../../Source/UI/SettingsPage/ComponentsList.cpp"
    "../../../Source/UI/SettingsPage/ComponentsList.h"
    "../../../Source/UI/SettingsPage/IntroSettingsWrapper.cpp"
    "../../../Source/UI/SettingsPage/IntroSettingsWrapper.h"
    "../../../Source/UI/SettingsPage/LabeledSettingsWrapper.cpp"
    "../../../Source/UI/SettingsPage/LabeledSettingsWrapper.h"
    "../../../Source/UI/SettingsPage/LogComponent.cpp"
    "../../../Source/UI/SettingsPage/LogComponent.h"
    "../../../Source/UI/SettingsPage/OpenGLSettings.cpp"
    "../../../Source/UI/SettingsPage/OpenGLSettings.h"
    "../../../Source/UI/SettingsPage/PluginsList.cpp"
    "../../../Source/UI/SettingsPage/PluginsList.h"
    "../../../Source/UI/SettingsPage/SettingsPage.cpp"
    "../../../Source/UI/SettingsPage/SettingsPage.h"
    "../../../Source/UI/SettingsPage/SettingsListItemHighlighter.cpp"
    "../../../Source/UI/SettingsPage/SettingsListItemHighlighter.h"
    "../../../Source/UI/SettingsPage/SettingsListItemSelection.cpp"
    "../../../Source/UI/SettingsPage/SettingsListItemSelection.h"
    "../../../Source/UI/SettingsPage/ThemeSettings.cpp"
    "../../../Source/UI/SettingsPage/ThemeSettings.h"
    "../../../Source/UI/SettingsPage/ThemeSettingsItem.cpp"
    "../../../Source/UI/SettingsPage/ThemeSettingsItem.h"
    "../../../Source/UI/SettingsPage/TranslationSettings.cpp"
    "../../../Source/UI/SettingsPage/TranslationSettings.h"
    "../../../Source/UI/SettingsPage/TranslationSettingsItem.cpp"
    "../../../Source/UI/SettingsPage/TranslationSettingsItem.h"
    "../../../Source/UI/Themes/ComponentFader.cpp"
    "../../../Source/UI/Themes/ComponentFader.h"
    "../../../Source/UI/Themes/DialogBackground.h"
    "../../../Source/UI/Themes/FontSerializer.cpp"
    "../../../Source/UI/Themes/FontSerializer.h"
    "../../../Source/UI/Themes/GradientVertical.cpp"
    "../../../Source/UI/Themes/GradientVertical.h"
    "../../../Source/UI/Themes/GradientVerticalReversed.cpp"
    "../../../Source/UI/Themes/GradientVerticalReversed.h"
    "../../../Source/UI/Themes/HelioTheme.cpp"
    "../../../Source/UI/Themes/HelioTheme.h"
    "../../../Source/UI/Themes/Icons.cpp"
    "../../../Source/UI/Themes/Icons.h"
    "../../../Source/UI/Themes/LighterShadowDownwards.cpp"
    "../../../Source/UI/Themes/LighterShadowDownwards.h"
    "../../../Source/UI/Themes/LighterShadowUpwards.cpp"
    "../../../Source/UI/Themes/LighterShadowUpwards.h"
    "../../../Source/UI/Themes/LightShadowDownwards.cpp"
    "../../../Source/UI/Themes/LightShadowDownwards.h"
    "../../../Source/UI/Themes/LightShadowLeftwards.cpp"
    "../../../Source/UI/Themes/LightShadowLeftwards.h"
    "../../../Source/UI/Themes/LightShadowRightwards.cpp"
    "../../../Source/UI/Themes/LightShadowRightwards.h"
    "../../../Source/UI/Themes/LightShadowUpwards.cpp"
    "../../../Source/UI/Themes/LightShadowUpwards.h"
    "../../../Source/UI/Themes/PanelA.cpp"
    "../../../Source/UI/Themes/PanelA.h"
    "../../../Source/UI/Themes/PanelB.cpp"
    "../../../Source/UI/Themes/PanelB.h"
    "../../../Source/UI/Themes/PanelBackgroundA.cpp"
    "../../../Source/UI/Themes/PanelBackgroundA.h"
    "../../../Source/UI/Themes/PanelBackgroundB.cpp"
    "../../../Source/UI/Themes/PanelBackgroundB.h"
    "../../../Source/UI/Themes/PanelBackgroundC.cpp"
    "../../../Source/UI/Themes/PanelBackgroundC.h"
    "../../../Source/UI/Themes/PanelC.cpp"
    "../../../Source/UI/Themes/PanelC.h"
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
    "../../../Source/UI/Themes/SeparatorVerticalReversed.cpp"
    "../../../Source/UI/Themes/SeparatorVerticalReversed.h"
    "../../../Source/UI/Themes/ShadeDark.cpp"
    "../../../Source/UI/Themes/ShadeDark.h"
    "../../../Source/UI/Themes/ShadeLight.cpp"
    "../../../Source/UI/Themes/ShadeLight.h"
    "../../../Source/UI/Themes/ShadowDownwards.cpp"
    "../../../Source/UI/Themes/ShadowDownwards.h"
    "../../../Source/UI/Themes/ShadowHorizontalFading.cpp"
    "../../../Source/UI/Themes/ShadowHorizontalFading.h"
    "../../../Source/UI/Themes/ShadowLeftwards.cpp"
    "../../../Source/UI/Themes/ShadowLeftwards.h"
    "../../../Source/UI/Themes/ShadowRightwards.cpp"
    "../../../Source/UI/Themes/ShadowRightwards.h"
    "../../../Source/UI/Themes/ShadowUpwards.cpp"
    "../../../Source/UI/Themes/ShadowUpwards.h"
    "../../../Source/UI/Themes/ViewportKineticSlider.cpp"
    "../../../Source/UI/Themes/ViewportKineticSlider.h"
    "../../../Source/UI/Tree/TreeItemComponent.cpp"
    "../../../Source/UI/Tree/TreeItemComponent.h"
    "../../../Source/UI/Tree/TreeItemComponentDefault.cpp"
    "../../../Source/UI/Tree/TreeItemComponentDefault.h"
    "../../../Source/UI/Tree/TreeItemComponentCompact.cpp"
    "../../../Source/UI/Tree/TreeItemComponentCompact.h"
    "../../../Source/UI/Tree/TreeItemMarkerDefault.cpp"
    "../../../Source/UI/Tree/TreeItemMarkerDefault.h"
    "../../../Source/UI/Tree/TreeItemMarkerCompact.cpp"
    "../../../Source/UI/Tree/TreeItemMarkerCompact.h"
    "../../../Source/UI/Tree/TreeItemMenuButton.cpp"
    "../../../Source/UI/Tree/TreeItemMenuButton.h"
    "../../../Source/UI/Tree/TreePanel.cpp"
    "../../../Source/UI/Tree/TreePanel.h"
    "../../../Source/UI/Tree/TreePanelDefault.cpp"
    "../../../Source/UI/Tree/TreePanelDefault.h"
    "../../../Source/UI/Tree/TreePanelPhone.cpp"
    "../../../Source/UI/Tree/TreePanelPhone.h"
    "../../../Source/UI/VCSPage/HistoryComponent.cpp"
    "../../../Source/UI/VCSPage/HistoryComponent.h"
    "../../../Source/UI/VCSPage/RevisionComponent.cpp"
    "../../../Source/UI/VCSPage/RevisionComponent.h"
    "../../../Source/UI/VCSPage/RevisionConnectorComponent.cpp"
    "../../../Source/UI/VCSPage/RevisionConnectorComponent.h"
    "../../../Source/UI/VCSPage/RevisionItemComponent.cpp"
    "../../../Source/UI/VCSPage/RevisionItemComponent.h"
    "../../../Source/UI/VCSPage/RevisionTooltipComponent.cpp"
    "../../../Source/UI/VCSPage/RevisionTooltipComponent.h"
    "../../../Source/UI/VCSPage/RevisionTreeComponent.cpp"
    "../../../Source/UI/VCSPage/RevisionTreeComponent.h"
    "../../../Source/UI/VCSPage/StageComponent.cpp"
    "../../../Source/UI/VCSPage/StageComponent.h"
    "../../../Source/UI/VCSPage/VersionControlEditor.cpp"
    "../../../Source/UI/VCSPage/VersionControlEditor.h"
    "../../../Source/UI/VCSPage/VersionControlEditorDefault.cpp"
    "../../../Source/UI/VCSPage/VersionControlEditorDefault.h"
    "../../../Source/UI/VCSPage/VersionControlEditorPhone.cpp"
    "../../../Source/UI/VCSPage/VersionControlEditorPhone.h"
    "../../../Source/UI/VCSPage/VersionControlEditorPhoneViewportContent.cpp"
    "../../../Source/UI/VCSPage/VersionControlEditorPhoneViewportContent.h"
    "../../../Source/UI/WorkspacePage/Menu/CreateProjectRow.cpp"
    "../../../Source/UI/WorkspacePage/Menu/CreateProjectRow.h"
    "../../../Source/UI/WorkspacePage/Menu/OpenProjectRow.cpp"
    "../../../Source/UI/WorkspacePage/Menu/OpenProjectRow.h"
    "../../../Source/UI/WorkspacePage/Menu/RecentProjectRow.cpp"
    "../../../Source/UI/WorkspacePage/Menu/RecentProjectRow.h"
    "../../../Source/UI/WorkspacePage/Menu/SignInRow.cpp"
    "../../../Source/UI/WorkspacePage/Menu/SignInRow.h"
    "../../../Source/UI/WorkspacePage/Menu/WorkspaceMenu.cpp"
    "../../../Source/UI/WorkspacePage/Menu/WorkspaceMenu.h"
    "../../../Source/UI/WorkspacePage/LogoFader.cpp"
    "../../../Source/UI/WorkspacePage/LogoFader.h"
    "../../../Source/UI/WorkspacePage/LogoImage.cpp"
    "../../../Source/UI/WorkspacePage/LogoImage.h"
    "../../../Source/UI/WorkspacePage/WorkspacePage.cpp"
    "../../../Source/UI/WorkspacePage/WorkspacePage.h"
    "../../../Source/UI/MainLayout.cpp"
    "../../../Source/UI/MainLayout.h"
    "../../../Source/UI/MainWindow.cpp"
    "../../../Source/UI/MainWindow.h"
    "../../../Source/Common.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/audio_play_head/juce_AudioPlayHead.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioChannelSet.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioChannelSet.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioDataConverters.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioDataConverters.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioSampleBuffer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_FloatVectorOperations.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_FloatVectorOperations.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_CatmullRomInterpolator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_CatmullRomInterpolator.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_Decibels.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_FFT.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_FFT.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_IIRFilter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_IIRFilter.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_LagrangeInterpolator.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_LagrangeInterpolator.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_LinearSmoothedValue.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_Reverb.h"
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
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEValue.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEValue.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZone.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZone.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZoneLayout.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZoneLayout.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_AudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_BufferingAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_BufferingAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ChannelRemappingAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ChannelRemappingAudioSource.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_IIRFilterAudioSource.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_IIRFilterAudioSource.h"
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
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioDeviceManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioDeviceManager.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODevice.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODevice.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODeviceType.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODeviceType.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_SystemAudioVolume.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiInput.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiMessageCollector.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiMessageCollector.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiOutput.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiOutput.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Audio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Midi.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_OpenSL.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_ios_Audio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_ios_Audio.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_ALSA.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_JackAudio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_Midi.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_mac_CoreAudio.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_mac_CoreMidi.cpp"
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
    "../../../ThirdParty/JUCE/modules/juce_audio_devices/juce_audio_devices.h"
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
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/books/coupled/res_books_51.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/books/coupled/res_books_stereo.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/books/floor/floor_books.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/books/uncoupled/res_books_uncoupled.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/floor_all.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/psych_8.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/psych_11.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/psych_16.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/psych_44.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_8.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_16.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_44.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_44p51.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_44u.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_8.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_11.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_16.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_22.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_32.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_44.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_44p51.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_44u.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_X.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/analysis.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/backends.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/bitrate.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/bitrate.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/block.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/codebook.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/codebook.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/codec_internal.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/envelope.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/envelope.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/floor0.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/floor1.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/highlevel.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/info.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lookup.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lookup.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lookup_data.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lpc.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lpc.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lsp.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lsp.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/mapping0.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/masking.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/mdct.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/mdct.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/misc.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/os.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/psy.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/psy.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/registry.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/registry.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/res0.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/scales.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/sharedbook.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/smallft.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/smallft.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/synthesis.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/vorbisenc.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/vorbisfile.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/window.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/window.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/bitwise.c"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/codec.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/config_types.h"
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
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_QuickTimeAudioFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_QuickTimeAudioFormat.h"
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
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormatManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormatManager.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AU_Shared.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AudioUnitPluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AudioUnitPluginFormat.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LADSPAPluginFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LADSPAPluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3Common.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3Headers.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3PluginFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3PluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTCommon.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTInterface.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTMidiEventList.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTPluginFormat.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTPluginFormat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioPluginInstance.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorEditor.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorEditor.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorGraph.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorGraph.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorListener.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorParameter.h"
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
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterBool.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterChoice.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterFloat.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterInt.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorParameters.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorParameterWithID.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/audio_cd/juce_AudioCDBurner.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/audio_cd/juce_AudioCDReader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/audio_cd/juce_AudioCDReader.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioAppComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioAppComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioDeviceSelectorComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioDeviceSelectorComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnail.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnail.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnailBase.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnailCache.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnailCache.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioVisualiserComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioVisualiserComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_BluetoothMidiDevicePairingDialogue.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_MidiKeyboardComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_MidiKeyboardComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_android_BluetoothMidiDevicePairingDialogue.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_ios_BluetoothMidiDevicePairingDialogue.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_linux_AudioCDReader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_linux_BluetoothMidiDevicePairingDialogue.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_mac_AudioCDBurner.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_mac_AudioCDReader.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_mac_BluetoothMidiDevicePairingDialogue.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_win_BluetoothMidiDevicePairingDialogue.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_win32_AudioCDBurner.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_win32_AudioCDReader.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/players/juce_AudioProcessorPlayer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/players/juce_AudioProcessorPlayer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/players/juce_SoundPlayer.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/players/juce_SoundPlayer.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/juce_audio_utils.cpp"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/juce_audio_utils.h"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/juce_audio_utils.mm"
    "../../../ThirdParty/JUCE/modules/juce_audio_utils/juce_audio_utils.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_AbstractFifo.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_AbstractFifo.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_Array.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ArrayAllocationBase.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_DynamicObject.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_DynamicObject.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ElementComparator.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_HashMap.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_LinkedListPointer.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ListenerList.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ListenerList.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_NamedValueSet.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_NamedValueSet.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_OwnedArray.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_PropertySet.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_PropertySet.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ReferenceCountedArray.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ScopedValueSetter.h"
    "../../../ThirdParty/JUCE/modules/juce_core/containers/juce_SortedSet.h"
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
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_Atomic.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ByteOrder.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ContainerDeletePolicy.h"
    "../../../ThirdParty/JUCE/modules/juce_core/memory/juce_HeapBlock.h"
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
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Result.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Result.h"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_RuntimePermissions.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_RuntimePermissions.h"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Uuid.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Uuid.h"
    "../../../ThirdParty/JUCE/modules/juce_core/misc/juce_WindowsRegistry.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Files.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_JNIHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Misc.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Network.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_RuntimePermissions.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_SystemStats.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Threads.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_BasicNativeHeaders.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_curl_Network.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_CommonFile.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Files.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Network.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_SystemStats.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Threads.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_ClangBugWorkaround.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Files.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Network.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Strings.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_SystemStats.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Threads.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_osx_ObjCHelpers.h"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_NamedPipe.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_SharedCode.h"
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
    "../../../ThirdParty/JUCE/modules/juce_core/system/juce_CompilerSupport.h"
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
    "../../../ThirdParty/JUCE/modules/juce_core/threads/juce_WaitableEvent.h"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_PerformanceCounter.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_PerformanceCounter.h"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_RelativeTime.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_RelativeTime.h"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_Time.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/time/juce_Time.h"
    "../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTest.cpp"
    "../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTest.h"
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
    "../../../ThirdParty/JUCE/modules/juce_core/juce_core.h"
    "../../../ThirdParty/JUCE/modules/juce_core/juce_core.mm"
    "../../../ThirdParty/JUCE/modules/juce_core/juce_core.h"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_BlowFish.cpp"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_BlowFish.h"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_Primes.cpp"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_Primes.h"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_RSAKey.cpp"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_RSAKey.h"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_MD5.cpp"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_MD5.h"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_SHA256.cpp"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_SHA256.h"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_Whirlpool.cpp"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_Whirlpool.h"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/juce_cryptography.cpp"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/juce_cryptography.h"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/juce_cryptography.mm"
    "../../../ThirdParty/JUCE/modules/juce_cryptography/juce_cryptography.h"
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
    "../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.cpp"
    "../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.h"
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
    "../../../ThirdParty/JUCE/modules/juce_events/juce_events.h"
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
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_DirectWriteTypeface.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_DirectWriteTypeLayout.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_Fonts.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_IconHelpers.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_Justification.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_RectanglePlacement.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_RectanglePlacement.h"
    "../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.cpp"
    "../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.h"
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
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_Desktop.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_Desktop.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ModalComponentManager.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ModalComponentManager.h"
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
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_FileChooser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_Windowing.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_UIViewComponentPeer.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_Windowing.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_FileChooser.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_X11.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_X11.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_X11_Clipboard.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_X11_Windowing.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_FileChooser.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_MainMenu.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_MouseCursor.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_NSViewComponentPeer.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_Windowing.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_MultiTouchMapper.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_DragAndDrop.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_FileChooser.cpp"
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
    "../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.h"
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
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_NSViewComponent.h"
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
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SplashScreen.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SplashScreen.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SystemTrayIconComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SystemTrayIconComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_WebBrowserComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_android_WebBrowserComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_ios_UIViewComponent.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_X11_SystemTrayIcon.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_X11_WebBrowserComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_XEmbedComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_AppleRemote.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_CarbonViewWrapperComponent.h"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_NSViewComponent.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_SystemTrayIcon.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_WebBrowserComponent.mm"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_ActiveXComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_SystemTrayIcon.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_WebBrowserComponent.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.cpp"
    "../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.h"
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
    "../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.h"
    "../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.mm"
    "../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCAddress.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCAddress.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCArgument.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCArgument.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCBundle.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCBundle.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCMessage.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCMessage.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCReceiver.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCReceiver.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCSender.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCSender.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCTimeTag.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCTimeTag.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCTypes.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCTypes.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/juce_osc.cpp"
    "../../../ThirdParty/JUCE/modules/juce_osc/juce_osc.h"
    "../../../ThirdParty/JUCE/modules/juce_osc/juce_osc.h"
    "../../Projucer/JuceLibraryCode/AppConfig.h"
    "../../Projucer/JuceLibraryCode/BinaryData.cpp"
    "../../Projucer/JuceLibraryCode/BinaryData.h"
    "../../Projucer/JuceLibraryCode/BinaryData2.cpp"
    "../../Projucer/JuceLibraryCode/BinaryData3.cpp"
    "../../Projucer/JuceLibraryCode/juce_audio_basics.cpp"
    "../../Projucer/JuceLibraryCode/juce_audio_devices.cpp"
    "../../Projucer/JuceLibraryCode/juce_audio_formats.cpp"
    "../../Projucer/JuceLibraryCode/juce_audio_processors.cpp"
    "../../Projucer/JuceLibraryCode/juce_audio_utils.cpp"
    "../../Projucer/JuceLibraryCode/juce_core.cpp"
    "../../Projucer/JuceLibraryCode/juce_cryptography.cpp"
    "../../Projucer/JuceLibraryCode/juce_data_structures.cpp"
    "../../Projucer/JuceLibraryCode/juce_events.cpp"
    "../../Projucer/JuceLibraryCode/juce_graphics.cpp"
    "../../Projucer/JuceLibraryCode/juce_gui_basics.cpp"
    "../../Projucer/JuceLibraryCode/juce_gui_extra.cpp"
    "../../Projucer/JuceLibraryCode/juce_opengl.cpp"
    "../../Projucer/JuceLibraryCode/juce_osc.cpp"
    "../../Projucer/JuceLibraryCode/JuceHeader.h"
)

set_source_files_properties("../../../Resources/Fonts/comfortaa.font" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Fonts/lato.fnt" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Themes/Backgrounds/defaultPattern.png" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Themes/ColourSchemes.xml" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/A0v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/A1v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/A2v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/A3v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/A4v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/A5v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/A6v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/A7v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/C1v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/C2v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/C3v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/C4v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/C5v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/C6v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/C7v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/C8v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/D#1v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/D#2v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/D#3v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/D#4v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/D#5v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/D#6v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/D#7v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/F#1v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/F#2v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/F#3v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/F#4v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/F#5v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/F#6v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/PianoSamples/F#7v9.ogg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/angle-double-down.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/angle-double-left.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/angle-double-right.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/angle-double-up.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/angle-down.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/angle-left.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/angle-right.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/angle-up.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/arpeggiator.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/arrow-back.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/arrow-forward.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/arrow-left2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/arrow-right2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/arrows.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/beamed_note.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/bezier.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/brush.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/check.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/chevron-left2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/chevron-right2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/clef.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cloud-download.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cloud-upload.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cloud2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/clouds.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/columns.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/copy.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/crop.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cup.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/cursor2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/diskette.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/download2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/drive.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/eight_note.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/ellipsis-h.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/ellipsis-v.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/folder-open.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/folder.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/folder2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/heptagram2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/history.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/hourglass.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/insert-space.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/key.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/knob.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/logo2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/marquee.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/menu.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/minus2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/paste.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/pause2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/pencil.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/pencil4.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/play2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/plus2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/poetry.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/quote.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/reroute.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/saxophone.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/scissors.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/settings2.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/switch.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/terminal.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/times.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/toggle-off.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/toggle-on.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/updown.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/volume-off.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/volume-up.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/waveform.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/wipe-space.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/zoom-in.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icons/zoom-out.svg" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/DefaultArps.xml" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/DefaultTranslations.xml" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Icon.png" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Resources/Logo.png" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/App/App.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/App/Config.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/App/HelioLogger.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/App/Workspace.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynthAudioPlugin.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynthFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/BuiltInSynthPiano.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/BuiltIn/InternalPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/Instrument.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/OrchestraListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/OrchestraPit.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/PluginManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Instruments/PluginSmartDescription.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Monitoring/AudioMonitor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/PlayerThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/ProjectSequencesWrapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/RendererThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/Transport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/Transport/TransportListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/AudiobusOutput.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/AudiobusOutput.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Audio/AudioCore.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Clipboard/ClipboardOwner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Clipboard/InternalClipboard.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Events/AnnotationEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Events/AutomationEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Events/MidiEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Events/Note.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Events/TimeSignatureEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Layers/AnnotationsLayer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Layers/AutomationLayer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Layers/MidiLayer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Layers/PianoLayer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Layers/TimeSignaturesLayer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/AuthorizationManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/HelioServerDefines.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/LoginThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/LogoutThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/RequestArpeggiatorsThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/RequestColourSchemesThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/RequestProjectsListThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/RequestTranslationsThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Network/UpdateManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Autosaver.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/DataEncoder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Document.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/DocumentOwner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/FileUtils.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/Serializable.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Serialization/SerializationKeys.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Supervisor/Session.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Supervisor/SessionManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Supervisor/Supervisor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tools/Arpeggiator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tools/ArpeggiatorsManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tools/ColourScheme.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tools/ColourSchemeManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tools/MidiRollToolbox.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Translation/TranslationKeys.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Translation/TranslationManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/RecentFilesList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/AudioPluginTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/AutomationLayerTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/InstrumentsRootTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/InstrumentTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/LastShownTreeItems.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/LayerGroupTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/LayerTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/PianoLayerTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectInfo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectTimeline.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/ProjectTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/RootTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/SafeTreeItemPointer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/SettingsTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/TreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/TreeItemChildrenSerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Tree/VersionControlTreeItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/AnnotationEventActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/AutoLayerTreeItemActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/AutomationEventActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/LayerTreeItemActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/MidiLayerActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/NoteActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/PianoLayerTreeItemActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/TimeSignatureEventActions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/Actions/UndoAction.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/Undo/UndoStack.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/AutoLayerDeltas.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/AutomationLayerDiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/DiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/PianoLayerDeltas.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/PianoLayerDiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/ProjectInfoDeltas.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/ProjectTimelineDeltas.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Network/PullThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Network/PushThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Network/RemovalThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Network/SyncMessage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Network/SyncThread.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Client.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Delta.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Diff.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Head.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/HeadState.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Key.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Pack.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/Revision.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/RevisionItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/StashesRepository.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/TrackedItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/TrackedItemsSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Core/VCS/VersionControl.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/Base/CommandItemComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/Base/CommandItemComponentMarker.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/Base/CommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/AnnotationCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/AnnotationsCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/ArpeggiatorEditorPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/ArpeggiatorPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/AutomationsCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/InstrumentCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/InstrumentsCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/LayerCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/MidiRollCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/MidiRollCommandPanelDefault.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/MidiRollCommandPanelPhone.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/MoveToLayerCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/NotesTuningPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/ProjectCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/RootTreeItemPanelDefault.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/RootTreeItemPanelCompact.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/CommandPanels/VCSCommandPanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/Meters/SpectrumComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/Meters/VolumeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/Origami/Origami.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/Origami/OrigamiHorizontal.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/Origami/OrigamiVertical.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/CommandIDs.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/DraggingListBoxComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/FatalErrorScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/HelperRectangle.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/HighlightedComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/IconButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/IconComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/MenuButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/PlayButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/PluginWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ShapeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/SizeSwitcherComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/SpectralLogo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Common/ViewportFitProxyComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/AuthorizationDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/FadingDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/ModalDialogConfirmation.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/ModalDialogInput.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/RenderDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Dialogs/UpdateDialog.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/LongHoldController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/LongHoldListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/LongTapController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/LongTapListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/MultiTouchController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/MultiTouchListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothPanController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothPanListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothZoomController.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Input/SmoothZoomListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/InstrumentsPage/Editor/AudioPluginEditorPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/InstrumentsPage/Editor/InstrumentEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorNode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/InstrumentsPage/Editor/InstrumentEditorPin.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/InstrumentsPage/InstrumentRow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/InstrumentsPage/InstrumentsPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Intro/InitScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationLargeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationSmallComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/AnnotationsMap/AnnotationsTrackMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/AutomationMap/AutomationCurveHelper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/AutomationMap/AutomationEventComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/AutomationMap/AutomationEventsConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/AutomationMap/AutomationTrackMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Header/HeaderSelectionIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Header/MidiRollHeader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Header/SoundProbeIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Header/TimeDistanceIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Header/TrackEndIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Header/TrackStartIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Header/TransportIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Helpers/TimelineWarningMarker.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Helpers/ComponentConnectorCurve.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Helpers/InsertSpaceHelper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Helpers/MidiRollExpandMark.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Helpers/NoteResizerLeft.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Helpers/NoteResizerRight.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/Helpers/WipeSpaceHelper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignatureLargeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignatureSmallComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TimeSignaturesMap/TimeSignaturesTrackMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TrackMap/PianoTrackMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TrackMap/TrackScroller.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TrackMap/TrackScrollerScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TriggersMap/TriggerEventComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TriggersMap/TriggerEventConnector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/TriggersMap/TriggersTrackMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/MidiEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/MidiEventComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/MidiEventComponentLasso.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/MidiEventSelection.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/MidiLayerOwner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/MidiRoll.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/MidiRollEditMode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/MidiRollListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/NoteComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MidiEditor/PianoRoll.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/CenteredTooltipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ChordTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/FailTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/GenericTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/HelioCallout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/NoNotesPopup.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/NotePopupListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupButtonOwner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupCustomButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupImageButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/PopupMenuComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ProgressIndicator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/ProgressTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/SuccessTooltip.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Popups/TooltipContainer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Rollovers/RolloverBackButtonLeft.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Rollovers/RolloverBackButtonRight.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Rollovers/RolloverHeaderLeft.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Rollovers/RolloverHeaderRight.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Rollovers/RolloverContainer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Rollovers/ProjectRollover.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Rollovers/SettingsRollover.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/ProjectPage/ProjectPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/ProjectPage/ProjectPageDefault.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/ProjectPage/ProjectPagePhone.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/AudioSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/AuthorizationSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/ColourChangeButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/ComponentsList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/IntroSettingsWrapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/LabeledSettingsWrapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/LogComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/OpenGLSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/PluginsList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/SettingsPage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/SettingsListItemHighlighter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/SettingsListItemSelection.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/ThemeSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/ThemeSettingsItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/TranslationSettings.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/SettingsPage/TranslationSettingsItem.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ComponentFader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/DialogBackground.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/FontSerializer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/GradientVertical.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/GradientVerticalReversed.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/HelioTheme.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/Icons.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/LighterShadowDownwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/LighterShadowUpwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/LightShadowDownwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/LightShadowLeftwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/LightShadowRightwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/LightShadowUpwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelA.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelB.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelBackgroundA.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelBackgroundB.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelBackgroundC.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/PanelC.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorHorizontal.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorHorizontalReversed.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorHorizontalFading.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorHorizontalFadingReversed.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorVertical.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/SeparatorVerticalReversed.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadeDark.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadeLight.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowDownwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowHorizontalFading.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowLeftwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowRightwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ShadowUpwards.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Themes/ViewportKineticSlider.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreeItemComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreeItemComponentDefault.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreeItemComponentCompact.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreeItemMarkerDefault.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreeItemMarkerCompact.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreeItemMenuButton.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreePanel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreePanelDefault.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/Tree/TreePanelPhone.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/HistoryComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/RevisionComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/RevisionConnectorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/RevisionItemComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/RevisionTooltipComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/RevisionTreeComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/StageComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/VersionControlEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/VersionControlEditorDefault.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/VersionControlEditorPhone.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/VCSPage/VersionControlEditorPhoneViewportContent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/WorkspacePage/Menu/CreateProjectRow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/WorkspacePage/Menu/OpenProjectRow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/WorkspacePage/Menu/RecentProjectRow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/WorkspacePage/Menu/SignInRow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/WorkspacePage/Menu/WorkspaceMenu.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/WorkspacePage/LogoFader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/WorkspacePage/LogoImage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/WorkspacePage/WorkspacePage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MainLayout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/UI/MainWindow.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../Source/Common.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/audio_play_head/juce_AudioPlayHead.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioChannelSet.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioChannelSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioDataConverters.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioDataConverters.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_AudioSampleBuffer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_FloatVectorOperations.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/buffers/juce_FloatVectorOperations.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_CatmullRomInterpolator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_CatmullRomInterpolator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_Decibels.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_FFT.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_FFT.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_IIRFilter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_IIRFilter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_LagrangeInterpolator.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_LagrangeInterpolator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_LinearSmoothedValue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/effects/juce_Reverb.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEValue.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEValue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZone.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZone.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZoneLayout.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/mpe/juce_MPEZoneLayout.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_AudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_BufferingAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_BufferingAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ChannelRemappingAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_ChannelRemappingAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_IIRFilterAudioSource.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/sources/juce_IIRFilterAudioSource.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_basics/juce_audio_basics.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioDeviceManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioDeviceManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODevice.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODevice.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODeviceType.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_AudioIODeviceType.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/audio_io/juce_SystemAudioVolume.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiInput.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiMessageCollector.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiMessageCollector.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiOutput.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/midi_io/juce_MidiOutput.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Audio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_Midi.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_android_OpenSL.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_ios_Audio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_ios_Audio.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_ALSA.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_JackAudio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_linux_Midi.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_mac_CoreAudio.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/native/juce_mac_CoreMidi.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_devices/juce_audio_devices.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/books/coupled/res_books_51.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/books/coupled/res_books_stereo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/books/floor/floor_books.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/books/uncoupled/res_books_uncoupled.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/floor_all.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/psych_8.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/psych_11.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/psych_16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/psych_44.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_8.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_44.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_44p51.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/residue_44u.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_8.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_11.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_16.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_22.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_32.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_44.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_44p51.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_44u.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/modes/setup_X.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/analysis.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/backends.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/bitrate.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/bitrate.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/block.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/codebook.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/codebook.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/codec_internal.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/envelope.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/envelope.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/floor0.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/floor1.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/highlevel.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/info.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lookup.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lookup.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lookup_data.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lpc.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lpc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lsp.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/lsp.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/mapping0.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/masking.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/mdct.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/mdct.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/misc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/os.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/psy.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/psy.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/registry.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/registry.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/res0.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/scales.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/sharedbook.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/smallft.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/smallft.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/synthesis.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/vorbisenc.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/vorbisfile.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/window.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/libvorbis-1.3.2/lib/window.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/bitwise.c" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/codec.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/oggvorbis/config_types.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_QuickTimeAudioFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/codecs/juce_QuickTimeAudioFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_formats/juce_audio_formats.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormatManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format/juce_AudioPluginFormatManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AU_Shared.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AudioUnitPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_AudioUnitPluginFormat.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LADSPAPluginFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_LADSPAPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3Common.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3Headers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3PluginFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VST3PluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTCommon.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTInterface.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTMidiEventList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTPluginFormat.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/juce_VSTPluginFormat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioPluginInstance.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorEditor.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorEditor.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorGraph.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorGraph.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorListener.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/processors/juce_AudioProcessorParameter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterBool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterChoice.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterFloat.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioParameterInt.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorParameters.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorParameterWithID.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_processors/juce_audio_processors.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/audio_cd/juce_AudioCDBurner.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/audio_cd/juce_AudioCDReader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/audio_cd/juce_AudioCDReader.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioAppComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioAppComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioDeviceSelectorComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioDeviceSelectorComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnail.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnail.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnailBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnailCache.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioThumbnailCache.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioVisualiserComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_AudioVisualiserComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_BluetoothMidiDevicePairingDialogue.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_MidiKeyboardComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/gui/juce_MidiKeyboardComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_android_BluetoothMidiDevicePairingDialogue.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_ios_BluetoothMidiDevicePairingDialogue.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_linux_AudioCDReader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_linux_BluetoothMidiDevicePairingDialogue.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_mac_AudioCDBurner.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_mac_AudioCDReader.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_mac_BluetoothMidiDevicePairingDialogue.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_win_BluetoothMidiDevicePairingDialogue.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_win32_AudioCDBurner.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/native/juce_win32_AudioCDReader.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/players/juce_AudioProcessorPlayer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/players/juce_AudioProcessorPlayer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/players/juce_SoundPlayer.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/players/juce_SoundPlayer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/juce_audio_utils.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/juce_audio_utils.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/juce_audio_utils.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_audio_utils/juce_audio_utils.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_AbstractFifo.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_AbstractFifo.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_Array.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ArrayAllocationBase.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_DynamicObject.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_DynamicObject.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ElementComparator.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_HashMap.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_LinkedListPointer.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ListenerList.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ListenerList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_NamedValueSet.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_NamedValueSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_OwnedArray.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_PropertySet.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_PropertySet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ReferenceCountedArray.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_ScopedValueSetter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/containers/juce_SortedSet.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_Atomic.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ByteOrder.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_ContainerDeletePolicy.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/memory/juce_HeapBlock.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Result.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Result.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_RuntimePermissions.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_RuntimePermissions.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Uuid.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_Uuid.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/misc/juce_WindowsRegistry.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Files.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_JNIHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Misc.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Network.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_RuntimePermissions.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_SystemStats.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_android_Threads.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_BasicNativeHeaders.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_curl_Network.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_CommonFile.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Files.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Network.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_SystemStats.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_linux_Threads.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_ClangBugWorkaround.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Files.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Network.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Strings.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_SystemStats.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_mac_Threads.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_osx_ObjCHelpers.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_NamedPipe.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/native/juce_posix_SharedCode.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/system/juce_CompilerSupport.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/threads/juce_WaitableEvent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_PerformanceCounter.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_PerformanceCounter.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_RelativeTime.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_RelativeTime.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_Time.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/time/juce_Time.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTest.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/unit_tests/juce_UnitTest.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/juce_core.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/juce_core.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_core/juce_core.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_BlowFish.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_BlowFish.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_Primes.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_Primes.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_RSAKey.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/encryption/juce_RSAKey.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_MD5.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_MD5.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_SHA256.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_SHA256.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_Whirlpool.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/hashing/juce_Whirlpool.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/juce_cryptography.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/juce_cryptography.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/juce_cryptography.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_cryptography/juce_cryptography.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_data_structures/juce_data_structures.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_events/juce_events.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_DirectWriteTypeface.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_DirectWriteTypeLayout.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_Fonts.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/native/juce_win32_IconHelpers.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_Justification.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_RectanglePlacement.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/placement/juce_RectanglePlacement.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_graphics/juce_graphics.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_Desktop.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_Desktop.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ModalComponentManager.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/components/juce_ModalComponentManager.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_FileChooser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_android_Windowing.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_UIViewComponentPeer.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_ios_Windowing.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_FileChooser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_X11.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_X11.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_X11_Clipboard.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_linux_X11_Windowing.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_FileChooser.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_MainMenu.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_MouseCursor.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_NSViewComponentPeer.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_mac_Windowing.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_MultiTouchMapper.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_DragAndDrop.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/native/juce_win32_FileChooser.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_basics/juce_gui_basics.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/embedding/juce_NSViewComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_RecentlyOpenedFilesList.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SplashScreen.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SplashScreen.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SystemTrayIconComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_SystemTrayIconComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/misc/juce_WebBrowserComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_android_WebBrowserComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_ios_UIViewComponent.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_X11_SystemTrayIcon.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_X11_WebBrowserComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_linux_XEmbedComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_AppleRemote.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_CarbonViewWrapperComponent.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_NSViewComponent.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_SystemTrayIcon.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_mac_WebBrowserComponent.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_ActiveXComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_SystemTrayIcon.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/native/juce_win32_WebBrowserComponent.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_gui_extra/juce_gui_extra.h" PROPERTIES HEADER_FILE_ONLY TRUE)
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
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.mm" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_opengl/juce_opengl.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCAddress.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCAddress.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCArgument.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCArgument.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCBundle.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCBundle.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCMessage.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCMessage.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCReceiver.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCReceiver.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCSender.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCSender.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCTimeTag.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCTimeTag.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCTypes.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/osc/juce_OSCTypes.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/juce_osc.cpp" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/juce_osc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../../ThirdParty/JUCE/modules/juce_osc/juce_osc.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/AppConfig.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/BinaryData.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/JuceHeader.h" PROPERTIES HEADER_FILE_ONLY TRUE)

find_library(log "log")
find_library(android "android")
find_library(glesv2 "GLESv2")
find_library(egl "EGL")

target_link_libraries( ${BINARY_NAME}

    ${log}
    ${android}
    ${glesv2}
    ${egl}
)
