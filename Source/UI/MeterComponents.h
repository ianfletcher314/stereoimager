#pragma once

#include <JuceHeader.h>
#include "LookAndFeel.h"

class LevelMeter : public juce::Component
{
public:
    enum Orientation { Vertical, Horizontal };

    LevelMeter(Orientation orient = Vertical) : orientation(orient) {}

    void setLevel(float newLevel)
    {
        level = newLevel;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(1.0f);

        // Background
        g.setColour(juce::Colour(0xff151515));
        g.fillRoundedRectangle(bounds, 3.0f);

        // Convert to dB and normalize
        float db = juce::Decibels::gainToDecibels(level, -60.0f);
        float normalized = juce::jmap(db, -60.0f, 0.0f, 0.0f, 1.0f);
        normalized = juce::jlimit(0.0f, 1.0f, normalized);

        // Level bar with color zones
        juce::Colour barColour;
        if (normalized < 0.6f)
            barColour = Colors::meterGreen;
        else if (normalized < 0.85f)
            barColour = Colors::meterYellow;
        else
            barColour = Colors::meterRed;

        if (orientation == Vertical)
        {
            float barHeight = bounds.getHeight() * normalized;
            g.setColour(barColour);
            g.fillRoundedRectangle(bounds.getX(), bounds.getBottom() - barHeight,
                                   bounds.getWidth(), barHeight, 2.0f);
        }
        else
        {
            float barWidth = bounds.getWidth() * normalized;
            g.setColour(barColour);
            g.fillRoundedRectangle(bounds.getX(), bounds.getY(),
                                   barWidth, bounds.getHeight(), 2.0f);
        }
    }

private:
    float level = 0.0f;
    Orientation orientation;
};

class StereoMeter : public juce::Component
{
public:
    void setLevels(float left, float right)
    {
        leftLevel = left;
        rightLevel = right;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(1.0f);
        float meterWidth = (bounds.getWidth() - 4.0f) / 2.0f;

        // Left meter
        auto leftBounds = bounds.removeFromLeft(meterWidth);
        g.setColour(juce::Colour(0xff151515));
        g.fillRoundedRectangle(leftBounds, 2.0f);

        float leftDb = juce::Decibels::gainToDecibels(leftLevel, -60.0f);
        float leftNorm = juce::jlimit(0.0f, 1.0f, juce::jmap(leftDb, -60.0f, 0.0f, 0.0f, 1.0f));
        float leftHeight = leftBounds.getHeight() * leftNorm;

        juce::Colour leftColour = leftNorm < 0.6f ? Colors::meterGreen :
                                  leftNorm < 0.85f ? Colors::meterYellow : Colors::meterRed;
        g.setColour(leftColour);
        g.fillRoundedRectangle(leftBounds.getX(), leftBounds.getBottom() - leftHeight,
                               leftBounds.getWidth(), leftHeight, 2.0f);

        bounds.removeFromLeft(4.0f); // Gap

        // Right meter
        auto rightBounds = bounds;
        g.setColour(juce::Colour(0xff151515));
        g.fillRoundedRectangle(rightBounds, 2.0f);

        float rightDb = juce::Decibels::gainToDecibels(rightLevel, -60.0f);
        float rightNorm = juce::jlimit(0.0f, 1.0f, juce::jmap(rightDb, -60.0f, 0.0f, 0.0f, 1.0f));
        float rightHeight = rightBounds.getHeight() * rightNorm;

        juce::Colour rightColour = rightNorm < 0.6f ? Colors::meterGreen :
                                   rightNorm < 0.85f ? Colors::meterYellow : Colors::meterRed;
        g.setColour(rightColour);
        g.fillRoundedRectangle(rightBounds.getX(), rightBounds.getBottom() - rightHeight,
                               rightBounds.getWidth(), rightHeight, 2.0f);

        // Labels
        g.setColour(Colors::textSecondary);
        g.setFont(10.0f);
        g.drawText("L", leftBounds.removeFromBottom(12.0f), juce::Justification::centred);
        g.drawText("R", rightBounds.removeFromBottom(12.0f), juce::Justification::centred);
    }

private:
    float leftLevel = 0.0f;
    float rightLevel = 0.0f;
};

class CorrelationMeter : public juce::Component
{
public:
    void setCorrelation(float newCorrelation)
    {
        correlation = juce::jlimit(-1.0f, 1.0f, newCorrelation);
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);

        // Background
        g.setColour(juce::Colour(0xff151515));
        g.fillRoundedRectangle(bounds, 3.0f);

        // Draw scale marks
        g.setColour(Colors::textSecondary.withAlpha(0.5f));
        float centerX = bounds.getCentreX();
        g.drawLine(centerX, bounds.getY() + 2, centerX, bounds.getBottom() - 10, 1.0f);

        // Draw quarter marks
        float quarterWidth = bounds.getWidth() / 4.0f;
        g.drawLine(bounds.getX() + quarterWidth, bounds.getY() + 4, bounds.getX() + quarterWidth, bounds.getBottom() - 12, 0.5f);
        g.drawLine(bounds.getRight() - quarterWidth, bounds.getY() + 4, bounds.getRight() - quarterWidth, bounds.getBottom() - 12, 0.5f);

        // Calculate indicator position (-1 to +1 mapped to width)
        float normalized = (correlation + 1.0f) / 2.0f;
        float indicatorX = bounds.getX() + bounds.getWidth() * normalized;

        // Determine color based on correlation
        juce::Colour indicatorColour;
        if (correlation > 0.5f)
            indicatorColour = Colors::correlationGood;
        else if (correlation > 0.0f)
            indicatorColour = Colors::correlationWarn;
        else
            indicatorColour = Colors::correlationBad;

