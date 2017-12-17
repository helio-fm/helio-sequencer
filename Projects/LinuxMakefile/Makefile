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

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DDEBUG=1 -D_DEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=1.7.6 -DJUCE_APP_VERSION_HEX=0x10706 $(shell pkg-config --cflags alsa freetype2 libcurl x11 xext xinerama) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/ASIO/common -I../../ThirdParty/JUCE/modules/juce_audio_basics -I../../ThirdParty/JUCE/modules/juce_audio_devices -I../../ThirdParty/JUCE/modules/juce_audio_formats -I../../ThirdParty/JUCE/modules/juce_audio_processors -I../../ThirdParty/JUCE/modules/juce_audio_utils -I../../ThirdParty/JUCE/modules/juce_audio_utils/players -I../../ThirdParty/JUCE/modules/juce_core -I../../ThirdParty/JUCE/modules/juce_cryptography -I../../ThirdParty/JUCE/modules/juce_data_structures -I../../ThirdParty/JUCE/modules/juce_events -I../../ThirdParty/JUCE/modules/juce_graphics -I../../ThirdParty/JUCE/modules/juce_gui_basics -I../../ThirdParty/JUCE/modules/juce_gui_extra -I../../ThirdParty/JUCE/modules/juce_opengl -I../../ThirdParty/JUCE/modules/juce_osc -I../../Source/ -I../../Source/Core/ -I../../Source/Core/App -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Audio/Waveform -I../../Source/Core/Clipboard -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Serialization -I../../Source/Core/Supervisor -I../../Source/Core/Tools -I../../Source/Core/Translation -I../../Source/Core/Tree -I../../Source/Core/Tutorial -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/VCS/Network -I../../Source/UI/ -I../../Source/UI/CodeEditor -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Console -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Intro -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/AnnotationsMap -I../../Source/UI/Sequencer/AutomationMap -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/TimeSignaturesMap -I../../Source/UI/Sequencer/KeySignaturesMap -I../../Source/UI/Sequencer/TrackMap -I../../Source/UI/Sequencer/TriggersMap -I../../Source/UI/Sequencer/WaveformMap -I../../Source/UI/Popups -I../../Source/UI/Popups/ChordBuilder -I../../Source/UI/Pages/Project -I../../Source/UI/Sidebars -I../../Source/UI/Pages/Settings -I../../Source/UI/Themes -I../../Source/UI/Tree -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Workspace -I../../Source/UI/Pages/Workspace/Menu -I../../Source/UI/Pages/Workspace/Scroller $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++14 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl x11 xext xinerama) -lGL -ldl -lpthread -lrt $(LDFLAGS)

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

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=1.7.6 -DJUCE_APP_VERSION_HEX=0x10706 $(shell pkg-config --cflags alsa freetype2 libcurl x11 xext xinerama) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/ASIO/common -I../../ThirdParty/JUCE/modules/juce_audio_basics -I../../ThirdParty/JUCE/modules/juce_audio_devices -I../../ThirdParty/JUCE/modules/juce_audio_formats -I../../ThirdParty/JUCE/modules/juce_audio_processors -I../../ThirdParty/JUCE/modules/juce_audio_utils -I../../ThirdParty/JUCE/modules/juce_audio_utils/players -I../../ThirdParty/JUCE/modules/juce_core -I../../ThirdParty/JUCE/modules/juce_cryptography -I../../ThirdParty/JUCE/modules/juce_data_structures -I../../ThirdParty/JUCE/modules/juce_events -I../../ThirdParty/JUCE/modules/juce_graphics -I../../ThirdParty/JUCE/modules/juce_gui_basics -I../../ThirdParty/JUCE/modules/juce_gui_extra -I../../ThirdParty/JUCE/modules/juce_opengl -I../../ThirdParty/JUCE/modules/juce_osc -I../../Source/ -I../../Source/Core/ -I../../Source/Core/App -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Audio/Waveform -I../../Source/Core/Clipboard -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Serialization -I../../Source/Core/Supervisor -I../../Source/Core/Tools -I../../Source/Core/Translation -I../../Source/Core/Tree -I../../Source/Core/Tutorial -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/VCS/Network -I../../Source/UI/ -I../../Source/UI/CodeEditor -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Console -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Intro -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/AnnotationsMap -I../../Source/UI/Sequencer/AutomationMap -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/TimeSignaturesMap -I../../Source/UI/Sequencer/KeySignaturesMap -I../../Source/UI/Sequencer/TrackMap -I../../Source/UI/Sequencer/TriggersMap -I../../Source/UI/Sequencer/WaveformMap -I../../Source/UI/Popups -I../../Source/UI/Popups/ChordBuilder -I../../Source/UI/Pages/Project -I../../Source/UI/Sidebars -I../../Source/UI/Pages/Settings -I../../Source/UI/Themes -I../../Source/UI/Tree -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Workspace -I../../Source/UI/Pages/Workspace/Menu -I../../Source/UI/Pages/Workspace/Scroller $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 -flto -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++14 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl x11 xext xinerama) -fvisibility=hidden -flto -lGL -ldl -lpthread -lrt $(LDFLAGS)

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

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=1.7.6 -DJUCE_APP_VERSION_HEX=0x10706 $(shell pkg-config --cflags alsa freetype2 libcurl x11 xext xinerama) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/ASIO/common -I../../ThirdParty/JUCE/modules/juce_audio_basics -I../../ThirdParty/JUCE/modules/juce_audio_devices -I../../ThirdParty/JUCE/modules/juce_audio_formats -I../../ThirdParty/JUCE/modules/juce_audio_processors -I../../ThirdParty/JUCE/modules/juce_audio_utils -I../../ThirdParty/JUCE/modules/juce_audio_utils/players -I../../ThirdParty/JUCE/modules/juce_core -I../../ThirdParty/JUCE/modules/juce_cryptography -I../../ThirdParty/JUCE/modules/juce_data_structures -I../../ThirdParty/JUCE/modules/juce_events -I../../ThirdParty/JUCE/modules/juce_graphics -I../../ThirdParty/JUCE/modules/juce_gui_basics -I../../ThirdParty/JUCE/modules/juce_gui_extra -I../../ThirdParty/JUCE/modules/juce_opengl -I../../ThirdParty/JUCE/modules/juce_osc -I../../Source/ -I../../Source/Core/ -I../../Source/Core/App -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Audio/Waveform -I../../Source/Core/Clipboard -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Serialization -I../../Source/Core/Supervisor -I../../Source/Core/Tools -I../../Source/Core/Translation -I../../Source/Core/Tree -I../../Source/Core/Tutorial -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/VCS/Network -I../../Source/UI/ -I../../Source/UI/CodeEditor -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Console -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Intro -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/AnnotationsMap -I../../Source/UI/Sequencer/AutomationMap -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/TimeSignaturesMap -I../../Source/UI/Sequencer/KeySignaturesMap -I../../Source/UI/Sequencer/TrackMap -I../../Source/UI/Sequencer/TriggersMap -I../../Source/UI/Sequencer/WaveformMap -I../../Source/UI/Popups -I../../Source/UI/Popups/ChordBuilder -I../../Source/UI/Pages/Project -I../../Source/UI/Sidebars -I../../Source/UI/Pages/Settings -I../../Source/UI/Themes -I../../Source/UI/Tree -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Workspace -I../../Source/UI/Pages/Workspace/Menu -I../../Source/UI/Pages/Workspace/Scroller $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 -flto -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++14 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl x11 xext xinerama) -fvisibility=hidden -flto -lGL -ldl -lpthread -lrt $(LDFLAGS)

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

  JUCE_CPPFLAGS := $(DEPFLAGS) -DLINUX=1 -DNDEBUG=1 -DJUCE_DONT_DECLARE_PROJECTINFO=1 -DJUCER_LINUX_MAKE_B650AE49=1 -DJUCE_APP_VERSION=1.7.6 -DJUCE_APP_VERSION_HEX=0x10706 $(shell pkg-config --cflags alsa freetype2 libcurl x11 xext xinerama) -pthread -I../../ThirdParty/VST_SDK/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/ASIO/common -I../../ThirdParty/JUCE/modules/juce_audio_basics -I../../ThirdParty/JUCE/modules/juce_audio_devices -I../../ThirdParty/JUCE/modules/juce_audio_formats -I../../ThirdParty/JUCE/modules/juce_audio_processors -I../../ThirdParty/JUCE/modules/juce_audio_utils -I../../ThirdParty/JUCE/modules/juce_audio_utils/players -I../../ThirdParty/JUCE/modules/juce_core -I../../ThirdParty/JUCE/modules/juce_cryptography -I../../ThirdParty/JUCE/modules/juce_data_structures -I../../ThirdParty/JUCE/modules/juce_events -I../../ThirdParty/JUCE/modules/juce_graphics -I../../ThirdParty/JUCE/modules/juce_gui_basics -I../../ThirdParty/JUCE/modules/juce_gui_extra -I../../ThirdParty/JUCE/modules/juce_opengl -I../../ThirdParty/JUCE/modules/juce_osc -I../../Source/ -I../../Source/Core/ -I../../Source/Core/App -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Audio/Waveform -I../../Source/Core/Clipboard -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Serialization -I../../Source/Core/Supervisor -I../../Source/Core/Tools -I../../Source/Core/Translation -I../../Source/Core/Tree -I../../Source/Core/Tutorial -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/VCS/Network -I../../Source/UI/ -I../../Source/UI/CodeEditor -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Console -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Intro -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/AnnotationsMap -I../../Source/UI/Sequencer/AutomationMap -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/TimeSignaturesMap -I../../Source/UI/Sequencer/KeySignaturesMap -I../../Source/UI/Sequencer/TrackMap -I../../Source/UI/Sequencer/TriggersMap -I../../Source/UI/Sequencer/WaveformMap -I../../Source/UI/Popups -I../../Source/UI/Popups/ChordBuilder -I../../Source/UI/Pages/Project -I../../Source/UI/Sidebars -I../../Source/UI/Sidebars/Rollovers -I../../Source/UI/Pages/Settings -I../../Source/UI/Themes -I../../Source/UI/Tree -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Workspace -I../../Source/UI/Pages/Workspace/Menu -I../../Source/UI/Pages/Workspace/Scroller $(CPPFLAGS)
  JUCE_CPPFLAGS_APP := -DJucePlugin_Build_VST=0 -DJucePlugin_Build_VST3=0 -DJucePlugin_Build_AU=0 -DJucePlugin_Build_AUv3=0 -DJucePlugin_Build_RTAS=0 -DJucePlugin_Build_AAX=0 -DJucePlugin_Build_Standalone=0
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O0 -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++14 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl x11 xext xinerama) -fvisibility=hidden -lGL -ldl -lpthread -lrt $(LDFLAGS)

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
  $(JUCE_OBJDIR)/Scale_67df17ad.o \
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
  $(JUCE_OBJDIR)/PatternEditorTreeItem_35a7d6ae.o \
  $(JUCE_OBJDIR)/PianoTrackTreeItem_850d2d9d.o \
  $(JUCE_OBJDIR)/ProjectInfo_52f6a347.o \
  $(JUCE_OBJDIR)/ProjectTimeline_9655ff5a.o \
  $(JUCE_OBJDIR)/ProjectTreeItem_334546ea.o \
  $(JUCE_OBJDIR)/RootTreeItem_3b150bd.o \
  $(JUCE_OBJDIR)/SettingsTreeItem_c1194ffe.o \
  $(JUCE_OBJDIR)/TrackGroupTreeItem_8371d50f.o \
  $(JUCE_OBJDIR)/TransientTreeItems_f574f302.o \
  $(JUCE_OBJDIR)/TreeItem_3983613b.o \
  $(JUCE_OBJDIR)/TreeItemChildrenSerializer_7365544c.o \
  $(JUCE_OBJDIR)/TreeNavigationHistory_ed66d482.o \
  $(JUCE_OBJDIR)/VersionControlTreeItem_f871c280.o \
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
  $(JUCE_OBJDIR)/FatalErrorScreen_2f541f02.o \
  $(JUCE_OBJDIR)/KeySelector_58be6196.o \
  $(JUCE_OBJDIR)/MenuButton_1d9ba4c3.o \
  $(JUCE_OBJDIR)/ModeIndicatorComponent_7338ae83.o \
  $(JUCE_OBJDIR)/PlayButton_38579458.o \
  $(JUCE_OBJDIR)/PluginWindow_66751815.o \
  $(JUCE_OBJDIR)/RadioButton_45b0c9c5.o \
  $(JUCE_OBJDIR)/ScaleEditor_6bc10d0f.o \
  $(JUCE_OBJDIR)/SpectralLogo_f6755a41.o \
  $(JUCE_OBJDIR)/ViewportFitProxyComponent_a93fb792.o \
  $(JUCE_OBJDIR)/AnnotationDialog_bb76a597.o \
  $(JUCE_OBJDIR)/AuthorizationDialog_ebcfd46b.o \
  $(JUCE_OBJDIR)/DialogComboBox_157eeac5.o \
  $(JUCE_OBJDIR)/FadingDialog_905af7a1.o \
  $(JUCE_OBJDIR)/KeySignatureDialog_b4559c41.o \
  $(JUCE_OBJDIR)/ModalDialogConfirmation_23a5c174.o \
  $(JUCE_OBJDIR)/ModalDialogInput_6af917b5.o \
  $(JUCE_OBJDIR)/RenderDialog_d54ea4be.o \
  $(JUCE_OBJDIR)/TimeSignatureDialog_7d6669dd.o \
  $(JUCE_OBJDIR)/UpdateDialog_9c0a7831.o \
  $(JUCE_OBJDIR)/Headline_2606435d.o \
  $(JUCE_OBJDIR)/HeadlineDropdown_494f430e.o \
  $(JUCE_OBJDIR)/HeadlineItem_cd7bcd10.o \
  $(JUCE_OBJDIR)/HeadlineNavigationPanel_eeb8893d.o \
  $(JUCE_OBJDIR)/HotkeyScheme_81e868f8.o \
  $(JUCE_OBJDIR)/MultiTouchController_63afb603.o \
  $(JUCE_OBJDIR)/CommandItemComponent_4a58c510.o \
  $(JUCE_OBJDIR)/CommandItemComponentMarker_88aa182a.o \
  $(JUCE_OBJDIR)/CommandPanel_54120bea.o \
  $(JUCE_OBJDIR)/AnnotationCommandPanel_753f231f.o \
  $(JUCE_OBJDIR)/ArpeggiatorEditorPanel_592308f5.o \
  $(JUCE_OBJDIR)/InstrumentCommandPanel_65442bd7.o \
  $(JUCE_OBJDIR)/InstrumentsCommandPanel_b7074758.o \
  $(JUCE_OBJDIR)/LayerCommandPanel_c07f07bd.o \
  $(JUCE_OBJDIR)/NotesTuningPanel_5fadfa25.o \
  $(JUCE_OBJDIR)/PatternEditorCommandPanel_72c1a989.o \
  $(JUCE_OBJDIR)/PatternRollSelectionCommandPanel_adfdf14f.o \
  $(JUCE_OBJDIR)/PianoRollSelectionCommandPanel_668e86b6.o \
  $(JUCE_OBJDIR)/ProjectCommandPanel_cb573e05.o \
  $(JUCE_OBJDIR)/TimelineCommandPanel_3e4bc8b1.o \
  $(JUCE_OBJDIR)/TimeSignatureCommandPanel_1f1b7cd7.o \
  $(JUCE_OBJDIR)/VCSCommandPanel_5a9dbb52.o \
  $(JUCE_OBJDIR)/AudioPluginEditorPage_d260a64d.o \
  $(JUCE_OBJDIR)/InstrumentEditor_e7548c16.o \
  $(JUCE_OBJDIR)/InstrumentEditorConnector_813d7c21.o \
  $(JUCE_OBJDIR)/InstrumentEditorNode_355634b8.o \
  $(JUCE_OBJDIR)/InstrumentEditorPin_387238a9.o \
  $(JUCE_OBJDIR)/InstrumentRow_dc9ffb13.o \
  $(JUCE_OBJDIR)/InstrumentsPage_929491fb.o \
  $(JUCE_OBJDIR)/InitScreen_afb02946.o \
  $(JUCE_OBJDIR)/ProjectPage_f686b2d5.o \
  $(JUCE_OBJDIR)/ProjectPageDefault_85a13436.o \
  $(JUCE_OBJDIR)/ProjectPagePhone_dd232ce3.o \
  $(JUCE_OBJDIR)/AudioSettings_716d894.o \
  $(JUCE_OBJDIR)/AuthorizationSettings_d0749ab7.o \
  $(JUCE_OBJDIR)/ComponentsList_fad5fa03.o \
  $(JUCE_OBJDIR)/IntroSettingsWrapper_2fdb093.o \
  $(JUCE_OBJDIR)/LabeledSettingsWrapper_3ae3ca8c.o \
  $(JUCE_OBJDIR)/LogComponent_cf667f48.o \
  $(JUCE_OBJDIR)/OpenGLSettings_5037d421.o \
  $(JUCE_OBJDIR)/PluginsList_be67cdd9.o \
  $(JUCE_OBJDIR)/SettingsListItemHighlighter_e14317a8.o \
  $(JUCE_OBJDIR)/SettingsListItemSelection_3054dbd3.o \
  $(JUCE_OBJDIR)/SettingsPage_ab9efe01.o \
  $(JUCE_OBJDIR)/ThemeSettings_1f7e5e07.o \
  $(JUCE_OBJDIR)/ThemeSettingsItem_c2b282ba.o \
  $(JUCE_OBJDIR)/TranslationSettings_f4a7f26f.o \
  $(JUCE_OBJDIR)/TranslationSettingsItem_6aba322.o \
  $(JUCE_OBJDIR)/HistoryComponent_115e47f9.o \
  $(JUCE_OBJDIR)/RevisionComponent_51e2497c.o \
  $(JUCE_OBJDIR)/RevisionConnectorComponent_a2024cdb.o \
  $(JUCE_OBJDIR)/RevisionItemComponent_8654a029.o \
  $(JUCE_OBJDIR)/RevisionTooltipComponent_c893ee65.o \
  $(JUCE_OBJDIR)/RevisionTreeComponent_2b8f217e.o \
  $(JUCE_OBJDIR)/StageComponent_f36288f.o \
  $(JUCE_OBJDIR)/VersionControlEditor_c12aa262.o \
  $(JUCE_OBJDIR)/VersionControlEditorDefault_7d881c09.o \
  $(JUCE_OBJDIR)/VersionControlEditorPhone_737ed76.o \
  $(JUCE_OBJDIR)/VersionControlEditorPhoneViewportContent_776ef127.o \
  $(JUCE_OBJDIR)/CreateProjectRow_dcd38f6.o \
  $(JUCE_OBJDIR)/OpenProjectRow_a3f0b6c4.o \
  $(JUCE_OBJDIR)/RecentProjectRow_e2958c15.o \
  $(JUCE_OBJDIR)/SignInRow_52d8a749.o \
  $(JUCE_OBJDIR)/WorkspaceMenu_24248825.o \
  $(JUCE_OBJDIR)/LogoFader_cee20d94.o \
  $(JUCE_OBJDIR)/LogoImage_ce3d42f9.o \
  $(JUCE_OBJDIR)/WorkspacePage_9d6d994d.o \
  $(JUCE_OBJDIR)/ChordBuilder_b1861b03.o \
  $(JUCE_OBJDIR)/ChordTooltip_3e97876b.o \
  $(JUCE_OBJDIR)/FailTooltip_99f74519.o \
  $(JUCE_OBJDIR)/HelioCallout_90a2f131.o \
  $(JUCE_OBJDIR)/PopupButton_5c5e0092.o \
  $(JUCE_OBJDIR)/PopupCustomButton_6d33e2a3.o \
  $(JUCE_OBJDIR)/PopupImageButton_54133d7.o \
  $(JUCE_OBJDIR)/ProgressTooltip_f470232a.o \
  $(JUCE_OBJDIR)/SuccessTooltip_7d75a6f6.o \
  $(JUCE_OBJDIR)/TooltipContainer_52ddacf4.o \
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
  $(JUCE_OBJDIR)/TimelineWarningMarker_6d5c36fb.o \
  $(JUCE_OBJDIR)/WipeSpaceHelper_16b49913.o \
  $(JUCE_OBJDIR)/KeySignatureLargeComponent_99338c78.o \
  $(JUCE_OBJDIR)/KeySignatureSmallComponent_b4d0fe2c.o \
  $(JUCE_OBJDIR)/KeySignaturesTrackMap_7fdcb18.o \
  $(JUCE_OBJDIR)/ClipComponent_3c63e97b.o \
  $(JUCE_OBJDIR)/AutomationClipComponent_ecbdf1e4.o \
  $(JUCE_OBJDIR)/DummyClipComponent_76f3c1e1.o \
  $(JUCE_OBJDIR)/PianoClipComponent_726e7f60.o \
  $(JUCE_OBJDIR)/MidiTrackHeader_edf82885.o \
  $(JUCE_OBJDIR)/MidiTrackInsertHelper_7c398a7f.o \
  $(JUCE_OBJDIR)/PatternRoll_9707f85b.o \
  $(JUCE_OBJDIR)/NoteComponent_465c17d2.o \
  $(JUCE_OBJDIR)/NoteResizerLeft_c3d11e3a.o \
  $(JUCE_OBJDIR)/NoteResizerRight_f3a02f93.o \
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
  $(JUCE_OBJDIR)/NavigationSidebar_4b60998c.o \
  $(JUCE_OBJDIR)/ToolsSidebar_9e5195a7.o \
  $(JUCE_OBJDIR)/ComponentFader_6572b0c.o \
  $(JUCE_OBJDIR)/FontSerializer_ad448bd4.o \
  $(JUCE_OBJDIR)/HelioTheme_d6ec9067.o \
  $(JUCE_OBJDIR)/Icons_7879bcb1.o \
  $(JUCE_OBJDIR)/LightShadowDownwards_329253f2.o \
  $(JUCE_OBJDIR)/LightShadowLeftwards_3042da2d.o \
  $(JUCE_OBJDIR)/LightShadowRightwards_b4145968.o \
  $(JUCE_OBJDIR)/LightShadowUpwards_7f8ddc19.o \
  $(JUCE_OBJDIR)/LighterShadowDownwards_5a0a0425.o \
  $(JUCE_OBJDIR)/LighterShadowUpwards_3589fd0c.o \
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
  $(JUCE_OBJDIR)/ShadowDownwards_29eb3a6c.o \
  $(JUCE_OBJDIR)/ShadowHorizontalFading_a0923750.o \
  $(JUCE_OBJDIR)/ShadowLeftwards_279bc0a7.o \
  $(JUCE_OBJDIR)/ShadowRightwards_a7d8422e.o \
  $(JUCE_OBJDIR)/ShadowUpwards_83021913.o \
  $(JUCE_OBJDIR)/ViewportKineticSlider_45304cab.o \
  $(JUCE_OBJDIR)/TreeItemComponent_45f5f057.o \
  $(JUCE_OBJDIR)/TreeItemComponentCompact_1c0e4456.o \
  $(JUCE_OBJDIR)/TreeItemComponentDefault_335c2074.o \
  $(JUCE_OBJDIR)/TreeItemMarkerDefault_f4465881.o \
  $(JUCE_OBJDIR)/TreeItemMarkerCompact_dcf87c63.o \
  $(JUCE_OBJDIR)/TreeItemMenuButton_55ef6521.o \
  $(JUCE_OBJDIR)/MainLayout_4f58d3f.o \
  $(JUCE_OBJDIR)/MainWindow_5f9c9b05.o \
  $(JUCE_OBJDIR)/Common_6ff75bf4.o \
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

