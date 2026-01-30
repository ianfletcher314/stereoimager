#include "PluginProcessor.h"
#include "PluginEditor.h"

StereoImagerAudioProcessorEditor::StereoImagerAudioProcessorEditor(StereoImagerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Create and set LookAndFeel
    lookAndFeel = std::make_unique<StereoImagerLookAndFeel>();
    setLookAndFeel(lookAndFeel.get());

    // Title
    setupLabel(titleLabel, "STEREO IMAGER", 24.0f, juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, Colors::accent);

    // Main controls
    setupSlider(widthSlider, widthLabel, "WIDTH");
    widthSlider.setTextValueSuffix(" %");

    setupSlider(panSlider, panLabel, "PAN");
    panSlider.setTextValueSuffix(" %");

    setupSlider(balanceSlider, balanceLabel, "BALANCE");
    balanceSlider.setTextValueSuffix(" %");

    setupSlider(monoBassFreqSlider, monoBassLabel, "MONO BASS");
    monoBassFreqSlider.setTextValueSuffix(" Hz");

    monoBassButton.setButtonText("Mono Bass");
    addAndMakeVisible(monoBassButton);

    setupSlider(inputGainSlider, inputGainLabel, "INPUT");
    inputGainSlider.setTextValueSuffix(" dB");

    setupSlider(outputGainSlider, outputGainLabel, "OUTPUT");
    outputGainSlider.setTextValueSuffix(" dB");

    bypassButton.setButtonText("Bypass");
    addAndMakeVisible(bypassButton);

    // Multiband controls
    multibandButton.setButtonText("Multiband");
    addAndMakeVisible(multibandButton);

    setupSlider(lowMidXoverSlider, lowMidXoverLabel, "LOW-MID");
    lowMidXoverSlider.setTextValueSuffix(" Hz");

    setupSlider(midHighXoverSlider, midHighXoverLabel, "MID-HIGH");
    midHighXoverSlider.setTextValueSuffix(" Hz");

    setupSlider(lowWidthSlider, lowWidthLabel, "LOW");
    lowWidthSlider.setTextValueSuffix(" %");

    setupSlider(midWidthSlider, midWidthLabel, "MID");
    midWidthSlider.setTextValueSuffix(" %");

    setupSlider(highWidthSlider, highWidthLabel, "HIGH");
    highWidthSlider.setTextValueSuffix(" %");

    // Meters
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    addAndMakeVisible(correlationMeter);
    addAndMakeVisible(vectorscope);
    addAndMakeVisible(bandMeter);
    addAndMakeVisible(midMeter);
    addAndMakeVisible(sideMeter);

    // Meter labels
    setupLabel(inputMeterLabel, "IN", 10.0f);
    setupLabel(outputMeterLabel, "OUT", 10.0f);
    setupLabel(correlationLabel, "CORRELATION", 10.0f);
    setupLabel(vectorscopeLabel, "VECTORSCOPE", 10.0f);
    setupLabel(midMeterLabel, "M", 10.0f);
    setupLabel(sideMeterLabel, "S", 10.0f);

    // Create attachments
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "width", widthSlider);
    panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "pan", panSlider);
    balanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "balance", balanceSlider);
    monoBassFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "monoBassFreq", monoBassFreqSlider);
    monoBassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getAPVTS(), "monoBassEnabled", monoBassButton);
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "inputGain", inputGainSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "outputGain", outputGainSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getAPVTS(), "bypass", bypassButton);

    multibandAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getAPVTS(), "multibandEnabled", multibandButton);
    lowMidXoverAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "lowMidXover", lowMidXoverSlider);
    midHighXoverAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "midHighXover", midHighXoverSlider);
    lowWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "lowWidth", lowWidthSlider);
    midWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "midWidth", midWidthSlider);
    highWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "highWidth", highWidthSlider);

    // Start timer for metering updates
    startTimerHz(30);

    setSize(800, 550);
}

StereoImagerAudioProcessorEditor::~StereoImagerAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void StereoImagerAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                                    const juce::String& labelText,
                                                    juce::Slider::SliderStyle style)
{
    slider.setSliderStyle(style);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    addAndMakeVisible(slider);

    setupLabel(label, labelText);
}

void StereoImagerAudioProcessorEditor::setupLabel(juce::Label& label, const juce::String& text,
                                                   float fontSize, juce::Justification justification)
{
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(fontSize));
    label.setJustificationType(justification);
    label.setColour(juce::Label::textColourId, Colors::textSecondary);
    addAndMakeVisible(label);
}

void StereoImagerAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(Colors::background);

    // Header area
    g.setColour(Colors::panelBg);
    g.fillRect(0, 0, getWidth(), 50);

    // Main section panel
    g.setColour(Colors::panelBg);
    g.fillRoundedRectangle(10.0f, 60.0f, 380.0f, 220.0f, 8.0f);

    // Multiband section panel
    g.setColour(Colors::panelBg);
    g.fillRoundedRectangle(400.0f, 60.0f, 390.0f, 220.0f, 8.0f);

    // Meters section panel
    g.setColour(Colors::panelBg);
    g.fillRoundedRectangle(10.0f, 290.0f, 780.0f, 250.0f, 8.0f);

    // Section labels
    g.setColour(Colors::textSecondary);
    g.setFont(11.0f);
    g.drawText("STEREO", 20, 65, 100, 16, juce::Justification::centredLeft);
    g.drawText("MULTIBAND", 410, 65, 100, 16, juce::Justification::centredLeft);
    g.drawText("ANALYSIS", 20, 295, 100, 16, juce::Justification::centredLeft);
}

void StereoImagerAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Header
    auto header = bounds.removeFromTop(50);
    titleLabel.setBounds(header.reduced(15, 10));
    bypassButton.setBounds(header.removeFromRight(100).reduced(10, 12));

    // Main content
    auto mainArea = bounds.reduced(10, 0);

    // Main stereo controls (left panel)
    auto stereoPanel = mainArea.removeFromTop(220).removeFromLeft(380).reduced(10, 20);

    // Row 1: Width, Pan, Balance
    auto row1 = stereoPanel.removeFromTop(90);
    int knobWidth = 80;
    int labelHeight = 16;

    auto widthArea = row1.removeFromLeft(knobWidth + 20);
    widthLabel.setBounds(widthArea.removeFromTop(labelHeight));
    widthSlider.setBounds(widthArea);

    row1.removeFromLeft(20);
    auto panArea = row1.removeFromLeft(knobWidth + 20);
    panLabel.setBounds(panArea.removeFromTop(labelHeight));
    panSlider.setBounds(panArea);

    row1.removeFromLeft(20);
    auto balanceArea = row1.removeFromLeft(knobWidth + 20);
    balanceLabel.setBounds(balanceArea.removeFromTop(labelHeight));
    balanceSlider.setBounds(balanceArea);

    stereoPanel.removeFromTop(10);

    // Row 2: Mono Bass, Input, Output
    auto row2 = stereoPanel.removeFromTop(90);

    auto monoBassArea = row2.removeFromLeft(knobWidth + 20);
    monoBassLabel.setBounds(monoBassArea.removeFromTop(labelHeight));
    monoBassFreqSlider.setBounds(monoBassArea.removeFromTop(60));
    monoBassButton.setBounds(monoBassArea.removeFromTop(24).reduced(0, 2));

    row2.removeFromLeft(20);
    auto inputArea = row2.removeFromLeft(knobWidth + 20);
    inputGainLabel.setBounds(inputArea.removeFromTop(labelHeight));
    inputGainSlider.setBounds(inputArea);

    row2.removeFromLeft(20);
    auto outputArea = row2.removeFromLeft(knobWidth + 20);
    outputGainLabel.setBounds(outputArea.removeFromTop(labelHeight));
    outputGainSlider.setBounds(outputArea);

    // Multiband panel (right of stereo panel)
    mainArea = getLocalBounds().reduced(10, 0);
    mainArea.removeFromTop(50);
    auto multibandPanel = mainArea.removeFromTop(220).removeFromRight(390).reduced(10, 20);

    // Multiband enable button
    multibandButton.setBounds(multibandPanel.removeFromTop(24).removeFromLeft(120));
    multibandPanel.removeFromTop(10);

    // Crossover controls
    auto xoverRow = multibandPanel.removeFromTop(70);
    auto lowMidArea = xoverRow.removeFromLeft(90);
    lowMidXoverLabel.setBounds(lowMidArea.removeFromTop(labelHeight));
    lowMidXoverSlider.setBounds(lowMidArea);

    xoverRow.removeFromLeft(20);
    auto midHighArea = xoverRow.removeFromLeft(90);
    midHighXoverLabel.setBounds(midHighArea.removeFromTop(labelHeight));
    midHighXoverSlider.setBounds(midHighArea);

    multibandPanel.removeFromTop(10);

    // Band width controls
    auto bandRow = multibandPanel.removeFromTop(70);
    auto lowArea = bandRow.removeFromLeft(80);
    lowWidthLabel.setBounds(lowArea.removeFromTop(labelHeight));
    lowWidthSlider.setBounds(lowArea);

    bandRow.removeFromLeft(15);
    auto midArea = bandRow.removeFromLeft(80);
    midWidthLabel.setBounds(midArea.removeFromTop(labelHeight));
    midWidthSlider.setBounds(midArea);

    bandRow.removeFromLeft(15);
    auto highArea = bandRow.removeFromLeft(80);
    highWidthLabel.setBounds(highArea.removeFromTop(labelHeight));
    highWidthSlider.setBounds(highArea);

    // Analysis/Meters section
    auto metersPanel = getLocalBounds().reduced(10);
    metersPanel.removeFromTop(290);
    metersPanel = metersPanel.reduced(10, 10);

    // Left: Input/Output meters
    auto leftMeters = metersPanel.removeFromLeft(80);
    leftMeters.removeFromTop(20);
    inputMeterLabel.setBounds(leftMeters.removeFromTop(14));
    inputMeter.setBounds(leftMeters.removeFromTop(80));
    leftMeters.removeFromTop(10);
    outputMeterLabel.setBounds(leftMeters.removeFromTop(14));
    outputMeter.setBounds(leftMeters.removeFromTop(80));

    metersPanel.removeFromLeft(20);

    // M/S meters
    auto msMeters = metersPanel.removeFromLeft(50);
    msMeters.removeFromTop(20);
    midMeterLabel.setBounds(msMeters.removeFromTop(14));
    midMeter.setBounds(msMeters.removeFromTop(70));
    msMeters.removeFromTop(10);
    sideMeterLabel.setBounds(msMeters.removeFromTop(14));
    sideMeter.setBounds(msMeters.removeFromTop(70));

    metersPanel.removeFromLeft(20);

    // Vectorscope (center)
    auto scopeArea = metersPanel.removeFromLeft(200);
    vectorscopeLabel.setBounds(scopeArea.removeFromTop(14).withTrimmedTop(6));
    vectorscope.setBounds(scopeArea.reduced(0, 5));

    metersPanel.removeFromLeft(20);

    // Band meter
    auto bandMeterArea = metersPanel.removeFromLeft(80);
    bandMeterArea.removeFromTop(20);
    bandMeter.setBounds(bandMeterArea.removeFromTop(180));

    metersPanel.removeFromLeft(20);

    // Correlation meter (bottom)
    auto corrArea = metersPanel;
    corrArea.removeFromTop(20);
    correlationLabel.setBounds(corrArea.removeFromTop(14));
    correlationMeter.setBounds(corrArea.removeFromTop(40).withWidth(200));
}

