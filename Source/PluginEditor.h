#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include <vector>

namespace AetherUI
{

//==============================================================================
/** Minimalist Figma / Teenage Engineering dark aesthetic LookAndFeel. */
class ModernSkinLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernSkinLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override;

    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override;

    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override;
    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                            bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                            const juce::String& text, const juce::String& shortcutKeyText,
                            const juce::Drawable* icon, const juce::Colour* textColour) override;

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                                bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool shouldDrawButtonAsHighlighted,
                         bool shouldDrawButtonAsDown) override;
};

//==============================================================================
/** Real-time oscilloscope vector component displaying micro-transient waveforms. */
class WaveformVisualizer : public juce::Component, public juce::Timer
{
public:
    WaveformVisualizer (AetherUIAudioProcessor& proc);
    ~WaveformVisualizer() override;

    void paint (juce::Graphics& g) override;
    void timerCallback() override;

private:
    AetherUIAudioProcessor& processor;
    static constexpr int kDisplayPoints = 512;
    std::array<float, kDisplayPoints> displayBuffer {};
    float peakLevel = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformVisualizer)
};

//==============================================================================
/** Interactive vector envelope graph displaying ADSR and module contours. */
class EnvelopeVisualizer : public juce::Component
{
public:
    EnvelopeVisualizer (AetherUIAudioProcessor& proc);
    ~EnvelopeVisualizer() override = default;

    void setMode (int newMode)
    {
        currentMode = newMode;
        repaint();
    }

    int getMode() const noexcept { return currentMode; }

    void paint (juce::Graphics& g) override;

private:
    AetherUIAudioProcessor& processor;
    int currentMode = 0; // 0: Global ADSR, 1: Transient, 2: FM, 3: Pitch

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeVisualizer)
};

} // namespace AetherUI

//==============================================================================
class AetherUIAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit AetherUIAudioProcessorEditor (AetherUIAudioProcessor&);
    ~AetherUIAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AetherUIAudioProcessor& audioProcessor;
    AetherUI::ModernSkinLookAndFeel customLookAndFeel;

    // Visualizers
    AetherUI::WaveformVisualizer visualizer;
    AetherUI::EnvelopeVisualizer envVisualizer;

    // Header controls
    juce::ComboBox presetBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> presetAttachment;
    juce::Label presetLabel;
    juce::Label titleLabel;
    juce::Label subTitleLabel;
    juce::Label voiceCountLabel;

    // Preset & Randomize Action Buttons
    juce::TextButton btnRandomize { "RND" };
    juce::TextButton btnSavePreset { "SAVE" };
    juce::TextButton btnLoadPreset { "LOAD" };
    std::unique_ptr<juce::FileChooser> fileChooser;

    // Envelope Mode Tabs
    juce::TextButton btnEnvGlobal { "AMP ADSR" };
    juce::TextButton btnEnvTrans  { "TRANSIENT" };
    juce::TextButton btnEnvFm     { "FM MOD" };
    juce::TextButton btnEnvPitch  { "PITCH" };
    void updateEnvTabStyles();

    // Helper for creating attached rotary sliders
    struct AttachedRotary
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    struct AttachedToggle
    {
        juce::ToggleButton button;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
    };

    struct AttachedCombo
    {
        juce::ComboBox box;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
    };

    // Master
    AttachedRotary sMasterGain;
    AttachedRotary sStereoWidth;
    AttachedRotary sVelocitySens;
    AttachedRotary sMasterPan;

    // Transient Section
    AttachedToggle tTransEnable;
    AttachedCombo  cTransType;
    AttachedRotary sTransDecay;
    AttachedCombo  cTransFilterType;
    AttachedRotary sTransFilterFreq;
    AttachedRotary sTransFilterQ;
    AttachedRotary sTransLevel;

    // FM Section
    AttachedToggle tFmEnable;
    AttachedRotary sFmRatio;
    AttachedRotary sFmDepth;
    AttachedRotary sFmDecay;
    AttachedRotary sFmFeedback;
    AttachedRotary sFmCarrierDecay;
    AttachedRotary sFmPitchEnvDepth;
    AttachedRotary sFmPitchEnvDecay;
    AttachedRotary sFmLevel;

    // Resonator Section
    AttachedToggle tResEnable;
    AttachedCombo  cResTuneMode;
    AttachedRotary sResFreqOffset;
    AttachedRotary sResDamping;
    AttachedRotary sResFeedback;
    AttachedRotary sResMix;

    // Shaper Section
    AttachedRotary sShaperDrive;
    AttachedCombo  cShaperType;

    // Envelopes
    AttachedRotary sEnvAttack;
    AttachedRotary sEnvDecay;
    AttachedRotary sEnvSustain;
    AttachedRotary sEnvRelease;
    AttachedRotary sTransAttack;
    AttachedRotary sFmAttack;

    // Output FX: Bitcrusher
    AttachedToggle tCrushEnable;
    AttachedRotary sCrushBits;
    AttachedRotary sCrushDownsample;
    AttachedRotary sCrushMix;

    // Output FX: Stereo Delay
    AttachedToggle tDelayEnable;
    AttachedRotary sDelayTime;
    AttachedRotary sDelayFeedback;
    AttachedRotary sDelayDamping;
    AttachedRotary sDelayPingPong;
    AttachedRotary sDelayMix;

    // Output FX: Reverb
    AttachedToggle tReverbEnable;
    AttachedRotary sReverbSize;
    AttachedRotary sReverbDamping;
    AttachedRotary sReverbWidth;
    AttachedRotary sReverbMix;

    void initRotary (AttachedRotary& r, const juce::String& paramId, const juce::String& name, const juce::String& suffix = "");
    void initCombo (AttachedCombo& c, const juce::String& paramId, const juce::String& name, const juce::StringArray& items);
    void initToggle (AttachedToggle& t, const juce::String& paramId, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AetherUIAudioProcessorEditor)
};
