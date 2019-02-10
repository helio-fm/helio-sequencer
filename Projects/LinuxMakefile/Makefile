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

JUCE_ARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Debug)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Debug
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DDEBUG=1 -D_DEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=2.0 -DJUCE_APP_VERSION_HEX=0x20000 $(shell pkg-config --cflags alsa freetype2 x11 xext xinerama libcurl) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../../ThirdParty/ASIO/common -I../../Source/ -I../../Source/Core -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Configuration -I../../Source/Core/Configuration/Models -I../../Source/Core/Configuration/ResourceManagers -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Network/Models -I../../Source/Core/Network/Requests -I../../Source/Core/Network/Services -I../../Source/Core/Scripting -I../../Source/Core/Serialization -I../../Source/Core/Tree -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/Workspace -I../../Source/UI/ -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Menus/SelectionMenus -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Project -I../../Source/UI/Pages/Settings -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Dashboard -I../../Source/UI/Pages/Dashboard/Menu -I../../Source/UI/Popups -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/MiniMaps/AnnotationsMap -I../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap -I../../Source/UI/Sequencer/MiniMaps/PianoMap -I../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PatternRoll/ClipComponents -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/Sidebars -I../../Source/UI/Themes $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 x11 xext xinerama libcurl) -lGL -ldl -lpthread -lrt $(LDFLAGS)

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

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=2.0 -DJUCE_APP_VERSION_HEX=0x20000 $(shell pkg-config --cflags alsa freetype2 x11 xext xinerama libcurl) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../../ThirdParty/ASIO/common -I../../Source/ -I../../Source/Core -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Configuration -I../../Source/Core/Configuration/Models -I../../Source/Core/Configuration/ResourceManagers -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Network/Models -I../../Source/Core/Network/Requests -I../../Source/Core/Network/Services -I../../Source/Core/Scripting -I../../Source/Core/Serialization -I../../Source/Core/Tree -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/Workspace -I../../Source/UI/ -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Menus/SelectionMenus -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Project -I../../Source/UI/Pages/Settings -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Dashboard -I../../Source/UI/Pages/Dashboard/Menu -I../../Source/UI/Popups -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/MiniMaps/AnnotationsMap -I../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap -I../../Source/UI/Sequencer/MiniMaps/PianoMap -I../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PatternRoll/ClipComponents -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/Sidebars -I../../Source/UI/Themes $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -Ofast -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 x11 xext xinerama libcurl) -fvisibility=hidden -lGL -ldl -lpthread -lrt $(LDFLAGS)

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

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=2.0 -DJUCE_APP_VERSION_HEX=0x20000 $(shell pkg-config --cflags alsa freetype2 x11 xext xinerama libcurl) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../../ThirdParty/ASIO/common -I../../Source/ -I../../Source/Core -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Configuration -I../../Source/Core/Configuration/Models -I../../Source/Core/Configuration/ResourceManagers -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Network/Models -I../../Source/Core/Network/Requests -I../../Source/Core/Network/Services -I../../Source/Core/Scripting -I../../Source/Core/Serialization -I../../Source/Core/Tree -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/Workspace -I../../Source/UI/ -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Menus/SelectionMenus -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Project -I../../Source/UI/Pages/Settings -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Dashboard -I../../Source/UI/Pages/Dashboard/Menu -I../../Source/UI/Popups -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/MiniMaps/AnnotationsMap -I../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap -I../../Source/UI/Sequencer/MiniMaps/PianoMap -I../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PatternRoll/ClipComponents -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/Sidebars -I../../Source/UI/Themes $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -Ofast -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 x11 xext xinerama libcurl) -fvisibility=hidden -lGL -ldl -lpthread -lrt $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Travis)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Travis
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -m64
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=2.0 -DJUCE_APP_VERSION_HEX=0x20000 $(shell pkg-config --cflags alsa freetype2 x11 xext xinerama libcurl) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../../ThirdParty/ASIO/common -I../../Source/ -I../../Source/Core -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Configuration -I../../Source/Core/Configuration/Models -I../../Source/Core/Configuration/ResourceManagers -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Network/Models -I../../Source/Core/Network/Requests -I../../Source/Core/Network/Services -I../../Source/Core/Scripting -I../../Source/Core/Serialization -I../../Source/Core/Tree -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/Workspace -I../../Source/UI/ -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Menus/SelectionMenus -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Project -I../../Source/UI/Pages/Settings -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Dashboard -I../../Source/UI/Pages/Dashboard/Menu -I../../Source/UI/Popups -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/MiniMaps/AnnotationsMap -I../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap -I../../Source/UI/Sequencer/MiniMaps/PianoMap -I../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PatternRoll/ClipComponents -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/Sidebars -I../../Source/UI/Themes $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0 -DJucePlugin_Build_Unity=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O0 -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 x11 xext xinerama libcurl) -fvisibility=hidden -lGL -ldl -lpthread -lrt $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/BuiltInSynthAudioPlugin_fa4a5d64.o \
  $(JUCE_OBJDIR)/BuiltInSynthFormat_faaea2e6.o \
  $(JUCE_OBJDIR)/BuiltInSynthPiano_eacea884.o \
  $(JUCE_OBJDIR)/InternalPluginFormat_b472d97d.o \
  $(JUCE_OBJDIR)/Instrument_bb3fff74.o \
  $(JUCE_OBJDIR)/OrchestraPit_a67292bb.o \
  $(JUCE_OBJDIR)/PluginScanner_cf690128.o \
  $(JUCE_OBJDIR)/SerializablePluginDescription_dc94bde7.o \
  $(JUCE_OBJDIR)/AudioMonitor_3e55a9cb.o \
  $(JUCE_OBJDIR)/SpectrumAnalyzer_e1c0fa3e.o \
  $(JUCE_OBJDIR)/PlayerThread_2ab68fb.o \
  $(JUCE_OBJDIR)/RendererThread_511aa99d.o \
  $(JUCE_OBJDIR)/Transport_931cdbc3.o \
  $(JUCE_OBJDIR)/AudioCore_ec8fdd75.o \
  $(JUCE_OBJDIR)/Arpeggiator_23dd22be.o \
  $(JUCE_OBJDIR)/Chord_707125e1.o \
  $(JUCE_OBJDIR)/ColourScheme_dd9dc9f6.o \
  $(JUCE_OBJDIR)/HotkeyScheme_317fdf9c.o \
  $(JUCE_OBJDIR)/Scale_be8bfbcf.o \
  $(JUCE_OBJDIR)/Script_70bc3b30.o \
  $(JUCE_OBJDIR)/Translation_a854fdb6.o \
  $(JUCE_OBJDIR)/ResourceManager_28676aae.o \
  $(JUCE_OBJDIR)/ArpeggiatorsManager_5e3e8e62.o \
  $(JUCE_OBJDIR)/ChordsManager_e14cf605.o \
  $(JUCE_OBJDIR)/ColourSchemesManager_252e7706.o \
  $(JUCE_OBJDIR)/HotkeySchemesManager_388b86ac.o \
  $(JUCE_OBJDIR)/ScalesManager_3d61fdf3.o \
  $(JUCE_OBJDIR)/ScriptsManager_8941c6c0.o \
  $(JUCE_OBJDIR)/TranslationsManager_6487f15a.o \
  $(JUCE_OBJDIR)/Config_db7b8b0c.o \
  $(JUCE_OBJDIR)/Clip_5929fe7f.o \
  $(JUCE_OBJDIR)/Pattern_a3a86b8b.o \
  $(JUCE_OBJDIR)/AnnotationEvent_8fa9bdd9.o \
  $(JUCE_OBJDIR)/AutomationEvent_5ea238f1.o \
  $(JUCE_OBJDIR)/KeySignatureEvent_80a740ef.o \
  $(JUCE_OBJDIR)/MidiEvent_cfd604e7.o \
  $(JUCE_OBJDIR)/Note_42fe2f4e.o \
  $(JUCE_OBJDIR)/TimeSignatureEvent_5ddd998b.o \
  $(JUCE_OBJDIR)/AnnotationsSequence_1997bf8b.o \
  $(JUCE_OBJDIR)/AutomationSequence_84d9de3c.o \
  $(JUCE_OBJDIR)/KeySignaturesSequence_6a59f1a1.o \
  $(JUCE_OBJDIR)/MidiSequence_310d4486.o \
  $(JUCE_OBJDIR)/PianoSequence_e11a82f0.o \
  $(JUCE_OBJDIR)/TimeSignaturesSequence_5fa7c98d.o \
  $(JUCE_OBJDIR)/MidiTrack_6604020d.o \
  $(JUCE_OBJDIR)/BackendRequest_7a512d9e.o \
  $(JUCE_OBJDIR)/UserConfigSyncThread_281a0595.o \
  $(JUCE_OBJDIR)/ProjectCloneThread_838580b1.o \
  $(JUCE_OBJDIR)/ProjectDeleteThread_df81aeb5.o \
  $(JUCE_OBJDIR)/RevisionsSyncHelpers_6a382a95.o \
  $(JUCE_OBJDIR)/RevisionsSyncThread_7ea15884.o \
  $(JUCE_OBJDIR)/ProjectSyncService_904c332a.o \
  $(JUCE_OBJDIR)/ResourceSyncService_3cb65bad.o \
  $(JUCE_OBJDIR)/SessionService_7b0612a8.o \
  $(JUCE_OBJDIR)/Network_fc4306a6.o \
  $(JUCE_OBJDIR)/KeySignatureClass_f5399760.o \
  $(JUCE_OBJDIR)/NoteClass_f17feca7.o \
  $(JUCE_OBJDIR)/ScaleClass_6ebbf737.o \
  $(JUCE_OBJDIR)/ScriptingHelpers_4a20537.o \
  $(JUCE_OBJDIR)/SelectionTransformExecutor_42c5da9c.o \
  $(JUCE_OBJDIR)/TimeSignatureClass_c51c4536.o \
  $(JUCE_OBJDIR)/Autosaver_8ecb1540.o \
  $(JUCE_OBJDIR)/Document_25ea426b.o \
  $(JUCE_OBJDIR)/DocumentHelpers_16095e24.o \
  $(JUCE_OBJDIR)/BinarySerializer_c8c2cac3.o \
  $(JUCE_OBJDIR)/JsonSerializer_97d7162a.o \
  $(JUCE_OBJDIR)/LegacySerializer_6e2748b.o \
  $(JUCE_OBJDIR)/XmlSerializer_489b3c03.o \
  $(JUCE_OBJDIR)/AudioPluginNode_28638ab.o \
  $(JUCE_OBJDIR)/AutomationTrackNode_e0dff3f6.o \
  $(JUCE_OBJDIR)/InstrumentNode_69832033.o \
  $(JUCE_OBJDIR)/MidiTrackNode_12dce26c.o \
  $(JUCE_OBJDIR)/OrchestraPitNode_9ee364ba.o \
  $(JUCE_OBJDIR)/PatternEditorNode_ffd5345f.o \
  $(JUCE_OBJDIR)/PianoTrackNode_f7a15ace.o \
  $(JUCE_OBJDIR)/ProjectInfo_52f6a347.o \
  $(JUCE_OBJDIR)/ProjectTimeline_9655ff5a.o \
  $(JUCE_OBJDIR)/ProjectNode_8b4c229b.o \
  $(JUCE_OBJDIR)/RootNode_a3140dee.o \
  $(JUCE_OBJDIR)/SettingsNode_1b17d5af.o \
  $(JUCE_OBJDIR)/TrackGroupNode_96578b40.o \
  $(JUCE_OBJDIR)/TreeNode_364484aa.o \
  $(JUCE_OBJDIR)/TreeNodeSerializer_1d017f1c.o \
  $(JUCE_OBJDIR)/VersionControlNode_a601d931.o \
  $(JUCE_OBJDIR)/AnnotationEventActions_e2dc4268.o \
  $(JUCE_OBJDIR)/AutomationTrackActions_7e56b4ff.o \
  $(JUCE_OBJDIR)/AutomationEventActions_ba8f9c50.o \
  $(JUCE_OBJDIR)/KeySignatureEventActions_3b0ee412.o \
  $(JUCE_OBJDIR)/MidiTrackActions_47b06bc9.o \
  $(JUCE_OBJDIR)/NoteActions_b5a2e99f.o \
  $(JUCE_OBJDIR)/PatternActions_60a33363.o \
  $(JUCE_OBJDIR)/PianoTrackActions_78338acf.o \
  $(JUCE_OBJDIR)/TimeSignatureEventActions_c6f6be42.o \
  $(JUCE_OBJDIR)/UndoStack_c8cfe6ea.o \
  $(JUCE_OBJDIR)/AutomationTrackDiffLogic_d3a28db5.o \
  $(JUCE_OBJDIR)/DiffLogic_e39316b3.o \
  $(JUCE_OBJDIR)/PatternDiffHelpers_2a43df40.o \
  $(JUCE_OBJDIR)/PianoTrackDiffLogic_1fe33611.o \
  $(JUCE_OBJDIR)/ProjectInfoDiffLogic_85d6d922.o \
  $(JUCE_OBJDIR)/ProjectTimelineDiffLogic_dd926f6f.o \
  $(JUCE_OBJDIR)/Delta_dc1eed28.o \
  $(JUCE_OBJDIR)/Diff_3af4c61f.o \
  $(JUCE_OBJDIR)/Head_fe3c227a.o \
  $(JUCE_OBJDIR)/RemoteCache_d9eaca0c.o \
  $(JUCE_OBJDIR)/Revision_ddbb1c75.o \
  $(JUCE_OBJDIR)/RevisionItem_7e6e5a28.o \
  $(JUCE_OBJDIR)/Snapshot_f0be1dde.o \
  $(JUCE_OBJDIR)/StashesRepository_bb52fdfd.o \
  $(JUCE_OBJDIR)/VersionControl_bc67ed3f.o \
  $(JUCE_OBJDIR)/NavigationHistory_39a99bf.o \
  $(JUCE_OBJDIR)/RecentProjectInfo_9bf049eb.o \
  $(JUCE_OBJDIR)/SyncedConfigurationInfo_f19402c9.o \
  $(JUCE_OBJDIR)/UserSessionInfo_61ea4378.o \
  $(JUCE_OBJDIR)/UserProfile_63ae89bd.o \
  $(JUCE_OBJDIR)/Workspace_e70d39f4.o \
  $(JUCE_OBJDIR)/App_9f51319a.o \
  $(JUCE_OBJDIR)/GenericAudioMonitorComponent_2d069bd4.o \
  $(JUCE_OBJDIR)/SpectrogramAudioMonitorComponent_b36fb87e.o \
  $(JUCE_OBJDIR)/WaveformAudioMonitorComponent_95e1ad4.o \
  $(JUCE_OBJDIR)/Origami_677a1bc1.o \
  $(JUCE_OBJDIR)/OrigamiHorizontal_4d5ae305.o \
  $(JUCE_OBJDIR)/OrigamiVertical_9fe96057.o \
  $(JUCE_OBJDIR)/ColourButton_d92c95b0.o \
  $(JUCE_OBJDIR)/ColourSwatches_e2a4628.o \
  $(JUCE_OBJDIR)/CommandIDs_ca65c4df.o \
  $(JUCE_OBJDIR)/DraggingListBoxComponent_34f40031.o \
  $(JUCE_OBJDIR)/FineTuningComponentDragger_21c9d9e4.o \
  $(JUCE_OBJDIR)/FineTuningValueIndicator_c936e91f.o \
  $(JUCE_OBJDIR)/KeySelector_58be6196.o \
  $(JUCE_OBJDIR)/MenuButton_1d9ba4c3.o \
  $(JUCE_OBJDIR)/MobileComboBox_bdac55f1.o \
  $(JUCE_OBJDIR)/ModeIndicatorComponent_7338ae83.o \
  $(JUCE_OBJDIR)/OverlayButton_7ef2bf1a.o \
  $(JUCE_OBJDIR)/PlayButton_38579458.o \
  $(JUCE_OBJDIR)/PluginWindow_66751815.o \
  $(JUCE_OBJDIR)/RadioButton_45b0c9c5.o \
  $(JUCE_OBJDIR)/ScaleEditor_6bc10d0f.o \
  $(JUCE_OBJDIR)/SpectralLogo_f6755a41.o \
  $(JUCE_OBJDIR)/ViewportFitProxyComponent_a93fb792.o \
  $(JUCE_OBJDIR)/AnnotationDialog_bb76a597.o \
  $(JUCE_OBJDIR)/FadingDialog_905af7a1.o \
  $(JUCE_OBJDIR)/KeySignatureDialog_b4559c41.o \
  $(JUCE_OBJDIR)/ModalDialogConfirmation_23a5c174.o \
  $(JUCE_OBJDIR)/ModalDialogInput_6af917b5.o \
  $(JUCE_OBJDIR)/RenderDialog_d54ea4be.o \
  $(JUCE_OBJDIR)/TimeSignatureDialog_7d6669dd.o \
  $(JUCE_OBJDIR)/Headline_2606435d.o \
  $(JUCE_OBJDIR)/HeadlineDropdown_494f430e.o \
  $(JUCE_OBJDIR)/HeadlineItem_cd7bcd10.o \
  $(JUCE_OBJDIR)/HeadlineItemArrow_da2ab0a3.o \
  $(JUCE_OBJDIR)/HeadlineItemHighlighter_98266d3b.o \
  $(JUCE_OBJDIR)/HeadlineNavigationPanel_eeb8893d.o \
  $(JUCE_OBJDIR)/MultiTouchController_63afb603.o \
  $(JUCE_OBJDIR)/MenuItemComponent_2eff5284.o \
  $(JUCE_OBJDIR)/MenuItemComponentMarker_9fc5be9e.o \
  $(JUCE_OBJDIR)/MenuPanel_ee874d5e.o \
  $(JUCE_OBJDIR)/AudioPluginSelectionMenu_3e54c8bc.o \
  $(JUCE_OBJDIR)/InstrumentNodeSelectionMenu_d61c1e72.o \
  $(JUCE_OBJDIR)/PatternRollSelectionMenu_1f7a27b8.o \
  $(JUCE_OBJDIR)/PianoRollSelectionMenu_69b358df.o \
  $(JUCE_OBJDIR)/VersionControlHistorySelectionMenu_55cc6e76.o \
  $(JUCE_OBJDIR)/VersionControlStageSelectionMenu_d66783cc.o \
  $(JUCE_OBJDIR)/AnnotationMenu_27bac505.o \
  $(JUCE_OBJDIR)/InstrumentMenu_4fd005bd.o \
  $(JUCE_OBJDIR)/MidiTrackMenu_ae6d14fc.o \
  $(JUCE_OBJDIR)/NotesTuningPanel_5fadfa25.o \
  $(JUCE_OBJDIR)/OrchestraPitMenu_2065d304.o \
  $(JUCE_OBJDIR)/ProjectMenu_5ab725eb.o \
  $(JUCE_OBJDIR)/TimelineMenu_2c071c97.o \
  $(JUCE_OBJDIR)/TimeSignatureMenu_9a8856bd.o \
  $(JUCE_OBJDIR)/VersionControlMenu_cb7aa03b.o \
  $(JUCE_OBJDIR)/WorkspaceMenu_25163be7.o \
  $(JUCE_OBJDIR)/DashboardMenu_3382ca45.o \
  $(JUCE_OBJDIR)/RecentProjectRow_4cd5f954.o \
  $(JUCE_OBJDIR)/LoginButton_795d376d.o \
  $(JUCE_OBJDIR)/CreateProjectButton_44f66081.o \
  $(JUCE_OBJDIR)/OpenProjectButton_4f878c33.o \
  $(JUCE_OBJDIR)/Dashboard_81cfc8dc.o \
  $(JUCE_OBJDIR)/InitScreen_e3d0171e.o \
  $(JUCE_OBJDIR)/LogoFader_b8652153.o \
  $(JUCE_OBJDIR)/UpdatesInfoComponent_ebc1dc07.o \
  $(JUCE_OBJDIR)/UserProfileComponent_f7816701.o \
  $(JUCE_OBJDIR)/AudioPluginEditorPage_d260a64d.o \
  $(JUCE_OBJDIR)/InstrumentEditor_e7548c16.o \
  $(JUCE_OBJDIR)/InstrumentEditorConnector_813d7c21.o \
  $(JUCE_OBJDIR)/InstrumentComponent_f2b63e3e.o \
  $(JUCE_OBJDIR)/InstrumentEditorPin_387238a9.o \
  $(JUCE_OBJDIR)/AudioPluginsListComponent_27b60455.o \
  $(JUCE_OBJDIR)/InstrumentsListComponent_2905dadd.o \
  $(JUCE_OBJDIR)/OrchestraPitPage_52882f47.o \
  $(JUCE_OBJDIR)/ProjectPage_f686b2d5.o \
  $(JUCE_OBJDIR)/AudioSettings_716d894.o \
  $(JUCE_OBJDIR)/ComponentsList_fad5fa03.o \
  $(JUCE_OBJDIR)/LabeledSettingsWrapper_3ae3ca8c.o \
  $(JUCE_OBJDIR)/SimpleSettingsWrapper_7dac70d9.o \
  $(JUCE_OBJDIR)/SettingsListItemHighlighter_e14317a8.o \
  $(JUCE_OBJDIR)/SettingsListItemSelection_3054dbd3.o \
  $(JUCE_OBJDIR)/SettingsPage_ab9efe01.o \
  $(JUCE_OBJDIR)/SyncSettings_7b017e8d.o \
  $(JUCE_OBJDIR)/SyncSettingsItem_6f05f040.o \
  $(JUCE_OBJDIR)/ThemeSettings_1f7e5e07.o \
  $(JUCE_OBJDIR)/ThemeSettingsItem_c2b282ba.o \
  $(JUCE_OBJDIR)/TranslationSettings_f4a7f26f.o \
  $(JUCE_OBJDIR)/TranslationSettingsItem_6aba322.o \
  $(JUCE_OBJDIR)/UserInterfaceSettings_8b19486c.o \
  $(JUCE_OBJDIR)/HistoryComponent_115e47f9.o \
  $(JUCE_OBJDIR)/RevisionComponent_51e2497c.o \
  $(JUCE_OBJDIR)/RevisionConnectorComponent_a2024cdb.o \
  $(JUCE_OBJDIR)/RevisionItemComponent_8654a029.o \
  $(JUCE_OBJDIR)/RevisionTooltipComponent_c893ee65.o \
  $(JUCE_OBJDIR)/RevisionTreeComponent_2b8f217e.o \
  $(JUCE_OBJDIR)/StageComponent_f36288f.o \
  $(JUCE_OBJDIR)/VersionControlEditor_c12aa262.o \
  $(JUCE_OBJDIR)/ArpPreviewTool_98f46997.o \
  $(JUCE_OBJDIR)/ChordPreviewTool_422991ba.o \
  $(JUCE_OBJDIR)/ScalePreviewTool_f58f5b0c.o \
  $(JUCE_OBJDIR)/RescalePreviewTool_6e22f57f.o \
  $(JUCE_OBJDIR)/FailTooltip_99f74519.o \
  $(JUCE_OBJDIR)/HelioCallout_90a2f131.o \
  $(JUCE_OBJDIR)/PopupButton_5c5e0092.o \
  $(JUCE_OBJDIR)/PopupCustomButton_6d33e2a3.o \
  $(JUCE_OBJDIR)/ProgressTooltip_f470232a.o \
  $(JUCE_OBJDIR)/SuccessTooltip_7d75a6f6.o \
  $(JUCE_OBJDIR)/TooltipContainer_52ddacf4.o \
  $(JUCE_OBJDIR)/ClipRangeIndicator_c202e394.o \
  $(JUCE_OBJDIR)/HeaderSelectionIndicator_5aed2562.o \
  $(JUCE_OBJDIR)/HybridRollHeader_e7c11898.o \
  $(JUCE_OBJDIR)/Playhead_49aa3d26.o \
  $(JUCE_OBJDIR)/SoundProbeIndicator_fa380566.o \
  $(JUCE_OBJDIR)/TimeDistanceIndicator_e1c13445.o \
  $(JUCE_OBJDIR)/TrackEndIndicator_698c6d7.o \
  $(JUCE_OBJDIR)/TrackStartIndicator_fa80cb30.o \
  $(JUCE_OBJDIR)/CutPointMark_e432a7df.o \
  $(JUCE_OBJDIR)/HybridRollExpandMark_c3022404.o \
  $(JUCE_OBJDIR)/KnifeToolHelper_d68a471.o \
  $(JUCE_OBJDIR)/TimelineWarningMarker_6d5c36fb.o \
  $(JUCE_OBJDIR)/PatternOperations_aae46f62.o \
  $(JUCE_OBJDIR)/SequencerOperations_4fa1bea3.o \
  $(JUCE_OBJDIR)/AutomationCurveClipComponent_9bff9d3.o \
  $(JUCE_OBJDIR)/AutomationCurveHelper_c47ea72.o \
  $(JUCE_OBJDIR)/AutomationCurveEventComponent_7e9080c7.o \
  $(JUCE_OBJDIR)/AutomationCurveEventsConnector_d23999ba.o \
  $(JUCE_OBJDIR)/AutomationStepsClipComponent_a9f38f63.o \
  $(JUCE_OBJDIR)/AutomationStepEventComponent_875750ac.o \
  $(JUCE_OBJDIR)/AutomationStepEventsConnector_e24cc675.o \
  $(JUCE_OBJDIR)/PianoClipComponent_4f1366b1.o \
  $(JUCE_OBJDIR)/ClipComponent_43b43980.o \
  $(JUCE_OBJDIR)/DummyClipComponent_dddacfc.o \
  $(JUCE_OBJDIR)/PatternRoll_9707f85b.o \
  $(JUCE_OBJDIR)/NoteComponent_465c17d2.o \
  $(JUCE_OBJDIR)/NoteResizerLeft_c3d11e3a.o \
  $(JUCE_OBJDIR)/NoteResizerRight_f3a02f93.o \
  $(JUCE_OBJDIR)/PianoRoll_c125538d.o \
  $(JUCE_OBJDIR)/SequencerSidebarLeft_1fad65fa.o \
  $(JUCE_OBJDIR)/SequencerSidebarRight_134cdfd3.o \
  $(JUCE_OBJDIR)/AnnotationLargeComponent_d95ee3c1.o \
  $(JUCE_OBJDIR)/AnnotationSmallComponent_f4fc5575.o \
  $(JUCE_OBJDIR)/AnnotationsProjectMap_e8c59661.o \
  $(JUCE_OBJDIR)/KeySignatureLargeComponent_7ce9b141.o \
  $(JUCE_OBJDIR)/KeySignatureSmallComponent_988722f5.o \
  $(JUCE_OBJDIR)/KeySignaturesProjectMap_4269b8e1.o \
  $(JUCE_OBJDIR)/PianoProjectMap_b13b64c1.o \
  $(JUCE_OBJDIR)/TrackScroller_dba3353a.o \
  $(JUCE_OBJDIR)/TrackScrollerScreen_86ca0406.o \
  $(JUCE_OBJDIR)/TimeSignatureLargeComponent_9bda06b.o \
  $(JUCE_OBJDIR)/TimeSignatureSmallComponent_255b121f.o \
  $(JUCE_OBJDIR)/TimeSignaturesProjectMap_407e29f7.o \
  $(JUCE_OBJDIR)/HybridRoll_b60b10f3.o \
  $(JUCE_OBJDIR)/HybridRollEditMode_46b31f60.o \
  $(JUCE_OBJDIR)/Lasso_7dd838ea.o \
  $(JUCE_OBJDIR)/LassoListeners_6bea92bf.o \
  $(JUCE_OBJDIR)/MidiEventComponent_a6266ede.o \
  $(JUCE_OBJDIR)/SelectionComponent_9dff102b.o \
  $(JUCE_OBJDIR)/SequencerLayout_58e2f98b.o \
  $(JUCE_OBJDIR)/ComponentFader_6572b0c.o \
  $(JUCE_OBJDIR)/HelioTheme_d6ec9067.o \
  $(JUCE_OBJDIR)/Icons_7879bcb1.o \
  $(JUCE_OBJDIR)/FramePanel_4a84952a.o \
  $(JUCE_OBJDIR)/DialogPanel_d1de90b3.o \
  $(JUCE_OBJDIR)/PanelBackgroundA_49900242.o \
  $(JUCE_OBJDIR)/PanelBackgroundB_499e19c3.o \
  $(JUCE_OBJDIR)/PanelBackgroundC_49ac3144.o \
  $(JUCE_OBJDIR)/SeparatorHorizontal_240c2780.o \
  $(JUCE_OBJDIR)/SeparatorHorizontalReversed_385cd962.o \
  $(JUCE_OBJDIR)/SeparatorHorizontalFading_a0277f19.o \
  $(JUCE_OBJDIR)/SeparatorHorizontalFadingReversed_481787fb.o \
  $(JUCE_OBJDIR)/SeparatorVertical_65878792.o \
  $(JUCE_OBJDIR)/SeparatorVerticalSkew_58e6853c.o \
  $(JUCE_OBJDIR)/ShadowDownwards_29eb3a6c.o \
  $(JUCE_OBJDIR)/ShadowUpwards_83021913.o \
  $(JUCE_OBJDIR)/ShadowLeftwards_279bc0a7.o \
  $(JUCE_OBJDIR)/ShadowRightwards_a7d8422e.o \
  $(JUCE_OBJDIR)/ViewportKineticSlider_45304cab.o \
  $(JUCE_OBJDIR)/MainLayout_4f58d3f.o \
  $(JUCE_OBJDIR)/Common_6ff75bf4.o \
  $(JUCE_OBJDIR)/BinaryData_7821001c.o \
  $(JUCE_OBJDIR)/BinaryData2_8c1f1960.o \
  $(JUCE_OBJDIR)/BinaryData3_8c2d30e1.o \
  $(JUCE_OBJDIR)/include_juce_audio_basics_68629202.o \
  $(JUCE_OBJDIR)/include_juce_audio_devices_477c5a4a.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_fa635d49.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_6c50941e.o \
  $(JUCE_OBJDIR)/include_juce_core_9b124993.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_eb02bd2b.o \
  $(JUCE_OBJDIR)/include_juce_events_23f77a4d.o \
  $(JUCE_OBJDIR)/include_juce_graphics_82dd76ff.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_d3ae9f3d.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_3bdb3d62.o \
  $(JUCE_OBJDIR)/include_juce_opengl_1ea9a6e3.o \