$(JUCE_OBJDIR)/Scale_67df17ad.o: ../../Source/Core/Midi/Scale.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Scale.cpp"
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

$(JUCE_OBJDIR)/PatternEditorTreeItem_35a7d6ae.o: ../../Source/Core/Tree/PatternEditorTreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternEditorTreeItem.cpp"
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

$(JUCE_OBJDIR)/TransientTreeItems_f574f302.o: ../../Source/Core/Tree/TransientTreeItems.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TransientTreeItems.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItem_3983613b.o: ../../Source/Core/Tree/TreeItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemChildrenSerializer_7365544c.o: ../../Source/Core/Tree/TreeItemChildrenSerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemChildrenSerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeNavigationHistory_ed66d482.o: ../../Source/Core/Tree/TreeNavigationHistory.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeNavigationHistory.cpp"
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

$(JUCE_OBJDIR)/FatalErrorScreen_2f541f02.o: ../../Source/UI/Common/FatalErrorScreen.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FatalErrorScreen.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySelector_58be6196.o: ../../Source/UI/Common/KeySelector.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySelector.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MenuButton_1d9ba4c3.o: ../../Source/UI/Common/MenuButton.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MenuButton.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ModeIndicatorComponent_7338ae83.o: ../../Source/UI/Common/ModeIndicatorComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ModeIndicatorComponent.cpp"
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

