#pragma once

#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour background    = juce::Colour(0xff1a1a1a);
    const juce::Colour panelBg       = juce::Colour(0xff252525);
    const juce::Colour accent        = juce::Colour(0xff4a9eff);
    const juce::Colour accentGreen   = juce::Colour(0xff22c55e);
    const juce::Colour accentPurple  = juce::Colour(0xff9b59b6);
    const juce::Colour textPrimary   = juce::Colour(0xfff0f0f0);
    const juce::Colour textSecondary = juce::Colour(0xff909090);
    const juce::Colour meterGreen    = juce::Colour(0xff22c55e);
    const juce::Colour meterYellow   = juce::Colour(0xffeab308);
    const juce::Colour meterRed      = juce::Colour(0xffef4444);
    const juce::Colour knobBody      = juce::Colour(0xff404040);
    const juce::Colour correlationGood = juce::Colour(0xff22c55e);
    const juce::Colour correlationWarn = juce::Colour(0xffeab308);
    const juce::Colour correlationBad  = juce::Colour(0xffef4444);
}

class StereoImagerLookAndFeel : public juce::LookAndFeel_V4
{
public:
    StereoImagerLookAndFeel()
    {
        setColour(juce::Slider::textBoxTextColourId, Colors::textPrimary);
        setColour(juce::Slider::textBoxBackgroundColourId, Colors::panelBg);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Label::textColourId, Colors::textPrimary);
        setColour(juce::ToggleButton::textColourId, Colors::textPrimary);
        setColour(juce::ToggleButton::tickColourId, Colors::accent);
        setColour(juce::ComboBox::backgroundColourId, Colors::panelBg);
        setColour(juce::ComboBox::textColourId, Colors::textPrimary);
        setColour(juce::ComboBox::outlineColourId, Colors::knobBody);
        setColour(juce::PopupMenu::backgroundColourId, Colors::panelBg);
        setColour(juce::PopupMenu::textColourId, Colors::textPrimary);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, Colors::accent);
    }

    void setAccentColour(juce::Colour c) { accentColour = c; }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float, float,
                          juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(2.0f);
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY();
        float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 2.0f;

        // Outer ring (knurled edge)
        g.setColour(juce::Colour(0xff303030));
        g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

        // Knurl pattern
        g.setColour(juce::Colour(0xff404040));
        int numKnurls = 24;
        for (int i = 0; i < numKnurls; ++i)
        {
            float angle = i * juce::MathConstants<float>::twoPi / numKnurls;
            float x1 = cx + (radius - 1.0f) * std::cos(angle);
            float y1 = cy + (radius - 1.0f) * std::sin(angle);
            float x2 = cx + (radius - 4.0f) * std::cos(angle);
            float y2 = cy + (radius - 4.0f) * std::sin(angle);
            g.drawLine(x1, y1, x2, y2, 1.5f);
        }

        // Main knob body with gradient
        float innerRadius = radius * 0.78f;
        juce::ColourGradient knobGradient(juce::Colour(0xff555555), cx - innerRadius * 0.5f, cy - innerRadius * 0.5f,
                                           juce::Colour(0xff252525), cx + innerRadius * 0.5f, cy + innerRadius * 0.5f, true);
        g.setGradientFill(knobGradient);
        g.fillEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

        // Ring
        g.setColour(juce::Colour(0xff606060));
        g.drawEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.0f);

        // Indicator line (7 o'clock to 5 o'clock range)
        float indicatorAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, -1.047f, 4.189f) + juce::MathConstants<float>::pi;
        float indicatorLength = innerRadius * 0.65f;
        float ix1 = cx + (innerRadius * 0.2f) * std::cos(indicatorAngle);
        float iy1 = cy + (innerRadius * 0.2f) * std::sin(indicatorAngle);
        float ix2 = cx + indicatorLength * std::cos(indicatorAngle);
        float iy2 = cy + indicatorLength * std::sin(indicatorAngle);
        g.setColour(accentColour);
        g.drawLine(ix1, iy1, ix2, iy2, 3.0f);

        // Center cap
        float capRadius = innerRadius * 0.25f;
        g.setColour(juce::Colour(0xff404040));
        g.fillEllipse(cx - capRadius, cy - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style == juce::Slider::LinearHorizontal || style == juce::Slider::LinearVertical)
        {
            auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();

            // Track
            float trackWidth = 4.0f;
            juce::Rectangle<float> track;

            if (style == juce::Slider::LinearHorizontal)
            {
                track = bounds.withSizeKeepingCentre(bounds.getWidth(), trackWidth);
            }
            else
            {
                track = bounds.withSizeKeepingCentre(trackWidth, bounds.getHeight());
            }

            g.setColour(juce::Colour(0xff303030));
            g.fillRoundedRectangle(track, 2.0f);

            // Filled portion
            juce::Rectangle<float> filled;
            if (style == juce::Slider::LinearHorizontal)
            {
                float fillWidth = sliderPos - track.getX();
                filled = track.withWidth(fillWidth);
            }
            else
            {
                float fillHeight = track.getBottom() - sliderPos;
                filled = track.withTop(sliderPos);
            }

            g.setColour(accentColour);
            g.fillRoundedRectangle(filled, 2.0f);

            // Thumb
            float thumbSize = 14.0f;
            juce::Rectangle<float> thumb;
            if (style == juce::Slider::LinearHorizontal)
            {
                thumb = juce::Rectangle<float>(thumbSize, thumbSize).withCentre({ sliderPos, bounds.getCentreY() });
            }
            else
            {
                thumb = juce::Rectangle<float>(thumbSize, thumbSize).withCentre({ bounds.getCentreX(), sliderPos });
            }

            g.setColour(juce::Colour(0xff555555));
            g.fillEllipse(thumb);
            g.setColour(juce::Colour(0xff707070));
            g.drawEllipse(thumb, 1.0f);
        }
        else
        {
            LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        auto bounds = button.getLocalBounds().toFloat();
        auto buttonArea = bounds.removeFromLeft(bounds.getHeight()).reduced(4.0f);

        // Button background
        g.setColour(button.getToggleState() ? accentColour : juce::Colour(0xff303030));
        g.fillRoundedRectangle(buttonArea, 3.0f);

        // Border
        g.setColour(juce::Colour(0xff505050));
        g.drawRoundedRectangle(buttonArea, 3.0f, 1.0f);

        // Checkmark if toggled
        if (button.getToggleState())
        {
            g.setColour(Colors::textPrimary);
            auto tick = buttonArea.reduced(buttonArea.getWidth() * 0.25f);
            juce::Path tickPath;
            tickPath.startNewSubPath(tick.getX(), tick.getCentreY());
            tickPath.lineTo(tick.getCentreX() - 2, tick.getBottom() - 3);
            tickPath.lineTo(tick.getRight(), tick.getY() + 3);
            g.strokePath(tickPath, juce::PathStrokeType(2.0f));
        }

        // Text
        g.setColour(Colors::textPrimary);
        g.setFont(14.0f);
        g.drawText(button.getButtonText(), bounds.reduced(4.0f, 0.0f), juce::Justification::centredLeft);
    }

private:
    juce::Colour accentColour = Colors::accent;
};