PCH_SRC = ../../Source/Common.h
PCH_OUT = ../../Source/Common.h.gch

.PHONY: clean all

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(PCH_OUT): $(PCH_SRC)
	@echo "Generating precompiled header"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : check-pkg-config $(PCH_OUT) $(OBJECTS_APP) $(RESOURCES)
	@echo Linking "Helio - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

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

$(JUCE_OBJDIR)/PluginScanner_cf690128.o: ../../Source/Core/Audio/Instruments/PluginScanner.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PluginScanner.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SerializablePluginDescription_dc94bde7.o: ../../Source/Core/Audio/Instruments/SerializablePluginDescription.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SerializablePluginDescription.cpp"
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

$(JUCE_OBJDIR)/Arpeggiator_23dd22be.o: ../../Source/Core/Configuration/Models/Arpeggiator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Arpeggiator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Chord_707125e1.o: ../../Source/Core/Configuration/Models/Chord.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Chord.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColourScheme_dd9dc9f6.o: ../../Source/Core/Configuration/Models/ColourScheme.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColourScheme.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HotkeyScheme_317fdf9c.o: ../../Source/Core/Configuration/Models/HotkeyScheme.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HotkeyScheme.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Scale_be8bfbcf.o: ../../Source/Core/Configuration/Models/Scale.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Scale.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Script_70bc3b30.o: ../../Source/Core/Configuration/Models/Script.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Script.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Translation_a854fdb6.o: ../../Source/Core/Configuration/Models/Translation.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Translation.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ResourceManager_28676aae.o: ../../Source/Core/Configuration/ResourceManagers/ResourceManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ResourceManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ArpeggiatorsManager_5e3e8e62.o: ../../Source/Core/Configuration/ResourceManagers/ArpeggiatorsManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ArpeggiatorsManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordsManager_e14cf605.o: ../../Source/Core/Configuration/ResourceManagers/ChordsManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ChordsManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColourSchemesManager_252e7706.o: ../../Source/Core/Configuration/ResourceManagers/ColourSchemesManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColourSchemesManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HotkeySchemesManager_388b86ac.o: ../../Source/Core/Configuration/ResourceManagers/HotkeySchemesManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HotkeySchemesManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ScalesManager_3d61fdf3.o: ../../Source/Core/Configuration/ResourceManagers/ScalesManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ScalesManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ScriptsManager_8941c6c0.o: ../../Source/Core/Configuration/ResourceManagers/ScriptsManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ScriptsManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TranslationsManager_6487f15a.o: ../../Source/Core/Configuration/ResourceManagers/TranslationsManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TranslationsManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Config_db7b8b0c.o: ../../Source/Core/Configuration/Config.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Config.cpp"
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

