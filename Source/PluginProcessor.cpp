#include "PluginProcessor.h"
#include "PluginEditor.h"

StereoImagerAudioProcessor::StereoImagerAudioProcessor()
     : AudioProcessor(BusesProperties()
                      .withInput("Input", juce::AudioChannelSet::stereo(), true)
                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Cache parameter pointers
    widthParam = apvts.getRawParameterValue("width");
    panParam = apvts.getRawParameterValue("pan");
    balanceParam = apvts.getRawParameterValue("balance");
    monoBassFreqParam = apvts.getRawParameterValue("monoBassFreq");
    monoBassEnabledParam = apvts.getRawParameterValue("monoBassEnabled");
    inputGainParam = apvts.getRawParameterValue("inputGain");
    outputGainParam = apvts.getRawParameterValue("outputGain");
    bypassParam = apvts.getRawParameterValue("bypass");

    // Multiband parameters
    multibandEnabledParam = apvts.getRawParameterValue("multibandEnabled");
    lowMidXoverParam = apvts.getRawParameterValue("lowMidXover");
    midHighXoverParam = apvts.getRawParameterValue("midHighXover");
    lowWidthParam = apvts.getRawParameterValue("lowWidth");
    midWidthParam = apvts.getRawParameterValue("midWidth");
    highWidthParam = apvts.getRawParameterValue("highWidth");
}

StereoImagerAudioProcessor::~StereoImagerAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout StereoImagerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Main stereo controls
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("width", 1),
        "Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 0.1f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("pan", 1),
        "Pan",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("balance", 1),
        "Balance",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Mono bass
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("monoBassFreq", 1),
        "Mono Bass Freq",
        juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.4f),
        120.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("monoBassEnabled", 1),
        "Mono Bass",
        true));

    // Multiband controls
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("multibandEnabled", 1),
        "Multiband",
        false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lowMidXover", 1),
        "Low-Mid Crossover",
        juce::NormalisableRange<float>(80.0f, 1000.0f, 1.0f, 0.4f),
        250.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("midHighXover", 1),
        "Mid-High Crossover",
        juce::NormalisableRange<float>(1000.0f, 10000.0f, 1.0f, 0.4f),
        4000.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("lowWidth", 1),
        "Low Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 0.1f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("midWidth", 1),
        "Mid Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 0.1f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("highWidth", 1),
        "High Width",
        juce::NormalisableRange<float>(0.0f, 200.0f, 0.1f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Input/Output gain
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("inputGain", 1),
        "Input Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("outputGain", 1),
        "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Bypass
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("bypass", 1),
        "Bypass",
        false));

    return { params.begin(), params.end() };
}

const juce::String StereoImagerAudioProcessor::getName() const { return JucePlugin_Name; }
bool StereoImagerAudioProcessor::acceptsMidi() const { return false; }
bool StereoImagerAudioProcessor::producesMidi() const { return false; }
bool StereoImagerAudioProcessor::isMidiEffect() const { return false; }
double StereoImagerAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int StereoImagerAudioProcessor::getNumPrograms() { return 1; }
int StereoImagerAudioProcessor::getCurrentProgram() { return 0; }
void StereoImagerAudioProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }
const juce::String StereoImagerAudioProcessor::getProgramName(int index) { juce::ignoreUnused(index); return {}; }
void StereoImagerAudioProcessor::changeProgramName(int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

void StereoImagerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    stereoProcessor.prepare(sampleRate, samplesPerBlock);
    multibandProcessor.prepare(sampleRate, samplesPerBlock);
}

void StereoImagerAudioProcessor::releaseResources()
{
    stereoProcessor.reset();
    multibandProcessor.reset();
}

bool StereoImagerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Only support stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

void StereoImagerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Check bypass
    if (bypassParam->load() > 0.5f)
        return;

    // Apply input gain
    float inputGain = juce::Decibels::decibelsToGain(inputGainParam->load());
    buffer.applyGain(inputGain);

    // Measure input levels
    if (totalNumInputChannels >= 2)
    {
        inputLevelL.store(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
        inputLevelR.store(buffer.getMagnitude(1, 0, buffer.getNumSamples()));
    }

    // Update stereo processor parameters
    stereoProcessor.setWidth(widthParam->load());
    stereoProcessor.setPan(panParam->load() / 100.0f);  // Convert from -100/+100 to -1/+1
    stereoProcessor.setBalance(balanceParam->load() / 100.0f);
    stereoProcessor.setMonoBassFreq(monoBassFreqParam->load());
    stereoProcessor.setMonoBassEnabled(monoBassEnabledParam->load() > 0.5f);

    // Update multiband processor parameters
    bool multibandEnabled = multibandEnabledParam->load() > 0.5f;
    multibandProcessor.setEnabled(multibandEnabled);
    multibandProcessor.setLowMidCrossover(lowMidXoverParam->load());
    multibandProcessor.setMidHighCrossover(midHighXoverParam->load());
    multibandProcessor.setLowWidth(lowWidthParam->load());
    multibandProcessor.setMidWidth(midWidthParam->load());
    multibandProcessor.setHighWidth(highWidthParam->load());

    // Process through DSP chain
    if (multibandEnabled)
    {
        // Multiband mode - disable main width control's M/S processing
        // (just use mono bass and pan/balance from stereo processor)
        stereoProcessor.setWidth(100.0f); // Neutral
        stereoProcessor.process(buffer);
        multibandProcessor.process(buffer);
    }
    else
    {
        // Single-band mode
        stereoProcessor.process(buffer);
    }

    // Apply output gain
    float outputGain = juce::Decibels::decibelsToGain(outputGainParam->load());
    buffer.applyGain(outputGain);

    // Measure output levels
    if (totalNumInputChannels >= 2)
    {
        outputLevelL.store(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
        outputLevelR.store(buffer.getMagnitude(1, 0, buffer.getNumSamples()));
    }
}

bool StereoImagerAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* StereoImagerAudioProcessor::createEditor()
{
    return new StereoImagerAudioProcessorEditor(*this);
}

void StereoImagerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void StereoImagerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StereoImagerAudioProcessor();
}
