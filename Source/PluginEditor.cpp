#include "PluginEditor.h"
#include <cmath>

namespace AetherUI
{

//==============================================================================
ModernSkinLookAndFeel::ModernSkinLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0xff111215));
    setColour (juce::Label::textColourId, juce::Colour (0xffe2e8f0));
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff181a1f));
    setColour (juce::ComboBox::textColourId, juce::Colour (0xffe2e8f0));
    setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff2b2f38));
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff181a1f));
    setColour (juce::PopupMenu::textColourId, juce::Colour (0xffe2e8f0));
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xff252932));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colour (0xff00f0ff));
}

void ModernSkinLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                              juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f - 6.0f;
    const auto center = bounds.getCentre();

    if (radius <= 0.0f)
        return;

    // Outer ring background
    const float trackThickness = 3.5f;
    const float arcRadius = radius - trackThickness * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc (center.x, center.y, arcRadius, arcRadius, 0.0f,
                                 rotaryStartAngle, rotaryEndAngle, true);

    g.setColour (juce::Colour (0xff22252c));
    g.strokePath (backgroundArc, juce::PathStrokeType (trackThickness, juce::PathStrokeType::curved,
                                                       juce::PathStrokeType::rounded));

    // Dial body
    const float innerRadius = radius - trackThickness - 3.0f;
    if (innerRadius > 2.0f)
    {
        g.setColour (juce::Colour (0xff181a1f));
        g.fillEllipse (center.x - innerRadius, center.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

        g.setColour (juce::Colour (0xff2b2f38));
        g.drawEllipse (center.x - innerRadius, center.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.0f);
    }

    // Active value arc (Electric Cyan)
    const float currentAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    if (std::abs (currentAngle - rotaryStartAngle) > 0.001f)
    {
        juce::Path valueArc;
        valueArc.addCentredArc (center.x, center.y, arcRadius, arcRadius, 0.0f,
                                rotaryStartAngle, currentAngle, true);

        g.setColour (juce::Colour (0xff00f0ff));
        g.strokePath (valueArc, juce::PathStrokeType (trackThickness, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));
    }

    // Indicator pointer dot
    const float pointerDist = innerRadius * 0.65f;
    const float px = center.x + pointerDist * std::sin (currentAngle);
    const float py = center.y - pointerDist * std::cos (currentAngle);

    g.setColour (slider.isEnabled() ? juce::Colour (0xff00f0ff) : juce::Colour (0xff555a66));
    g.fillEllipse (px - 2.5f, py - 2.5f, 5.0f, 5.0f);
}

void ModernSkinLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                          int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                          juce::ComboBox& box)
{
    const auto r = juce::Rectangle<int> (0, 0, width, height).toFloat();
    g.setColour (juce::Colour (0xff181a1f));
    g.fillRoundedRectangle (r, 4.0f);

    g.setColour (box.hasKeyboardFocus (true) ? juce::Colour (0xff00f0ff) : juce::Colour (0xff2b2f38));
    g.drawRoundedRectangle (r, 4.0f, 1.0f);

    // Minimal chevron arrow
    const float arrowX = static_cast<float> (width - 16);
    const float arrowY = static_cast<float> (height) * 0.5f - 2.0f;

    juce::Path p;
    p.startNewSubPath (arrowX - 4.0f, arrowY);
    p.lineTo (arrowX, arrowY + 4.0f);
    p.lineTo (arrowX + 4.0f, arrowY);

    g.setColour (juce::Colour (0xff7e8696));
    g.strokePath (p, juce::PathStrokeType (1.5f));
}

void ModernSkinLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int width, int height)
{
    g.setColour (juce::Colour (0xff181a1f));
    g.fillRoundedRectangle (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height), 4.0f);

    g.setColour (juce::Colour (0xff2b2f38));
    g.drawRoundedRectangle (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height), 4.0f, 1.0f);
}

void ModernSkinLookAndFeel::drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                                               bool isSeparator, bool isActive, bool isHighlighted, bool /*isTicked*/, bool /*hasSubMenu*/,
                                               const juce::String& text, const juce::String& /*shortcutKeyText*/,
                                               const juce::Drawable* /*icon*/, const juce::Colour* /*textColour*/)
{
    if (isSeparator)
    {
        g.setColour (juce::Colour (0xff252932));
        g.fillRect (area.reduced (8, 0).removeFromTop (1));
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour (juce::Colour (0xff22262f));
        g.fillRect (area);
        g.setColour (juce::Colour (0xff00f0ff));
    }
    else
    {
        g.setColour (isActive ? juce::Colour (0xffc8cfdc) : juce::Colour (0xff606673));
    }

    g.setFont (juce::Font (13.0f, juce::Font::plain));
    g.drawText (text, area.reduced (10, 0), juce::Justification::centredLeft, true);
}

void ModernSkinLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                              bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
{
    const auto r = button.getLocalBounds().toFloat();
    const bool on = button.getToggleState();

    // Compact modern status switch
    const float pillWidth = 28.0f;
    const float pillHeight = 14.0f;
    const auto pillArea = juce::Rectangle<float> (r.getX() + 2.0f, r.getCentreY() - pillHeight * 0.5f,
                                                  pillWidth, pillHeight);

    g.setColour (on ? juce::Colour (0xff14353d) : juce::Colour (0xff20232a));
    g.fillRoundedRectangle (pillArea, pillHeight * 0.5f);

    g.setColour (on ? juce::Colour (0xff00f0ff) : juce::Colour (0xff323742));
    g.drawRoundedRectangle (pillArea, pillHeight * 0.5f, 1.0f);

    const float dotRadius = 4.5f;
    const float dotX = on ? (pillArea.getRight() - dotRadius - 2.5f) : (pillArea.getX() + dotRadius + 2.5f);
    const float dotY = pillArea.getCentreY();

    g.setColour (on ? juce::Colour (0xff00f0ff) : juce::Colour (0xff707684));
    g.fillEllipse (dotX - dotRadius, dotY - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);

    if (button.getButtonText().isNotEmpty())
    {
        g.setColour (on ? juce::Colour (0xffe2e8f0) : juce::Colour (0xff7e8696));
        g.setFont (juce::Font (11.0f, juce::Font::bold));
        g.drawText (button.getButtonText(),
                    r.withTrimmedLeft (pillWidth + 8.0f),
                    juce::Justification::centredLeft, true);
    }
}

void ModernSkinLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                                  const juce::Colour& /*backgroundColour*/,
                                                  bool shouldDrawButtonAsHighlighted,
                                                  bool shouldDrawButtonAsDown)
{
    const auto bounds = button.getLocalBounds().toFloat();
    const bool isRnd = button.getButtonText().contains ("RND");

    juce::Colour bg = juce::Colour (0xff181a1f);
    juce::Colour border = juce::Colour (0xff2b2f38);

    if (shouldDrawButtonAsDown)
    {
        bg = isRnd ? juce::Colour (0xff163842) : juce::Colour (0xff282c35);
        border = juce::Colour (0xff00f0ff);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        bg = isRnd ? juce::Colour (0xff142d35) : juce::Colour (0xff22252c);
        border = isRnd ? juce::Colour (0xff00f0ff) : juce::Colour (0xff3d4350);
    }

    g.setColour (bg);
    g.fillRoundedRectangle (bounds, 4.0f);

    g.setColour (border);
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);
}

void ModernSkinLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown)
{
    const bool isRnd = button.getButtonText().contains ("RND");
    juce::Colour textColour = isRnd ? juce::Colour (0xff00f0ff) : juce::Colour (0xffc8cfdc);

    if (shouldDrawButtonAsDown)
        textColour = juce::Colour (0xffffffff);
    else if (shouldDrawButtonAsHighlighted)
        textColour = isRnd ? juce::Colour (0xff70ffff) : juce::Colour (0xffe2e8f0);

    g.setColour (textColour);
    g.setFont (juce::Font (11.0f, juce::Font::bold));
    g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, true);
}

//==============================================================================
WaveformVisualizer::WaveformVisualizer (AetherUIAudioProcessor& proc)
    : processor (proc)
{
    startTimerHz (60);
}

WaveformVisualizer::~WaveformVisualizer()
{
    stopTimer();
}

void WaveformVisualizer::timerCallback()
{
    auto& fifo = processor.getVisualFifo();
    const auto& buffer = processor.getVisualBuffer();
    const int numReady = fifo.getNumReady();

    if (numReady > 0)
    {
        const int readSize = juce::jmin (numReady, static_cast<int> (displayBuffer.size()));
        int start1, size1, start2, size2;
        fifo.prepareToRead (readSize, start1, size1, start2, size2);

        // Shift left
        const int shift = size1 + size2;
        if (shift < static_cast<int> (displayBuffer.size()))
        {
            std::copy (displayBuffer.begin() + shift, displayBuffer.end(), displayBuffer.begin());
        }

        int writePos = static_cast<int> (displayBuffer.size()) - shift;
        if (size1 > 0)
            std::copy_n (buffer.data() + start1, size1, displayBuffer.data() + writePos);
        if (size2 > 0)
            std::copy_n (buffer.data() + start2, size2, displayBuffer.data() + writePos + size1);

        fifo.finishedRead (shift);

        // Track peak
        float maxVal = 0.0f;
        for (float sample : displayBuffer)
            maxVal = juce::jmax (maxVal, std::abs (sample));
        peakLevel = maxVal;

        repaint();
    }
}