$(JUCE_OBJDIR)/KeySignatureEvent_80a740ef.o: ../../Source/Core/Midi/Sequences/Events/KeySignatureEvent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignatureEvent.cpp"
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

$(JUCE_OBJDIR)/KeySignaturesSequence_6a59f1a1.o: ../../Source/Core/Midi/Sequences/KeySignaturesSequence.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignaturesSequence.cpp"
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

$(JUCE_OBJDIR)/BackendRequest_7a512d9e.o: ../../Source/Core/Network/Requests/BackendRequest.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BackendRequest.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UserConfigSyncThread_281a0595.o: ../../Source/Core/Network/Requests/UserConfigSyncThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UserConfigSyncThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectCloneThread_838580b1.o: ../../Source/Core/Network/Requests/ProjectCloneThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectCloneThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectDeleteThread_df81aeb5.o: ../../Source/Core/Network/Requests/ProjectDeleteThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectDeleteThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionsSyncHelpers_6a382a95.o: ../../Source/Core/Network/Requests/RevisionsSyncHelpers.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionsSyncHelpers.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionsSyncThread_7ea15884.o: ../../Source/Core/Network/Requests/RevisionsSyncThread.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionsSyncThread.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectSyncService_904c332a.o: ../../Source/Core/Network/Services/ProjectSyncService.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectSyncService.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ResourceSyncService_3cb65bad.o: ../../Source/Core/Network/Services/ResourceSyncService.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ResourceSyncService.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SessionService_7b0612a8.o: ../../Source/Core/Network/Services/SessionService.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SessionService.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Network_fc4306a6.o: ../../Source/Core/Network/Network.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Network.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySignatureClass_f5399760.o: ../../Source/Core/Scripting/KeySignatureClass.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignatureClass.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoteClass_f17feca7.o: ../../Source/Core/Scripting/NoteClass.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoteClass.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ScaleClass_6ebbf737.o: ../../Source/Core/Scripting/ScaleClass.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ScaleClass.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ScriptingHelpers_4a20537.o: ../../Source/Core/Scripting/ScriptingHelpers.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ScriptingHelpers.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SelectionTransformExecutor_42c5da9c.o: ../../Source/Core/Scripting/SelectionTransformExecutor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SelectionTransformExecutor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureClass_c51c4536.o: ../../Source/Core/Scripting/TimeSignatureClass.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureClass.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Autosaver_8ecb1540.o: ../../Source/Core/Serialization/Autosaver.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Autosaver.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Document_25ea426b.o: ../../Source/Core/Serialization/Document.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Document.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DocumentHelpers_16095e24.o: ../../Source/Core/Serialization/DocumentHelpers.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DocumentHelpers.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinarySerializer_c8c2cac3.o: ../../Source/Core/Serialization/BinarySerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinarySerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/JsonSerializer_97d7162a.o: ../../Source/Core/Serialization/JsonSerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling JsonSerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LegacySerializer_6e2748b.o: ../../Source/Core/Serialization/LegacySerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LegacySerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/XmlSerializer_489b3c03.o: ../../Source/Core/Serialization/XmlSerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling XmlSerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioPluginNode_28638ab.o: ../../Source/Core/Tree/AudioPluginNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioPluginNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationTrackNode_e0dff3f6.o: ../../Source/Core/Tree/AutomationTrackNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationTrackNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentNode_69832033.o: ../../Source/Core/Tree/InstrumentNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiTrackNode_12dce26c.o: ../../Source/Core/Tree/MidiTrackNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiTrackNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OrchestraPitNode_9ee364ba.o: ../../Source/Core/Tree/OrchestraPitNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OrchestraPitNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternEditorNode_ffd5345f.o: ../../Source/Core/Tree/PatternEditorNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternEditorNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoTrackNode_f7a15ace.o: ../../Source/Core/Tree/PianoTrackNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoTrackNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectInfo_52f6a347.o: ../../Source/Core/Tree/ProjectInfo.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectInfo.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectTimeline_9655ff5a.o: ../../Source/Core/Tree/ProjectTimeline.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectTimeline.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectNode_8b4c229b.o: ../../Source/Core/Tree/ProjectNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RootNode_a3140dee.o: ../../Source/Core/Tree/RootNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RootNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsNode_1b17d5af.o: ../../Source/Core/Tree/SettingsNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackGroupNode_96578b40.o: ../../Source/Core/Tree/TrackGroupNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TrackGroupNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeNode_364484aa.o: ../../Source/Core/Tree/TreeNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeNodeSerializer_1d017f1c.o: ../../Source/Core/Tree/TreeNodeSerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeNodeSerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlNode_a601d931.o: ../../Source/Core/Tree/VersionControlNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlNode.cpp"
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

