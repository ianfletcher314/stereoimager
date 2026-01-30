#include "StereoProcessor.h"

StereoProcessor::StereoProcessor()
{
    vectorscopeBuffer.resize(vectorscopeBufferSize, { 0.0f, 0.0f });
}

void StereoProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // Initialize smoothed parameters
    widthSmoothed.reset(sampleRate, 20.0f);
    widthSmoothed.setCurrentAndTargetValue(1.0f); // 100% = unchanged

    panSmoothed.reset(sampleRate, 20.0f);
    panSmoothed.setCurrentAndTargetValue(0.0f);

    balanceSmoothed.reset(sampleRate, 20.0f);
    balanceSmoothed.setCurrentAndTargetValue(0.0f);

    // Initialize mono bass filter
    monoBassLPCoeffs = DSPUtils::calcLowPassLR(sampleRate, monoBassFreq);
    monoBassHPCoeffs = DSPUtils::calcHighPassLR(sampleRate, monoBassFreq);

    reset();
}

void StereoProcessor::reset()
{
    // Reset filter states
    monoBassLPStateL1.reset();
    monoBassLPStateL2.reset();
    monoBassLPStateR1.reset();
    monoBassLPStateR2.reset();
    monoBassHPStateL1.reset();
    monoBassHPStateL2.reset();
    monoBassHPStateR1.reset();
    monoBassHPStateR2.reset();

    // Reset correlation
    corrSum = 0.0f;
    leftSqSum = 0.0f;
    rightSqSum = 0.0f;
    corrSampleCount = 0;

    // Reset vectorscope buffer
    std::lock_guard<std::mutex> lock(vectorscopeMutex);
    std::fill(vectorscopeBuffer.begin(), vectorscopeBuffer.end(), std::make_pair(0.0f, 0.0f));
    vectorscopeWriteIndex = 0;
}

void StereoProcessor::setWidth(float widthPercent)
{
    // Convert 0-200% to 0-2 multiplier
    widthSmoothed.setTargetValue(widthPercent / 100.0f);
}

void StereoProcessor::setPan(float panValue)
{
    panSmoothed.setTargetValue(std::clamp(panValue, -1.0f, 1.0f));
}

void StereoProcessor::setBalance(float balanceValue)
{
    balanceSmoothed.setTargetValue(std::clamp(balanceValue, -1.0f, 1.0f));
}

void StereoProcessor::setMonoBassFreq(float freqHz)
{
    monoBassFreq = std::clamp(freqHz, 20.0f, 500.0f);
    monoBassLPCoeffs = DSPUtils::calcLowPassLR(currentSampleRate, monoBassFreq);
    monoBassHPCoeffs = DSPUtils::calcHighPassLR(currentSampleRate, monoBassFreq);
}

void StereoProcessor::setMonoBassEnabled(bool enabled)
{
    monoBassEnabled = enabled;
}

void StereoProcessor::setBypass(bool shouldBypass)
{
    bypassed = shouldBypass;
}

void StereoProcessor::encodeMS(float left, float right, float& mid, float& side)
{
    mid = (left + right) * 0.5f;
    side = (left - right) * 0.5f;
}

void StereoProcessor::decodeMS(float mid, float side, float& left, float& right)
{
    left = mid + side;
    right = mid - side;
}

void StereoProcessor::process(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels < 2)
        return; // Need stereo

    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    // Level accumulators for metering
    float leftSum = 0.0f;
    float rightSum = 0.0f;
    float midSum = 0.0f;
    float sideSum = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        float left = leftChannel[i];
        float right = rightChannel[i];

        // Get smoothed parameter values
        float width = widthSmoothed.getNextValue();
        float pan = panSmoothed.getNextValue();
        float balance = balanceSmoothed.getNextValue();

        // Mono bass processing
        if (monoBassEnabled)
        {
            // Split into low and high bands using Linkwitz-Riley (cascade of 2 Butterworth)
            float lowL = monoBassLPStateL2.process(monoBassLPStateL1.process(left, monoBassLPCoeffs), monoBassLPCoeffs);
            float lowR = monoBassLPStateR2.process(monoBassLPStateR1.process(right, monoBassLPCoeffs), monoBassLPCoeffs);
            float highL = monoBassHPStateL2.process(monoBassHPStateL1.process(left, monoBassHPCoeffs), monoBassHPCoeffs);
            float highR = monoBassHPStateR2.process(monoBassHPStateR1.process(right, monoBassHPCoeffs), monoBassHPCoeffs);

            // Sum low frequencies to mono
            float lowMono = (lowL + lowR) * 0.5f;

            // Recombine: mono lows + stereo highs
            left = lowMono + highL;
            right = lowMono + highR;
        }

        // Convert to M/S
        float mid, side;
        encodeMS(left, right, mid, side);

        // Apply width (scale side signal)
        side *= width;

        // Convert back to L/R
        decodeMS(mid, side, left, right);

        // Apply balance (relative L/R level)
        if (balance < 0.0f)
        {
            // Left heavy - reduce right
            right *= (1.0f + balance);
        }
        else if (balance > 0.0f)
        {
            // Right heavy - reduce left
            left *= (1.0f - balance);
        }

        // Apply pan (constant power panning)
        float panAngle = (pan + 1.0f) * 0.25f * 3.14159265358979323846f; // 0 to pi/2
        float panL = std::cos(panAngle);
        float panR = std::sin(panAngle);

        float monoMix = (left + right) * 0.5f;
        left = left * panL + monoMix * (1.0f - panL);
        right = right * panR + monoMix * (1.0f - panR);

        // Correlation calculation
        corrSum += left * right;
        leftSqSum += left * left;
        rightSqSum += right * right;
        corrSampleCount++;

        if (corrSampleCount >= corrWindowSize)
        {
            float denom = std::sqrt(leftSqSum * rightSqSum);
            if (denom > 0.0001f)
                correlation.store(corrSum / denom);
            else
                correlation.store(1.0f);

            corrSum = 0.0f;
            leftSqSum = 0.0f;
            rightSqSum = 0.0f;
            corrSampleCount = 0;
        }

        // Store for vectorscope (downsample)
        if (i % 4 == 0)
        {
            std::lock_guard<std::mutex> lock(vectorscopeMutex);
            vectorscopeBuffer[vectorscopeWriteIndex] = { left, right };
            vectorscopeWriteIndex = (vectorscopeWriteIndex + 1) % vectorscopeBufferSize;
        }

        // Accumulate levels
        leftSum += std::abs(left);
        rightSum += std::abs(right);

        float m, s;
        encodeMS(left, right, m, s);
        midSum += std::abs(m);
        sideSum += std::abs(s);

        // Write output
        leftChannel[i] = left;
        rightChannel[i] = right;
    }

    // Update level meters (RMS-ish average)
    leftLevel.store(leftSum / numSamples);
    rightLevel.store(rightSum / numSamples);
    midLevel.store(midSum / numSamples);
    sideLevel.store(sideSum / numSamples);
}

void StereoProcessor::getStereoSamples(std::vector<std::pair<float, float>>& samples) const
{
    std::lock_guard<std::mutex> lock(vectorscopeMutex);
    samples = vectorscopeBuffer;
}
