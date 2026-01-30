#pragma once

#include <cmath>
#include <algorithm>

namespace DSPUtils
{
    // Level conversion
    inline float linearToDecibels(float linear)
    {
        return linear > 0.0f ? 20.0f * std::log10(linear) : -100.0f;
    }

    inline float decibelsToLinear(float dB)
    {
        return std::pow(10.0f, dB / 20.0f);
    }

    // Range mapping
    inline float mapRange(float value, float inMin, float inMax, float outMin, float outMax)
    {
        return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
    }

    // Clipping functions
    inline float softClip(float sample)
    {
        return std::tanh(sample);
    }

    inline float hardClip(float sample, float threshold = 1.0f)
    {
        return std::clamp(sample, -threshold, threshold);
    }

    // One-pole filter coefficient
    inline float calculateCoefficient(double sampleRate, float timeMs)
    {
        if (timeMs <= 0.0f) return 1.0f;
        return 1.0f - std::exp(-1.0f / (static_cast<float>(sampleRate) * timeMs * 0.001f));
    }

    // Biquad coefficient structure
    struct BiquadCoeffs
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
    };

    // Linkwitz-Riley 4th order crossover (cascade of two 2nd-order Butterworth)
    // Returns coefficients for one 2nd-order section (use two in series for LR4)
    inline BiquadCoeffs calcLowPassLR(double sampleRate, float freq)
    {
        BiquadCoeffs c;
        float w0 = 2.0f * 3.14159265358979323846f * freq / static_cast<float>(sampleRate);
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * 0.7071067811865476f); // Q = 0.707 for Butterworth

        float a0 = 1.0f + alpha;
        c.b0 = ((1.0f - cosw0) / 2.0f) / a0;
        c.b1 = (1.0f - cosw0) / a0;
        c.b2 = ((1.0f - cosw0) / 2.0f) / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha) / a0;

        return c;
    }

    inline BiquadCoeffs calcHighPassLR(double sampleRate, float freq)
    {
        BiquadCoeffs c;
        float w0 = 2.0f * 3.14159265358979323846f * freq / static_cast<float>(sampleRate);
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * 0.7071067811865476f); // Q = 0.707 for Butterworth

        float a0 = 1.0f + alpha;
        c.b0 = ((1.0f + cosw0) / 2.0f) / a0;
        c.b1 = (-(1.0f + cosw0)) / a0;
        c.b2 = ((1.0f + cosw0) / 2.0f) / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha) / a0;

        return c;
    }

    inline BiquadCoeffs calcAllPass(double sampleRate, float freq, float q = 0.7071067811865476f)
    {
        BiquadCoeffs c;
        float w0 = 2.0f * 3.14159265358979323846f * freq / static_cast<float>(sampleRate);
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * q);

        float a0 = 1.0f + alpha;
        c.b0 = (1.0f - alpha) / a0;
        c.b1 = (-2.0f * cosw0) / a0;
        c.b2 = (1.0f + alpha) / a0;
        c.a1 = (-2.0f * cosw0) / a0;
        c.a2 = (1.0f - alpha) / a0;

        return c;
    }

    // Biquad filter state
    struct BiquadState
    {
        float z1 = 0.0f, z2 = 0.0f;

        void reset()
        {
            z1 = z2 = 0.0f;
        }

        inline float process(float input, const BiquadCoeffs& c)
        {
            float output = c.b0 * input + z1;
            z1 = c.b1 * input - c.a1 * output + z2;
            z2 = c.b2 * input - c.a2 * output;
            return output;
        }
    };

    // Parameter smoothing
    class SmoothedValue
    {
    public:
        void reset(double sampleRate, float rampTimeMs = 20.0f)
        {
            coeff = calculateCoefficient(sampleRate, rampTimeMs);
        }

        void setTargetValue(float target)
        {
            targetValue = target;
        }

        void setCurrentAndTargetValue(float value)
        {
            currentValue = targetValue = value;
        }

        float getNextValue()
        {
            currentValue += coeff * (targetValue - currentValue);
            return currentValue;
        }

        float getCurrentValue() const { return currentValue; }
        float getTargetValue() const { return targetValue; }
        bool isSmoothing() const { return std::abs(currentValue - targetValue) > 0.0001f; }

    private:
        float currentValue = 0.0f;
        float targetValue = 0.0f;
        float coeff = 0.1f;
    };
}
