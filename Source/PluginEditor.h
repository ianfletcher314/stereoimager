#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"
#include "UI/MeterComponents.h"

class StereoImagerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          public juce::Timer
{
public:
    StereoImagerAudioProcessorEditor(StereoImagerAudioProcessor&);
    ~StereoImagerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    StereoImagerAudioProcessor& audioProcessor;

    // Custom LookAndFeel
    std::unique_ptr<StereoImagerLookAndFeel> lookAndFeel;

    // Main controls
    juce::Slider widthSlider;
    juce::Slider panSlider;
    juce::Slider balanceSlider;
    juce::Slider monoBassFreqSlider;
    juce::ToggleButton monoBassButton;
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::ToggleButton bypassButton;

    // Multiband controls
    juce::ToggleButton multibandButton;
    juce::Slider lowMidXoverSlider;
    juce::Slider midHighXoverSlider;
    juce::Slider lowWidthSlider;
    juce::Slider midWidthSlider;
    juce::Slider highWidthSlider;

    // Labels
    juce::Label widthLabel, panLabel, balanceLabel, monoBassLabel;
    juce::Label inputGainLabel, outputGainLabel;
    juce::Label lowMidXoverLabel, midHighXoverLabel;
    juce::Label lowWidthLabel, midWidthLabel, highWidthLabel;
    juce::Label titleLabel;

    // Meters
    StereoMeter inputMeter;
    StereoMeter outputMeter;
    CorrelationMeter correlationMeter;
    Vectorscope vectorscope;
    BandMeter bandMeter;
    LevelMeter midMeter;
    LevelMeter sideMeter;

    // Labels for meters
    juce::Label inputMeterLabel, outputMeterLabel;
    juce::Label correlationLabel, vectorscopeLabel;
    juce::Label midMeterLabel, sideMeterLabel;

    // Smoothed levels for display
    float smoothedInputL = 0.0f;
    float smoothedInputR = 0.0f;
    float smoothedOutputL = 0.0f;
    float smoothedOutputR = 0.0f;
    float smoothedCorrelation = 1.0f;

    // APVTS Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> balanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> monoBassFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> monoBassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> multibandAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowMidXoverAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midHighXoverAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highWidthAttachment;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText,
                     juce::Slider::SliderStyle style = juce::Slider::RotaryHorizontalVerticalDrag);
    void setupLabel(juce::Label& label, const juce::String& text, float fontSize = 12.0f,
                    juce::Justification justification = juce::Justification::centred);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoImagerAudioProcessorEditor)
};