$(JUCE_OBJDIR)/AuthorizationDialog_ebcfd46b.o: ../../Source/UI/Dialogs/AuthorizationDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AuthorizationDialog.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DialogComboBox_157eeac5.o: ../../Source/UI/Dialogs/DialogComboBox.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DialogComboBox.cpp"
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

$(JUCE_OBJDIR)/UpdateDialog_9c0a7831.o: ../../Source/UI/Dialogs/UpdateDialog.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UpdateDialog.cpp"
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

$(JUCE_OBJDIR)/HeadlineNavigationPanel_eeb8893d.o: ../../Source/UI/Headline/HeadlineNavigationPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HeadlineNavigationPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HotkeyScheme_81e868f8.o: ../../Source/UI/Input/HotkeyScheme.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HotkeyScheme.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MultiTouchController_63afb603.o: ../../Source/UI/Input/MultiTouchController.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MultiTouchController.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandItemComponent_4a58c510.o: ../../Source/UI/Menus/Base/CommandItemComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CommandItemComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandItemComponentMarker_88aa182a.o: ../../Source/UI/Menus/Base/CommandItemComponentMarker.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CommandItemComponentMarker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CommandPanel_54120bea.o: ../../Source/UI/Menus/Base/CommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AnnotationCommandPanel_753f231f.o: ../../Source/UI/Menus/AnnotationCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AnnotationCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ArpeggiatorEditorPanel_592308f5.o: ../../Source/UI/Menus/ArpeggiatorEditorPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ArpeggiatorEditorPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentCommandPanel_65442bd7.o: ../../Source/UI/Menus/InstrumentCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentsCommandPanel_b7074758.o: ../../Source/UI/Menus/InstrumentsCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentsCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LayerCommandPanel_c07f07bd.o: ../../Source/UI/Menus/LayerCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LayerCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NotesTuningPanel_5fadfa25.o: ../../Source/UI/Menus/NotesTuningPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NotesTuningPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternEditorCommandPanel_72c1a989.o: ../../Source/UI/Menus/PatternEditorCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternEditorCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternRollSelectionCommandPanel_adfdf14f.o: ../../Source/UI/Menus/PatternRollSelectionCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternRollSelectionCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PianoRollSelectionCommandPanel_668e86b6.o: ../../Source/UI/Menus/PianoRollSelectionCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PianoRollSelectionCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectCommandPanel_cb573e05.o: ../../Source/UI/Menus/ProjectCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineCommandPanel_3e4bc8b1.o: ../../Source/UI/Menus/TimelineCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimeSignatureCommandPanel_1f1b7cd7.o: ../../Source/UI/Menus/TimeSignatureCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimeSignatureCommandPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VCSCommandPanel_5a9dbb52.o: ../../Source/UI/Menus/VCSCommandPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VCSCommandPanel.cpp"
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