$(JUCE_OBJDIR)/KeySignatureEventActions_3b0ee412.o: ../../Source/Core/Undo/Actions/KeySignatureEventActions.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignatureEventActions.cpp"
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

$(JUCE_OBJDIR)/AutomationTrackDiffLogic_d3a28db5.o: ../../Source/Core/VCS/DiffLogic/AutomationTrackDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationTrackDiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DiffLogic_e39316b3.o: ../../Source/Core/VCS/DiffLogic/DiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternDiffHelpers_2a43df40.o: ../../Source/Core/VCS/DiffLogic/PatternDiffHelpers.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternDiffHelpers.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoTrackDiffLogic_1fe33611.o: ../../Source/Core/VCS/DiffLogic/PianoTrackDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoTrackDiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectInfoDiffLogic_85d6d922.o: ../../Source/Core/VCS/DiffLogic/ProjectInfoDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectInfoDiffLogic.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectTimelineDiffLogic_dd926f6f.o: ../../Source/Core/VCS/DiffLogic/ProjectTimelineDiffLogic.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectTimelineDiffLogic.cpp"
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

$(JUCE_OBJDIR)/RemoteCache_d9eaca0c.o: ../../Source/Core/VCS/RemoteCache.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RemoteCache.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Revision_ddbb1c75.o: ../../Source/Core/VCS/Revision.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Revision.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionItem_7e6e5a28.o: ../../Source/Core/VCS/RevisionItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Snapshot_f0be1dde.o: ../../Source/Core/VCS/Snapshot.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Snapshot.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/StashesRepository_bb52fdfd.o: ../../Source/Core/VCS/StashesRepository.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling StashesRepository.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControl_bc67ed3f.o: ../../Source/Core/VCS/VersionControl.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NavigationHistory_39a99bf.o: ../../Source/Core/Workspace/NavigationHistory.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NavigationHistory.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RecentProjectInfo_9bf049eb.o: ../../Source/Core/Workspace/RecentProjectInfo.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RecentProjectInfo.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SyncedConfigurationInfo_f19402c9.o: ../../Source/Core/Workspace/SyncedConfigurationInfo.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SyncedConfigurationInfo.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UserSessionInfo_61ea4378.o: ../../Source/Core/Workspace/UserSessionInfo.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UserSessionInfo.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UserProfile_63ae89bd.o: ../../Source/Core/Workspace/UserProfile.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UserProfile.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Workspace_e70d39f4.o: ../../Source/Core/Workspace/Workspace.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Workspace.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/App_9f51319a.o: ../../Source/Core/App.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling App.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/GenericAudioMonitorComponent_2d069bd4.o: ../../Source/UI/Common/AudioMonitors/GenericAudioMonitorComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling GenericAudioMonitorComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpectrogramAudioMonitorComponent_b36fb87e.o: ../../Source/UI/Common/AudioMonitors/SpectrogramAudioMonitorComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SpectrogramAudioMonitorComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WaveformAudioMonitorComponent_95e1ad4.o: ../../Source/UI/Common/AudioMonitors/WaveformAudioMonitorComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling WaveformAudioMonitorComponent.cpp"
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