void StereoImagerAudioProcessorEditor::timerCallback()
{
    // Smooth level metering
    float targetInL = audioProcessor.getInputLevelL();
    float targetInR = audioProcessor.getInputLevelR();
    float targetOutL = audioProcessor.getOutputLevelL();
    float targetOutR = audioProcessor.getOutputLevelR();
    float targetCorr = audioProcessor.getCorrelation();

    // Smoothing with faster decay
    smoothedInputL = smoothedInputL * 0.8f + targetInL * 0.2f;
    smoothedInputR = smoothedInputR * 0.8f + targetInR * 0.2f;
    smoothedOutputL = smoothedOutputL * 0.8f + targetOutL * 0.2f;
    smoothedOutputR = smoothedOutputR * 0.8f + targetOutR * 0.2f;
    smoothedCorrelation = smoothedCorrelation * 0.9f + targetCorr * 0.1f;

    // Faster decay when signal drops
    if (targetInL < smoothedInputL) smoothedInputL *= 0.92f;
    if (targetInR < smoothedInputR) smoothedInputR *= 0.92f;
    if (targetOutL < smoothedOutputL) smoothedOutputL *= 0.92f;
    if (targetOutR < smoothedOutputR) smoothedOutputR *= 0.92f;

    // Update meters
    inputMeter.setLevels(smoothedInputL, smoothedInputR);
    outputMeter.setLevels(smoothedOutputL, smoothedOutputR);
    correlationMeter.setCorrelation(smoothedCorrelation);

    // M/S meters
    midMeter.setLevel(audioProcessor.getMidLevel());
    sideMeter.setLevel(audioProcessor.getSideLevel());

    // Band meter
    bandMeter.setLevels(audioProcessor.getLowBandLevel(),
                        audioProcessor.getMidBandLevel(),
                        audioProcessor.getHighBandLevel());

    // Vectorscope
    std::vector<std::pair<float, float>> samples;
    audioProcessor.getStereoSamples(samples);
    vectorscope.setSamples(samples);
}