$(JUCE_OBJDIR)/InstrumentEditorNode_355634b8.o: ../../Source/UI/Pages/Instruments/Editor/InstrumentEditorNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditorNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentEditorPin_387238a9.o: ../../Source/UI/Pages/Instruments/Editor/InstrumentEditorPin.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentEditorPin.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentRow_dc9ffb13.o: ../../Source/UI/Pages/Instruments/InstrumentRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InstrumentsPage_929491fb.o: ../../Source/UI/Pages/Instruments/InstrumentsPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InstrumentsPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/InitScreen_afb02946.o: ../../Source/UI/Pages/Intro/InitScreen.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling InitScreen.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectPage_f686b2d5.o: ../../Source/UI/Pages/Project/ProjectPage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectPage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectPageDefault_85a13436.o: ../../Source/UI/Pages/Project/ProjectPageDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectPageDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ProjectPagePhone_dd232ce3.o: ../../Source/UI/Pages/Project/ProjectPagePhone.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ProjectPagePhone.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioSettings_716d894.o: ../../Source/UI/Pages/Settings/AudioSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AuthorizationSettings_d0749ab7.o: ../../Source/UI/Pages/Settings/AuthorizationSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AuthorizationSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ComponentsList_fad5fa03.o: ../../Source/UI/Pages/Settings/ComponentsList.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ComponentsList.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/IntroSettingsWrapper_2fdb093.o: ../../Source/UI/Pages/Settings/IntroSettingsWrapper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling IntroSettingsWrapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LabeledSettingsWrapper_3ae3ca8c.o: ../../Source/UI/Pages/Settings/LabeledSettingsWrapper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LabeledSettingsWrapper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogComponent_cf667f48.o: ../../Source/UI/Pages/Settings/LogComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LogComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OpenGLSettings_5037d421.o: ../../Source/UI/Pages/Settings/OpenGLSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OpenGLSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PluginsList_be67cdd9.o: ../../Source/UI/Pages/Settings/PluginsList.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PluginsList.cpp"
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