void WaveformVisualizer::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    // Dark oscilloscope background
    g.setColour (juce::Colour (0xff0f1013));
    g.fillRoundedRectangle (bounds, 4.0f);

    // Subtle grid lines
    g.setColour (juce::Colour (0xff1a1d24));
    const int numGridX = 8;
    for (int i = 1; i < numGridX; ++i)
    {
        const float gx = bounds.getX() + bounds.getWidth() * (static_cast<float> (i) / numGridX);
        g.drawVerticalLine (static_cast<int> (gx), bounds.getY(), bounds.getBottom());
    }

    const float centerY = bounds.getCentreY();
    g.setColour (juce::Colour (0xff232731));
    g.drawHorizontalLine (static_cast<int> (centerY), bounds.getX(), bounds.getRight());

    // Waveform Path
    juce::Path wavePath;
    const float w = bounds.getWidth();
    const float h = bounds.getHeight();
    const float halfH = h * 0.45f;

    bool pathStarted = false;
    for (size_t i = 0; i < displayBuffer.size(); ++i)
    {
        const float x = bounds.getX() + (static_cast<float> (i) / static_cast<float> (displayBuffer.size() - 1)) * w;
        const float y = centerY - juce::jlimit (-1.0f, 1.0f, displayBuffer[i]) * halfH;

        if (! pathStarted)
        {
            wavePath.startNewSubPath (x, y);
            pathStarted = true;
        }
        else
        {
            wavePath.lineTo (x, y);
        }
    }

    // Glow under-layer
    g.setColour (juce::Colour (0x2800f0ff));
    g.strokePath (wavePath, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Sharp phosphor cyan line
    g.setColour (juce::Colour (0xff00f0ff));
    g.strokePath (wavePath, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Border
    g.setColour (juce::Colour (0xff2b2f38));
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

    // HUD labels
    g.setColour (juce::Colour (0xff656d7c));
    g.setFont (juce::Font (10.0f, juce::Font::plain));
    g.drawText ("OSCILLOSCOPE // REALTIME", bounds.reduced (8, 6), juce::Justification::topLeft, false);

    const juce::String peakStr = juce::String (juce::Decibels::gainToDecibels (juce::jmax (0.0001f, peakLevel)), 1) + " dB";
    g.drawText ("PEAK " + peakStr, bounds.reduced (8, 6), juce::Justification::topRight, false);
}

//==============================================================================
EnvelopeVisualizer::EnvelopeVisualizer (AetherUIAudioProcessor& proc)
    : processor (proc)
{
}

void EnvelopeVisualizer::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const float w = bounds.getWidth();
    const float h = bounds.getHeight();

    // Dark vector screen background
    g.setColour (juce::Colour (0xff0e1014));
    g.fillRoundedRectangle (bounds, 4.0f);

    // Subtle oscilloscope grid lines
    g.setColour (juce::Colour (0xff1b1e26));
    for (int i = 1; i < 6; ++i)
    {
        const float gx = bounds.getX() + w * (static_cast<float> (i) / 6.0f);
        g.drawVerticalLine (static_cast<int> (gx), bounds.getY(), bounds.getBottom());
    }
    for (int j = 1; j < 4; ++j)
    {
        const float gy = bounds.getY() + h * (static_cast<float> (j) / 4.0f);
        g.drawHorizontalLine (static_cast<int> (gy), bounds.getX(), bounds.getRight());
    }

    // Outer border
    g.setColour (juce::Colour (0xff282c36));
    g.drawRoundedRectangle (bounds, 4.0f, 1.0f);

    const float padX = 14.0f;
    const float padY = 16.0f;
    const float drawW = w - padX * 2.0f;
    const float drawH = h - padY * 2.0f;
    const float baselineY = bounds.getBottom() - padY;
    const float topY = bounds.getY() + padY;

    auto& apvts = processor.getAPVTS();

    if (currentMode == 0) // GLOBAL ADSR
    {
        const float att = apvts.getRawParameterValue ("env_attack_ms")->load();
        const float dec = apvts.getRawParameterValue ("env_decay_ms")->load();
        const float sus = juce::jlimit (0.0f, 1.0f, apvts.getRawParameterValue ("env_sustain")->load());
        const float rel = apvts.getRawParameterValue ("env_release_ms")->load();

        const float totalTime = att + dec + 200.0f + rel;
        const float x0 = bounds.getX() + padX;
        const float x1 = x0 + (att / totalTime) * drawW;
        const float x2 = x1 + (dec / totalTime) * drawW;
        const float x3 = x2 + (200.0f / totalTime) * drawW;
        const float x4 = bounds.getX() + padX + drawW;

        const float ySus = baselineY - sus * drawH;

        juce::Path p;
        p.startNewSubPath (x0, baselineY);
        p.lineTo (x1, topY);
        const int decSteps = 16;
        for (int s = 1; s <= decSteps; ++s)
        {
            const float frac = static_cast<float> (s) / static_cast<float> (decSteps);
            const float px = x1 + frac * (x2 - x1);
            const float py = topY + (1.0f - std::exp (-3.0f * frac)) / (1.0f - std::exp (-3.0f)) * (ySus - topY);
            p.lineTo (px, py);
        }
        p.lineTo (x3, ySus);
        const int relSteps = 16;
        for (int s = 1; s <= relSteps; ++s)
        {
            const float frac = static_cast<float> (s) / static_cast<float> (relSteps);
            const float px = x3 + frac * (x4 - x3);
            const float py = ySus + (1.0f - std::exp (-3.0f * frac)) / (1.0f - std::exp (-3.0f)) * (baselineY - ySus);
            p.lineTo (px, py);
        }

        // Fill under curve
        juce::Path fillP = p;
        fillP.lineTo (x4, baselineY);
        fillP.closeSubPath();
        g.setGradientFill (juce::ColourGradient (juce::Colour (0x3500f0ff), bounds.getCentreX(), topY,
                                                 juce::Colour (0x0200f0ff), bounds.getCentreX(), baselineY, false));
        g.fillPath (fillP);

        // Vector stroke
        g.setColour (juce::Colour (0x4000f0ff));
        g.strokePath (p, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (juce::Colour (0xff00f0ff));
        g.strokePath (p, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Vertex node dots
        auto drawDot = [&g] (float x, float y) {
            g.setColour (juce::Colour (0xff00f0ff));
            g.fillEllipse (x - 3.0f, y - 3.0f, 6.0f, 6.0f);
            g.setColour (juce::Colour (0xffffffff));
            g.fillEllipse (x - 1.5f, y - 1.5f, 3.0f, 3.0f);
        };
        drawDot (x0, baselineY);
        drawDot (x1, topY);
        drawDot (x2, ySus);
        drawDot (x3, ySus);
        drawDot (x4, baselineY);

        // HUD Text
        g.setColour (juce::Colour (0xff828a99));
        g.setFont (juce::Font (9.5f, juce::Font::bold));
        g.drawText ("GLOBAL AMP ADSR", bounds.reduced (8, 6), juce::Justification::topLeft, false);
        g.setColour (juce::Colour (0xff00f0ff));
        g.drawText (juce::String::formatted ("A:%.1fms  D:%.0fms  S:%.2f  R:%.0fms", att, dec, sus, rel),
                    bounds.reduced (8, 6), juce::Justification::topRight, false);
    }
    else if (currentMode == 1) // TRANSIENT
    {
        const float att = apvts.getRawParameterValue ("trans_attack_ms")->load();
        const float dec = apvts.getRawParameterValue ("trans_decay_ms")->load();

        const float totalTime = att + dec;
        const float x0 = bounds.getX() + padX;
        const float x1 = x0 + (att / totalTime) * (drawW * 0.25f);
        const float x2 = bounds.getX() + padX + drawW;

        juce::Path p;
        p.startNewSubPath (x0, baselineY);
        p.lineTo (x1, topY);
        const int steps = 32;
        for (int s = 1; s <= steps; ++s)
        {
            const float frac = static_cast<float> (s) / static_cast<float> (steps);
            const float px = x1 + frac * (x2 - x1);
            const float py = topY + (1.0f - std::exp (-4.5f * frac)) / (1.0f - std::exp (-4.5f)) * (baselineY - topY);
            p.lineTo (px, py);
        }

        juce::Path fillP = p;
        fillP.lineTo (x2, baselineY);
        fillP.closeSubPath();
        g.setGradientFill (juce::ColourGradient (juce::Colour (0x35ffaa00), bounds.getCentreX(), topY,
                                                 juce::Colour (0x02ffaa00), bounds.getCentreX(), baselineY, false));
        g.fillPath (fillP);

        g.setColour (juce::Colour (0x40ffaa00));
        g.strokePath (p, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (juce::Colour (0xffffaa00));
        g.strokePath (p, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour (juce::Colour (0xff828a99));
        g.setFont (juce::Font (9.5f, juce::Font::bold));
        g.drawText ("TRANSIENT IMPULSE CONTOUR", bounds.reduced (8, 6), juce::Justification::topLeft, false);
        g.setColour (juce::Colour (0xffffaa00));
        g.drawText (juce::String::formatted ("ATT: %.2f ms  |  DECAY: %.2f ms", att, dec),
                    bounds.reduced (8, 6), juce::Justification::topRight, false);
    }
    else if (currentMode == 2) // FM
    {
        const float fmAtt = apvts.getRawParameterValue ("fm_attack_ms")->load();
        const float fmDec = apvts.getRawParameterValue ("fm_decay_ms")->load();
        const float carDec = apvts.getRawParameterValue ("fm_carrier_decay_ms")->load();

        const float maxT = std::max (fmDec, carDec) + fmAtt;
        const float x0 = bounds.getX() + padX;
        const float x1 = x0 + (fmAtt / maxT) * (drawW * 0.2f);
        const float x2 = bounds.getX() + padX + drawW;

        // Carrier curve (green)
        juce::Path pCar;
        pCar.startNewSubPath (x0, baselineY);
        pCar.lineTo (x1, topY);
        for (int s = 1; s <= 24; ++s)
        {
            const float frac = static_cast<float> (s) / 24.0f;
            const float px = x1 + frac * (x2 - x1);
            const float py = topY + (1.0f - std::exp (-3.0f * frac)) / (1.0f - std::exp (-3.0f)) * (baselineY - topY);
            pCar.lineTo (px, py);
        }

        // FM Mod Index curve (cyan)
        juce::Path pMod;
        pMod.startNewSubPath (x0, baselineY);
        pMod.lineTo (x1, topY);
        const float modScale = std::min (1.0f, (fmDec + fmAtt) / maxT);
        const float modXEnd = x1 + modScale * (x2 - x1);
        for (int s = 1; s <= 24; ++s)
        {
            const float frac = static_cast<float> (s) / 24.0f;
            const float px = x1 + frac * (modXEnd - x1);
            const float py = topY + (1.0f - std::exp (-3.5f * frac)) / (1.0f - std::exp (-3.5f)) * (baselineY - topY);
            pMod.lineTo (px, py);
        }
        pMod.lineTo (x2, baselineY);

        g.setColour (juce::Colour (0x8038ef7d));
        g.strokePath (pCar, juce::PathStrokeType (1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (juce::Colour (0xff00f0ff));
        g.strokePath (pMod, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour (juce::Colour (0xff828a99));
        g.setFont (juce::Font (9.5f, juce::Font::bold));
        g.drawText ("FM DUAL-OP CONTOURS (MOD=CYAN, AMP=GREEN)", bounds.reduced (8, 6), juce::Justification::topLeft, false);
        g.setColour (juce::Colour (0xff00f0ff));
        g.drawText (juce::String::formatted ("FM MOD: %.0fms  |  AMP: %.0fms", fmDec, carDec),
                    bounds.reduced (8, 6), juce::Justification::topRight, false);
    }
    else // PITCH ENV
    {
        const float pDepth = apvts.getRawParameterValue ("fm_pitch_env_depth")->load();
        const float pDec = apvts.getRawParameterValue ("fm_pitch_env_decay_ms")->load();

        const float midY = bounds.getCentreY();
        g.setColour (juce::Colour (0xff252932));
        g.drawHorizontalLine (static_cast<int> (midY), bounds.getX() + padX, bounds.getX() + padX + drawW);

        const float x0 = bounds.getX() + padX;
        const float xEnd = x0 + drawW;

        const float normDepth = juce::jlimit (-1.0f, 1.0f, pDepth / 48.0f);
        const float startY = midY - normDepth * (drawH * 0.45f);

        juce::Path p;
        p.startNewSubPath (x0, startY);
        for (int s = 1; s <= 32; ++s)
        {
            const float frac = static_cast<float> (s) / 32.0f;
            const float px = x0 + frac * (xEnd - x0);
            const float py = midY + (startY - midY) * std::exp (-4.0f * frac);
            p.lineTo (px, py);
        }

        g.setColour (juce::Colour (0x30d946ef));
        g.strokePath (p, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour (juce::Colour (0xffd946ef));
        g.strokePath (p, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour (juce::Colour (0xff828a99));
        g.setFont (juce::Font (9.5f, juce::Font::bold));
        g.drawText ("PITCH SWEEP CONTOUR", bounds.reduced (8, 6), juce::Justification::topLeft, false);
        g.setColour (juce::Colour (0xffd946ef));
        g.drawText (juce::String::formatted ("DEPTH: %+.1f st  |  DECAY: %.0f ms", pDepth, pDec),
                    bounds.reduced (8, 6), juce::Justification::topRight, false);
    }
}

} // namespace AetherUI

//==============================================================================
AetherUIAudioProcessorEditor::AetherUIAudioProcessorEditor (AetherUIAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), visualizer (p), envVisualizer (p)
{
    setLookAndFeel (&customLookAndFeel);

    // Title & Header Labels
    titleLabel.setText ("AETHER // UI", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (18.0f, juce::Font::bold));
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xff00f0ff));
    addAndMakeVisible (titleLabel);

    subTitleLabel.setText ("MICRO-TACTILE SYNTHESIZER", juce::dontSendNotification);
    subTitleLabel.setFont (juce::Font (10.0f, juce::Font::plain));
    subTitleLabel.setColour (juce::Label::textColourId, juce::Colour (0xff798394));
    addAndMakeVisible (subTitleLabel);

    // Preset ComboBox
    presetLabel.setText ("PRESET", juce::dontSendNotification);
    presetLabel.setFont (juce::Font (10.0f, juce::Font::bold));
    presetLabel.setColour (juce::Label::textColourId, juce::Colour (0xff798394));
    addAndMakeVisible (presetLabel);

    const auto& presets = AetherUI::getFactoryPresets();
    for (int i = 0; i < static_cast<int> (presets.size()); ++i)
    {
        presetBox.addItem (juce::String (i + 1) + ". " + presets[static_cast<size_t> (i)].name, i + 1);
    }
    presetBox.setSelectedId (audioProcessor.getCurrentPresetIndex() + 1, juce::dontSendNotification);
    presetBox.onChange = [this]()
    {
        const int id = presetBox.getSelectedId() - 1;
        if (id >= 0)
            audioProcessor.loadPreset (id);
    };
    addAndMakeVisible (presetBox);

    // Preset & Randomize Action Buttons
    btnRandomize.setTooltip ("Randomize all synthesis parameters");
    btnRandomize.onClick = [this]()
    {
        audioProcessor.randomizeAllParameters();
    };
    addAndMakeVisible (btnRandomize);

    btnSavePreset.setTooltip ("Save current sound to .aetherpreset file");
    btnSavePreset.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser> (
            "Save AetherUI Preset",
            juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("Preset.aetherpreset"),
            "*.aetherpreset");

        const auto flags = juce::FileBrowserComponent::saveMode
                         | juce::FileBrowserComponent::canSelectFiles
                         | juce::FileBrowserComponent::warnAboutOverwriting;

        fileChooser->launchAsync (flags, [this] (const juce::FileChooser& fc)
        {
            const auto result = fc.getResult();
            if (result != juce::File{})
                audioProcessor.savePresetToFile (result);
        });
    };
    addAndMakeVisible (btnSavePreset);

    btnLoadPreset.setTooltip ("Load .aetherpreset file");
    btnLoadPreset.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser> (
            "Load AetherUI Preset",
            juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
            "*.aetherpreset");

        const auto flags = juce::FileBrowserComponent::openMode
                         | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync (flags, [this] (const juce::FileChooser& fc)
        {
            const auto result = fc.getResult();
            if (result.existsAsFile())
                audioProcessor.loadPresetFromFile (result);
        });
    };
    addAndMakeVisible (btnLoadPreset);

    // Polyphony Voice Count Label
    voiceCountLabel.setText ("VOICES: 0 / 16", juce::dontSendNotification);
    voiceCountLabel.setFont (juce::Font (11.0f, juce::Font::bold));
    voiceCountLabel.setColour (juce::Label::textColourId, juce::Colour (0xff38ef7d));
    voiceCountLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (voiceCountLabel);

    // Master Controls
    initRotary (sMasterGain,   "master_gain",    "MASTER", " dB");
    initRotary (sStereoWidth,  "stereo_width",   "WIDTH",  "x");
    initRotary (sVelocitySens, "velocity_sens",  "VEL SENS", "");
    initRotary (sMasterPan,    "master_pan",     "PAN",    "");

    // Transient Section Controls
    initToggle (tTransEnable,      "trans_enable", "ACTIVE");
    initCombo  (cTransType,        "trans_type",   "TYPE", { "Dirac", "White", "Pink", "Crackle" });
    initRotary (sTransDecay,       "trans_decay_ms", "DECAY", " ms");
    initCombo  (cTransFilterType,  "trans_filter_type", "FILTER", { "Bandpass", "Highpass", "Lowpass" });
    initRotary (sTransFilterFreq,  "trans_filter_freq", "CUTOFF", " Hz");
    initRotary (sTransFilterQ,     "trans_filter_q",    "RES (Q)", "");
    initRotary (sTransLevel,       "trans_level",       "LEVEL", "");

    // FM Section Controls
    initToggle (tFmEnable,         "fm_enable", "ACTIVE");
    initRotary (sFmRatio,          "fm_ratio",            "RATIO", "x");
    initRotary (sFmDepth,          "fm_depth",            "DEPTH", "");
    initRotary (sFmDecay,          "fm_decay_ms",         "FM DECAY", " ms");
    initRotary (sFmFeedback,       "fm_feedback",         "FEEDBACK", "");
    initRotary (sFmCarrierDecay,   "fm_carrier_decay_ms", "AMP DECAY", " ms");
    initRotary (sFmPitchEnvDepth,  "fm_pitch_env_depth",  "PITCH DEPTH", " st");
    initRotary (sFmPitchEnvDecay,  "fm_pitch_env_decay_ms","PITCH DECAY", " ms");
    initRotary (sFmLevel,          "fm_level",            "LEVEL", "");

    // Resonator Section Controls
    initToggle (tResEnable,        "res_enable", "ACTIVE");
    initCombo  (cResTuneMode,      "res_tune_mode", "MODE", { "Track MIDI", "Fixed Pitch" });
    initRotary (sResFreqOffset,    "res_freq_offset_semi", "TUNE", " st");
    initRotary (sResDamping,       "res_damping",          "DAMPING", "");
    initRotary (sResFeedback,      "res_feedback",         "RING", "");
    initRotary (sResMix,           "res_mix",              "MIX", "");

    // Shaper Section Controls
    initRotary (sShaperDrive,      "shaper_drive", "DRIVE", " dB");
    initCombo  (cShaperType,       "shaper_type",  "SHAPER", { "Tanh Soft", "Polynomial" });

    // Envelope Tabs & Visualizer
    btnEnvGlobal.onClick = [this] { envVisualizer.setMode (0); updateEnvTabStyles(); };
    btnEnvTrans.onClick  = [this] { envVisualizer.setMode (1); updateEnvTabStyles(); };
    btnEnvFm.onClick     = [this] { envVisualizer.setMode (2); updateEnvTabStyles(); };
    btnEnvPitch.onClick  = [this] { envVisualizer.setMode (3); updateEnvTabStyles(); };
    addAndMakeVisible (btnEnvGlobal);
    addAndMakeVisible (btnEnvTrans);
    addAndMakeVisible (btnEnvFm);
    addAndMakeVisible (btnEnvPitch);
    addAndMakeVisible (envVisualizer);
    updateEnvTabStyles();

    // Envelope Rotaries
    initRotary (sEnvAttack,   "env_attack_ms",   "AMP ATT",  " ms");
    initRotary (sEnvDecay,    "env_decay_ms",    "AMP DEC",  " ms");
    initRotary (sEnvSustain,  "env_sustain",     "AMP SUS",  "");
    initRotary (sEnvRelease,  "env_release_ms",  "AMP REL",  " ms");
    initRotary (sTransAttack, "trans_attack_ms", "TRN ATT",  " ms");
    initRotary (sFmAttack,    "fm_attack_ms",    "FM ATT",   " ms");

    // Output FX: Bitcrusher
    initToggle (tCrushEnable,     "crush_enable", "CRUSH");
    initRotary (sCrushBits,       "crush_bits",       "BITS", "");
    initRotary (sCrushDownsample, "crush_downsample", "DOWNSMP", "x");
    initRotary (sCrushMix,        "crush_mix",        "MIX", "");

    // Output FX: Stereo Delay
    initToggle (tDelayEnable,     "delay_enable", "DELAY");
    initRotary (sDelayTime,       "delay_time_ms",  "TIME", " ms");
    initRotary (sDelayFeedback,   "delay_feedback", "FDBK", "");
    initRotary (sDelayDamping,    "delay_damping",  "DAMP", "");
    initRotary (sDelayPingPong,   "delay_pingpong", "P-PONG", "");
    initRotary (sDelayMix,        "delay_mix",      "MIX", "");

    // Output FX: Reverb
    initToggle (tReverbEnable,    "reverb_enable", "REVERB");
    initRotary (sReverbSize,      "reverb_size",    "SIZE", "");
    initRotary (sReverbDamping,   "reverb_damping", "DAMP", "");
    initRotary (sReverbWidth,     "reverb_width",   "WIDTH", "");
    initRotary (sReverbMix,       "reverb_mix",     "MIX", "");

    // Visualizer
    addAndMakeVisible (visualizer);

    setSize (1120, 780);
    startTimerHz (30);
}

AetherUIAudioProcessorEditor::~AetherUIAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void AetherUIAudioProcessorEditor::updateEnvTabStyles()
{
    const int mode = envVisualizer.getMode();
    auto styleBtn = [] (juce::TextButton& btn, bool active)
    {
        btn.setColour (juce::TextButton::buttonColourId, active ? juce::Colour (0xff252a34) : juce::Colour (0xff14161b));
        btn.setColour (juce::TextButton::textColourOnId,  active ? juce::Colour (0xff00f0ff) : juce::Colour (0xff798394));
        btn.setColour (juce::TextButton::textColourOffId, active ? juce::Colour (0xff00f0ff) : juce::Colour (0xff798394));
    };

    styleBtn (btnEnvGlobal, mode == 0);
    styleBtn (btnEnvTrans,  mode == 1);
    styleBtn (btnEnvFm,     mode == 2);
    styleBtn (btnEnvPitch,  mode == 3);
}

void AetherUIAudioProcessorEditor::initRotary (AttachedRotary& r, const juce::String& paramId,
                                              const juce::String& name, const juce::String& suffix)
{
    r.slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    r.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 16);
    r.slider.setTextValueSuffix (suffix);
    r.slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe2e8f0));
    r.slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (r.slider);

    r.label.setText (name, juce::dontSendNotification);
    r.label.setFont (juce::Font (10.0f, juce::Font::bold));
    r.label.setJustificationType (juce::Justification::centred);
    r.label.setColour (juce::Label::textColourId, juce::Colour (0xff828a99));
    addAndMakeVisible (r.label);

    r.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.getAPVTS(), paramId, r.slider);
}

