#pragma once

#include <JuceHeader.h>
#include "DSPUtils.h"

class MultibandProcessor
{
public:
    MultibandProcessor();

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Setters for parameters
    void setLowMidCrossover(float freqHz);       // 80-1000 Hz
    void setMidHighCrossover(float freqHz);      // 1000-10000 Hz
    void setLowWidth(float widthPercent);        // 0-200%
    void setMidWidth(float widthPercent);        // 0-200%
    void setHighWidth(float widthPercent);       // 0-200%
    void setEnabled(bool shouldEnable);
    void setBypass(bool shouldBypass);

    // Getters for band levels
    float getLowLevel() const { return lowLevel.load(); }
    float getMidLevel() const { return midLevel.load(); }
    float getHighLevel() const { return highLevel.load(); }

private:
    void updateCrossoverCoefficients();

    // Crossover filters (Linkwitz-Riley 4th order = 2 cascaded 2nd order Butterworth)
    // Low-Mid crossover
    DSPUtils::BiquadCoeffs lowMidLP;
    DSPUtils::BiquadCoeffs lowMidHP;
    DSPUtils::BiquadState lowMidLPStateL1, lowMidLPStateL2;
    DSPUtils::BiquadState lowMidLPStateR1, lowMidLPStateR2;
    DSPUtils::BiquadState lowMidHPStateL1, lowMidHPStateL2;
    DSPUtils::BiquadState lowMidHPStateR1, lowMidHPStateR2;

    // Mid-High crossover
    DSPUtils::BiquadCoeffs midHighLP;
    DSPUtils::BiquadCoeffs midHighHP;
    DSPUtils::BiquadState midHighLPStateL1, midHighLPStateL2;
    DSPUtils::BiquadState midHighLPStateR1, midHighLPStateR2;
    DSPUtils::BiquadState midHighHPStateL1, midHighHPStateL2;
    DSPUtils::BiquadState midHighHPStateR1, midHighHPStateR2;

    // Crossover frequencies
    float lowMidFreq = 250.0f;
    float midHighFreq = 4000.0f;

    // Width parameters (smoothed)
    DSPUtils::SmoothedValue lowWidthSmoothed;
    DSPUtils::SmoothedValue midWidthSmoothed;
    DSPUtils::SmoothedValue highWidthSmoothed;

    // Band level metering
    std::atomic<float> lowLevel { 0.0f };
    std::atomic<float> midLevel { 0.0f };
    std::atomic<float> highLevel { 0.0f };

    // Runtime info
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    bool enabled = true;
    bool bypassed = false;
};