$(JUCE_OBJDIR)/VersionControlEditorDefault_7d881c09.o: ../../Source/UI/Pages/VCS/VersionControlEditorDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlEditorDefault.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlEditorPhone_737ed76.o: ../../Source/UI/Pages/VCS/VersionControlEditorPhone.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlEditorPhone.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VersionControlEditorPhoneViewportContent_776ef127.o: ../../Source/UI/Pages/VCS/VersionControlEditorPhoneViewportContent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VersionControlEditorPhoneViewportContent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CreateProjectRow_dcd38f6.o: ../../Source/UI/Pages/Workspace/Menu/CreateProjectRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CreateProjectRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/OpenProjectRow_a3f0b6c4.o: ../../Source/UI/Pages/Workspace/Menu/OpenProjectRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling OpenProjectRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RecentProjectRow_e2958c15.o: ../../Source/UI/Pages/Workspace/Menu/RecentProjectRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RecentProjectRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SignInRow_52d8a749.o: ../../Source/UI/Pages/Workspace/Menu/SignInRow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SignInRow.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WorkspaceMenu_24248825.o: ../../Source/UI/Pages/Workspace/Menu/WorkspaceMenu.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling WorkspaceMenu.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogoFader_cee20d94.o: ../../Source/UI/Pages/Workspace/LogoFader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LogoFader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LogoImage_ce3d42f9.o: ../../Source/UI/Pages/Workspace/LogoImage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LogoImage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WorkspacePage_9d6d994d.o: ../../Source/UI/Pages/Workspace/WorkspacePage.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling WorkspacePage.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordBuilder_b1861b03.o: ../../Source/UI/Popups/ChordBuilder/ChordBuilder.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ChordBuilder.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ChordTooltip_3e97876b.o: ../../Source/UI/Popups/ChordBuilder/ChordTooltip.cpp
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