void AetherUIAudioProcessorEditor::initCombo (AttachedCombo& c, const juce::String& paramId,
                                             const juce::String& name, const juce::StringArray& items)
{
    for (int i = 0; i < items.size(); ++i)
        c.box.addItem (items[i], i + 1);

    c.box.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (c.box);

    c.label.setText (name, juce::dontSendNotification);
    c.label.setFont (juce::Font (10.0f, juce::Font::bold));
    c.label.setJustificationType (juce::Justification::centredLeft);
    c.label.setColour (juce::Label::textColourId, juce::Colour (0xff828a99));
    addAndMakeVisible (c.label);

    c.attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.getAPVTS(), paramId, c.box);
}

void AetherUIAudioProcessorEditor::initToggle (AttachedToggle& t, const juce::String& paramId,
                                              const juce::String& text)
{
    t.button.setButtonText (text);
    addAndMakeVisible (t.button);

    t.attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.getAPVTS(), paramId, t.button);
}

void AetherUIAudioProcessorEditor::timerCallback()
{
    // Update active polyphony count
    const int activeVoices = audioProcessor.getActiveVoiceCount();
    voiceCountLabel.setText ("VOICES: " + juce::String (activeVoices) + " / 16", juce::dontSendNotification);
    voiceCountLabel.setColour (juce::Label::textColourId,
                               activeVoices > 0 ? juce::Colour (0xff00f0ff) : juce::Colour (0xff4b5563));

    // Keep preset dropdown or custom name synchronized
    const int currentPreset = audioProcessor.getCurrentPresetIndex();
    const juce::String displayName = audioProcessor.getCurrentPresetDisplayName();

    if (displayName.startsWith ("Randomized") || displayName != audioProcessor.getProgramName (currentPreset))
    {
        presetLabel.setText ("CUSTOM", juce::dontSendNotification);
        presetBox.setText (displayName, juce::dontSendNotification);
    }
    else
    {
        presetLabel.setText ("PRESET", juce::dontSendNotification);
        if (presetBox.getSelectedId() != currentPreset + 1)
        {
            presetBox.setSelectedId (currentPreset + 1, juce::dontSendNotification);
        }
    }

    envVisualizer.repaint();
}

void AetherUIAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff111215));

    // Top Header background bar
    g.setColour (juce::Colour (0xff17191e));
    g.fillRect (0, 0, getWidth(), 64);

    g.setColour (juce::Colour (0xff252932));
    g.drawHorizontalLine (64, 0.0f, static_cast<float> (getWidth()));

    // Draw Section Cards
    auto drawCard = [&g] (const juce::Rectangle<int>& area, const juce::String& title)
    {
        g.setColour (juce::Colour (0xff17191e));
        g.fillRoundedRectangle (area.toFloat(), 6.0f);

        g.setColour (juce::Colour (0xff252932));
        g.drawRoundedRectangle (area.toFloat(), 6.0f, 1.0f);

        // Card header line
        g.setColour (juce::Colour (0xff00f0ff));
        g.fillRect (area.getX() + 14, area.getY() + 13, 3, 11);

        g.setColour (juce::Colour (0xffc2c9d6));
        g.setFont (juce::Font (11.0f, juce::Font::bold));
        g.drawText (title, area.getX() + 24, area.getY() + 10, area.getWidth() - 30, 16,
                    juce::Justification::centredLeft);

        g.setColour (juce::Colour (0xff22252d));
        g.drawHorizontalLine (area.getY() + 34, static_cast<float> (area.getX()), static_cast<float> (area.getRight()));
    };

    // Card 1: Transient
    drawCard (juce::Rectangle<int> (16, 76, 320, 340), "01 // TRANSIENT IMPULSE");

    // Card 2: FM Engine
    drawCard (juce::Rectangle<int> (350, 76, 450, 340), "02 // DUAL-OP FM ENGINE");

    // Card 3: Resonator
    drawCard (juce::Rectangle<int> (814, 76, 290, 340), "03 // MODAL RESONATOR");

    // Card 4: Envelopes
    drawCard (juce::Rectangle<int> (16, 430, 370, 334), "04 // ENVELOPE MODULATOR");

    // Card 5: Output FX & Shaper
    drawCard (juce::Rectangle<int> (400, 430, 450, 334), "05 // OUTPUT FX & DYNAMICS");

    // Sub-dividers and section tags in Card 5
    g.setColour (juce::Colour (0xff20242c));
    g.drawHorizontalLine (536, 400.0f, 850.0f);
    g.drawHorizontalLine (636, 400.0f, 850.0f);

    g.setColour (juce::Colour (0xff656d7c));
    g.setFont (juce::Font (9.0f, juce::Font::bold));
    g.drawText ("SHAPER & CRUSHER", 414, 440, 150, 14, juce::Justification::centredLeft);
    g.drawText ("PING-PONG DELAY", 414, 542, 150, 14, juce::Justification::centredLeft);
    g.drawText ("SPATIAL REVERB", 414, 642, 150, 14, juce::Justification::centredLeft);

    // Card 6: Realtime Scope
    drawCard (juce::Rectangle<int> (864, 430, 240, 334), "06 // MICRO-OSCILLOSCOPE");
}

