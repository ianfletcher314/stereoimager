#pragma once

#include <JuceHeader.h>
#include "DSPUtils.h"

class StereoProcessor
{
public:
    StereoProcessor();

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Setters for parameters
    void setWidth(float widthPercent);           // 0-200%
    void setPan(float panValue);                 // -1 to +1
    void setBalance(float balanceValue);         // -1 to +1
    void setMonoBassFreq(float freqHz);          // Frequency below which is mono
    void setMonoBassEnabled(bool enabled);
    void setBypass(bool shouldBypass);

    // Getters for metering
    float getCorrelation() const { return correlation.load(); }
    float getLeftLevel() const { return leftLevel.load(); }
    float getRightLevel() const { return rightLevel.load(); }
    float getMidLevel() const { return midLevel.load(); }
    float getSideLevel() const { return sideLevel.load(); }

    // For vectorscope/goniometer
    void getStereoSamples(std::vector<std::pair<float, float>>& samples) const;

private:
    // M/S encoding
    void encodeMS(float left, float right, float& mid, float& side);
    void decodeMS(float mid, float side, float& left, float& right);

    // Parameters (smoothed)
    DSPUtils::SmoothedValue widthSmoothed;
    DSPUtils::SmoothedValue panSmoothed;
    DSPUtils::SmoothedValue balanceSmoothed;

    // Mono bass filter
    DSPUtils::BiquadCoeffs monoBassLPCoeffs;
    DSPUtils::BiquadCoeffs monoBassHPCoeffs;
    DSPUtils::BiquadState monoBassLPStateL1, monoBassLPStateL2;
    DSPUtils::BiquadState monoBassLPStateR1, monoBassLPStateR2;
    DSPUtils::BiquadState monoBassHPStateL1, monoBassHPStateL2;
    DSPUtils::BiquadState monoBassHPStateR1, monoBassHPStateR2;
    float monoBassFreq = 120.0f;
    bool monoBassEnabled = true;

    // Metering
    std::atomic<float> correlation { 0.0f };
    std::atomic<float> leftLevel { 0.0f };
    std::atomic<float> rightLevel { 0.0f };
    std::atomic<float> midLevel { 0.0f };
    std::atomic<float> sideLevel { 0.0f };

    // Correlation calculation state
    float corrSum = 0.0f;
    float leftSqSum = 0.0f;
    float rightSqSum = 0.0f;
    int corrSampleCount = 0;
    static constexpr int corrWindowSize = 2048;

    // Vectorscope buffer (circular buffer for display)
    mutable std::mutex vectorscopeMutex;
    std::vector<std::pair<float, float>> vectorscopeBuffer;
    int vectorscopeWriteIndex = 0;
    static constexpr int vectorscopeBufferSize = 512;

    // Runtime info
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    bool bypassed = false;
};