$(JUCE_OBJDIR)/TimelineWarningMarker_6d5c36fb.o: ../../Source/UI/Sequencer/Helpers/TimelineWarningMarker.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineWarningMarker.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/WipeSpaceHelper_16b49913.o: ../../Source/UI/Sequencer/Helpers/WipeSpaceHelper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling WipeSpaceHelper.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySignatureLargeComponent_99338c78.o: ../../Source/UI/Sequencer/KeySignaturesMap/KeySignatureLargeComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignatureLargeComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySignatureSmallComponent_b4d0fe2c.o: ../../Source/UI/Sequencer/KeySignaturesMap/KeySignatureSmallComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignatureSmallComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/KeySignaturesTrackMap_7fdcb18.o: ../../Source/UI/Sequencer/KeySignaturesMap/KeySignaturesTrackMap.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling KeySignaturesTrackMap.cpp"
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

$(JUCE_OBJDIR)/MidiTrackHeader_edf82885.o: ../../Source/UI/Sequencer/PatternRoll/MidiTrackHeader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiTrackHeader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MidiTrackInsertHelper_7c398a7f.o: ../../Source/UI/Sequencer/PatternRoll/MidiTrackInsertHelper.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MidiTrackInsertHelper.cpp"
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

