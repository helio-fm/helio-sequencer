# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DDEBUG=1 -D_DEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=1.7.6 -DJUCE_APP_VERSION_HEX=0x10706 $(shell pkg-config --cflags alsa freetype2 libcurl x11 xext xinerama) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/ASIO/common -I../../ThirdParty/JUCE/modules/juce_audio_basics -I../../ThirdParty/JUCE/modules/juce_audio_devices -I../../ThirdParty/JUCE/modules/juce_audio_formats -I../../ThirdParty/JUCE/modules/juce_audio_processors -I../../ThirdParty/JUCE/modules/juce_audio_utils -I../../ThirdParty/JUCE/modules/juce_audio_utils/players -I../../ThirdParty/JUCE/modules/juce_core -I../../ThirdParty/JUCE/modules/juce_cryptography -I../../ThirdParty/JUCE/modules/juce_data_structures -I../../ThirdParty/JUCE/modules/juce_events -I../../ThirdParty/JUCE/modules/juce_graphics -I../../ThirdParty/JUCE/modules/juce_gui_basics -I../../ThirdParty/JUCE/modules/juce_gui_extra -I../../ThirdParty/JUCE/modules/juce_opengl -I../../ThirdParty/JUCE/modules/juce_osc -I../../Source/ -I../../Source/Core/ -I../../Source/Core/App -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Audio/Waveform -I../../Source/Core/Clipboard -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Serialization -I../../Source/Core/Supervisor -I../../Source/Core/Tools -I../../Source/Core/Translation -I../../Source/Core/Tree -I../../Source/Core/Tutorial -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/VCS/Network -I../../Source/UI/ -I../../Source/UI/CodeEditor -I../../Source/UI/CommandPanels -I../../Source/UI/CommandPanels/Base -I../../Source/UI/Common -I../../Source/UI/Common/Meters -I../../Source/UI/Common/Origami -I../../Source/UI/Console -I../../Source/UI/Dialogs -I../../Source/UI/Input -I../../Source/UI/InstrumentsPage -I../../Source/UI/InstrumentsPage/Editor -I../../Source/UI/Intro -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/AnnotationsMap -I../../Source/UI/Sequencer/AutomationMap -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/TimeSignaturesMap -I../../Source/UI/Sequencer/TrackMap -I../../Source/UI/Sequencer/TriggersMap -I../../Source/UI/Sequencer/WaveformMap -I../../Source/UI/Popups -I../../Source/UI/ProjectPage -I../../Source/UI/Rollovers -I../../Source/UI/SettingsPage -I../../Source/UI/Themes -I../../Source/UI/Tree -I../../Source/UI/VCSPage -I../../Source/UI/WorkspacePage -I../../Source/UI/WorkspacePage/Menu -I../../Source/UI/WorkspacePage/Scroller $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 -fpermissive -Wno-unknown-pragmas -Wno-reorder $(CFLAGS)
  JUCE_CXXFLAGS += $(CXXFLAGS) $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl x11 xext xinerama) -lGL -ldl -lpthread -lrt  $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release32)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release32
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -m32
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=1.7.6 -DJUCE_APP_VERSION_HEX=0x10706 $(shell pkg-config --cflags alsa freetype2 libcurl x11 xext xinerama) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/ASIO/common -I../../ThirdParty/JUCE/modules/juce_audio_basics -I../../ThirdParty/JUCE/modules/juce_audio_devices -I../../ThirdParty/JUCE/modules/juce_audio_formats -I../../ThirdParty/JUCE/modules/juce_audio_processors -I../../ThirdParty/JUCE/modules/juce_audio_utils -I../../ThirdParty/JUCE/modules/juce_audio_utils/players -I../../ThirdParty/JUCE/modules/juce_core -I../../ThirdParty/JUCE/modules/juce_cryptography -I../../ThirdParty/JUCE/modules/juce_data_structures -I../../ThirdParty/JUCE/modules/juce_events -I../../ThirdParty/JUCE/modules/juce_graphics -I../../ThirdParty/JUCE/modules/juce_gui_basics -I../../ThirdParty/JUCE/modules/juce_gui_extra -I../../ThirdParty/JUCE/modules/juce_opengl -I../../ThirdParty/JUCE/modules/juce_osc -I../../Source/ -I../../Source/Core/ -I../../Source/Core/App -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Audio/Waveform -I../../Source/Core/Clipboard -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Serialization -I../../Source/Core/Supervisor -I../../Source/Core/Tools -I../../Source/Core/Translation -I../../Source/Core/Tree -I../../Source/Core/Tutorial -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/VCS/Network -I../../Source/UI/ -I../../Source/UI/CodeEditor -I../../Source/UI/CommandPanels -I../../Source/UI/CommandPanels/Base -I../../Source/UI/Common -I../../Source/UI/Common/Meters -I../../Source/UI/Common/Origami -I../../Source/UI/Console -I../../Source/UI/Dialogs -I../../Source/UI/Input -I../../Source/UI/InstrumentsPage -I../../Source/UI/InstrumentsPage/Editor -I../../Source/UI/Intro -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/AnnotationsMap -I../../Source/UI/Sequencer/AutomationMap -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/TimeSignaturesMap -I../../Source/UI/Sequencer/TrackMap -I../../Source/UI/Sequencer/TriggersMap -I../../Source/UI/Sequencer/WaveformMap -I../../Source/UI/Popups -I../../Source/UI/ProjectPage -I../../Source/UI/Rollovers -I../../Source/UI/SettingsPage -I../../Source/UI/Themes -I../../Source/UI/Tree -I../../Source/UI/VCSPage -I../../Source/UI/WorkspacePage -I../../Source/UI/WorkspacePage/Menu -I../../Source/UI/WorkspacePage/Scroller $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 -fpermissive -Wno-unknown-pragmas -Wno-reorder $(CFLAGS)
  JUCE_CXXFLAGS += $(CXXFLAGS) $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -fvisibility=hidden -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl x11 xext xinerama) -lGL -ldl -lpthread -lrt  $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Release64)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release64
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -m64
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=1.7.6 -DJUCE_APP_VERSION_HEX=0x10706 $(shell pkg-config --cflags alsa freetype2 libcurl x11 xext xinerama) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/ASIO/common -I../../ThirdParty/JUCE/modules/juce_audio_basics -I../../ThirdParty/JUCE/modules/juce_audio_devices -I../../ThirdParty/JUCE/modules/juce_audio_formats -I../../ThirdParty/JUCE/modules/juce_audio_processors -I../../ThirdParty/JUCE/modules/juce_audio_utils -I../../ThirdParty/JUCE/modules/juce_audio_utils/players -I../../ThirdParty/JUCE/modules/juce_core -I../../ThirdParty/JUCE/modules/juce_cryptography -I../../ThirdParty/JUCE/modules/juce_data_structures -I../../ThirdParty/JUCE/modules/juce_events -I../../ThirdParty/JUCE/modules/juce_graphics -I../../ThirdParty/JUCE/modules/juce_gui_basics -I../../ThirdParty/JUCE/modules/juce_gui_extra -I../../ThirdParty/JUCE/modules/juce_opengl -I../../ThirdParty/JUCE/modules/juce_osc -I../../Source/ -I../../Source/Core/ -I../../Source/Core/App -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Audio/Waveform -I../../Source/Core/Clipboard -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Serialization -I../../Source/Core/Supervisor -I../../Source/Core/Tools -I../../Source/Core/Translation -I../../Source/Core/Tree -I../../Source/Core/Tutorial -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/VCS/Network -I../../Source/UI/ -I../../Source/UI/CodeEditor -I../../Source/UI/CommandPanels -I../../Source/UI/CommandPanels/Base -I../../Source/UI/Common -I../../Source/UI/Common/Meters -I../../Source/UI/Common/Origami -I../../Source/UI/Console -I../../Source/UI/Dialogs -I../../Source/UI/Input -I../../Source/UI/InstrumentsPage -I../../Source/UI/InstrumentsPage/Editor -I../../Source/UI/Intro -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/AnnotationsMap -I../../Source/UI/Sequencer/AutomationMap -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/TimeSignaturesMap -I../../Source/UI/Sequencer/TrackMap -I../../Source/UI/Sequencer/TriggersMap -I../../Source/UI/Sequencer/WaveformMap -I../../Source/UI/Popups -I../../Source/UI/ProjectPage -I../../Source/UI/Rollovers -I../../Source/UI/SettingsPage -I../../Source/UI/Themes -I../../Source/UI/Tree -I../../Source/UI/VCSPage -I../../Source/UI/WorkspacePage -I../../Source/UI/WorkspacePage/Menu -I../../Source/UI/WorkspacePage/Scroller $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 -fpermissive -Wno-unknown-pragmas -Wno-reorder $(CFLAGS)
  JUCE_CXXFLAGS += $(CXXFLAGS) $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -fvisibility=hidden -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl x11 xext xinerama) -lGL -ldl -lpthread -lrt  $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/App_ab2e8d8c.o \
  $(JUCE_OBJDIR)/Config_bef4c801.o \
  $(JUCE_OBJDIR)/Workspace_7d726580.o \
  $(JUCE_OBJDIR)/BuiltInSynthAudioPlugin_fa4a5d64.o \
  $(JUCE_OBJDIR)/BuiltInSynthFormat_faaea2e6.o \
  $(JUCE_OBJDIR)/BuiltInSynthPiano_eacea884.o \
  $(JUCE_OBJDIR)/InternalPluginFormat_b472d97d.o \
  $(JUCE_OBJDIR)/Instrument_bb3fff74.o \
  $(JUCE_OBJDIR)/OrchestraPit_a67292bb.o \
  $(JUCE_OBJDIR)/PluginManager_3838ab57.o \
  $(JUCE_OBJDIR)/PluginSmartDescription_9dde0bd3.o \
  $(JUCE_OBJDIR)/AudioMonitor_3e55a9cb.o \
  $(JUCE_OBJDIR)/SpectrumAnalyzer_e1c0fa3e.o \
  $(JUCE_OBJDIR)/PlayerThread_2ab68fb.o \
  $(JUCE_OBJDIR)/RendererThread_511aa99d.o \
  $(JUCE_OBJDIR)/Transport_931cdbc3.o \
  $(JUCE_OBJDIR)/AudioCore_ec8fdd75.o \
  $(JUCE_OBJDIR)/InternalClipboard_11ddc6f9.o \
  $(JUCE_OBJDIR)/Clip_5929fe7f.o \
  $(JUCE_OBJDIR)/Pattern_a3a86b8b.o \
  $(JUCE_OBJDIR)/AnnotationEvent_8fa9bdd9.o \
  $(JUCE_OBJDIR)/AutomationEvent_5ea238f1.o \
  $(JUCE_OBJDIR)/MidiEvent_cfd604e7.o \
  $(JUCE_OBJDIR)/Note_42fe2f4e.o \
  $(JUCE_OBJDIR)/TimeSignatureEvent_5ddd998b.o \
  $(JUCE_OBJDIR)/AnnotationsSequence_1997bf8b.o \
  $(JUCE_OBJDIR)/AutomationSequence_84d9de3c.o \
  $(JUCE_OBJDIR)/MidiSequence_310d4486.o \
  $(JUCE_OBJDIR)/PianoSequence_e11a82f0.o \
  $(JUCE_OBJDIR)/TimeSignaturesSequence_5fa7c98d.o \
  $(JUCE_OBJDIR)/MidiTrack_6604020d.o \
  $(JUCE_OBJDIR)/AuthorizationManager_a8e59c6.o \
  $(JUCE_OBJDIR)/LoginThread_c2baf4b.o \
  $(JUCE_OBJDIR)/LogoutThread_1a2f6a06.o \
  $(JUCE_OBJDIR)/RequestArpeggiatorsThread_91988aab.o \
  $(JUCE_OBJDIR)/RequestColourSchemesThread_2433d82f.o \
  $(JUCE_OBJDIR)/RequestProjectsListThread_81e40189.o \
  $(JUCE_OBJDIR)/RequestTranslationsThread_cb9ae8b3.o \
  $(JUCE_OBJDIR)/UpdateManager_ab904ddc.o \
  $(JUCE_OBJDIR)/Autosaver_8ecb1540.o \
  $(JUCE_OBJDIR)/DataEncoder_3334e5cc.o \
  $(JUCE_OBJDIR)/Document_25ea426b.o \
  $(JUCE_OBJDIR)/FileUtils_5b02c80f.o \
  $(JUCE_OBJDIR)/Session_c2023840.o \
  $(JUCE_OBJDIR)/SessionManager_6d9673d7.o \
  $(JUCE_OBJDIR)/Supervisor_a07f8408.o \
  $(JUCE_OBJDIR)/Arpeggiator_36cea7de.o \
  $(JUCE_OBJDIR)/ArpeggiatorsManager_e69dd198.o \
  $(JUCE_OBJDIR)/ColourScheme_28dce8d6.o \
  $(JUCE_OBJDIR)/ColourSchemeManager_2a460e81.o \
  $(JUCE_OBJDIR)/TranslationManager_62b89deb.o \
  $(JUCE_OBJDIR)/RecentFilesList_3a41b07a.o \
  $(JUCE_OBJDIR)/AudioPluginTreeItem_b465d4fa.o \
  $(JUCE_OBJDIR)/AutomationTrackTreeItem_d8af2c5.o \
  $(JUCE_OBJDIR)/InstrumentsRootTreeItem_74c5c1bf.o \
  $(JUCE_OBJDIR)/InstrumentTreeItem_fc13b882.o \
  $(JUCE_OBJDIR)/MidiTrackTreeItem_1dfb63b.o \
  $(JUCE_OBJDIR)/PianoTrackTreeItem_850d2d9d.o \
  $(JUCE_OBJDIR)/ProjectInfo_52f6a347.o \
  $(JUCE_OBJDIR)/ProjectTimeline_9655ff5a.o \
  $(JUCE_OBJDIR)/ProjectTreeItem_334546ea.o \
  $(JUCE_OBJDIR)/RootTreeItem_3b150bd.o \
  $(JUCE_OBJDIR)/SettingsTreeItem_c1194ffe.o \
  $(JUCE_OBJDIR)/TrackGroupTreeItem_8371d50f.o \
  $(JUCE_OBJDIR)/TreeItem_3983613b.o \
  $(JUCE_OBJDIR)/TreeItemChildrenSerializer_7365544c.o \
  $(JUCE_OBJDIR)/VersionControlTreeItem_f871c280.o \
  $(JUCE_OBJDIR)/AnnotationEventActions_e2dc4268.o \
  $(JUCE_OBJDIR)/AutomationTrackActions_7e56b4ff.o \
  $(JUCE_OBJDIR)/AutomationEventActions_ba8f9c50.o \
  $(JUCE_OBJDIR)/MidiTrackActions_47b06bc9.o \
  $(JUCE_OBJDIR)/NoteActions_b5a2e99f.o \
  $(JUCE_OBJDIR)/PatternActions_60a33363.o \
  $(JUCE_OBJDIR)/PianoTrackActions_78338acf.o \
  $(JUCE_OBJDIR)/TimeSignatureEventActions_c6f6be42.o \
  $(JUCE_OBJDIR)/UndoStack_c8cfe6ea.o \
  $(JUCE_OBJDIR)/AutomationLayerDiffLogic_5a3fe36f.o \
  $(JUCE_OBJDIR)/DiffLogic_e39316b3.o \
  $(JUCE_OBJDIR)/PatternDiffLogic_3ab83d19.o \
  $(JUCE_OBJDIR)/PianoLayerDiffLogic_a6808bcb.o \
  $(JUCE_OBJDIR)/ProjectInfoDiffLogic_85d6d922.o \
  $(JUCE_OBJDIR)/ProjectTimelineDiffLogic_dd926f6f.o \
  $(JUCE_OBJDIR)/PullThread_38e0532a.o \
  $(JUCE_OBJDIR)/PushThread_1b290dbf.o \
  $(JUCE_OBJDIR)/RemovalThread_1f5e1bc5.o \
  $(JUCE_OBJDIR)/SyncThread_6bdecc00.o \
  $(JUCE_OBJDIR)/Client_8a2475a5.o \
  $(JUCE_OBJDIR)/Delta_dc1eed28.o \
  $(JUCE_OBJDIR)/Diff_3af4c61f.o \
  $(JUCE_OBJDIR)/Head_fe3c227a.o \
  $(JUCE_OBJDIR)/HeadState_a30bfa01.o \
  $(JUCE_OBJDIR)/Pack_6d78f233.o \
  $(JUCE_OBJDIR)/Revision_ddbb1c75.o \
  $(JUCE_OBJDIR)/RevisionItem_7e6e5a28.o \
  $(JUCE_OBJDIR)/StashesRepository_bb52fdfd.o \
  $(JUCE_OBJDIR)/VersionControl_bc67ed3f.o \
  $(JUCE_OBJDIR)/CommandItemComponent_3ac71cb6.o \
  $(JUCE_OBJDIR)/CommandItemComponentMarker_7a059b50.o \
  $(JUCE_OBJDIR)/CommandPanel_2ecd6990.o \
  $(JUCE_OBJDIR)/AnnotationCommandPanel_e55a5d39.o \
  $(JUCE_OBJDIR)/ArpeggiatorEditorPanel_c93e430f.o \
  $(JUCE_OBJDIR)/ArpeggiatorPanel_4c71683c.o \
  $(JUCE_OBJDIR)/AutomationsCommandPanel_586f254e.o \
  $(JUCE_OBJDIR)/HybridRollCommandPanel_2c6356a3.o \
  $(JUCE_OBJDIR)/HybridRollCommandPanelDefault_37094ca8.o \
  $(JUCE_OBJDIR)/HybridRollCommandPanelPhone_763ab9d5.o \
  $(JUCE_OBJDIR)/InstrumentCommandPanel_d55f65f1.o \
  $(JUCE_OBJDIR)/InstrumentsCommandPanel_4a53507e.o \
  $(JUCE_OBJDIR)/LayerCommandPanel_9b3a6563.o \
  $(JUCE_OBJDIR)/MoveToLayerCommandPanel_e3d52f97.o \
  $(JUCE_OBJDIR)/NotesTuningPanel_eadd4fbf.o \
  $(JUCE_OBJDIR)/ProjectCommandPanel_e4b1ca2b.o \
  $(JUCE_OBJDIR)/RootTreeItemPanelDefault_73fae601.o \
  $(JUCE_OBJDIR)/RootTreeItemPanelCompact_5cad09e3.o \
  $(JUCE_OBJDIR)/TimelineCommandPanel_5042c14b.o \
  $(JUCE_OBJDIR)/TimeSignatureCommandPanel_f89d47d.o \
  $(JUCE_OBJDIR)/VCSCommandPanel_f3c04a78.o \
  $(JUCE_OBJDIR)/SpectrumComponent_4605e8d9.o \
  $(JUCE_OBJDIR)/VolumeComponent_ff6a9630.o \
  $(JUCE_OBJDIR)/Origami_677a1bc1.o \
  $(JUCE_OBJDIR)/OrigamiHorizontal_4d5ae305.o \
  $(JUCE_OBJDIR)/OrigamiVertical_9fe96057.o \
  $(JUCE_OBJDIR)/ColourButton_d92c95b0.o \
  $(JUCE_OBJDIR)/ColourSwatches_e2a4628.o \
  $(JUCE_OBJDIR)/DraggingListBoxComponent_34f40031.o \
  $(JUCE_OBJDIR)/FatalErrorScreen_2f541f02.o \
  $(JUCE_OBJDIR)/MenuButton_1d9ba4c3.o \
  $(JUCE_OBJDIR)/PlayButton_38579458.o \
  $(JUCE_OBJDIR)/PluginWindow_66751815.o \
  $(JUCE_OBJDIR)/SizeSwitcherComponent_cef9f113.o \
  $(JUCE_OBJDIR)/SpectralLogo_f6755a41.o \
  $(JUCE_OBJDIR)/ViewportFitProxyComponent_a93fb792.o \
  $(JUCE_OBJDIR)/AnnotationDialog_bb76a597.o \
  $(JUCE_OBJDIR)/AuthorizationDialog_ebcfd46b.o \
  $(JUCE_OBJDIR)/FadingDialog_905af7a1.o \
  $(JUCE_OBJDIR)/ModalDialogConfirmation_23a5c174.o \
  $(JUCE_OBJDIR)/ModalDialogInput_6af917b5.o \
  $(JUCE_OBJDIR)/RenderDialog_d54ea4be.o \
  $(JUCE_OBJDIR)/TimeSignatureDialog_7d6669dd.o \
  $(JUCE_OBJDIR)/UpdateDialog_9c0a7831.o \
  $(JUCE_OBJDIR)/MultiTouchController_63afb603.o \
  $(JUCE_OBJDIR)/AudioPluginEditorPage_409ff4b3.o \
  $(JUCE_OBJDIR)/InstrumentEditor_78eb55f0.o \
  $(JUCE_OBJDIR)/InstrumentEditorConnector_8c32787.o \
  $(JUCE_OBJDIR)/InstrumentEditorNode_28608192.o \
  $(JUCE_OBJDIR)/InstrumentEditorPin_92dde88f.o \
  $(JUCE_OBJDIR)/InstrumentRow_21b66d6d.o \
  $(JUCE_OBJDIR)/InstrumentsPage_ebd7d5d5.o \
  $(JUCE_OBJDIR)/InitScreen_452f50fb.o \
  $(JUCE_OBJDIR)/ChordTooltip_d9bc553d.o \
  $(JUCE_OBJDIR)/FailTooltip_99f74519.o \
  $(JUCE_OBJDIR)/HelioCallout_90a2f131.o \
  $(JUCE_OBJDIR)/NoNotesPopup_ca8bfdc2.o \
  $(JUCE_OBJDIR)/PopupButton_5c5e0092.o \
  $(JUCE_OBJDIR)/PopupCustomButton_6d33e2a3.o \
  $(JUCE_OBJDIR)/PopupImageButton_54133d7.o \
  $(JUCE_OBJDIR)/ProgressTooltip_f470232a.o \
  $(JUCE_OBJDIR)/SuccessTooltip_7d75a6f6.o \
  $(JUCE_OBJDIR)/TooltipContainer_52ddacf4.o \
  $(JUCE_OBJDIR)/RolloverBackButtonLeft_f3afb1a.o \
  $(JUCE_OBJDIR)/RolloverBackButtonRight_1571eeb3.o \
  $(JUCE_OBJDIR)/RolloverHeaderLeft_31f5252e.o \
  $(JUCE_OBJDIR)/RolloverHeaderRight_49fd071f.o \
  $(JUCE_OBJDIR)/RolloverContainer_684b65d1.o \
  $(JUCE_OBJDIR)/ProjectRollover_298ec02b.o \
  $(JUCE_OBJDIR)/SettingsRollover_b0b344dd.o \
  $(JUCE_OBJDIR)/ProjectPage_591022af.o \
  $(JUCE_OBJDIR)/ProjectPageDefault_ff2d1b1c.o \
  $(JUCE_OBJDIR)/ProjectPagePhone_86279549.o \
  $(JUCE_OBJDIR)/AnnotationLargeComponent_cd4cf278.o \
  $(JUCE_OBJDIR)/AnnotationSmallComponent_e8ea642c.o \
  $(JUCE_OBJDIR)/AnnotationsTrackMap_971ca518.o \
  $(JUCE_OBJDIR)/AutomationCurveHelper_e99f382c.o \
  $(JUCE_OBJDIR)/AutomationEventComponent_7ed837fe.o \
  $(JUCE_OBJDIR)/AutomationEventsConnector_dae8c963.o \
  $(JUCE_OBJDIR)/AutomationTrackMap_823a4a0c.o \
  $(JUCE_OBJDIR)/HeaderSelectionIndicator_5aed2562.o \
  $(JUCE_OBJDIR)/HybridRollHeader_e7c11898.o \
  $(JUCE_OBJDIR)/Playhead_49aa3d26.o \
  $(JUCE_OBJDIR)/SoundProbeIndicator_fa380566.o \
  $(JUCE_OBJDIR)/TimeDistanceIndicator_e1c13445.o \
  $(JUCE_OBJDIR)/TrackEndIndicator_698c6d7.o \
  $(JUCE_OBJDIR)/TrackStartIndicator_fa80cb30.o \
  $(JUCE_OBJDIR)/ComponentConnectorCurve_50e8f885.o \
  $(JUCE_OBJDIR)/HybridRollExpandMark_c3022404.o \
  $(JUCE_OBJDIR)/InsertSpaceHelper_7c318421.o \
  $(JUCE_OBJDIR)/NoteResizerLeft_c91c2c99.o \
  $(JUCE_OBJDIR)/NoteResizerRight_97b6ed14.o \
  $(JUCE_OBJDIR)/TimelineWarningMarker_6d5c36fb.o \
  $(JUCE_OBJDIR)/WipeSpaceHelper_16b49913.o \
  $(JUCE_OBJDIR)/ClipComponent_3c63e97b.o \
  $(JUCE_OBJDIR)/AutomationClipComponent_ecbdf1e4.o \
  $(JUCE_OBJDIR)/DummyClipComponent_76f3c1e1.o \
  $(JUCE_OBJDIR)/PianoClipComponent_726e7f60.o \
  $(JUCE_OBJDIR)/PatternRoll_9707f85b.o \
  $(JUCE_OBJDIR)/NoteComponent_465c17d2.o \
  $(JUCE_OBJDIR)/PianoRoll_c125538d.o \
  $(JUCE_OBJDIR)/PianoRollToolbox_31a5c270.o \
  $(JUCE_OBJDIR)/TimeSignatureLargeComponent_3afd89e2.o \
  $(JUCE_OBJDIR)/TimeSignatureSmallComponent_569afb96.o \
  $(JUCE_OBJDIR)/TimeSignaturesTrackMap_1611936e.o \
  $(JUCE_OBJDIR)/PianoTrackMap_ab9c6236.o \
  $(JUCE_OBJDIR)/TrackScroller_af8066e1.o \
  $(JUCE_OBJDIR)/TrackScrollerScreen_d46f1c6d.o \
  $(JUCE_OBJDIR)/TriggerEventComponent_f070f9bd.o \
  $(JUCE_OBJDIR)/TriggerEventConnector_767fa90d.o \
  $(JUCE_OBJDIR)/TriggersTrackMap_68437e54.o \
  $(JUCE_OBJDIR)/HybridLassoComponent_4ff655d9.o \
  $(JUCE_OBJDIR)/HybridRoll_b60b10f3.o \
  $(JUCE_OBJDIR)/HybridRollEditMode_46b31f60.o \
  $(JUCE_OBJDIR)/HybridRollEventComponent_f733a696.o \
  $(JUCE_OBJDIR)/SequencerLayout_58e2f98b.o \
  $(JUCE_OBJDIR)/AudioSettings_1a236878.o \
  $(JUCE_OBJDIR)/AuthorizationSettings_5e75069b.o \
  $(JUCE_OBJDIR)/ComponentsList_495b669f.o \
  $(JUCE_OBJDIR)/IntroSettingsWrapper_f70e382f.o \
  $(JUCE_OBJDIR)/LabeledSettingsWrapper_6cf0db28.o \
  $(JUCE_OBJDIR)/LogComponent_1a5662e4.o \
  $(JUCE_OBJDIR)/OpenGLSettings_9ebd40bd.o \
  $(JUCE_OBJDIR)/PluginsList_f25f06bd.o \
  $(JUCE_OBJDIR)/SettingsPage_f68ee19d.o \
  $(JUCE_OBJDIR)/SettingsListItemHighlighter_4714088c.o \
  $(JUCE_OBJDIR)/SettingsListItemSelection_aeb4b5b7.o \
  $(JUCE_OBJDIR)/ThemeSettings_328aedeb.o \
  $(JUCE_OBJDIR)/ThemeSettingsItem_5b6c809e.o \
  $(JUCE_OBJDIR)/TranslationSettings_46da0753.o \
  $(JUCE_OBJDIR)/TranslationSettingsItem_1640a606.o \
  $(JUCE_OBJDIR)/ComponentFader_6572b0c.o \
  $(JUCE_OBJDIR)/FontSerializer_ad448bd4.o \
  $(JUCE_OBJDIR)/GradientVertical_67252d59.o \
  $(JUCE_OBJDIR)/GradientVerticalReversed_56a2f63b.o \
  $(JUCE_OBJDIR)/HelioTheme_d6ec9067.o \
  $(JUCE_OBJDIR)/Icons_7879bcb1.o \
  $(JUCE_OBJDIR)/LighterShadowDownwards_5a0a0425.o \
  $(JUCE_OBJDIR)/LighterShadowUpwards_3589fd0c.o \
  $(JUCE_OBJDIR)/LightShadowDownwards_329253f2.o \
  $(JUCE_OBJDIR)/LightShadowLeftwards_3042da2d.o \
  $(JUCE_OBJDIR)/LightShadowRightwards_b4145968.o \
  $(JUCE_OBJDIR)/LightShadowUpwards_7f8ddc19.o \
  $(JUCE_OBJDIR)/PanelA_cd3f5b50.o \
  $(JUCE_OBJDIR)/PanelB_cd4d72d1.o \
  $(JUCE_OBJDIR)/PanelBackgroundA_49900242.o \
  $(JUCE_OBJDIR)/PanelBackgroundB_499e19c3.o \
  $(JUCE_OBJDIR)/PanelBackgroundC_49ac3144.o \
  $(JUCE_OBJDIR)/PanelC_cd5b8a52.o \
  $(JUCE_OBJDIR)/SeparatorHorizontal_240c2780.o \
  $(JUCE_OBJDIR)/SeparatorHorizontalReversed_385cd962.o \
  $(JUCE_OBJDIR)/SeparatorHorizontalFading_a0277f19.o \
  $(JUCE_OBJDIR)/SeparatorHorizontalFadingReversed_481787fb.o \
  $(JUCE_OBJDIR)/SeparatorVertical_65878792.o \
  $(JUCE_OBJDIR)/SeparatorVerticalReversed_e5480774.o \
  $(JUCE_OBJDIR)/ShadeDark_2a393f5a.o \
  $(JUCE_OBJDIR)/ShadeLight_b6da9a7c.o \
  $(JUCE_OBJDIR)/ShadowDownwards_29eb3a6c.o \
  $(JUCE_OBJDIR)/ShadowHorizontalFading_a0923750.o \
  $(JUCE_OBJDIR)/ShadowLeftwards_279bc0a7.o \
  $(JUCE_OBJDIR)/ShadowRightwards_a7d8422e.o \
  $(JUCE_OBJDIR)/ShadowUpwards_83021913.o \
  $(JUCE_OBJDIR)/ViewportKineticSlider_45304cab.o \
  $(JUCE_OBJDIR)/TreeItemComponent_45f5f057.o \
  $(JUCE_OBJDIR)/TreeItemComponentDefault_335c2074.o \
  $(JUCE_OBJDIR)/TreeItemComponentCompact_1c0e4456.o \
  $(JUCE_OBJDIR)/TreeItemMarkerDefault_f4465881.o \
  $(JUCE_OBJDIR)/TreeItemMarkerCompact_dcf87c63.o \
  $(JUCE_OBJDIR)/TreeItemMenuButton_55ef6521.o \
  $(JUCE_OBJDIR)/TreePanel_62366e31.o \
  $(JUCE_OBJDIR)/TreePanelDefault_37c5395a.o \
  $(JUCE_OBJDIR)/TreePanelPhone_1e0e2b07.o \
  $(JUCE_OBJDIR)/HistoryComponent_76e9715f.o \
  $(JUCE_OBJDIR)/RevisionComponent_9dbc4cd6.o \
  $(JUCE_OBJDIR)/RevisionConnectorComponent_c98d6ec1.o \
  $(JUCE_OBJDIR)/RevisionItemComponent_84696683.o \
  $(JUCE_OBJDIR)/RevisionTooltipComponent_7c2a51cb.o \
  $(JUCE_OBJDIR)/RevisionTreeComponent_29a3e7d8.o \
  $(JUCE_OBJDIR)/StageComponent_e8287375.o \
  $(JUCE_OBJDIR)/VersionControlEditor_b096a8c8.o \
  $(JUCE_OBJDIR)/VersionControlEditorDefault_476136e3.o \
  $(JUCE_OBJDIR)/VersionControlEditorPhone_c66df6d0.o \
  $(JUCE_OBJDIR)/VersionControlEditorPhoneViewportContent_138dc88d.o \
  $(JUCE_OBJDIR)/CreateProjectRow_8ad7b390.o \
  $(JUCE_OBJDIR)/OpenProjectRow_dead0fde.o \
  $(JUCE_OBJDIR)/RecentProjectRow_5fa006af.o \
  $(JUCE_OBJDIR)/SignInRow_bc7b85ef.o \
  $(JUCE_OBJDIR)/WorkspaceMenu_2e4ba3cb.o \
  $(JUCE_OBJDIR)/LogoFader_7cac1f2e.o \
  $(JUCE_OBJDIR)/LogoImage_7c075493.o \
  $(JUCE_OBJDIR)/WorkspacePage_4e414de7.o \
  $(JUCE_OBJDIR)/MainLayout_4f58d3f.o \
  $(JUCE_OBJDIR)/MainWindow_5f9c9b05.o \
  $(JUCE_OBJDIR)/BinaryData_7821001c.o \
  $(JUCE_OBJDIR)/BinaryData2_8c1f1960.o \
  $(JUCE_OBJDIR)/BinaryData3_8c2d30e1.o \
  $(JUCE_OBJDIR)/BinaryData4_8c3b4862.o \
  $(JUCE_OBJDIR)/BinaryData5_8c495fe3.o \
  $(JUCE_OBJDIR)/BinaryData6_8c577764.o \
  $(JUCE_OBJDIR)/BinaryData7_8c658ee5.o \
  $(JUCE_OBJDIR)/BinaryData8_8c73a666.o \
  $(JUCE_OBJDIR)/BinaryData9_8c81bde7.o \
  $(JUCE_OBJDIR)/BinaryData10_f61320fb.o \
  $(JUCE_OBJDIR)/include_juce_audio_basics_68629202.o \
  $(JUCE_OBJDIR)/include_juce_audio_devices_477c5a4a.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_fa635d49.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_6c50941e.o \
  $(JUCE_OBJDIR)/include_juce_audio_utils_a6c9a21e.o \
  $(JUCE_OBJDIR)/include_juce_core_9b124993.o \
  $(JUCE_OBJDIR)/include_juce_cryptography_6acc0160.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_eb02bd2b.o \
  $(JUCE_OBJDIR)/include_juce_events_23f77a4d.o \
  $(JUCE_OBJDIR)/include_juce_graphics_82dd76ff.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_d3ae9f3d.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_3bdb3d62.o \
  $(JUCE_OBJDIR)/include_juce_opengl_1ea9a6e3.o \
  $(JUCE_OBJDIR)/include_juce_osc_7d711795.o \

