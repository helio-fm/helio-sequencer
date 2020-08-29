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
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DJUCER_LINUX_MAKE_B650AE49=1" "-DJUCE_APP_VERSION=3.2" "-DJUCE_APP_VERSION_HEX=0x30200" $(shell pkg-config --cflags alsa freetype2 libcurl) -pthread -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../../ThirdParty/ASIO/common -I../../Source/ -I../../Source/Core -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Configuration -I../../Source/Core/Configuration/Models -I../../Source/Core/Configuration/ResourceManagers -I../../Source/Core/CommandPalette -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Network/Models -I../../Source/Core/Network/Requests -I../../Source/Core/Network/Services -I../../Source/Core/Scripting -I../../Source/Core/Serialization -I../../Source/Core/Tree -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/Workspace -I../../Source/UI/ -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Menus/SelectionMenus -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Project -I../../Source/UI/Pages/Settings -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Dashboard -I../../Source/UI/Pages/Dashboard/Menu -I../../Source/UI/Popups -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/MiniMaps/AnnotationsMap -I../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap -I../../Source/UI/Sequencer/MiniMaps/LevelsMap -I../../Source/UI/Sequencer/MiniMaps/PianoMap -I../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PatternRoll/ClipComponents -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/Sidebars -I../../Source/UI/Themes $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_RTAS=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0"
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl) -fvisibility=hidden -lrt -ldl -lpthread -lGL $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