$(JUCE_OBJDIR)/NavigationSidebar_4b60998c.o: ../../Source/UI/Sidebars/NavigationSidebar.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NavigationSidebar.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ToolsSidebar_9e5195a7.o: ../../Source/UI/Sidebars/ToolsSidebar.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ToolsSidebar.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ComponentFader_6572b0c.o: ../../Source/UI/Themes/ComponentFader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ComponentFader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FontSerializer_ad448bd4.o: ../../Source/UI/Themes/FontSerializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FontSerializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HelioTheme_d6ec9067.o: ../../Source/UI/Themes/HelioTheme.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HelioTheme.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Icons_7879bcb1.o: ../../Source/UI/Themes/Icons.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Icons.cpp"
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

$(JUCE_OBJDIR)/LighterShadowDownwards_5a0a0425.o: ../../Source/UI/Themes/LighterShadowDownwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LighterShadowDownwards.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LighterShadowUpwards_3589fd0c.o: ../../Source/UI/Themes/LighterShadowUpwards.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LighterShadowUpwards.cpp"
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

$(JUCE_OBJDIR)/TreeItemComponentCompact_1c0e4456.o: ../../Source/UI/Tree/TreeItemComponentCompact.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemComponentCompact.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TreeItemComponentDefault_335c2074.o: ../../Source/UI/Tree/TreeItemComponentDefault.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TreeItemComponentDefault.cpp"
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

$(JUCE_OBJDIR)/MainLayout_4f58d3f.o: ../../Source/UI/MainLayout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MainLayout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainWindow_5f9c9b05.o: ../../Source/UI/MainWindow.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MainWindow.cpp"
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