$(JUCE_OBJDIR)/CommandIDs_ca65c4df.o: ../../Source/UI/Common/CommandIDs.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CommandIDs.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DraggingListBoxComponent_34f40031.o: ../../Source/UI/Common/DraggingListBoxComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DraggingListBoxComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FineTuningComponentDragger_21c9d9e4.o: ../../Source/UI/Common/FineTuningComponentDragger.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FineTuningComponentDragger.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FineTuningValueIndicator_c936e91f.o: ../../Source/UI/Common/FineTuningValueIndicator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FineTuningValueIndicator.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySelector_58be6196.o: ../../Source/UI/Common/KeySelector.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySelector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MenuButton_1d9ba4c3.o: ../../Source/UI/Common/MenuButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MenuButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MobileComboBox_bdac55f1.o: ../../Source/UI/Common/MobileComboBox.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MobileComboBox.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ModeIndicatorComponent_7338ae83.o: ../../Source/UI/Common/ModeIndicatorComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ModeIndicatorComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OverlayButton_7ef2bf1a.o: ../../Source/UI/Common/OverlayButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OverlayButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PlayButton_38579458.o: ../../Source/UI/Common/PlayButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PlayButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginWindow_66751815.o: ../../Source/UI/Common/PluginWindow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PluginWindow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RadioButton_45b0c9c5.o: ../../Source/UI/Common/RadioButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RadioButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ScaleEditor_6bc10d0f.o: ../../Source/UI/Common/ScaleEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ScaleEditor.cpp"
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