        // Draw indicator
        float indicatorHeight = bounds.getHeight() - 14.0f;
        g.setColour(indicatorColour);
        g.fillRoundedRectangle(indicatorX - 3.0f, bounds.getY() + 2, 6.0f, indicatorHeight, 2.0f);

        // Draw labels
        g.setColour(Colors::textSecondary);
        g.setFont(9.0f);
        g.drawText("-1", bounds.withHeight(12.0f).withY(bounds.getBottom() - 12), juce::Justification::centredLeft);
        g.drawText("+1", bounds.withHeight(12.0f).withY(bounds.getBottom() - 12), juce::Justification::centredRight);
        g.drawText("0", bounds.withHeight(12.0f).withY(bounds.getBottom() - 12), juce::Justification::centred);
    }

private:
    float correlation = 1.0f;
};

class Vectorscope : public juce::Component
{
public:
    void setSamples(const std::vector<std::pair<float, float>>& newSamples)
    {
        samples = newSamples;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(4.0f);
        float size = juce::jmin(bounds.getWidth(), bounds.getHeight());
        auto scopeBounds = bounds.withSizeKeepingCentre(size, size);

        // Background
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillEllipse(scopeBounds);

        // Grid lines
        g.setColour(juce::Colour(0xff303030));
        float cx = scopeBounds.getCentreX();
        float cy = scopeBounds.getCentreY();
        float radius = size / 2.0f;

        // Cross
        g.drawLine(cx - radius, cy, cx + radius, cy, 0.5f);
        g.drawLine(cx, cy - radius, cx, cy + radius, 0.5f);

        // Diagonals (L and R axes in M/S)
        g.drawLine(cx - radius * 0.707f, cy - radius * 0.707f,
                   cx + radius * 0.707f, cy + radius * 0.707f, 0.5f);
        g.drawLine(cx - radius * 0.707f, cy + radius * 0.707f,
                   cx + radius * 0.707f, cy - radius * 0.707f, 0.5f);

        // Circles
        g.drawEllipse(scopeBounds.reduced(radius * 0.33f), 0.5f);
        g.drawEllipse(scopeBounds.reduced(radius * 0.66f), 0.5f);

        // Draw labels
        g.setColour(Colors::textSecondary);
        g.setFont(10.0f);
        g.drawText("M", scopeBounds.withHeight(12.0f).translated(0, -radius + 2), juce::Justification::centred);
        g.drawText("S", scopeBounds.withHeight(12.0f).translated(radius - 12, -radius / 2.0f), juce::Justification::centred);
        g.drawText("L", scopeBounds.withHeight(12.0f).translated(-radius * 0.6f, -radius * 0.7f), juce::Justification::centred);
        g.drawText("R", scopeBounds.withHeight(12.0f).translated(radius * 0.5f, -radius * 0.7f), juce::Justification::centred);

        // Draw samples (rotated 45 degrees for M/S display)
        // In goniometer: x = (R-L)/2 = Side, y = (R+L)/2 = Mid
        g.setColour(Colors::accent.withAlpha(0.6f));

        for (size_t i = 0; i < samples.size(); ++i)
        {
            float left = samples[i].first;
            float right = samples[i].second;

            // Convert to M/S for display
            float mid = (left + right) * 0.5f;
            float side = (right - left) * 0.5f;

            // Scale and position
            float x = cx + side * radius * 0.8f;
            float y = cy - mid * radius * 0.8f;

            // Fade based on age
            float alpha = 0.3f + 0.5f * (1.0f - (float)i / samples.size());
            g.setColour(Colors::accent.withAlpha(alpha));
            g.fillEllipse(x - 1.0f, y - 1.0f, 2.0f, 2.0f);
        }
    }

private:
    std::vector<std::pair<float, float>> samples;
};

class BandMeter : public juce::Component
{
public:
    void setLevels(float low, float mid, float high)
    {
        lowLevel = low;
        midLevel = mid;
        highLevel = high;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        float barWidth = (bounds.getWidth() - 8.0f) / 3.0f;

        auto drawBar = [&](float x, float level, juce::Colour colour, const juce::String& label)
        {
            auto barBounds = juce::Rectangle<float>(x, bounds.getY(), barWidth, bounds.getHeight() - 14.0f);

            g.setColour(juce::Colour(0xff151515));
            g.fillRoundedRectangle(barBounds, 2.0f);

            float db = juce::Decibels::gainToDecibels(level, -60.0f);
            float normalized = juce::jlimit(0.0f, 1.0f, juce::jmap(db, -60.0f, 0.0f, 0.0f, 1.0f));
            float barHeight = barBounds.getHeight() * normalized;

            g.setColour(colour);
            g.fillRoundedRectangle(barBounds.getX(), barBounds.getBottom() - barHeight,
                                   barBounds.getWidth(), barHeight, 2.0f);

            g.setColour(Colors::textSecondary);
            g.setFont(9.0f);
            g.drawText(label, barBounds.withY(bounds.getBottom() - 12.0f).withHeight(12.0f),
                       juce::Justification::centred);
        };

        drawBar(bounds.getX(), lowLevel, juce::Colour(0xff3498db), "L");
        drawBar(bounds.getX() + barWidth + 4.0f, midLevel, juce::Colour(0xff9b59b6), "M");
        drawBar(bounds.getX() + (barWidth + 4.0f) * 2, highLevel, juce::Colour(0xffe74c3c), "H");
    }

private:
    float lowLevel = 0.0f;
    float midLevel = 0.0f;
    float highLevel = 0.0f;
};
