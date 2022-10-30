/*

    IMPORTANT! This file is auto-generated each time you save your
    project - if you alter its contents, your changes may be overwritten!

    Unity build.
    Incredible fucking awesomeness.

*/

#include "../../Source/Core/Audio/BuiltIn/SoundFont/SoundFont2Sound.cpp"
#include "../../Source/Core/Audio/BuiltIn/SoundFont/SoundFontSound.cpp"
#include "../../Source/Core/Audio/BuiltIn/SoundFont/SoundFontSynth.cpp"
#include "../../Source/Core/Audio/BuiltIn/InternalIODevicesPluginFormat.cpp"
#include "../../Source/Core/Audio/BuiltIn/BuiltInSynthsPluginFormat.cpp"
#include "../../Source/Core/Audio/BuiltIn/DefaultSynthAudioPlugin.cpp"
#include "../../Source/Core/Audio/BuiltIn/DefaultSynth.cpp"
#include "../../Source/Core/Audio/BuiltIn/MetronomeSynthAudioPlugin.cpp"
#include "../../Source/Core/Audio/BuiltIn/MetronomeSynth.cpp"
#include "../../Source/Core/Audio/BuiltIn/SoundFontSynthAudioPlugin.cpp"
#include "../../Source/Core/Audio/Instruments/Instrument.cpp"
#include "../../Source/Core/Audio/Instruments/OrchestraPit.cpp"
#include "../../Source/Core/Audio/Instruments/PluginScanner.cpp"
#include "../../Source/Core/Audio/Instruments/SerializablePluginDescription.cpp"
#include "../../Source/Core/Audio/Monitoring/AudioMonitor.cpp"
#include "../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.cpp"
#include "../../Source/Core/Audio/Transport/MidiRecorder.cpp"
#include "../../Source/Core/Audio/Transport/PlayerThread.cpp"
#include "../../Source/Core/Audio/Transport/RendererThread.cpp"
#include "../../Source/Core/Audio/Transport/Transport.cpp"
#include "../../Source/Core/Audio/AudioCore.cpp"
#include "../../Source/Core/Configuration/Resources/Models/Arpeggiator.cpp"
#include "../../Source/Core/Configuration/Resources/Models/Chord.cpp"
#include "../../Source/Core/Configuration/Resources/Models/ColourScheme.cpp"
#include "../../Source/Core/Configuration/Resources/Models/HotkeyScheme.cpp"
#include "../../Source/Core/Configuration/Resources/Models/KeyboardMapping.cpp"
#include "../../Source/Core/Configuration/Resources/Models/Meter.cpp"
#include "../../Source/Core/Configuration/Resources/Models/Scale.cpp"
#include "../../Source/Core/Configuration/Resources/Models/Temperament.cpp"
#include "../../Source/Core/Configuration/Resources/Models/Translation.cpp"
#include "../../Source/Core/Configuration/Resources/ConfigurationResourceCollection.cpp"
#include "../../Source/Core/Configuration/Resources/ArpeggiatorsCollection.cpp"
#include "../../Source/Core/Configuration/Resources/ChordsCollection.cpp"
#include "../../Source/Core/Configuration/Resources/ColourSchemesCollection.cpp"
#include "../../Source/Core/Configuration/Resources/HotkeySchemesCollection.cpp"
#include "../../Source/Core/Configuration/Resources/KeyboardMappingsCollection.cpp"
#include "../../Source/Core/Configuration/Resources/MetersCollection.cpp"
#include "../../Source/Core/Configuration/Resources/ScalesCollection.cpp"
#include "../../Source/Core/Configuration/Resources/TemperamentsCollection.cpp"
#include "../../Source/Core/Configuration/Resources/TranslationsCollection.cpp"
#include "../../Source/Core/Configuration/Config.cpp"
#include "../../Source/Core/Configuration/UserInterfaceFlags.cpp"
#include "../../Source/Core/CommandPalette/CommandPaletteActionsProvider.cpp"
#include "../../Source/Core/CommandPalette/CommandPaletteCommonActions.cpp"
#include "../../Source/Core/CommandPalette/CommandPaletteChordConstructor.cpp"
#include "../../Source/Core/CommandPalette/CommandPaletteMoveNotesMenu.cpp"
#include "../../Source/Core/CommandPalette/CommandPaletteProjectsList.cpp"
#include "../../Source/Core/CommandPalette/CommandPaletteTimelineEvents.cpp"
#include "../../Source/Core/Midi/Patterns/Clip.cpp"
#include "../../Source/Core/Midi/Patterns/Pattern.cpp"
#include "../../Source/Core/Midi/Sequences/Events/AnnotationEvent.cpp"
#include "../../Source/Core/Midi/Sequences/Events/AutomationEvent.cpp"
#include "../../Source/Core/Midi/Sequences/Events/KeySignatureEvent.cpp"
#include "../../Source/Core/Midi/Sequences/Events/MidiEvent.cpp"
#include "../../Source/Core/Midi/Sequences/Events/Note.cpp"
#include "../../Source/Core/Midi/Sequences/Events/TimeSignatureEvent.cpp"
#include "../../Source/Core/Midi/Sequences/AnnotationsSequence.cpp"
#include "../../Source/Core/Midi/Sequences/AutomationSequence.cpp"
#include "../../Source/Core/Midi/Sequences/KeySignaturesSequence.cpp"
#include "../../Source/Core/Midi/Sequences/MidiSequence.cpp"
#include "../../Source/Core/Midi/Sequences/PianoSequence.cpp"
#include "../../Source/Core/Midi/Sequences/TimeSignaturesSequence.cpp"
#include "../../Source/Core/Midi/Sequences/TimeSignaturesAggregator.cpp"
#include "../../Source/Core/Midi/MidiTrack.cpp"
#include "../../Source/Core/Network/Requests/BackendRequest.cpp"
#include "../../Source/Core/Network/Requests/UserConfigSyncThread.cpp"
#include "../../Source/Core/Network/Requests/ProjectCloneThread.cpp"
#include "../../Source/Core/Network/Requests/ProjectDeleteThread.cpp"
#include "../../Source/Core/Network/Requests/RevisionsSyncHelpers.cpp"
#include "../../Source/Core/Network/Requests/RevisionsSyncThread.cpp"
#include "../../Source/Core/Network/Services/ProjectSyncService.cpp"
#include "../../Source/Core/Network/Services/ResourceSyncService.cpp"
#include "../../Source/Core/Network/Services/SessionService.cpp"
#include "../../Source/Core/Network/Network.cpp"
#include "../../Source/Core/Serialization/Autosaver.cpp"
#include "../../Source/Core/Serialization/Document.cpp"
#include "../../Source/Core/Serialization/DocumentHelpers.cpp"
#include "../../Source/Core/Serialization/SerializedData.cpp"
#include "../../Source/Core/Serialization/BinarySerializer.cpp"
#include "../../Source/Core/Serialization/JsonSerializer.cpp"
#include "../../Source/Core/Serialization/XmlSerializer.cpp"
#include "../../Source/Core/Tree/AutomationTrackNode.cpp"
#include "../../Source/Core/Tree/InstrumentNode.cpp"
#include "../../Source/Core/Tree/MidiTrackNode.cpp"
#include "../../Source/Core/Tree/OrchestraPitNode.cpp"
#include "../../Source/Core/Tree/PatternEditorNode.cpp"
#include "../../Source/Core/Tree/PianoTrackNode.cpp"
#include "../../Source/Core/Tree/ProjectMetadata.cpp"
#include "../../Source/Core/Tree/ProjectTimeline.cpp"
#include "../../Source/Core/Tree/ProjectNode.cpp"
#include "../../Source/Core/Tree/RootNode.cpp"
#include "../../Source/Core/Tree/SettingsNode.cpp"
#include "../../Source/Core/Tree/TrackGroupNode.cpp"
#include "../../Source/Core/Tree/TreeNode.cpp"
#include "../../Source/Core/Tree/TreeNodeSerializer.cpp"
#include "../../Source/Core/Tree/VersionControlNode.cpp"
#include "../../Source/Core/Undo/Actions/AnnotationEventActions.cpp"
#include "../../Source/Core/Undo/Actions/AutomationTrackActions.cpp"
#include "../../Source/Core/Undo/Actions/AutomationEventActions.cpp"
#include "../../Source/Core/Undo/Actions/KeySignatureEventActions.cpp"
#include "../../Source/Core/Undo/Actions/MidiTrackActions.cpp"
#include "../../Source/Core/Undo/Actions/NoteActions.cpp"
#include "../../Source/Core/Undo/Actions/PatternActions.cpp"
#include "../../Source/Core/Undo/Actions/PianoTrackActions.cpp"
#include "../../Source/Core/Undo/Actions/ProjectMetadataActions.cpp"
#include "../../Source/Core/Undo/Actions/TimeSignatureEventActions.cpp"
#include "../../Source/Core/Undo/UndoStack.cpp"
#include "../../Source/Core/VCS/DiffLogic/AutomationTrackDiffLogic.cpp"
#include "../../Source/Core/VCS/DiffLogic/DiffLogic.cpp"
#include "../../Source/Core/VCS/DiffLogic/PatternDiffHelpers.cpp"
#include "../../Source/Core/VCS/DiffLogic/PianoTrackDiffLogic.cpp"
#include "../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.cpp"
#include "../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.cpp"
#include "../../Source/Core/VCS/Delta.cpp"
#include "../../Source/Core/VCS/Diff.cpp"
#include "../../Source/Core/VCS/Head.cpp"
#include "../../Source/Core/VCS/RemoteCache.cpp"
#include "../../Source/Core/VCS/Revision.cpp"
#include "../../Source/Core/VCS/RevisionItem.cpp"
#include "../../Source/Core/VCS/Snapshot.cpp"
#include "../../Source/Core/VCS/StashesRepository.cpp"
#include "../../Source/Core/VCS/VersionControl.cpp"
#include "../../Source/Core/Workspace/NavigationHistory.cpp"
#include "../../Source/Core/Workspace/RecentProjectInfo.cpp"
#include "../../Source/Core/Workspace/SyncedConfigurationInfo.cpp"
#include "../../Source/Core/Workspace/UserSessionInfo.cpp"
#include "../../Source/Core/Workspace/UserProfile.cpp"
#include "../../Source/Core/Workspace/Workspace.cpp"
#include "../../Source/Core/App.cpp"
#include "../../Source/UI/Common/AudioMonitors/SpectrogramAudioMonitorComponent.cpp"
#include "../../Source/UI/Common/AudioMonitors/WaveformAudioMonitorComponent.cpp"
#include "../../Source/UI/Common/Origami/Origami.cpp"
#include "../../Source/UI/Common/Origami/OrigamiHorizontal.cpp"
#include "../../Source/UI/Common/Origami/OrigamiVertical.cpp"
#include "../../Source/UI/Common/ColourButton.cpp"
#include "../../Source/UI/Common/ColourSwatches.cpp"
#include "../../Source/UI/Common/CommandIDs.cpp"
#include "../../Source/UI/Common/ColourIDs.cpp"
#include "../../Source/UI/Common/DraggingListBoxComponent.cpp"
#include "../../Source/UI/Common/FineTuningComponentDragger.cpp"
#include "../../Source/UI/Common/FineTuningValueIndicator.cpp"
#include "../../Source/UI/Common/KeySelector.cpp"
#include "../../Source/UI/Common/MobileComboBox.cpp"
#include "../../Source/UI/Common/ModeIndicatorComponent.cpp"
#include "../../Source/UI/Common/OverlayButton.cpp"
#include "../../Source/UI/Common/PlayButton.cpp"
#include "../../Source/UI/Common/PluginWindow.cpp"
#include "../../Source/UI/Common/RadioButton.cpp"
#include "../../Source/UI/Common/ScaleEditor.cpp"
#include "../../Source/UI/Common/SpectralLogo.cpp"
#include "../../Source/UI/Common/TransportControlComponent.cpp"
#include "../../Source/UI/Common/ViewportFitProxyComponent.cpp"
#include "../../Source/UI/Dialogs/DialogBase.cpp"
#include "../../Source/UI/Dialogs/AnnotationDialog.cpp"
#include "../../Source/UI/Dialogs/KeySignatureDialog.cpp"
#include "../../Source/UI/Dialogs/ModalDialogConfirmation.cpp"
#include "../../Source/UI/Dialogs/ModalDialogInput.cpp"
#include "../../Source/UI/Dialogs/RenderDialog.cpp"
#include "../../Source/UI/Dialogs/TempoDialog.cpp"
#include "../../Source/UI/Dialogs/TimeSignatureDialog.cpp"
#include "../../Source/UI/Dialogs/TrackPropertiesDialog.cpp"
#include "../../Source/UI/Headline/Headline.cpp"
#include "../../Source/UI/Headline/HeadlineDropdown.cpp"
#include "../../Source/UI/Headline/HeadlineItem.cpp"
#include "../../Source/UI/Headline/HeadlineContextMenuController.cpp"
#include "../../Source/UI/Input/MultiTouchController.cpp"
#include "../../Source/UI/Menus/Base/MenuItemComponent.cpp"
#include "../../Source/UI/Menus/Base/MenuPanel.cpp"
#include "../../Source/UI/Menus/SelectionMenus/AudioPluginSelectionMenu.cpp"
#include "../../Source/UI/Menus/SelectionMenus/InstrumentNodeSelectionMenu.cpp"
#include "../../Source/UI/Menus/SelectionMenus/PatternRollSelectionMenu.cpp"
#include "../../Source/UI/Menus/SelectionMenus/PianoRollSelectionMenu.cpp"
#include "../../Source/UI/Menus/SelectionMenus/VersionControlHistorySelectionMenu.cpp"
#include "../../Source/UI/Menus/SelectionMenus/VersionControlStageSelectionMenu.cpp"
#include "../../Source/UI/Menus/InstrumentMenu.cpp"
#include "../../Source/UI/Menus/MidiTrackMenu.cpp"
#include "../../Source/UI/Menus/OrchestraPitMenu.cpp"
#include "../../Source/UI/Menus/PatternsMenu.cpp"
#include "../../Source/UI/Menus/ProjectMenu.cpp"
#include "../../Source/UI/Menus/TimelineMenu.cpp"
#include "../../Source/UI/Menus/VersionControlMenu.cpp"
#include "../../Source/UI/Menus/WorkspaceMenu.cpp"
#include "../../Source/UI/Pages/Dashboard/Menu/DashboardMenu.cpp"
#include "../../Source/UI/Pages/Dashboard/Menu/RecentProjectRow.cpp"
#include "../../Source/UI/Pages/Dashboard/Dashboard.cpp"
#include "../../Source/UI/Pages/Dashboard/UpdatesInfoComponent.cpp"
#include "../../Source/UI/Pages/Instruments/Editor/AudioPluginEditorPage.cpp"
#include "../../Source/UI/Pages/Instruments/Editor/InstrumentEditor.cpp"
#include "../../Source/UI/Pages/Instruments/Editor/InstrumentEditorConnector.cpp"
#include "../../Source/UI/Pages/Instruments/Editor/InstrumentComponent.cpp"
#include "../../Source/UI/Pages/Instruments/Editor/InstrumentEditorPin.cpp"
#include "../../Source/UI/Pages/Instruments/Editor/KeyboardMappingPage.cpp"
#include "../../Source/UI/Pages/Instruments/AudioPluginsListComponent.cpp"
#include "../../Source/UI/Pages/Instruments/InstrumentsListComponent.cpp"
#include "../../Source/UI/Pages/Instruments/OrchestraPitPage.cpp"
#include "../../Source/UI/Pages/Project/ProjectPage.cpp"
#include "../../Source/UI/Pages/Settings/AudioSettings.cpp"
#include "../../Source/UI/Pages/Settings/ComponentsList.cpp"
#include "../../Source/UI/Pages/Settings/SettingsFrameWrapper.cpp"
#include "../../Source/UI/Pages/Settings/SettingsPage.cpp"
#include "../../Source/UI/Pages/Settings/SyncSettings.cpp"
#include "../../Source/UI/Pages/Settings/SyncSettingsItem.cpp"
#include "../../Source/UI/Pages/Settings/ThemeSettings.cpp"
#include "../../Source/UI/Pages/Settings/ThemeSettingsItem.cpp"
#include "../../Source/UI/Pages/Settings/TranslationSettings.cpp"
#include "../../Source/UI/Pages/Settings/TranslationSettingsItem.cpp"
#include "../../Source/UI/Pages/Settings/UserInterfaceSettings.cpp"
#include "../../Source/UI/Pages/VCS/HistoryComponent.cpp"
#include "../../Source/UI/Pages/VCS/RevisionComponent.cpp"
#include "../../Source/UI/Pages/VCS/RevisionConnectorComponent.cpp"
#include "../../Source/UI/Pages/VCS/RevisionItemComponent.cpp"
#include "../../Source/UI/Pages/VCS/RevisionTooltipComponent.cpp"
#include "../../Source/UI/Pages/VCS/RevisionTreeComponent.cpp"
#include "../../Source/UI/Pages/VCS/StageComponent.cpp"
#include "../../Source/UI/Pages/VCS/VersionControlEditor.cpp"
#include "../../Source/UI/Popups/CommandPalette.cpp"
#include "../../Source/UI/Popups/NotesTuningPanel.cpp"
#include "../../Source/UI/Popups/ArpPreviewTool.cpp"
#include "../../Source/UI/Popups/ChordPreviewTool.cpp"
#include "../../Source/UI/Popups/RescalePreviewTool.cpp"
#include "../../Source/UI/Popups/ScalePreviewTool.cpp"
#include "../../Source/UI/Popups/ModalCallout.cpp"
#include "../../Source/UI/Popups/PopupButton.cpp"
#include "../../Source/UI/Popups/PopupCustomButton.cpp"
#include "../../Source/UI/Popups/ProgressTooltip.cpp"
#include "../../Source/UI/Popups/TooltipContainer.cpp"
#include "../../Source/UI/Sequencer/Header/HeaderSelectionIndicator.cpp"
#include "../../Source/UI/Sequencer/Header/RollHeader.cpp"
#include "../../Source/UI/Sequencer/Header/Playhead.cpp"
#include "../../Source/UI/Sequencer/Helpers/CutPointMark.cpp"
#include "../../Source/UI/Sequencer/Helpers/RollExpandMark.cpp"
#include "../../Source/UI/Sequencer/Helpers/KnifeToolHelper.cpp"
#include "../../Source/UI/Sequencer/Helpers/MergingEventsConnector.cpp"
#include "../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.cpp"
#include "../../Source/UI/Sequencer/Helpers/PatternOperations.cpp"
#include "../../Source/UI/Sequencer/Helpers/SequencerOperations.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveClipComponent.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveHelper.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventComponent.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventsConnector.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepsClipComponent.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventComponent.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventsConnector.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip/PianoClipComponent.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/ClipComponent.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/ClipComponents/DummyClipComponent.cpp"
#include "../../Source/UI/Sequencer/PatternRoll/PatternRoll.cpp"
#include "../../Source/UI/Sequencer/PianoRoll/HighlightingScheme.cpp"
#include "../../Source/UI/Sequencer/PianoRoll/NoteComponent.cpp"
#include "../../Source/UI/Sequencer/PianoRoll/NoteNameGuide.cpp"
#include "../../Source/UI/Sequencer/PianoRoll/NoteNameGuidesBar.cpp"
#include "../../Source/UI/Sequencer/PianoRoll/NoteResizerLeft.cpp"
#include "../../Source/UI/Sequencer/PianoRoll/NoteResizerRight.cpp"
#include "../../Source/UI/Sequencer/PianoRoll/PianoRoll.cpp"
#include "../../Source/UI/Sequencer/Sidebars/SequencerSidebarLeft.cpp"
#include "../../Source/UI/Sequencer/Sidebars/SequencerSidebarRight.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationLargeComponent.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationSmallComponent.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationsProjectMap.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureLargeComponent.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureSmallComponent.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignaturesProjectMap.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/LevelsMap/LevelsMapScroller.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/LevelsMap/VelocityProjectMap.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/PianoMap/PianoProjectMap.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapScroller.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/PianoMap/ProjectMapScrollerScreen.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureLargeComponent.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureSmallComponent.cpp"
#include "../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignaturesProjectMap.cpp"
#include "../../Source/UI/Sequencer/RollBase.cpp"
#include "../../Source/UI/Sequencer/RollEditMode.cpp"
#include "../../Source/UI/Sequencer/Lasso.cpp"
#include "../../Source/UI/Sequencer/LassoListeners.cpp"
#include "../../Source/UI/Sequencer/MidiEventComponent.cpp"
#include "../../Source/UI/Sequencer/SelectionComponent.cpp"
#include "../../Source/UI/Sequencer/SequencerLayout.cpp"
#include "../../Source/UI/Themes/HelioTheme.cpp"
#include "../../Source/UI/Themes/Icons.cpp"
#include "../../Source/UI/Themes/ViewportKineticSlider.cpp"
#include "../../Source/UI/MainLayout.cpp"
#include "../../Source/Common.cpp"