ifeq ($(CONFIG),Tests)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Tests
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := 
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DDEBUG=1" "-D_DEBUG=1" "-DJUCE_UNIT_TESTS=1" "-DJUCER_LINUX_MAKE_B650AE49=1" "-DJUCE_APP_VERSION=3.2" "-DJUCE_APP_VERSION_HEX=0x30200" $(shell pkg-config --cflags alsa freetype2 libcurl) -pthread -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../../ThirdParty/ASIO/common -I../../Source/ -I../../Source/Core -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Configuration -I../../Source/Core/Configuration/Models -I../../Source/Core/Configuration/ResourceManagers -I../../Source/Core/CommandPalette -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Network/Models -I../../Source/Core/Network/Requests -I../../Source/Core/Network/Services -I../../Source/Core/Scripting -I../../Source/Core/Serialization -I../../Source/Core/Tree -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/Workspace -I../../Source/UI/ -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Menus/SelectionMenus -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Project -I../../Source/UI/Pages/Settings -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Dashboard -I../../Source/UI/Pages/Dashboard/Menu -I../../Source/UI/Popups -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/MiniMaps/AnnotationsMap -I../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap -I../../Source/UI/Sequencer/MiniMaps/LevelsMap -I../../Source/UI/Sequencer/MiniMaps/PianoMap -I../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PatternRoll/ClipComponents -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/Sidebars -I../../Source/UI/Themes $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_RTAS=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0"
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0 -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl) -fvisibility=hidden -lrt -ldl -lpthread -lGL $(LDFLAGS)

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

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCER_LINUX_MAKE_B650AE49=1" "-DJUCE_APP_VERSION=3.2" "-DJUCE_APP_VERSION_HEX=0x30200" $(shell pkg-config --cflags alsa freetype2 libcurl) -pthread -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../../ThirdParty/ASIO/common -I../../Source/ -I../../Source/Core -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Configuration -I../../Source/Core/Configuration/Models -I../../Source/Core/Configuration/ResourceManagers -I../../Source/Core/CommandPalette -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Network/Models -I../../Source/Core/Network/Requests -I../../Source/Core/Network/Services -I../../Source/Core/Scripting -I../../Source/Core/Serialization -I../../Source/Core/Tree -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/Workspace -I../../Source/UI/ -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Menus/SelectionMenus -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Project -I../../Source/UI/Pages/Settings -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Dashboard -I../../Source/UI/Pages/Dashboard/Menu -I../../Source/UI/Popups -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/MiniMaps/AnnotationsMap -I../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap -I../../Source/UI/Sequencer/MiniMaps/LevelsMap -I../../Source/UI/Sequencer/MiniMaps/PianoMap -I../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PatternRoll/ClipComponents -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/Sidebars -I../../Source/UI/Themes $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_RTAS=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0"
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -Os -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl) -fvisibility=hidden -lrt -ldl -lpthread -lGL $(LDFLAGS)

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

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCER_LINUX_MAKE_B650AE49=1" "-DJUCE_APP_VERSION=3.2" "-DJUCE_APP_VERSION_HEX=0x30200" $(shell pkg-config --cflags alsa freetype2 libcurl) -pthread -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../Projucer/JuceLibraryCode -I../../ThirdParty/JUCE/modules -I../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK -I../../ThirdParty/ASIO/common -I../../Source/ -I../../Source/Core -I../../Source/Core/Audio -I../../Source/Core/Audio/BuiltIn -I../../Source/Core/Audio/Instruments -I../../Source/Core/Audio/Monitoring -I../../Source/Core/Audio/Transport -I../../Source/Core/Configuration -I../../Source/Core/Configuration/Models -I../../Source/Core/Configuration/ResourceManagers -I../../Source/Core/CommandPalette -I../../Source/Core/Midi -I../../Source/Core/Midi/Patterns -I../../Source/Core/Midi/Sequences -I../../Source/Core/Midi/Sequences/Events -I../../Source/Core/Network -I../../Source/Core/Network/Models -I../../Source/Core/Network/Requests -I../../Source/Core/Network/Services -I../../Source/Core/Scripting -I../../Source/Core/Serialization -I../../Source/Core/Tree -I../../Source/Core/Undo -I../../Source/Core/Undo/Actions -I../../Source/Core/VCS -I../../Source/Core/VCS/DiffLogic -I../../Source/Core/Workspace -I../../Source/UI/ -I../../Source/UI/Common -I../../Source/UI/Common/AudioMonitors -I../../Source/UI/Common/Origami -I../../Source/UI/Dialogs -I../../Source/UI/Headline -I../../Source/UI/Input -I../../Source/UI/Menus -I../../Source/UI/Menus/Base -I../../Source/UI/Menus/SelectionMenus -I../../Source/UI/Pages/Instruments -I../../Source/UI/Pages/Instruments/Editor -I../../Source/UI/Pages/Project -I../../Source/UI/Pages/Settings -I../../Source/UI/Pages/VCS -I../../Source/UI/Pages/Dashboard -I../../Source/UI/Pages/Dashboard/Menu -I../../Source/UI/Popups -I../../Source/UI/Sequencer -I../../Source/UI/Sequencer/Header -I../../Source/UI/Sequencer/Helpers -I../../Source/UI/Sequencer/MiniMaps/AnnotationsMap -I../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap -I../../Source/UI/Sequencer/MiniMaps/LevelsMap -I../../Source/UI/Sequencer/MiniMaps/PianoMap -I../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap -I../../Source/UI/Sequencer/PatternRoll -I../../Source/UI/Sequencer/PatternRoll/ClipComponents -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationCurveClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/AutomationStepsClip -I../../Source/UI/Sequencer/PatternRoll/ClipComponents/PianoClip -I../../Source/UI/Sequencer/PianoRoll -I../../Source/UI/Sequencer/Sidebars -I../../Source/UI/Themes $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_RTAS=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0"
  JUCE_TARGET_APP := Helio

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -Os -fpermissive -Wno-unknown-pragmas -Wno-reorder -Wno-dynamic-class-memaccess $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++11 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa freetype2 libcurl) -fvisibility=hidden -lrt -ldl -lpthread -lGL $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/BinaryData_7821001c.o \
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
  $(JUCE_OBJDIR)/UnityBuild_19c451ca.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : $(OBJECTS_APP) $(RESOURCES)
	@command -v pkg-config >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@pkg-config --print-errors alsa freetype2 libcurl
	@echo Linking "Helio - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/BinaryData_7821001c.o: ../Projucer/JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData.cpp"
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

$(JUCE_OBJDIR)/UnityBuild_19c451ca.o: ../Projucer/JuceLibraryCode/UnityBuild.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UnityBuild.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

clean:
	@echo Cleaning Helio
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping Helio
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

-include $(OBJECTS_APP:%.o=%.d)
