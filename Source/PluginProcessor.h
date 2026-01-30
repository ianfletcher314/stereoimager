#pragma once

#include <JuceHeader.h>
#include "DSP/StereoProcessor.h"
#include "DSP/MultibandProcessor.h"

class StereoImagerAudioProcessor : public juce::AudioProcessor
{
public:
    StereoImagerAudioProcessor();
    ~StereoImagerAudioProcessor() override;

    // Standard AudioProcessor overrides
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Public API for editor access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Metering accessors
    float getInputLevelL() const { return inputLevelL.load(); }
    float getInputLevelR() const { return inputLevelR.load(); }
    float getOutputLevelL() const { return outputLevelL.load(); }
    float getOutputLevelR() const { return outputLevelR.load(); }
    float getCorrelation() const { return stereoProcessor.getCorrelation(); }
    float getMidLevel() const { return stereoProcessor.getMidLevel(); }
    float getSideLevel() const { return stereoProcessor.getSideLevel(); }

    // Band levels from multiband processor
    float getLowBandLevel() const { return multibandProcessor.getLowLevel(); }
    float getMidBandLevel() const { return multibandProcessor.getMidLevel(); }
    float getHighBandLevel() const { return multibandProcessor.getHighLevel(); }

    // For vectorscope
    void getStereoSamples(std::vector<std::pair<float, float>>& samples) const
    {
        stereoProcessor.getStereoSamples(samples);
    }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP Modules
    StereoProcessor stereoProcessor;
    MultibandProcessor multibandProcessor;

    // Parameter pointers (cached for fast access)
    std::atomic<float>* widthParam = nullptr;
    std::atomic<float>* panParam = nullptr;
    std::atomic<float>* balanceParam = nullptr;
    std::atomic<float>* monoBassFreqParam = nullptr;
    std::atomic<float>* monoBassEnabledParam = nullptr;
    std::atomic<float>* inputGainParam = nullptr;
    std::atomic<float>* outputGainParam = nullptr;
    std::atomic<float>* bypassParam = nullptr;

    // Multiband parameters
    std::atomic<float>* multibandEnabledParam = nullptr;
    std::atomic<float>* lowMidXoverParam = nullptr;
    std::atomic<float>* midHighXoverParam = nullptr;
    std::atomic<float>* lowWidthParam = nullptr;
    std::atomic<float>* midWidthParam = nullptr;
    std::atomic<float>* highWidthParam = nullptr;

    // Metering state
    std::atomic<float> inputLevelL { 0.0f };
    std::atomic<float> inputLevelR { 0.0f };
    std::atomic<float> outputLevelL { 0.0f };
    std::atomic<float> outputLevelR { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoImagerAudioProcessor)
};