$(JUCE_OBJDIR)/FadingDialog_905af7a1.o: ../../Source/UI/Dialogs/FadingDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FadingDialog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySignatureDialog_b4559c41.o: ../../Source/UI/Dialogs/KeySignatureDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignatureDialog.cpp"
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

$(JUCE_OBJDIR)/Headline_2606435d.o: ../../Source/UI/Headline/Headline.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Headline.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HeadlineDropdown_494f430e.o: ../../Source/UI/Headline/HeadlineDropdown.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HeadlineDropdown.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HeadlineItem_cd7bcd10.o: ../../Source/UI/Headline/HeadlineItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HeadlineItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HeadlineItemArrow_da2ab0a3.o: ../../Source/UI/Headline/HeadlineItemArrow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HeadlineItemArrow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HeadlineItemHighlighter_98266d3b.o: ../../Source/UI/Headline/HeadlineItemHighlighter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HeadlineItemHighlighter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HeadlineNavigationPanel_eeb8893d.o: ../../Source/UI/Headline/HeadlineNavigationPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HeadlineNavigationPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MultiTouchController_63afb603.o: ../../Source/UI/Input/MultiTouchController.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MultiTouchController.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MenuItemComponent_2eff5284.o: ../../Source/UI/Menus/Base/MenuItemComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MenuItemComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MenuItemComponentMarker_9fc5be9e.o: ../../Source/UI/Menus/Base/MenuItemComponentMarker.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MenuItemComponentMarker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MenuPanel_ee874d5e.o: ../../Source/UI/Menus/Base/MenuPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MenuPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioPluginSelectionMenu_3e54c8bc.o: ../../Source/UI/Menus/SelectionMenus/AudioPluginSelectionMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioPluginSelectionMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentNodeSelectionMenu_d61c1e72.o: ../../Source/UI/Menus/SelectionMenus/InstrumentNodeSelectionMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentNodeSelectionMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternRollSelectionMenu_1f7a27b8.o: ../../Source/UI/Menus/SelectionMenus/PatternRollSelectionMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternRollSelectionMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoRollSelectionMenu_69b358df.o: ../../Source/UI/Menus/SelectionMenus/PianoRollSelectionMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoRollSelectionMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlHistorySelectionMenu_55cc6e76.o: ../../Source/UI/Menus/SelectionMenus/VersionControlHistorySelectionMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlHistorySelectionMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlStageSelectionMenu_d66783cc.o: ../../Source/UI/Menus/SelectionMenus/VersionControlStageSelectionMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlStageSelectionMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationMenu_27bac505.o: ../../Source/UI/Menus/AnnotationMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentMenu_4fd005bd.o: ../../Source/UI/Menus/InstrumentMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiTrackMenu_ae6d14fc.o: ../../Source/UI/Menus/MidiTrackMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiTrackMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NotesTuningPanel_5fadfa25.o: ../../Source/UI/Menus/NotesTuningPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NotesTuningPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OrchestraPitMenu_2065d304.o: ../../Source/UI/Menus/OrchestraPitMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OrchestraPitMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectMenu_5ab725eb.o: ../../Source/UI/Menus/ProjectMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineMenu_2c071c97.o: ../../Source/UI/Menus/TimelineMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureMenu_9a8856bd.o: ../../Source/UI/Menus/TimeSignatureMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlMenu_cb7aa03b.o: ../../Source/UI/Menus/VersionControlMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WorkspaceMenu_25163be7.o: ../../Source/UI/Menus/WorkspaceMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling WorkspaceMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DashboardMenu_3382ca45.o: ../../Source/UI/Pages/Dashboard/Menu/DashboardMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DashboardMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RecentProjectRow_4cd5f954.o: ../../Source/UI/Pages/Dashboard/Menu/RecentProjectRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RecentProjectRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LoginButton_795d376d.o: ../../Source/UI/Pages/Dashboard/Menu/LoginButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LoginButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CreateProjectButton_44f66081.o: ../../Source/UI/Pages/Dashboard/Menu/CreateProjectButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CreateProjectButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OpenProjectButton_4f878c33.o: ../../Source/UI/Pages/Dashboard/Menu/OpenProjectButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OpenProjectButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Dashboard_81cfc8dc.o: ../../Source/UI/Pages/Dashboard/Dashboard.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Dashboard.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InitScreen_e3d0171e.o: ../../Source/UI/Pages/Dashboard/InitScreen.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InitScreen.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogoFader_b8652153.o: ../../Source/UI/Pages/Dashboard/LogoFader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LogoFader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UpdatesInfoComponent_ebc1dc07.o: ../../Source/UI/Pages/Dashboard/UpdatesInfoComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UpdatesInfoComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UserProfileComponent_f7816701.o: ../../Source/UI/Pages/Dashboard/UserProfileComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UserProfileComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioPluginEditorPage_d260a64d.o: ../../Source/UI/Pages/Instruments/Editor/AudioPluginEditorPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioPluginEditorPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentEditor_e7548c16.o: ../../Source/UI/Pages/Instruments/Editor/InstrumentEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentEditorConnector_813d7c21.o: ../../Source/UI/Pages/Instruments/Editor/InstrumentEditorConnector.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditorConnector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentComponent_f2b63e3e.o: ../../Source/UI/Pages/Instruments/Editor/InstrumentComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentEditorPin_387238a9.o: ../../Source/UI/Pages/Instruments/Editor/InstrumentEditorPin.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditorPin.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioPluginsListComponent_27b60455.o: ../../Source/UI/Pages/Instruments/AudioPluginsListComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioPluginsListComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentsListComponent_2905dadd.o: ../../Source/UI/Pages/Instruments/InstrumentsListComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentsListComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OrchestraPitPage_52882f47.o: ../../Source/UI/Pages/Instruments/OrchestraPitPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OrchestraPitPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectPage_f686b2d5.o: ../../Source/UI/Pages/Project/ProjectPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioSettings_716d894.o: ../../Source/UI/Pages/Settings/AudioSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ComponentsList_fad5fa03.o: ../../Source/UI/Pages/Settings/ComponentsList.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ComponentsList.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LabeledSettingsWrapper_3ae3ca8c.o: ../../Source/UI/Pages/Settings/LabeledSettingsWrapper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LabeledSettingsWrapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SimpleSettingsWrapper_7dac70d9.o: ../../Source/UI/Pages/Settings/SimpleSettingsWrapper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SimpleSettingsWrapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsListItemHighlighter_e14317a8.o: ../../Source/UI/Pages/Settings/SettingsListItemHighlighter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsListItemHighlighter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsListItemSelection_3054dbd3.o: ../../Source/UI/Pages/Settings/SettingsListItemSelection.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsListItemSelection.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SettingsPage_ab9efe01.o: ../../Source/UI/Pages/Settings/SettingsPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SettingsPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SyncSettings_7b017e8d.o: ../../Source/UI/Pages/Settings/SyncSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SyncSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SyncSettingsItem_6f05f040.o: ../../Source/UI/Pages/Settings/SyncSettingsItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SyncSettingsItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ThemeSettings_1f7e5e07.o: ../../Source/UI/Pages/Settings/ThemeSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ThemeSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ThemeSettingsItem_c2b282ba.o: ../../Source/UI/Pages/Settings/ThemeSettingsItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ThemeSettingsItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TranslationSettings_f4a7f26f.o: ../../Source/UI/Pages/Settings/TranslationSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TranslationSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TranslationSettingsItem_6aba322.o: ../../Source/UI/Pages/Settings/TranslationSettingsItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TranslationSettingsItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UserInterfaceSettings_8b19486c.o: ../../Source/UI/Pages/Settings/UserInterfaceSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UserInterfaceSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HistoryComponent_115e47f9.o: ../../Source/UI/Pages/VCS/HistoryComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HistoryComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionComponent_51e2497c.o: ../../Source/UI/Pages/VCS/RevisionComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionConnectorComponent_a2024cdb.o: ../../Source/UI/Pages/VCS/RevisionConnectorComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionConnectorComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionItemComponent_8654a029.o: ../../Source/UI/Pages/VCS/RevisionItemComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionItemComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionTooltipComponent_c893ee65.o: ../../Source/UI/Pages/VCS/RevisionTooltipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionTooltipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RevisionTreeComponent_2b8f217e.o: ../../Source/UI/Pages/VCS/RevisionTreeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RevisionTreeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/StageComponent_f36288f.o: ../../Source/UI/Pages/VCS/StageComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling StageComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlEditor_c12aa262.o: ../../Source/UI/Pages/VCS/VersionControlEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ArpPreviewTool_98f46997.o: ../../Source/UI/Popups/ArpPreviewTool.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ArpPreviewTool.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordPreviewTool_422991ba.o: ../../Source/UI/Popups/ChordPreviewTool.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ChordPreviewTool.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ScalePreviewTool_f58f5b0c.o: ../../Source/UI/Popups/ScalePreviewTool.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ScalePreviewTool.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RescalePreviewTool_6e22f57f.o: ../../Source/UI/Popups/RescalePreviewTool.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RescalePreviewTool.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FailTooltip_99f74519.o: ../../Source/UI/Popups/FailTooltip.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FailTooltip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HelioCallout_90a2f131.o: ../../Source/UI/Popups/HelioCallout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HelioCallout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PopupButton_5c5e0092.o: ../../Source/UI/Popups/PopupButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PopupButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PopupCustomButton_6d33e2a3.o: ../../Source/UI/Popups/PopupCustomButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PopupCustomButton.cpp"
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

