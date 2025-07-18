# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

cmake_minimum_required(VERSION 3.19)

project(Helio)

set(BINARY_NAME "juce_jni")

add_library("cpufeatures" STATIC "${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c")
set_source_files_properties("${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c" PROPERTIES COMPILE_FLAGS "-Wno-sign-conversion -Wno-gnu-statement-expression")

add_definitions("-DJUCE_ANDROID=1" "-DJUCE_ANDROID_API_VERSION=21" "-DJUCE_ANDROID_GL_ES_VERSION_3_0=1" "-DJUCER_ANDROIDSTUDIO_6AB8437A=1" "-DJUCE_APP_VERSION=3.16" "-DJUCE_APP_VERSION_HEX=0x31000")

include_directories( AFTER
    "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
    "../../Projucer/JuceLibraryCode"
    "../../../ThirdParty/JUCE/modules"
    "${ANDROID_NDK}/sources/android/cpufeatures"
)

enable_language(ASM)

if(JUCE_BUILD_CONFIGURATION MATCHES "DEBUG")
    add_definitions("-DJUCER_ANDROIDSTUDIO_6AB8437A=1" "-DJUCE_APP_VERSION=3.16" "-DJUCE_APP_VERSION_HEX=0x31000" "-DDEBUG=1" "-D_DEBUG=1")
    include_directories( AFTER
        "../../Projucer/JuceLibraryCode"
        "../../../ThirdParty/JUCE/modules"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/serd"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/sord"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/sord/src"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/sratom"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/lilv"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/lilv/src"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/lv2"
        "../../../ThirdParty/ASIO/common"
        "../../../Source"
        "../../../Source/Core"
        "../../../Source/Core/Audio"
        "../../../Source/Core/Audio/BuiltIn"
        "../../../Source/Core/Audio/BuiltIn/SoundFont"
        "../../../Source/Core/Audio/Instruments"
        "../../../Source/Core/Audio/Transport"
        "../../../Source/Core/Configuration"
        "../../../Source/Core/Configuration/Resources"
        "../../../Source/Core/Configuration/Resources/Models"
        "../../../Source/Core/CommandPalette"
        "../../../Source/Core/Midi"
        "../../../Source/Core/Midi/Patterns"
        "../../../Source/Core/Midi/Patterns/Modifiers"
        "../../../Source/Core/Midi/Sequences"
        "../../../Source/Core/Midi/Sequences/Events"
        "../../../Source/Core/Serialization"
        "../../../Source/Core/Tree"
        "../../../Source/Core/Undo"
        "../../../Source/Core/Undo/Actions"
        "../../../Source/Core/VCS"
        "../../../Source/Core/VCS/DiffLogic"
        "../../../Source/Core/Workspace"
        "../../../Source/UI"
        "../../../Source/UI/Common"
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
        "../../../Source/UI/Popups"
        "../../../Source/UI/Sequencer"
        "../../../Source/UI/Sequencer/EditorPanels"
        "../../../Source/UI/Sequencer/EditorPanels/AutomationEditor"
        "../../../Source/UI/Sequencer/EditorPanels/VelocityEditor"
        "../../../Source/UI/Sequencer/Header"
        "../../../Source/UI/Sequencer/Helpers"
        "../../../Source/UI/Sequencer/MiniMaps"
        "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap"
        "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap"
        "../../../Source/UI/Sequencer/MiniMaps/PianoMap"
        "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap"
        "../../../Source/UI/Sequencer/PatternRoll"
        "../../../Source/UI/Sequencer/PianoRoll"
        "../../../Source/UI/Sequencer/Sidebars"
        "../../../Source/UI/Themes"
    )

elseif(JUCE_BUILD_CONFIGURATION MATCHES "RELEASE")
    add_definitions("-DJUCER_ANDROIDSTUDIO_6AB8437A=1" "-DJUCE_APP_VERSION=3.16" "-DJUCE_APP_VERSION_HEX=0x31000" "-DNDEBUG=1")
    include_directories( AFTER
        "../../Projucer/JuceLibraryCode"
        "../../../ThirdParty/JUCE/modules"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/serd"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/sord"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/sord/src"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/sratom"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/lilv"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/lilv/src"
        "../../../ThirdParty/JUCE/modules/juce_audio_processors/format_types/LV2_SDK/lv2"
        "../../../ThirdParty/ASIO/common"
        "../../../Source"
        "../../../Source/Core"
        "../../../Source/Core/Audio"
        "../../../Source/Core/Audio/BuiltIn"
        "../../../Source/Core/Audio/BuiltIn/SoundFont"
        "../../../Source/Core/Audio/Instruments"
        "../../../Source/Core/Audio/Transport"
        "../../../Source/Core/Configuration"
        "../../../Source/Core/Configuration/Resources"
        "../../../Source/Core/Configuration/Resources/Models"
        "../../../Source/Core/CommandPalette"
        "../../../Source/Core/Midi"
        "../../../Source/Core/Midi/Patterns"
        "../../../Source/Core/Midi/Patterns/Modifiers"
        "../../../Source/Core/Midi/Sequences"
        "../../../Source/Core/Midi/Sequences/Events"
        "../../../Source/Core/Serialization"
        "../../../Source/Core/Tree"
        "../../../Source/Core/Undo"
        "../../../Source/Core/Undo/Actions"
        "../../../Source/Core/VCS"
        "../../../Source/Core/VCS/DiffLogic"
        "../../../Source/Core/Workspace"
        "../../../Source/UI"
        "../../../Source/UI/Common"
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
        "../../../Source/UI/Popups"
        "../../../Source/UI/Sequencer"
        "../../../Source/UI/Sequencer/EditorPanels"
        "../../../Source/UI/Sequencer/EditorPanels/AutomationEditor"
        "../../../Source/UI/Sequencer/EditorPanels/VelocityEditor"
        "../../../Source/UI/Sequencer/Header"
        "../../../Source/UI/Sequencer/Helpers"
        "../../../Source/UI/Sequencer/MiniMaps"
        "../../../Source/UI/Sequencer/MiniMaps/AnnotationsMap"
        "../../../Source/UI/Sequencer/MiniMaps/KeySignaturesMap"
        "../../../Source/UI/Sequencer/MiniMaps/PianoMap"
        "../../../Source/UI/Sequencer/MiniMaps/TimeSignaturesMap"
        "../../../Source/UI/Sequencer/PatternRoll"
        "../../../Source/UI/Sequencer/PianoRoll"
        "../../../Source/UI/Sequencer/Sidebars"
        "../../../Source/UI/Themes"
    )

    if(NOT (ANDROID_ABI STREQUAL "mips" OR ANDROID_ABI STREQUAL "mips64"))
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
    endif()
else()
    message( FATAL_ERROR "No matching build-configuration found." )
endif()

add_library( ${BINARY_NAME}

    SHARED

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
    "../../Projucer/JuceLibraryCode/UnityBuild.cpp"
)

set_source_files_properties("../../Projucer/JuceLibraryCode/AppConfig.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/BinaryData.h" PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties("../../Projucer/JuceLibraryCode/JuceHeader.h" PROPERTIES HEADER_FILE_ONLY TRUE)

target_compile_options( ${BINARY_NAME} PRIVATE "-fsigned-char" "-Wno-unused-command-line-argument" "-Wno-unqualified-std-cast-call" )

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
