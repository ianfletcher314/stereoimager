cmake_minimum_required(VERSION 3.22)

project(StereoImager VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JUCE - adjust the path as needed for your system
# Option 1: If JUCE is installed via CPM or as a subdirectory
# add_subdirectory(JUCE)

# Option 2: If JUCE is installed system-wide or in a known location
# find_package(JUCE CONFIG REQUIRED)

# Option 3: Fetch JUCE from GitHub (recommended for standalone projects)
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# Add the plugin target
juce_add_plugin(StereoImager
    COMPANY_NAME "Ian Fletcher"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE IFlt
    PLUGIN_CODE StIg
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Stereo Imager"
    BUNDLE_ID "com.ianfletcher.stereoimager"
)

# Add source files
target_sources(StereoImager
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/StereoProcessor.cpp
        Source/DSP/MultibandProcessor.cpp
)

# Add include directories
target_include_directories(StereoImager
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode
)

# Compile definitions
target_compile_definitions(StereoImager
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

# Link JUCE modules
target_link_libraries(StereoImager
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