$(JUCE_OBJDIR)/ClipRangeIndicator_c202e394.o: ../../Source/UI/Sequencer/Header/ClipRangeIndicator.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ClipRangeIndicator.cpp"
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

$(JUCE_OBJDIR)/CutPointMark_e432a7df.o: ../../Source/UI/Sequencer/Helpers/CutPointMark.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CutPointMark.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollExpandMark_c3022404.o: ../../Source/UI/Sequencer/Helpers/HybridRollExpandMark.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollExpandMark.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KnifeToolHelper_d68a471.o: ../../Source/UI/Sequencer/Helpers/KnifeToolHelper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KnifeToolHelper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineWarningMarker_6d5c36fb.o: ../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineWarningMarker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternOperations_aae46f62.o: ../../Source/UI/Sequencer/Helpers/PatternOperations.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternOperations.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SequencerOperations_4fa1bea3.o: ../../Source/UI/Sequencer/Helpers/SequencerOperations.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SequencerOperations.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationCurveClipComponent_9bff9d3.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationCurveClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationCurveHelper_c47ea72.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveHelper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationCurveHelper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationCurveEventComponent_7e9080c7.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationCurveEventComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationCurveEventsConnector_d23999ba.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip/AutomationCurveEventsConnector.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationCurveEventsConnector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationStepsClipComponent_a9f38f63.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepsClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationStepsClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationStepEventComponent_875750ac.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationStepEventComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AutomationStepEventsConnector_e24cc675.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip/AutomationStepEventsConnector.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AutomationStepEventsConnector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoClipComponent_4f1366b1.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip/PianoClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ClipComponent_43b43980.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/ClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DummyClipComponent_dddacfc.o: ../../Source/UI/Sequencer/PatternRoll/ClipComponents/DummyClipComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DummyClipComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternRoll_9707f85b.o: ../../Source/UI/Sequencer/PatternRoll/PatternRoll.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternRoll.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoteComponent_465c17d2.o: ../../Source/UI/Sequencer/PianoRoll/NoteComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoteComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoteResizerLeft_c3d11e3a.o: ../../Source/UI/Sequencer/PianoRoll/NoteResizerLeft.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoteResizerLeft.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NoteResizerRight_f3a02f93.o: ../../Source/UI/Sequencer/PianoRoll/NoteResizerRight.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NoteResizerRight.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoRoll_c125538d.o: ../../Source/UI/Sequencer/PianoRoll/PianoRoll.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoRoll.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SequencerSidebarLeft_1fad65fa.o: ../../Source/UI/Sequencer/Sidebars/SequencerSidebarLeft.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SequencerSidebarLeft.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SequencerSidebarRight_134cdfd3.o: ../../Source/UI/Sequencer/Sidebars/SequencerSidebarRight.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SequencerSidebarRight.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationLargeComponent_d95ee3c1.o: ../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationLargeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationLargeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationSmallComponent_f4fc5575.o: ../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationSmallComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationSmallComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationsProjectMap_e8c59661.o: ../../Source/UI/Sequencer/MiniMaps/AnnotationsMap/AnnotationsProjectMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationsProjectMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySignatureLargeComponent_7ce9b141.o: ../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureLargeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignatureLargeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySignatureSmallComponent_988722f5.o: ../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignatureSmallComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignatureSmallComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySignaturesProjectMap_4269b8e1.o: ../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap/KeySignaturesProjectMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignaturesProjectMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoProjectMap_b13b64c1.o: ../../Source/UI/Sequencer/MiniMaps/PianoMap/PianoProjectMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoProjectMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackScroller_dba3353a.o: ../../Source/UI/Sequencer/MiniMaps/PianoMap/TrackScroller.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TrackScroller.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TrackScrollerScreen_86ca0406.o: ../../Source/UI/Sequencer/MiniMaps/PianoMap/TrackScrollerScreen.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TrackScrollerScreen.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureLargeComponent_9bda06b.o: ../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureLargeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureLargeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureSmallComponent_255b121f.o: ../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignatureSmallComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureSmallComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignaturesProjectMap_407e29f7.o: ../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap/TimeSignaturesProjectMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignaturesProjectMap.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRoll_b60b10f3.o: ../../Source/UI/Sequencer/HybridRoll.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRoll.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HybridRollEditMode_46b31f60.o: ../../Source/UI/Sequencer/HybridRollEditMode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HybridRollEditMode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Lasso_7dd838ea.o: ../../Source/UI/Sequencer/Lasso.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Lasso.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LassoListeners_6bea92bf.o: ../../Source/UI/Sequencer/LassoListeners.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LassoListeners.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiEventComponent_a6266ede.o: ../../Source/UI/Sequencer/MidiEventComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiEventComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SelectionComponent_9dff102b.o: ../../Source/UI/Sequencer/SelectionComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SelectionComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SequencerLayout_58e2f98b.o: ../../Source/UI/Sequencer/SequencerLayout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SequencerLayout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ComponentFader_6572b0c.o: ../../Source/UI/Themes/ComponentFader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ComponentFader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HelioTheme_d6ec9067.o: ../../Source/UI/Themes/HelioTheme.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HelioTheme.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Icons_7879bcb1.o: ../../Source/UI/Themes/Icons.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Icons.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FramePanel_4a84952a.o: ../../Source/UI/Themes/FramePanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FramePanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DialogPanel_d1de90b3.o: ../../Source/UI/Themes/DialogPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DialogPanel.cpp"
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

