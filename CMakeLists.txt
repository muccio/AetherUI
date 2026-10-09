cmake_minimum_required(VERSION 3.22)

project(AetherUI VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Locate JUCE
if(DEFINED JUCE_PATH)
    message(STATUS "Using JUCE from JUCE_PATH: ${JUCE_PATH}")
    add_subdirectory(${JUCE_PATH} ./JUCE_build)
elseif(EXISTS "/Users/mariosalvucci/Documents/Development/JUCE")
    message(STATUS "Using local JUCE from /Users/mariosalvucci/Documents/Development/JUCE")
    add_subdirectory(/Users/mariosalvucci/Documents/Development/JUCE ./JUCE_build)
else()
    message(STATUS "Fetching JUCE via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 7.0.12
    )
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(AetherUI
    COMPANY_NAME "AetherAudio"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Aeth
    PLUGIN_CODE AeUI
    FORMATS VST3 Standalone
    PRODUCT_NAME "AetherUI"
)

target_sources(AetherUI
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/SynthEngine.cpp
        Source/SynthEngine.h
        Source/Presets.h
)

target_compile_features(AetherUI PRIVATE cxx_std_20)

target_compile_definitions(AetherUI
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(AetherUI
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Automated Verification Test Runner
add_executable(AetherUIVerificationTest
    Source/Tests/VerificationTest.cpp
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/SynthEngine.cpp
    Source/SynthEngine.h
    Source/Presets.h
)

target_compile_features(AetherUIVerificationTest PRIVATE cxx_std_20)

target_include_directories(AetherUIVerificationTest PRIVATE Source)

target_compile_definitions(AetherUIVerificationTest
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JucePlugin_Name="AetherUI"
)

target_link_libraries(AetherUIVerificationTest
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
