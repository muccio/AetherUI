#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "SynthEngine.h"
#include "Presets.h"
#include <atomic>

//==============================================================================
class AetherUIAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int kVisualizerFifoSize = 2048;

    AetherUIAudioProcessor();
    ~AetherUIAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts; }
    int getActiveVoiceCount() const noexcept { return synthEngine.getActiveVoices(); }

    void loadPreset (int presetIndex);
    int getCurrentPresetIndex() const noexcept { return currentPresetIndex.load (std::memory_order_relaxed); }

    // Randomization & File Presets
    void randomizeAllParameters();
    bool savePresetToFile (const juce::File& file, const juce::String& presetName = {});
    bool loadPresetFromFile (const juce::File& file, juce::String* loadedPresetName = nullptr);
    juce::String getCurrentPresetDisplayName() const;

    // Waveform visualizer FIFO interface
    juce::AbstractFifo& getVisualFifo() noexcept { return visualFifo; }
    const std::array<float, kVisualizerFifoSize>& getVisualBuffer() const noexcept { return visualBuffer; }

private:
    juce::String customPresetName;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateEngineParameters() noexcept;
    void pushVisualizerSamples (const float* samples, int numSamples) noexcept;

    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    AetherUI::SynthEngine synthEngine;
    AetherUI::SynthParams cachedParams;

    std::atomic<int> currentPresetIndex { 0 };
    std::atomic<int> pendingPresetChange { -1 };

    // Lock-free visualization ring buffer
    juce::AbstractFifo visualFifo { kVisualizerFifoSize };
    std::array<float, kVisualizerFifoSize> visualBuffer {};

    // Cached raw parameter pointers for allocation-free real-time reading
    std::atomic<float>* pMasterGain = nullptr;
    std::atomic<float>* pMasterPan = nullptr;
    std::atomic<float>* pStereoWidth = nullptr;
    std::atomic<float>* pVelocitySens = nullptr;

    std::atomic<float>* pTransEnable = nullptr;
    std::atomic<float>* pTransType = nullptr;
    std::atomic<float>* pTransDecayMs = nullptr;
    std::atomic<float>* pTransFilterType = nullptr;
    std::atomic<float>* pTransFilterFreq = nullptr;
    std::atomic<float>* pTransFilterQ = nullptr;
    std::atomic<float>* pTransLevel = nullptr;

    std::atomic<float>* pFmEnable = nullptr;
    std::atomic<float>* pFmRatio = nullptr;
    std::atomic<float>* pFmDepth = nullptr;
    std::atomic<float>* pFmDecayMs = nullptr;
    std::atomic<float>* pFmFeedback = nullptr;
    std::atomic<float>* pFmCarrierDecayMs = nullptr;
    std::atomic<float>* pFmPitchEnvDepth = nullptr;
    std::atomic<float>* pFmPitchEnvDecayMs = nullptr;
    std::atomic<float>* pFmLevel = nullptr;

    std::atomic<float>* pResEnable = nullptr;
    std::atomic<float>* pResTuneMode = nullptr;
    std::atomic<float>* pResFreqOffsetSemi = nullptr;
    std::atomic<float>* pResDamping = nullptr;
    std::atomic<float>* pResFeedback = nullptr;
    std::atomic<float>* pResMix = nullptr;

    std::atomic<float>* pShaperDrive = nullptr;
    std::atomic<float>* pShaperType = nullptr;

    // Envelopes
    std::atomic<float>* pEnvAttackMs = nullptr;
    std::atomic<float>* pEnvDecayMs = nullptr;
    std::atomic<float>* pEnvSustain = nullptr;
    std::atomic<float>* pEnvReleaseMs = nullptr;
    std::atomic<float>* pTransAttackMs = nullptr;
    std::atomic<float>* pFmAttackMs = nullptr;

    // Output FX: Bitcrusher
    std::atomic<float>* pCrushEnable = nullptr;
    std::atomic<float>* pCrushBits = nullptr;
    std::atomic<float>* pCrushDownsample = nullptr;
    std::atomic<float>* pCrushMix = nullptr;

    // Output FX: Stereo Delay
    std::atomic<float>* pDelayEnable = nullptr;
    std::atomic<float>* pDelayTimeMs = nullptr;
    std::atomic<float>* pDelayFeedback = nullptr;
    std::atomic<float>* pDelayDamping = nullptr;
    std::atomic<float>* pDelayPingPong = nullptr;
    std::atomic<float>* pDelayMix = nullptr;

    // Output FX: Reverb
    std::atomic<float>* pReverbEnable = nullptr;
    std::atomic<float>* pReverbSize = nullptr;
    std::atomic<float>* pReverbDamping = nullptr;
    std::atomic<float>* pReverbWidth = nullptr;
    std::atomic<float>* pReverbMix = nullptr;

    std::atomic<float>* pPresetIndex = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AetherUIAudioProcessor)
};