void AetherUIAudioProcessorEditor::resized()
{
    // Header layout
    titleLabel.setBounds (18, 12, 160, 24);
    subTitleLabel.setBounds (20, 36, 160, 16);

    presetLabel.setBounds (195, 22, 50, 20);
    presetBox.setBounds (250, 18, 180, 28);

    btnRandomize.setBounds (438, 18, 48, 28);
    btnSavePreset.setBounds (492, 18, 48, 28);
    btnLoadPreset.setBounds (546, 18, 48, 28);

    voiceCountLabel.setBounds (605, 22, 105, 20);

    // Master Header Mini-Controls
    const int masterY = 8;
    sMasterPan.label.setBounds    (730, masterY, 44, 12);
    sMasterPan.slider.setBounds   (730, masterY + 12, 44, 40);

    sStereoWidth.label.setBounds  (784, masterY, 48, 12);
    sStereoWidth.slider.setBounds (784, masterY + 12, 48, 40);

    sVelocitySens.label.setBounds (842, masterY, 60, 12);
    sVelocitySens.slider.setBounds(842, masterY + 12, 60, 40);

    sMasterGain.label.setBounds   (912, masterY, 64, 12);
    sMasterGain.slider.setBounds  (912, masterY + 12, 64, 40);

    // =========================================================================
    // SECTION 1: TRANSIENT IMPULSE (x: 16, y: 76, w: 320, h: 340)
    // =========================================================================
    tTransEnable.button.setBounds (252, 82, 70, 20);

    cTransType.label.setBounds (30, 118, 50, 16);
    cTransType.box.setBounds   (90, 116, 140, 24);

    cTransFilterType.label.setBounds (30, 154, 50, 16);
    cTransFilterType.box.setBounds   (90, 152, 140, 24);

    const int c1_row1_y = 195;
    const int c1_row2_y = 295;

    sTransDecay.label.setBounds      (45, c1_row1_y, 70, 14);
    sTransDecay.slider.setBounds     (45, c1_row1_y + 16, 70, 72);

    sTransFilterFreq.label.setBounds (185, c1_row1_y, 70, 14);
    sTransFilterFreq.slider.setBounds(185, c1_row1_y + 16, 70, 72);

    sTransFilterQ.label.setBounds    (45, c1_row2_y, 70, 14);
    sTransFilterQ.slider.setBounds   (45, c1_row2_y + 16, 70, 72);

    sTransLevel.label.setBounds      (185, c1_row2_y, 70, 14);
    sTransLevel.slider.setBounds     (185, c1_row2_y + 16, 70, 72);

    // =========================================================================
    // SECTION 2: DUAL-OP FM ENGINE (x: 350, y: 76, w: 450, h: 340)
    // =========================================================================
    tFmEnable.button.setBounds (714, 82, 70, 20);

    const int fm_x0 = 362;
    const int fm_spacing_x = 104;
    const int fm_row1_y = 125;
    const int fm_row2_y = 245;

    sFmRatio.label.setBounds            (fm_x0, fm_row1_y, 72, 14);
    sFmRatio.slider.setBounds           (fm_x0, fm_row1_y + 16, 72, 72);

    sFmDepth.label.setBounds            (fm_x0 + fm_spacing_x, fm_row1_y, 72, 14);
    sFmDepth.slider.setBounds           (fm_x0 + fm_spacing_x, fm_row1_y + 16, 72, 72);

    sFmDecay.label.setBounds            (fm_x0 + fm_spacing_x * 2, fm_row1_y, 72, 14);
    sFmDecay.slider.setBounds           (fm_x0 + fm_spacing_x * 2, fm_row1_y + 16, 72, 72);

    sFmFeedback.label.setBounds         (fm_x0 + fm_spacing_x * 3, fm_row1_y, 72, 14);
    sFmFeedback.slider.setBounds        (fm_x0 + fm_spacing_x * 3, fm_row1_y + 16, 72, 72);

    sFmPitchEnvDepth.label.setBounds    (fm_x0, fm_row2_y, 76, 14);
    sFmPitchEnvDepth.slider.setBounds   (fm_x0, fm_row2_y + 16, 76, 72);

    sFmPitchEnvDecay.label.setBounds    (fm_x0 + fm_spacing_x, fm_row2_y, 76, 14);
    sFmPitchEnvDecay.slider.setBounds   (fm_x0 + fm_spacing_x, fm_row2_y + 16, 76, 72);

    sFmCarrierDecay.label.setBounds     (fm_x0 + fm_spacing_x * 2, fm_row2_y, 72, 14);
    sFmCarrierDecay.slider.setBounds    (fm_x0 + fm_spacing_x * 2, fm_row2_y + 16, 72, 72);

    sFmLevel.label.setBounds           (fm_x0 + fm_spacing_x * 3, fm_row2_y, 72, 14);
    sFmLevel.slider.setBounds          (fm_x0 + fm_spacing_x * 3, fm_row2_y + 16, 72, 72);

    // =========================================================================
    // SECTION 3: MODAL RESONATOR (x: 814, y: 76, w: 290, h: 340)
    // =========================================================================
    tResEnable.button.setBounds (1020, 82, 70, 20);

    cResTuneMode.label.setBounds (830, 120, 45, 16);
    cResTuneMode.box.setBounds   (880, 118, 140, 24);

    const int res_row1_y = 175;
    const int res_row2_y = 275;

    sResFreqOffset.label.setBounds (835, res_row1_y, 68, 14);
    sResFreqOffset.slider.setBounds(835, res_row1_y + 16, 68, 70);

    sResDamping.label.setBounds    (955, res_row1_y, 68, 14);
    sResDamping.slider.setBounds   (955, res_row1_y + 16, 68, 70);

    sResFeedback.label.setBounds   (835, res_row2_y, 68, 14);
    sResFeedback.slider.setBounds  (835, res_row2_y + 16, 68, 70);

    sResMix.label.setBounds        (955, res_row2_y, 68, 14);
    sResMix.slider.setBounds       (955, res_row2_y + 16, 68, 70);

    // =========================================================================
    // SECTION 4: ENVELOPE MODULATOR (x: 16, y: 430, w: 370, h: 334)
    // =========================================================================
    btnEnvGlobal.setBounds (150, 436, 52, 18);
    btnEnvTrans.setBounds  (204, 436, 52, 18);
    btnEnvFm.setBounds     (258, 436, 52, 18);
    btnEnvPitch.setBounds  (312, 436, 52, 18);

    envVisualizer.setBounds (28, 468, 346, 136);

    const int env_row1_y = 614;
    sEnvAttack.label.setBounds  (30, env_row1_y, 72, 12);
    sEnvAttack.slider.setBounds (30, env_row1_y + 14, 72, 54);

    sEnvDecay.label.setBounds   (116, env_row1_y, 72, 12);
    sEnvDecay.slider.setBounds  (116, env_row1_y + 14, 72, 54);

    sEnvSustain.label.setBounds (202, env_row1_y, 72, 12);
    sEnvSustain.slider.setBounds(202, env_row1_y + 14, 72, 54);

    sEnvRelease.label.setBounds (288, env_row1_y, 72, 12);
    sEnvRelease.slider.setBounds(288, env_row1_y + 14, 72, 54);

    const int env_row2_y = 690;
    sTransAttack.label.setBounds  (65, env_row2_y, 100, 12);
    sTransAttack.slider.setBounds (65, env_row2_y + 14, 100, 52);

    sFmAttack.label.setBounds     (215, env_row2_y, 100, 12);
    sFmAttack.slider.setBounds    (215, env_row2_y + 14, 100, 52);

    // =========================================================================
    // SECTION 5: OUTPUT FX & DYNAMICS (x: 400, y: 430, w: 450, h: 334)
    // =========================================================================
    // Strip 1: Shaper & Crusher
    cShaperType.box.setBounds          (414, 480, 95, 22);
    sShaperDrive.label.setBounds       (514, 462, 50, 12);
    sShaperDrive.slider.setBounds      (514, 474, 50, 54);

    tCrushEnable.button.setBounds      (574, 480, 60, 20);
    sCrushBits.label.setBounds         (638, 462, 50, 12);
    sCrushBits.slider.setBounds        (638, 474, 50, 54);
    sCrushDownsample.label.setBounds   (698, 462, 58, 12);
    sCrushDownsample.slider.setBounds  (698, 474, 58, 54);
    sCrushMix.label.setBounds          (766, 462, 50, 12);
    sCrushMix.slider.setBounds         (766, 474, 50, 54);

    // Strip 2: Delay
    tDelayEnable.button.setBounds      (414, 576, 58, 20);
    sDelayTime.label.setBounds         (476, 554, 54, 12);
    sDelayTime.slider.setBounds        (476, 566, 54, 54);
    sDelayFeedback.label.setBounds     (540, 554, 54, 12);
    sDelayFeedback.slider.setBounds    (540, 566, 54, 54);
    sDelayDamping.label.setBounds      (604, 554, 54, 12);
    sDelayDamping.slider.setBounds     (604, 566, 54, 54);
    sDelayPingPong.label.setBounds     (668, 554, 54, 12);
    sDelayPingPong.slider.setBounds    (668, 566, 54, 54);
    sDelayMix.label.setBounds          (732, 554, 54, 12);
    sDelayMix.slider.setBounds         (732, 566, 54, 54);

    // Strip 3: Reverb
    tReverbEnable.button.setBounds     (414, 676, 68, 20);
    sReverbSize.label.setBounds        (492, 654, 60, 12);
    sReverbSize.slider.setBounds       (492, 666, 60, 56);
    sReverbDamping.label.setBounds     (564, 654, 60, 12);
    sReverbDamping.slider.setBounds    (564, 666, 60, 56);
    sReverbWidth.label.setBounds       (636, 654, 60, 12);
    sReverbWidth.slider.setBounds      (636, 666, 60, 56);
    sReverbMix.label.setBounds         (708, 654, 60, 12);
    sReverbMix.slider.setBounds        (708, 666, 60, 56);

    // =========================================================================
    // SECTION 6: REALTIME SCOPE (x: 864, y: 430, w: 240, h: 334)
    // =========================================================================
    visualizer.setBounds (876, 468, 216, 276);
}