.PHONY: clean all

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : check-pkg-config $(OBJECTS_APP) $(RESOURCES)
	@echo Linking "Helio - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/App_ab2e8d8c.o: ../../Source/Core/App/App.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling App.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Config_bef4c801.o: ../../Source/Core/App/Config.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Config.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Workspace_7d726580.o: ../../Source/Core/App/Workspace.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Workspace.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BuiltInSynthAudioPlugin_fa4a5d64.o: ../../Source/Core/Audio/BuiltIn/BuiltInSynthAudioPlugin.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BuiltInSynthAudioPlugin.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BuiltInSynthFormat_faaea2e6.o: ../../Source/Core/Audio/BuiltIn/BuiltInSynthFormat.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BuiltInSynthFormat.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BuiltInSynthPiano_eacea884.o: ../../Source/Core/Audio/BuiltIn/BuiltInSynthPiano.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BuiltInSynthPiano.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InternalPluginFormat_b472d97d.o: ../../Source/Core/Audio/BuiltIn/InternalPluginFormat.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InternalPluginFormat.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Instrument_bb3fff74.o: ../../Source/Core/Audio/Instruments/Instrument.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Instrument.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OrchestraPit_a67292bb.o: ../../Source/Core/Audio/Instruments/OrchestraPit.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OrchestraPit.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginManager_3838ab57.o: ../../Source/Core/Audio/Instruments/PluginManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PluginManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginSmartDescription_9dde0bd3.o: ../../Source/Core/Audio/Instruments/PluginSmartDescription.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PluginSmartDescription.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioMonitor_3e55a9cb.o: ../../Source/Core/Audio/Monitoring/AudioMonitor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioMonitor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpectrumAnalyzer_e1c0fa3e.o: ../../Source/Core/Audio/Monitoring/SpectrumAnalyzer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SpectrumAnalyzer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PlayerThread_2ab68fb.o: ../../Source/Core/Audio/Transport/PlayerThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PlayerThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RendererThread_511aa99d.o: ../../Source/Core/Audio/Transport/RendererThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RendererThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Transport_931cdbc3.o: ../../Source/Core/Audio/Transport/Transport.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Transport.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioCore_ec8fdd75.o: ../../Source/Core/Audio/AudioCore.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioCore.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InternalClipboard_11ddc6f9.o: ../../Source/Core/Clipboard/InternalClipboard.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InternalClipboard.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Clip_5929fe7f.o: ../../Source/Core/Midi/Patterns/Clip.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Clip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Pattern_a3a86b8b.o: ../../Source/Core/Midi/Patterns/Pattern.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Pattern.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationEvent_8fa9bdd9.o: ../../Source/Core/Midi/Sequences/Events/AnnotationEvent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationEvent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationEvent_5ea238f1.o: ../../Source/Core/Midi/Sequences/Events/AutomationEvent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationEvent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiEvent_cfd604e7.o: ../../Source/Core/Midi/Sequences/Events/MidiEvent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiEvent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Note_42fe2f4e.o: ../../Source/Core/Midi/Sequences/Events/Note.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Note.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureEvent_5ddd998b.o: ../../Source/Core/Midi/Sequences/Events/TimeSignatureEvent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureEvent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationsSequence_1997bf8b.o: ../../Source/Core/Midi/Sequences/AnnotationsSequence.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationsSequence.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationSequence_84d9de3c.o: ../../Source/Core/Midi/Sequences/AutomationSequence.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationSequence.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiSequence_310d4486.o: ../../Source/Core/Midi/Sequences/MidiSequence.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiSequence.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoSequence_e11a82f0.o: ../../Source/Core/Midi/Sequences/PianoSequence.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoSequence.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignaturesSequence_5fa7c98d.o: ../../Source/Core/Midi/Sequences/TimeSignaturesSequence.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignaturesSequence.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiTrack_6604020d.o: ../../Source/Core/Midi/MidiTrack.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiTrack.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AuthorizationManager_a8e59c6.o: ../../Source/Core/Network/AuthorizationManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AuthorizationManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LoginThread_c2baf4b.o: ../../Source/Core/Network/LoginThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LoginThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogoutThread_1a2f6a06.o: ../../Source/Core/Network/LogoutThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LogoutThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RequestArpeggiatorsThread_91988aab.o: ../../Source/Core/Network/RequestArpeggiatorsThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RequestArpeggiatorsThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RequestColourSchemesThread_2433d82f.o: ../../Source/Core/Network/RequestColourSchemesThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RequestColourSchemesThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RequestProjectsListThread_81e40189.o: ../../Source/Core/Network/RequestProjectsListThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RequestProjectsListThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RequestTranslationsThread_cb9ae8b3.o: ../../Source/Core/Network/RequestTranslationsThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RequestTranslationsThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UpdateManager_ab904ddc.o: ../../Source/Core/Network/UpdateManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UpdateManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Autosaver_8ecb1540.o: ../../Source/Core/Serialization/Autosaver.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Autosaver.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DataEncoder_3334e5cc.o: ../../Source/Core/Serialization/DataEncoder.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DataEncoder.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Document_25ea426b.o: ../../Source/Core/Serialization/Document.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Document.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FileUtils_5b02c80f.o: ../../Source/Core/Serialization/FileUtils.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FileUtils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Session_c2023840.o: ../../Source/Core/Supervisor/Session.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Session.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SessionManager_6d9673d7.o: ../../Source/Core/Supervisor/SessionManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SessionManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Supervisor_a07f8408.o: ../../Source/Core/Supervisor/Supervisor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Supervisor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Arpeggiator_36cea7de.o: ../../Source/Core/Tools/Arpeggiator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Arpeggiator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ArpeggiatorsManager_e69dd198.o: ../../Source/Core/Tools/ArpeggiatorsManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ArpeggiatorsManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColourScheme_28dce8d6.o: ../../Source/Core/Tools/ColourScheme.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColourScheme.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColourSchemeManager_2a460e81.o: ../../Source/Core/Tools/ColourSchemeManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColourSchemeManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TranslationManager_62b89deb.o: ../../Source/Core/Translation/TranslationManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TranslationManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RecentFilesList_3a41b07a.o: ../../Source/Core/Tree/RecentFilesList.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RecentFilesList.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioPluginTreeItem_b465d4fa.o: ../../Source/Core/Tree/AudioPluginTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioPluginTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationTrackTreeItem_d8af2c5.o: ../../Source/Core/Tree/AutomationTrackTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationTrackTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentsRootTreeItem_74c5c1bf.o: ../../Source/Core/Tree/InstrumentsRootTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentsRootTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentTreeItem_fc13b882.o: ../../Source/Core/Tree/InstrumentTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiTrackTreeItem_1dfb63b.o: ../../Source/Core/Tree/MidiTrackTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiTrackTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoTrackTreeItem_850d2d9d.o: ../../Source/Core/Tree/PianoTrackTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoTrackTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectInfo_52f6a347.o: ../../Source/Core/Tree/ProjectInfo.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectInfo.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectTimeline_9655ff5a.o: ../../Source/Core/Tree/ProjectTimeline.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectTimeline.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectTreeItem_334546ea.o: ../../Source/Core/Tree/ProjectTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RootTreeItem_3b150bd.o: ../../Source/Core/Tree/RootTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RootTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsTreeItem_c1194ffe.o: ../../Source/Core/Tree/SettingsTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackGroupTreeItem_8371d50f.o: ../../Source/Core/Tree/TrackGroupTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TrackGroupTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItem_3983613b.o: ../../Source/Core/Tree/TreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemChildrenSerializer_7365544c.o: ../../Source/Core/Tree/TreeItemChildrenSerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemChildrenSerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlTreeItem_f871c280.o: ../../Source/Core/Tree/VersionControlTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlTreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationEventActions_e2dc4268.o: ../../Source/Core/Undo/Actions/AnnotationEventActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationEventActions.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationTrackActions_7e56b4ff.o: ../../Source/Core/Undo/Actions/AutomationTrackActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationTrackActions.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationEventActions_ba8f9c50.o: ../../Source/Core/Undo/Actions/AutomationEventActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationEventActions.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiTrackActions_47b06bc9.o: ../../Source/Core/Undo/Actions/MidiTrackActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiTrackActions.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoteActions_b5a2e99f.o: ../../Source/Core/Undo/Actions/NoteActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoteActions.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternActions_60a33363.o: ../../Source/Core/Undo/Actions/PatternActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternActions.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoTrackActions_78338acf.o: ../../Source/Core/Undo/Actions/PianoTrackActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoTrackActions.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureEventActions_c6f6be42.o: ../../Source/Core/Undo/Actions/TimeSignatureEventActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureEventActions.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UndoStack_c8cfe6ea.o: ../../Source/Core/Undo/UndoStack.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UndoStack.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationLayerDiffLogic_5a3fe36f.o: ../../Source/Core/VCS/DiffLogic/AutomationLayerDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationLayerDiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DiffLogic_e39316b3.o: ../../Source/Core/VCS/DiffLogic/DiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternDiffLogic_3ab83d19.o: ../../Source/Core/VCS/DiffLogic/PatternDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternDiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoLayerDiffLogic_a6808bcb.o: ../../Source/Core/VCS/DiffLogic/PianoLayerDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoLayerDiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectInfoDiffLogic_85d6d922.o: ../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectInfoDiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectTimelineDiffLogic_dd926f6f.o: ../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectTimelineDiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PullThread_38e0532a.o: ../../Source/Core/VCS/Network/PullThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PullThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PushThread_1b290dbf.o: ../../Source/Core/VCS/Network/PushThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PushThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RemovalThread_1f5e1bc5.o: ../../Source/Core/VCS/Network/RemovalThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RemovalThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SyncThread_6bdecc00.o: ../../Source/Core/VCS/Network/SyncThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SyncThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Client_8a2475a5.o: ../../Source/Core/VCS/Client.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Client.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Delta_dc1eed28.o: ../../Source/Core/VCS/Delta.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Delta.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Diff_3af4c61f.o: ../../Source/Core/VCS/Diff.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Diff.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Head_fe3c227a.o: ../../Source/Core/VCS/Head.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Head.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HeadState_a30bfa01.o: ../../Source/Core/VCS/HeadState.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HeadState.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Pack_6d78f233.o: ../../Source/Core/VCS/Pack.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Pack.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Revision_ddbb1c75.o: ../../Source/Core/VCS/Revision.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Revision.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionItem_7e6e5a28.o: ../../Source/Core/VCS/RevisionItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/StashesRepository_bb52fdfd.o: ../../Source/Core/VCS/StashesRepository.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling StashesRepository.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControl_bc67ed3f.o: ../../Source/Core/VCS/VersionControl.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandItemComponent_3ac71cb6.o: ../../Source/UI/CommandPanels/Base/CommandItemComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CommandItemComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandItemComponentMarker_7a059b50.o: ../../Source/UI/CommandPanels/Base/CommandItemComponentMarker.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CommandItemComponentMarker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandPanel_2ecd6990.o: ../../Source/UI/CommandPanels/Base/CommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationCommandPanel_e55a5d39.o: ../../Source/UI/CommandPanels/AnnotationCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ArpeggiatorEditorPanel_c93e430f.o: ../../Source/UI/CommandPanels/ArpeggiatorEditorPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ArpeggiatorEditorPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ArpeggiatorPanel_4c71683c.o: ../../Source/UI/CommandPanels/ArpeggiatorPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ArpeggiatorPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationsCommandPanel_586f254e.o: ../../Source/UI/CommandPanels/AutomationsCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationsCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollCommandPanel_2c6356a3.o: ../../Source/UI/CommandPanels/HybridRollCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollCommandPanelDefault_37094ca8.o: ../../Source/UI/CommandPanels/HybridRollCommandPanelDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollCommandPanelDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollCommandPanelPhone_763ab9d5.o: ../../Source/UI/CommandPanels/HybridRollCommandPanelPhone.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollCommandPanelPhone.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentCommandPanel_d55f65f1.o: ../../Source/UI/CommandPanels/InstrumentCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentsCommandPanel_4a53507e.o: ../../Source/UI/CommandPanels/InstrumentsCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentsCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LayerCommandPanel_9b3a6563.o: ../../Source/UI/CommandPanels/LayerCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LayerCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MoveToLayerCommandPanel_e3d52f97.o: ../../Source/UI/CommandPanels/MoveToLayerCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MoveToLayerCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NotesTuningPanel_eadd4fbf.o: ../../Source/UI/CommandPanels/NotesTuningPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NotesTuningPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectCommandPanel_e4b1ca2b.o: ../../Source/UI/CommandPanels/ProjectCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RootTreeItemPanelDefault_73fae601.o: ../../Source/UI/CommandPanels/RootTreeItemPanelDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RootTreeItemPanelDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RootTreeItemPanelCompact_5cad09e3.o: ../../Source/UI/CommandPanels/RootTreeItemPanelCompact.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RootTreeItemPanelCompact.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineCommandPanel_5042c14b.o: ../../Source/UI/CommandPanels/TimelineCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureCommandPanel_f89d47d.o: ../../Source/UI/CommandPanels/TimeSignatureCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VCSCommandPanel_f3c04a78.o: ../../Source/UI/CommandPanels/VCSCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VCSCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpectrumComponent_4605e8d9.o: ../../Source/UI/Common/Meters/SpectrumComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SpectrumComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VolumeComponent_ff6a9630.o: ../../Source/UI/Common/Meters/VolumeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VolumeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Origami_677a1bc1.o: ../../Source/UI/Common/Origami/Origami.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Origami.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OrigamiHorizontal_4d5ae305.o: ../../Source/UI/Common/Origami/OrigamiHorizontal.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OrigamiHorizontal.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OrigamiVertical_9fe96057.o: ../../Source/UI/Common/Origami/OrigamiVertical.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OrigamiVertical.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColourButton_d92c95b0.o: ../../Source/UI/Common/ColourButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColourButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColourSwatches_e2a4628.o: ../../Source/UI/Common/ColourSwatches.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColourSwatches.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DraggingListBoxComponent_34f40031.o: ../../Source/UI/Common/DraggingListBoxComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DraggingListBoxComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FatalErrorScreen_2f541f02.o: ../../Source/UI/Common/FatalErrorScreen.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FatalErrorScreen.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MenuButton_1d9ba4c3.o: ../../Source/UI/Common/MenuButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MenuButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PlayButton_38579458.o: ../../Source/UI/Common/PlayButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PlayButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginWindow_66751815.o: ../../Source/UI/Common/PluginWindow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PluginWindow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SizeSwitcherComponent_cef9f113.o: ../../Source/UI/Common/SizeSwitcherComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SizeSwitcherComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpectralLogo_f6755a41.o: ../../Source/UI/Common/SpectralLogo.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SpectralLogo.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ViewportFitProxyComponent_a93fb792.o: ../../Source/UI/Common/ViewportFitProxyComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ViewportFitProxyComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationDialog_bb76a597.o: ../../Source/UI/Dialogs/AnnotationDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationDialog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AuthorizationDialog_ebcfd46b.o: ../../Source/UI/Dialogs/AuthorizationDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AuthorizationDialog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FadingDialog_905af7a1.o: ../../Source/UI/Dialogs/FadingDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FadingDialog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ModalDialogConfirmation_23a5c174.o: ../../Source/UI/Dialogs/ModalDialogConfirmation.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ModalDialogConfirmation.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ModalDialogInput_6af917b5.o: ../../Source/UI/Dialogs/ModalDialogInput.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ModalDialogInput.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RenderDialog_d54ea4be.o: ../../Source/UI/Dialogs/RenderDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RenderDialog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureDialog_7d6669dd.o: ../../Source/UI/Dialogs/TimeSignatureDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureDialog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UpdateDialog_9c0a7831.o: ../../Source/UI/Dialogs/UpdateDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UpdateDialog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MultiTouchController_63afb603.o: ../../Source/UI/Input/MultiTouchController.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MultiTouchController.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioPluginEditorPage_409ff4b3.o: ../../Source/UI/InstrumentsPage/Editor/AudioPluginEditorPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioPluginEditorPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentEditor_78eb55f0.o: ../../Source/UI/InstrumentsPage/Editor/InstrumentEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentEditorConnector_8c32787.o: ../../Source/UI/InstrumentsPage/Editor/InstrumentEditorConnector.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditorConnector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentEditorNode_28608192.o: ../../Source/UI/InstrumentsPage/Editor/InstrumentEditorNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditorNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentEditorPin_92dde88f.o: ../../Source/UI/InstrumentsPage/Editor/InstrumentEditorPin.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditorPin.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentRow_21b66d6d.o: ../../Source/UI/InstrumentsPage/InstrumentRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentsPage_ebd7d5d5.o: ../../Source/UI/InstrumentsPage/InstrumentsPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentsPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InitScreen_452f50fb.o: ../../Source/UI/Intro/InitScreen.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InitScreen.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordTooltip_d9bc553d.o: ../../Source/UI/Popups/ChordTooltip.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ChordTooltip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FailTooltip_99f74519.o: ../../Source/UI/Popups/FailTooltip.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FailTooltip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HelioCallout_90a2f131.o: ../../Source/UI/Popups/HelioCallout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HelioCallout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoNotesPopup_ca8bfdc2.o: ../../Source/UI/Popups/NoNotesPopup.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoNotesPopup.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PopupButton_5c5e0092.o: ../../Source/UI/Popups/PopupButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PopupButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PopupCustomButton_6d33e2a3.o: ../../Source/UI/Popups/PopupCustomButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PopupCustomButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PopupImageButton_54133d7.o: ../../Source/UI/Popups/PopupImageButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PopupImageButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProgressTooltip_f470232a.o: ../../Source/UI/Popups/ProgressTooltip.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProgressTooltip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SuccessTooltip_7d75a6f6.o: ../../Source/UI/Popups/SuccessTooltip.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SuccessTooltip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TooltipContainer_52ddacf4.o: ../../Source/UI/Popups/TooltipContainer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TooltipContainer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RolloverBackButtonLeft_f3afb1a.o: ../../Source/UI/Rollovers/RolloverBackButtonLeft.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RolloverBackButtonLeft.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RolloverBackButtonRight_1571eeb3.o: ../../Source/UI/Rollovers/RolloverBackButtonRight.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RolloverBackButtonRight.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RolloverHeaderLeft_31f5252e.o: ../../Source/UI/Rollovers/RolloverHeaderLeft.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RolloverHeaderLeft.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RolloverHeaderRight_49fd071f.o: ../../Source/UI/Rollovers/RolloverHeaderRight.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RolloverHeaderRight.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RolloverContainer_684b65d1.o: ../../Source/UI/Rollovers/RolloverContainer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RolloverContainer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectRollover_298ec02b.o: ../../Source/UI/Rollovers/ProjectRollover.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectRollover.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsRollover_b0b344dd.o: ../../Source/UI/Rollovers/SettingsRollover.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsRollover.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectPage_591022af.o: ../../Source/UI/ProjectPage/ProjectPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectPageDefault_ff2d1b1c.o: ../../Source/UI/ProjectPage/ProjectPageDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectPageDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectPagePhone_86279549.o: ../../Source/UI/ProjectPage/ProjectPagePhone.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectPagePhone.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationLargeComponent_cd4cf278.o: ../../Source/UI/Sequencer/AnnotationsMap/AnnotationLargeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationLargeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationSmallComponent_e8ea642c.o: ../../Source/UI/Sequencer/AnnotationsMap/AnnotationSmallComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationSmallComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationsTrackMap_971ca518.o: ../../Source/UI/Sequencer/AnnotationsMap/AnnotationsTrackMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationsTrackMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationCurveHelper_e99f382c.o: ../../Source/UI/Sequencer/AutomationMap/AutomationCurveHelper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationCurveHelper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationEventComponent_7ed837fe.o: ../../Source/UI/Sequencer/AutomationMap/AutomationEventComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationEventComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationEventsConnector_dae8c963.o: ../../Source/UI/Sequencer/AutomationMap/AutomationEventsConnector.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationEventsConnector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationTrackMap_823a4a0c.o: ../../Source/UI/Sequencer/AutomationMap/AutomationTrackMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationTrackMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HeaderSelectionIndicator_5aed2562.o: ../../Source/UI/Sequencer/Header/HeaderSelectionIndicator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HeaderSelectionIndicator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollHeader_e7c11898.o: ../../Source/UI/Sequencer/Header/HybridRollHeader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollHeader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Playhead_49aa3d26.o: ../../Source/UI/Sequencer/Header/Playhead.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Playhead.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SoundProbeIndicator_fa380566.o: ../../Source/UI/Sequencer/Header/SoundProbeIndicator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SoundProbeIndicator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeDistanceIndicator_e1c13445.o: ../../Source/UI/Sequencer/Header/TimeDistanceIndicator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeDistanceIndicator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackEndIndicator_698c6d7.o: ../../Source/UI/Sequencer/Header/TrackEndIndicator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TrackEndIndicator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackStartIndicator_fa80cb30.o: ../../Source/UI/Sequencer/Header/TrackStartIndicator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TrackStartIndicator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ComponentConnectorCurve_50e8f885.o: ../../Source/UI/Sequencer/Helpers/ComponentConnectorCurve.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ComponentConnectorCurve.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollExpandMark_c3022404.o: ../../Source/UI/Sequencer/Helpers/HybridRollExpandMark.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollExpandMark.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InsertSpaceHelper_7c318421.o: ../../Source/UI/Sequencer/Helpers/InsertSpaceHelper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InsertSpaceHelper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoteResizerLeft_c91c2c99.o: ../../Source/UI/Sequencer/Helpers/NoteResizerLeft.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoteResizerLeft.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoteResizerRight_97b6ed14.o: ../../Source/UI/Sequencer/Helpers/NoteResizerRight.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoteResizerRight.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineWarningMarker_6d5c36fb.o: ../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineWarningMarker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WipeSpaceHelper_16b49913.o: ../../Source/UI/Sequencer/Helpers/WipeSpaceHelper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling WipeSpaceHelper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ClipComponent_3c63e97b.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationClipComponent_ecbdf1e4.o: ../../Source/UI/Sequencer/PatternRoll/AutomationClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DummyClipComponent_76f3c1e1.o: ../../Source/UI/Sequencer/PatternRoll/DummyClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DummyClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoClipComponent_726e7f60.o: ../../Source/UI/Sequencer/PatternRoll/PianoClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternRoll_9707f85b.o: ../../Source/UI/Sequencer/PatternRoll/PatternRoll.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternRoll.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoteComponent_465c17d2.o: ../../Source/UI/Sequencer/PianoRoll/NoteComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoteComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoRoll_c125538d.o: ../../Source/UI/Sequencer/PianoRoll/PianoRoll.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoRoll.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoRollToolbox_31a5c270.o: ../../Source/UI/Sequencer/PianoRoll/PianoRollToolbox.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoRollToolbox.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureLargeComponent_3afd89e2.o: ../../Source/UI/Sequencer/TimeSignaturesMap/TimeSignatureLargeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureLargeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureSmallComponent_569afb96.o: ../../Source/UI/Sequencer/TimeSignaturesMap/TimeSignatureSmallComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureSmallComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignaturesTrackMap_1611936e.o: ../../Source/UI/Sequencer/TimeSignaturesMap/TimeSignaturesTrackMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignaturesTrackMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoTrackMap_ab9c6236.o: ../../Source/UI/Sequencer/TrackMap/PianoTrackMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoTrackMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackScroller_af8066e1.o: ../../Source/UI/Sequencer/TrackMap/TrackScroller.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TrackScroller.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackScrollerScreen_d46f1c6d.o: ../../Source/UI/Sequencer/TrackMap/TrackScrollerScreen.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TrackScrollerScreen.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TriggerEventComponent_f070f9bd.o: ../../Source/UI/Sequencer/TriggersMap/TriggerEventComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TriggerEventComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TriggerEventConnector_767fa90d.o: ../../Source/UI/Sequencer/TriggersMap/TriggerEventConnector.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TriggerEventConnector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TriggersTrackMap_68437e54.o: ../../Source/UI/Sequencer/TriggersMap/TriggersTrackMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TriggersTrackMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridLassoComponent_4ff655d9.o: ../../Source/UI/Sequencer/HybridLassoComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridLassoComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRoll_b60b10f3.o: ../../Source/UI/Sequencer/HybridRoll.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRoll.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollEditMode_46b31f60.o: ../../Source/UI/Sequencer/HybridRollEditMode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollEditMode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollEventComponent_f733a696.o: ../../Source/UI/Sequencer/HybridRollEventComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollEventComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SequencerLayout_58e2f98b.o: ../../Source/UI/Sequencer/SequencerLayout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SequencerLayout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioSettings_1a236878.o: ../../Source/UI/SettingsPage/AudioSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AuthorizationSettings_5e75069b.o: ../../Source/UI/SettingsPage/AuthorizationSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AuthorizationSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ComponentsList_495b669f.o: ../../Source/UI/SettingsPage/ComponentsList.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ComponentsList.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/IntroSettingsWrapper_f70e382f.o: ../../Source/UI/SettingsPage/IntroSettingsWrapper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling IntroSettingsWrapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LabeledSettingsWrapper_6cf0db28.o: ../../Source/UI/SettingsPage/LabeledSettingsWrapper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LabeledSettingsWrapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogComponent_1a5662e4.o: ../../Source/UI/SettingsPage/LogComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LogComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OpenGLSettings_9ebd40bd.o: ../../Source/UI/SettingsPage/OpenGLSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OpenGLSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginsList_f25f06bd.o: ../../Source/UI/SettingsPage/PluginsList.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PluginsList.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsPage_f68ee19d.o: ../../Source/UI/SettingsPage/SettingsPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsListItemHighlighter_4714088c.o: ../../Source/UI/SettingsPage/SettingsListItemHighlighter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsListItemHighlighter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsListItemSelection_aeb4b5b7.o: ../../Source/UI/SettingsPage/SettingsListItemSelection.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsListItemSelection.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ThemeSettings_328aedeb.o: ../../Source/UI/SettingsPage/ThemeSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ThemeSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ThemeSettingsItem_5b6c809e.o: ../../Source/UI/SettingsPage/ThemeSettingsItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ThemeSettingsItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TranslationSettings_46da0753.o: ../../Source/UI/SettingsPage/TranslationSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TranslationSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TranslationSettingsItem_1640a606.o: ../../Source/UI/SettingsPage/TranslationSettingsItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TranslationSettingsItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ComponentFader_6572b0c.o: ../../Source/UI/Themes/ComponentFader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ComponentFader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FontSerializer_ad448bd4.o: ../../Source/UI/Themes/FontSerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FontSerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GradientVertical_67252d59.o: ../../Source/UI/Themes/GradientVertical.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling GradientVertical.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GradientVerticalReversed_56a2f63b.o: ../../Source/UI/Themes/GradientVerticalReversed.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling GradientVerticalReversed.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HelioTheme_d6ec9067.o: ../../Source/UI/Themes/HelioTheme.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HelioTheme.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Icons_7879bcb1.o: ../../Source/UI/Themes/Icons.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Icons.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LighterShadowDownwards_5a0a0425.o: ../../Source/UI/Themes/LighterShadowDownwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LighterShadowDownwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LighterShadowUpwards_3589fd0c.o: ../../Source/UI/Themes/LighterShadowUpwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LighterShadowUpwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightShadowDownwards_329253f2.o: ../../Source/UI/Themes/LightShadowDownwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightShadowDownwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightShadowLeftwards_3042da2d.o: ../../Source/UI/Themes/LightShadowLeftwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightShadowLeftwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightShadowRightwards_b4145968.o: ../../Source/UI/Themes/LightShadowRightwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightShadowRightwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightShadowUpwards_7f8ddc19.o: ../../Source/UI/Themes/LightShadowUpwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightShadowUpwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PanelA_cd3f5b50.o: ../../Source/UI/Themes/PanelA.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PanelA.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PanelB_cd4d72d1.o: ../../Source/UI/Themes/PanelB.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PanelB.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PanelBackgroundA_49900242.o: ../../Source/UI/Themes/PanelBackgroundA.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PanelBackgroundA.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PanelBackgroundB_499e19c3.o: ../../Source/UI/Themes/PanelBackgroundB.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PanelBackgroundB.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PanelBackgroundC_49ac3144.o: ../../Source/UI/Themes/PanelBackgroundC.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PanelBackgroundC.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PanelC_cd5b8a52.o: ../../Source/UI/Themes/PanelC.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PanelC.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SeparatorHorizontal_240c2780.o: ../../Source/UI/Themes/SeparatorHorizontal.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SeparatorHorizontal.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SeparatorHorizontalReversed_385cd962.o: ../../Source/UI/Themes/SeparatorHorizontalReversed.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SeparatorHorizontalReversed.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SeparatorHorizontalFading_a0277f19.o: ../../Source/UI/Themes/SeparatorHorizontalFading.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SeparatorHorizontalFading.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SeparatorHorizontalFadingReversed_481787fb.o: ../../Source/UI/Themes/SeparatorHorizontalFadingReversed.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SeparatorHorizontalFadingReversed.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SeparatorVertical_65878792.o: ../../Source/UI/Themes/SeparatorVertical.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SeparatorVertical.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SeparatorVerticalReversed_e5480774.o: ../../Source/UI/Themes/SeparatorVerticalReversed.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SeparatorVerticalReversed.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadeDark_2a393f5a.o: ../../Source/UI/Themes/ShadeDark.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadeDark.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadeLight_b6da9a7c.o: ../../Source/UI/Themes/ShadeLight.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadeLight.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowDownwards_29eb3a6c.o: ../../Source/UI/Themes/ShadowDownwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowDownwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowHorizontalFading_a0923750.o: ../../Source/UI/Themes/ShadowHorizontalFading.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowHorizontalFading.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowLeftwards_279bc0a7.o: ../../Source/UI/Themes/ShadowLeftwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowLeftwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowRightwards_a7d8422e.o: ../../Source/UI/Themes/ShadowRightwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowRightwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowUpwards_83021913.o: ../../Source/UI/Themes/ShadowUpwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowUpwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ViewportKineticSlider_45304cab.o: ../../Source/UI/Themes/ViewportKineticSlider.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ViewportKineticSlider.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemComponent_45f5f057.o: ../../Source/UI/Tree/TreeItemComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemComponentDefault_335c2074.o: ../../Source/UI/Tree/TreeItemComponentDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemComponentDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemComponentCompact_1c0e4456.o: ../../Source/UI/Tree/TreeItemComponentCompact.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemComponentCompact.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemMarkerDefault_f4465881.o: ../../Source/UI/Tree/TreeItemMarkerDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemMarkerDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemMarkerCompact_dcf87c63.o: ../../Source/UI/Tree/TreeItemMarkerCompact.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemMarkerCompact.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemMenuButton_55ef6521.o: ../../Source/UI/Tree/TreeItemMenuButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemMenuButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreePanel_62366e31.o: ../../Source/UI/Tree/TreePanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreePanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreePanelDefault_37c5395a.o: ../../Source/UI/Tree/TreePanelDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreePanelDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreePanelPhone_1e0e2b07.o: ../../Source/UI/Tree/TreePanelPhone.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreePanelPhone.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HistoryComponent_76e9715f.o: ../../Source/UI/VCSPage/HistoryComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HistoryComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionComponent_9dbc4cd6.o: ../../Source/UI/VCSPage/RevisionComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionConnectorComponent_c98d6ec1.o: ../../Source/UI/VCSPage/RevisionConnectorComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionConnectorComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionItemComponent_84696683.o: ../../Source/UI/VCSPage/RevisionItemComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionItemComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionTooltipComponent_7c2a51cb.o: ../../Source/UI/VCSPage/RevisionTooltipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionTooltipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionTreeComponent_29a3e7d8.o: ../../Source/UI/VCSPage/RevisionTreeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionTreeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/StageComponent_e8287375.o: ../../Source/UI/VCSPage/StageComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling StageComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlEditor_b096a8c8.o: ../../Source/UI/VCSPage/VersionControlEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlEditorDefault_476136e3.o: ../../Source/UI/VCSPage/VersionControlEditorDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlEditorDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlEditorPhone_c66df6d0.o: ../../Source/UI/VCSPage/VersionControlEditorPhone.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlEditorPhone.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlEditorPhoneViewportContent_138dc88d.o: ../../Source/UI/VCSPage/VersionControlEditorPhoneViewportContent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlEditorPhoneViewportContent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CreateProjectRow_8ad7b390.o: ../../Source/UI/WorkspacePage/Menu/CreateProjectRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CreateProjectRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OpenProjectRow_dead0fde.o: ../../Source/UI/WorkspacePage/Menu/OpenProjectRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OpenProjectRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RecentProjectRow_5fa006af.o: ../../Source/UI/WorkspacePage/Menu/RecentProjectRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RecentProjectRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SignInRow_bc7b85ef.o: ../../Source/UI/WorkspacePage/Menu/SignInRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SignInRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WorkspaceMenu_2e4ba3cb.o: ../../Source/UI/WorkspacePage/Menu/WorkspaceMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling WorkspaceMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogoFader_7cac1f2e.o: ../../Source/UI/WorkspacePage/LogoFader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LogoFader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogoImage_7c075493.o: ../../Source/UI/WorkspacePage/LogoImage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LogoImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WorkspacePage_4e414de7.o: ../../Source/UI/WorkspacePage/WorkspacePage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling WorkspacePage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainLayout_4f58d3f.o: ../../Source/UI/MainLayout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MainLayout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainWindow_5f9c9b05.o: ../../Source/UI/MainWindow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MainWindow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData_7821001c.o: ../Projucer/JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData2_8c1f1960.o: ../Projucer/JuceLibraryCode/BinaryData2.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData3_8c2d30e1.o: ../Projucer/JuceLibraryCode/BinaryData3.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData3.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData4_8c3b4862.o: ../Projucer/JuceLibraryCode/BinaryData4.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData4.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData5_8c495fe3.o: ../Projucer/JuceLibraryCode/BinaryData5.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData5.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData6_8c577764.o: ../Projucer/JuceLibraryCode/BinaryData6.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData6.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData7_8c658ee5.o: ../Projucer/JuceLibraryCode/BinaryData7.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData7.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData8_8c73a666.o: ../Projucer/JuceLibraryCode/BinaryData8.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData8.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData9_8c81bde7.o: ../Projucer/JuceLibraryCode/BinaryData9.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData9.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData10_f61320fb.o: ../Projucer/JuceLibraryCode/BinaryData10.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData10.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_basics_68629202.o: ../Projucer/JuceLibraryCode/include_juce_audio_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_devices_477c5a4a.o: ../Projucer/JuceLibraryCode/include_juce_audio_devices.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_devices.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_fa635d49.o: ../Projucer/JuceLibraryCode/include_juce_audio_formats.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_formats.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_6c50941e.o: ../Projucer/JuceLibraryCode/include_juce_audio_processors.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_processors.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_utils_a6c9a21e.o: ../Projucer/JuceLibraryCode/include_juce_audio_utils.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_utils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_9b124993.o: ../Projucer/JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_cryptography_6acc0160.o: ../Projucer/JuceLibraryCode/include_juce_cryptography.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_cryptography.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_eb02bd2b.o: ../Projucer/JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_23f77a4d.o: ../Projucer/JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_82dd76ff.o: ../Projucer/JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_d3ae9f3d.o: ../Projucer/JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_3bdb3d62.o: ../Projucer/JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_opengl_1ea9a6e3.o: ../Projucer/JuceLibraryCode/include_juce_opengl.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_opengl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_osc_7d711795.o: ../Projucer/JuceLibraryCode/include_juce_osc.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_osc.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

check-pkg-config:
	@command -v pkg-config >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@pkg-config --print-errors alsa freetype2 libcurl x11 xext xinerama

clean:
	@echo Cleaning Helio
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping Helio
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(TARGET)

-include $(OBJECTS_APP:%.o=%.d)
