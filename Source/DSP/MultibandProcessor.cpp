#include "MultibandProcessor.h"

MultibandProcessor::MultibandProcessor()
{
}

void MultibandProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // Initialize smoothed parameters
    lowWidthSmoothed.reset(sampleRate, 20.0f);
    lowWidthSmoothed.setCurrentAndTargetValue(1.0f);

    midWidthSmoothed.reset(sampleRate, 20.0f);
    midWidthSmoothed.setCurrentAndTargetValue(1.0f);

    highWidthSmoothed.reset(sampleRate, 20.0f);
    highWidthSmoothed.setCurrentAndTargetValue(1.0f);

    updateCrossoverCoefficients();
    reset();
}

void MultibandProcessor::reset()
{
    // Reset all filter states
    lowMidLPStateL1.reset();
    lowMidLPStateL2.reset();
    lowMidLPStateR1.reset();
    lowMidLPStateR2.reset();
    lowMidHPStateL1.reset();
    lowMidHPStateL2.reset();
    lowMidHPStateR1.reset();
    lowMidHPStateR2.reset();

    midHighLPStateL1.reset();
    midHighLPStateL2.reset();
    midHighLPStateR1.reset();
    midHighLPStateR2.reset();
    midHighHPStateL1.reset();
    midHighHPStateL2.reset();
    midHighHPStateR1.reset();
    midHighHPStateR2.reset();
}

void MultibandProcessor::updateCrossoverCoefficients()
{
    lowMidLP = DSPUtils::calcLowPassLR(currentSampleRate, lowMidFreq);
    lowMidHP = DSPUtils::calcHighPassLR(currentSampleRate, lowMidFreq);
    midHighLP = DSPUtils::calcLowPassLR(currentSampleRate, midHighFreq);
    midHighHP = DSPUtils::calcHighPassLR(currentSampleRate, midHighFreq);
}

void MultibandProcessor::setLowMidCrossover(float freqHz)
{
    lowMidFreq = std::clamp(freqHz, 80.0f, 1000.0f);
    updateCrossoverCoefficients();
}

void MultibandProcessor::setMidHighCrossover(float freqHz)
{
    midHighFreq = std::clamp(freqHz, 1000.0f, 10000.0f);
    updateCrossoverCoefficients();
}

void MultibandProcessor::setLowWidth(float widthPercent)
{
    lowWidthSmoothed.setTargetValue(widthPercent / 100.0f);
}

void MultibandProcessor::setMidWidth(float widthPercent)
{
    midWidthSmoothed.setTargetValue(widthPercent / 100.0f);
}

void MultibandProcessor::setHighWidth(float widthPercent)
{
    highWidthSmoothed.setTargetValue(widthPercent / 100.0f);
}

void MultibandProcessor::setEnabled(bool shouldEnable)
{
    enabled = shouldEnable;
}

void MultibandProcessor::setBypass(bool shouldBypass)
{
    bypassed = shouldBypass;
}

void MultibandProcessor::process(juce::AudioBuffer<float>& buffer)
{
    if (bypassed || !enabled)
        return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels < 2)
        return;

    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    float lowSum = 0.0f;
    float midSum = 0.0f;
    float highSum = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        float left = leftChannel[i];
        float right = rightChannel[i];

        // Get smoothed width values
        float lowWidth = lowWidthSmoothed.getNextValue();
        float midWidth = midWidthSmoothed.getNextValue();
        float highWidth = highWidthSmoothed.getNextValue();

        // Split into 3 bands using Linkwitz-Riley crossovers
        // First split: low vs (mid+high)
        float lowL = lowMidLPStateL2.process(lowMidLPStateL1.process(left, lowMidLP), lowMidLP);
        float lowR = lowMidLPStateR2.process(lowMidLPStateR1.process(right, lowMidLP), lowMidLP);
        float midHighL = lowMidHPStateL2.process(lowMidHPStateL1.process(left, lowMidHP), lowMidHP);
        float midHighR = lowMidHPStateR2.process(lowMidHPStateR1.process(right, lowMidHP), lowMidHP);

        // Second split: mid vs high (from midHigh signal)
        float midL = midHighLPStateL2.process(midHighLPStateL1.process(midHighL, midHighLP), midHighLP);
        float midR = midHighLPStateR2.process(midHighLPStateR1.process(midHighR, midHighLP), midHighLP);
        float highL = midHighHPStateL2.process(midHighHPStateL1.process(midHighL, midHighHP), midHighHP);
        float highR = midHighHPStateR2.process(midHighHPStateR1.process(midHighR, midHighHP), midHighHP);

        // Apply width to each band using M/S processing
        // Low band
        float lowMid = (lowL + lowR) * 0.5f;
        float lowSide = (lowL - lowR) * 0.5f;
        lowSide *= lowWidth;
        lowL = lowMid + lowSide;
        lowR = lowMid - lowSide;

        // Mid band
        float midMid = (midL + midR) * 0.5f;
        float midSide = (midL - midR) * 0.5f;
        midSide *= midWidth;
        midL = midMid + midSide;
        midR = midMid - midSide;

        // High band
        float highMid = (highL + highR) * 0.5f;
        float highSide = (highL - highR) * 0.5f;
        highSide *= highWidth;
        highL = highMid + highSide;
        highR = highMid - highSide;

        // Sum all bands
        left = lowL + midL + highL;
        right = lowR + midR + highR;

        // Level metering
        lowSum += std::abs(lowL) + std::abs(lowR);
        midSum += std::abs(midL) + std::abs(midR);
        highSum += std::abs(highL) + std::abs(highR);

        leftChannel[i] = left;
        rightChannel[i] = right;
    }

    // Update level meters
    lowLevel.store(lowSum / (numSamples * 2));
    midLevel.store(midSum / (numSamples * 2));
    highLevel.store(highSum / (numSamples * 2));
}