$(JUCE_OBJDIR)/SeparatorVerticalSkew_58e6853c.o: ../../Source/UI/Themes/SeparatorVerticalSkew.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SeparatorVerticalSkew.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowDownwards_29eb3a6c.o: ../../Source/UI/Themes/ShadowDownwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowDownwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowUpwards_83021913.o: ../../Source/UI/Themes/ShadowUpwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowUpwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowLeftwards_279bc0a7.o: ../../Source/UI/Themes/ShadowLeftwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowLeftwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ShadowRightwards_a7d8422e.o: ../../Source/UI/Themes/ShadowRightwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ShadowRightwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ViewportKineticSlider_45304cab.o: ../../Source/UI/Themes/ViewportKineticSlider.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ViewportKineticSlider.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainLayout_4f58d3f.o: ../../Source/UI/MainLayout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MainLayout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Common_6ff75bf4.o: ../../Source/Common.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Common.cpp"
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

$(JUCE_OBJDIR)/include_juce_core_9b124993.o: ../Projucer/JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_core.cpp"
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

check-pkg-config:
	@command -v pkg-config >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@pkg-config --print-errors alsa freetype2 x11 xext xinerama libcurl

clean:
	@echo Cleaning Helio
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping Helio
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(TARGET)

-include $(OBJECTS_APP:%.o=%.d)
