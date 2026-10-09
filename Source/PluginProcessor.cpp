#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AetherUIAudioProcessor::AetherUIAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    // Cache raw atomic pointers for allocation-free real-time reading
    pMasterGain         = apvts.getRawParameterValue ("master_gain");
    pMasterPan          = apvts.getRawParameterValue ("master_pan");
    pStereoWidth        = apvts.getRawParameterValue ("stereo_width");
    pVelocitySens       = apvts.getRawParameterValue ("velocity_sens");

    pTransEnable        = apvts.getRawParameterValue ("trans_enable");
    pTransType          = apvts.getRawParameterValue ("trans_type");
    pTransDecayMs       = apvts.getRawParameterValue ("trans_decay_ms");
    pTransFilterType    = apvts.getRawParameterValue ("trans_filter_type");
    pTransFilterFreq    = apvts.getRawParameterValue ("trans_filter_freq");
    pTransFilterQ       = apvts.getRawParameterValue ("trans_filter_q");
    pTransLevel         = apvts.getRawParameterValue ("trans_level");

    pFmEnable           = apvts.getRawParameterValue ("fm_enable");
    pFmRatio            = apvts.getRawParameterValue ("fm_ratio");
    pFmDepth            = apvts.getRawParameterValue ("fm_depth");
    pFmDecayMs          = apvts.getRawParameterValue ("fm_decay_ms");
    pFmFeedback         = apvts.getRawParameterValue ("fm_feedback");
    pFmCarrierDecayMs   = apvts.getRawParameterValue ("fm_carrier_decay_ms");
    pFmPitchEnvDepth    = apvts.getRawParameterValue ("fm_pitch_env_depth");
    pFmPitchEnvDecayMs  = apvts.getRawParameterValue ("fm_pitch_env_decay_ms");
    pFmLevel            = apvts.getRawParameterValue ("fm_level");

    pResEnable          = apvts.getRawParameterValue ("res_enable");
    pResTuneMode        = apvts.getRawParameterValue ("res_tune_mode");
    pResFreqOffsetSemi  = apvts.getRawParameterValue ("res_freq_offset_semi");
    pResDamping         = apvts.getRawParameterValue ("res_damping");
    pResFeedback        = apvts.getRawParameterValue ("res_feedback");
    pResMix             = apvts.getRawParameterValue ("res_mix");

    pShaperDrive        = apvts.getRawParameterValue ("shaper_drive");
    pShaperType         = apvts.getRawParameterValue ("shaper_type");

    pEnvAttackMs        = apvts.getRawParameterValue ("env_attack_ms");
    pEnvDecayMs         = apvts.getRawParameterValue ("env_decay_ms");
    pEnvSustain         = apvts.getRawParameterValue ("env_sustain");
    pEnvReleaseMs       = apvts.getRawParameterValue ("env_release_ms");
    pTransAttackMs      = apvts.getRawParameterValue ("trans_attack_ms");
    pFmAttackMs         = apvts.getRawParameterValue ("fm_attack_ms");

    pCrushEnable        = apvts.getRawParameterValue ("crush_enable");
    pCrushBits          = apvts.getRawParameterValue ("crush_bits");
    pCrushDownsample    = apvts.getRawParameterValue ("crush_downsample");
    pCrushMix           = apvts.getRawParameterValue ("crush_mix");

    pDelayEnable        = apvts.getRawParameterValue ("delay_enable");
    pDelayTimeMs        = apvts.getRawParameterValue ("delay_time_ms");
    pDelayFeedback      = apvts.getRawParameterValue ("delay_feedback");
    pDelayDamping       = apvts.getRawParameterValue ("delay_damping");
    pDelayPingPong      = apvts.getRawParameterValue ("delay_pingpong");
    pDelayMix           = apvts.getRawParameterValue ("delay_mix");

    pReverbEnable       = apvts.getRawParameterValue ("reverb_enable");
    pReverbSize         = apvts.getRawParameterValue ("reverb_size");
    pReverbDamping      = apvts.getRawParameterValue ("reverb_damping");
    pReverbWidth        = apvts.getRawParameterValue ("reverb_width");
    pReverbMix          = apvts.getRawParameterValue ("reverb_mix");

    pPresetIndex        = apvts.getRawParameterValue ("preset_index");

    updateEngineParameters();
}

AetherUIAudioProcessor::~AetherUIAudioProcessor() = default;

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout AetherUIAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Master
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "master_gain", 1 }, "Master Gain",
        juce::NormalisableRange<float> (-60.0f, 12.0f, 0.1f, 2.5f), -3.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "master_pan", 1 }, "Master Pan",
        juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "stereo_width", 1 }, "Stereo Width",
        juce::NormalisableRange<float> (0.0f, 2.0f, 0.01f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "velocity_sens", 1 }, "Velocity Sensitivity",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.75f));

    // Transient
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "trans_enable", 1 }, "Transient Enable", true));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "trans_type", 1 }, "Transient Type",
        juce::StringArray { "Dirac", "White Noise", "Pink Noise", "Crackle Dust" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "trans_decay_ms", 1 }, "Transient Decay",
        juce::NormalisableRange<float> (0.5f, 60.0f, 0.05f, 0.45f), 4.0f));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "trans_filter_type", 1 }, "Transient Filter Type",
        juce::StringArray { "Bandpass", "Highpass", "Lowpass" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "trans_filter_freq", 1 }, "Transient Filter Freq",
        juce::NormalisableRange<float> (100.0f, 16000.0f, 1.0f, 0.35f), 3800.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "trans_filter_q", 1 }, "Transient Filter Q",
        juce::NormalisableRange<float> (0.5f, 25.0f, 0.1f, 0.6f), 3.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "trans_level", 1 }, "Transient Level",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));

    // FM Engine
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "fm_enable", 1 }, "FM Enable", true));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_ratio", 1 }, "FM Ratio",
        juce::NormalisableRange<float> (0.25f, 16.0f, 0.01f, 0.6f), 2.41f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_depth", 1 }, "FM Depth",
        juce::NormalisableRange<float> (0.0f, 10.0f, 0.05f), 2.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_decay_ms", 1 }, "FM Mod Decay",
        juce::NormalisableRange<float> (1.0f, 2500.0f, 0.5f, 0.35f), 180.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_feedback", 1 }, "FM Feedback",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_carrier_decay_ms", 1 }, "FM Carrier Decay",
        juce::NormalisableRange<float> (5.0f, 3000.0f, 1.0f, 0.35f), 250.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_pitch_env_depth", 1 }, "FM Pitch Env Depth",
        juce::NormalisableRange<float> (-48.0f, 48.0f, 0.1f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_pitch_env_decay_ms", 1 }, "FM Pitch Env Decay",
        juce::NormalisableRange<float> (1.0f, 1000.0f, 0.5f, 0.35f), 25.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_level", 1 }, "FM Level",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));

    // Resonator
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "res_enable", 1 }, "Resonator Enable", true));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "res_tune_mode", 1 }, "Resonator Tune Mode",
        juce::StringArray { "Track MIDI", "Fixed Microtonal" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "res_freq_offset_semi", 1 }, "Resonator Offset (st)",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "res_damping", 1 }, "Resonator Damping",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.3f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "res_feedback", 1 }, "Resonator Feedback",
        juce::NormalisableRange<float> (0.0f, 0.99f, 0.01f), 0.6f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "res_mix", 1 }, "Resonator Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));

    // Shaper
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "shaper_drive", 1 }, "Shaper Drive (dB)",
        juce::NormalisableRange<float> (0.0f, 24.0f, 0.1f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "shaper_type", 1 }, "Shaper Type",
        juce::StringArray { "Tanh Soft", "Polynomial Hard" }, 0));

    // Envelopes (Global ADSR & Module Attacks)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "env_attack_ms", 1 }, "Amp Attack",
        juce::NormalisableRange<float> (0.05f, 2000.0f, 0.05f, 0.35f), 0.1f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "env_decay_ms", 1 }, "Amp Decay",
        juce::NormalisableRange<float> (1.0f, 4000.0f, 0.1f, 0.35f), 300.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "env_sustain", 1 }, "Amp Sustain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "env_release_ms", 1 }, "Amp Release",
        juce::NormalisableRange<float> (1.0f, 3000.0f, 0.1f, 0.35f), 120.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "trans_attack_ms", 1 }, "Transient Attack",
        juce::NormalisableRange<float> (0.01f, 30.0f, 0.01f, 0.35f), 0.05f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "fm_attack_ms", 1 }, "FM Attack",
        juce::NormalisableRange<float> (0.1f, 500.0f, 0.1f, 0.35f), 0.5f));

    // Output FX: Bitcrusher
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "crush_enable", 1 }, "Bitcrush Enable", false));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "crush_bits", 1 }, "Bitcrush Depth",
        juce::NormalisableRange<float> (4.0f, 16.0f, 0.1f), 16.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "crush_downsample", 1 }, "Bitcrush Downsample",
        juce::NormalisableRange<float> (1.0f, 32.0f, 1.0f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "crush_mix", 1 }, "Bitcrush Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

    // Output FX: Stereo Delay
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "delay_enable", 1 }, "Delay Enable", false));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delay_time_ms", 1 }, "Delay Time",
        juce::NormalisableRange<float> (1.0f, 800.0f, 0.5f, 0.4f), 140.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delay_feedback", 1 }, "Delay Feedback",
        juce::NormalisableRange<float> (0.0f, 0.95f, 0.01f), 0.4f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delay_damping", 1 }, "Delay Damping",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.3f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delay_pingpong", 1 }, "Delay Ping-Pong",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delay_mix", 1 }, "Delay Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

    // Output FX: Spatial Reverb
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "reverb_enable", 1 }, "Reverb Enable", false));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverb_size", 1 }, "Reverb Size",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverb_damping", 1 }, "Reverb Damping",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.4f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverb_width", 1 }, "Reverb Width",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "reverb_mix", 1 }, "Reverb Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

    // Presets selection
    juce::StringArray presetNames;
    for (const auto& pr : AetherUI::getFactoryPresets())
        presetNames.add (pr.name);

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "preset_index", 1 }, "Preset", presetNames, 0));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String AetherUIAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AetherUIAudioProcessor::acceptsMidi() const  { return true; }
bool AetherUIAudioProcessor::producesMidi() const { return false; }
bool AetherUIAudioProcessor::isMidiEffect() const { return false; }
double AetherUIAudioProcessor::getTailLengthSeconds() const { return 0.5; }

int AetherUIAudioProcessor::getNumPrograms()
{
    return static_cast<int> (AetherUI::getFactoryPresets().size());
}

int AetherUIAudioProcessor::getCurrentProgram()
{
    return getCurrentPresetIndex();
}

void AetherUIAudioProcessor::setCurrentProgram (int index)
{
    loadPreset (index);
}

const juce::String AetherUIAudioProcessor::getProgramName (int index)
{
    const auto& presets = AetherUI::getFactoryPresets();
    if (index >= 0 && index < static_cast<int> (presets.size()))
        return presets[static_cast<size_t> (index)].name;
    return {};
}

void AetherUIAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/) {}

//==============================================================================
void AetherUIAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synthEngine.prepare (sampleRate, samplesPerBlock);
    visualFifo.reset();
    std::fill (visualBuffer.begin(), visualBuffer.end(), 0.0f);
    updateEngineParameters();
}

void AetherUIAudioProcessor::releaseResources()
{
    synthEngine.reset();
}

bool AetherUIAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void AetherUIAudioProcessor::updateEngineParameters() noexcept
{
    // Master
    const float gainDb = pMasterGain != nullptr ? pMasterGain->load (std::memory_order_relaxed) : -3.0f;
    cachedParams.masterGain = juce::Decibels::decibelsToGain (gainDb);
    cachedParams.masterPan = pMasterPan != nullptr ? pMasterPan->load (std::memory_order_relaxed) : 0.0f;
    cachedParams.stereoWidth = pStereoWidth != nullptr ? pStereoWidth->load (std::memory_order_relaxed) : 1.0f;
    cachedParams.velocitySens = pVelocitySens != nullptr ? pVelocitySens->load (std::memory_order_relaxed) : 0.75f;

    // Transient
    cachedParams.transEnable = (pTransEnable != nullptr && pTransEnable->load (std::memory_order_relaxed) > 0.5f);
    cachedParams.transType = pTransType != nullptr ? static_cast<int> (pTransType->load (std::memory_order_relaxed)) : 0;
    cachedParams.transDecayMs = pTransDecayMs != nullptr ? pTransDecayMs->load (std::memory_order_relaxed) : 4.0f;
    cachedParams.transFilterType = pTransFilterType != nullptr ? static_cast<int> (pTransFilterType->load (std::memory_order_relaxed)) : 0;
    cachedParams.transFilterFreq = pTransFilterFreq != nullptr ? pTransFilterFreq->load (std::memory_order_relaxed) : 3800.0f;
    cachedParams.transFilterQ = pTransFilterQ != nullptr ? pTransFilterQ->load (std::memory_order_relaxed) : 3.5f;
    cachedParams.transLevel = pTransLevel != nullptr ? pTransLevel->load (std::memory_order_relaxed) : 0.8f;

    // FM
    cachedParams.fmEnable = (pFmEnable != nullptr && pFmEnable->load (std::memory_order_relaxed) > 0.5f);
    cachedParams.fmRatio = pFmRatio != nullptr ? pFmRatio->load (std::memory_order_relaxed) : 2.41f;
    cachedParams.fmDepth = pFmDepth != nullptr ? pFmDepth->load (std::memory_order_relaxed) : 2.0f;
    cachedParams.fmDecayMs = pFmDecayMs != nullptr ? pFmDecayMs->load (std::memory_order_relaxed) : 180.0f;
    cachedParams.fmFeedback = pFmFeedback != nullptr ? pFmFeedback->load (std::memory_order_relaxed) : 0.15f;
    cachedParams.fmCarrierDecayMs = pFmCarrierDecayMs != nullptr ? pFmCarrierDecayMs->load (std::memory_order_relaxed) : 250.0f;
    cachedParams.fmPitchEnvDepth = pFmPitchEnvDepth != nullptr ? pFmPitchEnvDepth->load (std::memory_order_relaxed) : 0.0f;
    cachedParams.fmPitchEnvDecayMs = pFmPitchEnvDecayMs != nullptr ? pFmPitchEnvDecayMs->load (std::memory_order_relaxed) : 25.0f;
    cachedParams.fmLevel = pFmLevel != nullptr ? pFmLevel->load (std::memory_order_relaxed) : 0.8f;

    // Resonator
    cachedParams.resEnable = (pResEnable != nullptr && pResEnable->load (std::memory_order_relaxed) > 0.5f);
    cachedParams.resTuneMode = pResTuneMode != nullptr ? static_cast<int> (pResTuneMode->load (std::memory_order_relaxed)) : 0;
    cachedParams.resFreqOffsetSemi = pResFreqOffsetSemi != nullptr ? pResFreqOffsetSemi->load (std::memory_order_relaxed) : 0.0f;
    cachedParams.resDamping = pResDamping != nullptr ? pResDamping->load (std::memory_order_relaxed) : 0.3f;
    cachedParams.resFeedback = pResFeedback != nullptr ? pResFeedback->load (std::memory_order_relaxed) : 0.6f;
    cachedParams.resMix = pResMix != nullptr ? pResMix->load (std::memory_order_relaxed) : 0.5f;

    // Shaper
    const float driveDb = pShaperDrive != nullptr ? pShaperDrive->load (std::memory_order_relaxed) : 0.0f;
    cachedParams.shaperDrive = juce::Decibels::decibelsToGain (driveDb);
    cachedParams.shaperType = pShaperType != nullptr ? static_cast<int> (pShaperType->load (std::memory_order_relaxed)) : 0;

    // Envelopes
    cachedParams.envAttackMs = pEnvAttackMs != nullptr ? pEnvAttackMs->load (std::memory_order_relaxed) : 0.1f;
    cachedParams.envDecayMs = pEnvDecayMs != nullptr ? pEnvDecayMs->load (std::memory_order_relaxed) : 300.0f;
    cachedParams.envSustain = pEnvSustain != nullptr ? pEnvSustain->load (std::memory_order_relaxed) : 0.0f;
    cachedParams.envReleaseMs = pEnvReleaseMs != nullptr ? pEnvReleaseMs->load (std::memory_order_relaxed) : 120.0f;
    cachedParams.transAttackMs = pTransAttackMs != nullptr ? pTransAttackMs->load (std::memory_order_relaxed) : 0.05f;
    cachedParams.fmAttackMs = pFmAttackMs != nullptr ? pFmAttackMs->load (std::memory_order_relaxed) : 0.5f;

    // Output FX: Bitcrusher
    cachedParams.crushEnable = (pCrushEnable != nullptr && pCrushEnable->load (std::memory_order_relaxed) > 0.5f);
    cachedParams.crushBits = pCrushBits != nullptr ? pCrushBits->load (std::memory_order_relaxed) : 16.0f;
    cachedParams.crushDownsample = pCrushDownsample != nullptr ? static_cast<int> (pCrushDownsample->load (std::memory_order_relaxed)) : 1;
    cachedParams.crushMix = pCrushMix != nullptr ? pCrushMix->load (std::memory_order_relaxed) : 0.0f;

    // Output FX: Stereo Delay
    cachedParams.delayEnable = (pDelayEnable != nullptr && pDelayEnable->load (std::memory_order_relaxed) > 0.5f);
    cachedParams.delayTimeMs = pDelayTimeMs != nullptr ? pDelayTimeMs->load (std::memory_order_relaxed) : 140.0f;
    cachedParams.delayFeedback = pDelayFeedback != nullptr ? pDelayFeedback->load (std::memory_order_relaxed) : 0.4f;
    cachedParams.delayDamping = pDelayDamping != nullptr ? pDelayDamping->load (std::memory_order_relaxed) : 0.3f;
    cachedParams.delayPingPong = pDelayPingPong != nullptr ? pDelayPingPong->load (std::memory_order_relaxed) : 0.5f;
    cachedParams.delayMix = pDelayMix != nullptr ? pDelayMix->load (std::memory_order_relaxed) : 0.0f;

    // Output FX: Reverb
    cachedParams.reverbEnable = (pReverbEnable != nullptr && pReverbEnable->load (std::memory_order_relaxed) > 0.5f);
    cachedParams.reverbSize = pReverbSize != nullptr ? pReverbSize->load (std::memory_order_relaxed) : 0.5f;
    cachedParams.reverbDamping = pReverbDamping != nullptr ? pReverbDamping->load (std::memory_order_relaxed) : 0.4f;
    cachedParams.reverbWidth = pReverbWidth != nullptr ? pReverbWidth->load (std::memory_order_relaxed) : 1.0f;
    cachedParams.reverbMix = pReverbMix != nullptr ? pReverbMix->load (std::memory_order_relaxed) : 0.0f;
}

void AetherUIAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Check for pending MIDI Program Change
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isProgramChange())
        {
            const int prog = msg.getProgramChangeNumber();
            if (prog >= 0 && prog < static_cast<int> (AetherUI::getFactoryPresets().size()))
            {
                pendingPresetChange.store (prog, std::memory_order_relaxed);
            }
        }
    }

    const int pending = pendingPresetChange.exchange (-1, std::memory_order_relaxed);
    if (pending >= 0)
    {
        loadPreset (pending);
    }

    // Refresh parameters
    updateEngineParameters();

    // Clear buffer outputs before synth voice rendering
    buffer.clear();

    // Process synthesis engine
    synthEngine.process (buffer, midiMessages, cachedParams);

    // Push audio samples into lock-free visualizer FIFO
    if (buffer.getNumChannels() > 0 && buffer.getNumSamples() > 0)
    {
        pushVisualizerSamples (buffer.getReadPointer (0), buffer.getNumSamples());
    }
}

void AetherUIAudioProcessor::pushVisualizerSamples (const float* samples, int numSamples) noexcept
{
    if (samples == nullptr || numSamples <= 0)
        return;

    // Downsample or push to ring buffer
    int start1, size1, start2, size2;
    visualFifo.prepareToWrite (numSamples, start1, size1, start2, size2);

    if (size1 > 0)
        std::copy_n (samples, size1, visualBuffer.data() + start1);

    if (size2 > 0)
        std::copy_n (samples + size1, size2, visualBuffer.data() + start2);

    visualFifo.finishedWrite (size1 + size2);
}

//==============================================================================
void AetherUIAudioProcessor::loadPreset (int presetIndex)
{
    const auto& presets = AetherUI::getFactoryPresets();
    if (presetIndex < 0 || presetIndex >= static_cast<int> (presets.size()))
        return;

    const auto& p = presets[static_cast<size_t> (presetIndex)];
    currentPresetIndex.store (presetIndex, std::memory_order_relaxed);

    auto setParam = [this] (const juce::String& paramId, float targetVal)
    {
        if (auto* param = apvts.getParameter (paramId))
        {
            const float normalized = param->convertTo0to1 (targetVal);
            param->setValueNotifyingHost (normalized);
        }
    };

    setParam ("master_gain",          p.masterGainDb);
    setParam ("stereo_width",         p.stereoWidth);
    setParam ("velocity_sens",        p.velocitySens);

    setParam ("trans_enable",         p.transEnable ? 1.0f : 0.0f);
    setParam ("trans_type",           static_cast<float> (p.transType));
    setParam ("trans_decay_ms",       p.transDecayMs);
    setParam ("trans_filter_type",    static_cast<float> (p.transFilterType));
    setParam ("trans_filter_freq",    p.transFilterFreq);
    setParam ("trans_filter_q",       p.transFilterQ);
    setParam ("trans_level",          p.transLevel);

    setParam ("fm_enable",            p.fmEnable ? 1.0f : 0.0f);
    setParam ("fm_ratio",             p.fmRatio);
    setParam ("fm_depth",             p.fmDepth);
    setParam ("fm_decay_ms",          p.fmDecayMs);
    setParam ("fm_feedback",          p.fmFeedback);
    setParam ("fm_carrier_decay_ms",  p.fmCarrierDecayMs);
    setParam ("fm_pitch_env_depth",   p.fmPitchEnvDepth);
    setParam ("fm_pitch_env_decay_ms",p.fmPitchEnvDecayMs);
    setParam ("fm_level",             p.fmLevel);

    setParam ("res_enable",           p.resEnable ? 1.0f : 0.0f);
    setParam ("res_tune_mode",        static_cast<float> (p.resTuneMode));
    setParam ("res_freq_offset_semi", p.resFreqOffsetSemi);
    setParam ("res_damping",          p.resDamping);
    setParam ("res_feedback",         p.resFeedback);
    setParam ("res_mix",              p.resMix);

    setParam ("shaper_drive",         p.shaperDriveDb);
    setParam ("shaper_type",          static_cast<float> (p.shaperType));

    setParam ("env_attack_ms",        p.envAttackMs);
    setParam ("env_decay_ms",         p.envDecayMs);
    setParam ("env_sustain",          p.envSustain);
    setParam ("env_release_ms",       p.envReleaseMs);
    setParam ("trans_attack_ms",      p.transAttackMs);
    setParam ("fm_attack_ms",         p.fmAttackMs);

    setParam ("crush_enable",         p.crushEnable ? 1.0f : 0.0f);
    setParam ("crush_bits",           p.crushBits);
    setParam ("crush_downsample",     static_cast<float> (p.crushDownsample));
    setParam ("crush_mix",            p.crushMix);

    setParam ("delay_enable",         p.delayEnable ? 1.0f : 0.0f);
    setParam ("delay_time_ms",        p.delayTimeMs);
    setParam ("delay_feedback",       p.delayFeedback);
    setParam ("delay_damping",        p.delayDamping);
    setParam ("delay_pingpong",       p.delayPingPong);
    setParam ("delay_mix",            p.delayMix);

    setParam ("reverb_enable",        p.reverbEnable ? 1.0f : 0.0f);
    setParam ("reverb_size",          p.reverbSize);
    setParam ("reverb_damping",       p.reverbDamping);
    setParam ("reverb_width",         p.reverbWidth);
    setParam ("reverb_mix",           p.reverbMix);

    setParam ("preset_index",         static_cast<float> (presetIndex));
    customPresetName.clear();
}

//==============================================================================
void AetherUIAudioProcessor::randomizeAllParameters()
{
    auto& rng = juce::Random::getSystemRandom();

    auto setParam = [this] (const juce::String& paramId, float targetVal)
    {
        if (auto* param = apvts.getParameter (paramId))
        {
            const float normalized = param->convertTo0to1 (targetVal);
            param->setValueNotifyingHost (normalized);
        }
    };

    // Master
    setParam ("stereo_width",  0.4f + rng.nextFloat() * 1.4f);
    setParam ("master_pan",    (rng.nextFloat() * 2.0f - 1.0f) * 0.25f);
    setParam ("velocity_sens", 0.4f + rng.nextFloat() * 0.55f);

    // Transient
    const bool tEnable = rng.nextFloat() > 0.12f;
    setParam ("trans_enable",      tEnable ? 1.0f : 0.0f);
    setParam ("trans_type",        static_cast<float> (rng.nextInt (4)));
    const float tDecay = 0.5f * std::pow (90.0f, rng.nextFloat());
    setParam ("trans_decay_ms",    juce::jlimit (0.5f, 60.0f, tDecay));
    setParam ("trans_filter_type", static_cast<float> (rng.nextInt (3)));
    const float tCutoff = 150.0f * std::pow (93.0f, rng.nextFloat());
    setParam ("trans_filter_freq", juce::jlimit (100.0f, 16000.0f, tCutoff));
    setParam ("trans_filter_q",    0.8f + rng.nextFloat() * rng.nextFloat() * 22.0f);
    setParam ("trans_level",       0.35f + rng.nextFloat() * 0.65f);

    // FM
    const bool fmEnable = rng.nextFloat() > 0.10f;
    setParam ("fm_enable",         fmEnable ? 1.0f : 0.0f);
    float fmRatio = 1.0f;
    if (rng.nextFloat() < 0.45f)
    {
        const float commonRatios[] = { 0.5f, 1.0f, 1.414f, 1.5f, 2.0f, 2.414f, 2.76f, 3.0f, 3.1415f, 4.0f, 5.18f, 7.23f };
        fmRatio = commonRatios[rng.nextInt (12)];
    }
    else
    {
        fmRatio = 0.25f + rng.nextFloat() * 10.0f;
    }
    setParam ("fm_ratio",            fmRatio);
    setParam ("fm_depth",            rng.nextFloat() * 7.0f);
    const float fmDecay = 2.0f * std::pow (800.0f, rng.nextFloat());
    setParam ("fm_decay_ms",         juce::jlimit (1.0f, 2500.0f, fmDecay));
    setParam ("fm_feedback",         rng.nextFloat() * 0.55f);
    const float carrierDecay = 10.0f * std::pow (200.0f, rng.nextFloat());
    setParam ("fm_carrier_decay_ms", juce::jlimit (5.0f, 3000.0f, carrierDecay));
    const float pDepth = (rng.nextFloat() < 0.45f) ? (rng.nextFloat() * 2.0f - 1.0f) * 24.0f : 0.0f;
    setParam ("fm_pitch_env_depth",  pDepth);
    const float pDecay = 2.0f * std::pow (180.0f, rng.nextFloat());
    setParam ("fm_pitch_env_decay_ms", juce::jlimit (1.0f, 1000.0f, pDecay));
    setParam ("fm_level",            0.35f + rng.nextFloat() * 0.65f);

    // Resonator
    const bool resEnable = rng.nextFloat() > 0.25f;
    setParam ("res_enable",          resEnable ? 1.0f : 0.0f);
    setParam ("res_tune_mode",       rng.nextFloat() > 0.35f ? 0.0f : 1.0f);
    const float resOffset = std::round ((rng.nextFloat() * 2.0f - 1.0f) * 12.0f);
    setParam ("res_freq_offset_semi", resOffset);
    setParam ("res_damping",         0.08f + rng.nextFloat() * 0.85f);
    setParam ("res_feedback",        0.1f + rng.nextFloat() * 0.78f);
    setParam ("res_mix",             0.1f + rng.nextFloat() * 0.75f);

    // Shaper
    setParam ("shaper_drive",        rng.nextFloat() * 10.0f);
    setParam ("shaper_type",         static_cast<float> (rng.nextInt (2)));

    // Envelopes
    const bool isPad = rng.nextFloat() < 0.2f;
    const float envAttack = isPad ? (50.0f + rng.nextFloat() * 400.0f) : (0.05f + rng.nextFloat() * 10.0f);
    const float envDecay  = 20.0f * std::pow (150.0f, rng.nextFloat());
    const float envSustain = isPad ? (0.2f + rng.nextFloat() * 0.5f) : (rng.nextFloat() < 0.25f ? rng.nextFloat() * 0.3f : 0.0f);
    const float envRelease = 10.0f * std::pow (100.0f, rng.nextFloat());
    setParam ("env_attack_ms",   juce::jlimit (0.05f, 2000.0f, envAttack));
    setParam ("env_decay_ms",    juce::jlimit (1.0f, 4000.0f, envDecay));
    setParam ("env_sustain",     juce::jlimit (0.0f, 1.0f, envSustain));
    setParam ("env_release_ms",  juce::jlimit (1.0f, 3000.0f, envRelease));
    setParam ("trans_attack_ms", juce::jlimit (0.01f, 30.0f, 0.02f + rng.nextFloat() * 1.5f));
    setParam ("fm_attack_ms",    juce::jlimit (0.1f, 500.0f, 0.1f + rng.nextFloat() * 5.0f));

    // Output FX: Bitcrusher
    const bool crushOn = rng.nextFloat() < 0.25f;
    setParam ("crush_enable",     crushOn ? 1.0f : 0.0f);
    setParam ("crush_bits",       6.0f + rng.nextFloat() * 9.0f);
    setParam ("crush_downsample", static_cast<float> (rng.nextInt (6) + 1));
    setParam ("crush_mix",        crushOn ? (0.15f + rng.nextFloat() * 0.5f) : 0.0f);

    // Output FX: Stereo Delay
    const bool delayOn = rng.nextFloat() < 0.35f;
    setParam ("delay_enable",     delayOn ? 1.0f : 0.0f);
    setParam ("delay_time_ms",    30.0f + rng.nextFloat() * 250.0f);
    setParam ("delay_feedback",   0.15f + rng.nextFloat() * 0.55f);
    setParam ("delay_damping",    0.2f + rng.nextFloat() * 0.6f);
    setParam ("delay_pingpong",   rng.nextFloat());
    setParam ("delay_mix",        delayOn ? (0.12f + rng.nextFloat() * 0.38f) : 0.0f);

    // Output FX: Spatial Reverb
    const bool verbOn = rng.nextFloat() < 0.30f;
    setParam ("reverb_enable",    verbOn ? 1.0f : 0.0f);
    setParam ("reverb_size",      0.2f + rng.nextFloat() * 0.65f);
    setParam ("reverb_damping",   0.2f + rng.nextFloat() * 0.6f);
    setParam ("reverb_width",     0.5f + rng.nextFloat() * 0.5f);
    setParam ("reverb_mix",       verbOn ? (0.1f + rng.nextFloat() * 0.35f) : 0.0f);

    customPresetName = "Randomized #" + juce::String (rng.nextInt ({ 100, 999 }));
}

bool AetherUIAudioProcessor::savePresetToFile (const juce::File& file, const juce::String& presetName)
{
    auto state = apvts.copyState();
    const juce::String nameToUse = presetName.isNotEmpty() ? presetName : file.getFileNameWithoutExtension();
    state.setProperty ("presetName", nameToUse, nullptr);
    state.setProperty ("pluginName", "AetherUI", nullptr);
    state.setProperty ("formatVersion", 1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    if (xml != nullptr)
    {
        customPresetName = nameToUse;
        return xml->writeTo (file);
    }
    return false;
}

bool AetherUIAudioProcessor::loadPresetFromFile (const juce::File& file, juce::String* loadedPresetName)
{
    if (! file.existsAsFile())
        return false;

    std::unique_ptr<juce::XmlElement> xmlState (juce::XmlDocument::parse (file));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
    {
        auto newState = juce::ValueTree::fromXml (*xmlState);
        if (newState.isValid())
        {
            apvts.replaceState (newState);
            customPresetName = newState.getProperty ("presetName", file.getFileNameWithoutExtension()).toString();
            if (loadedPresetName != nullptr)
                *loadedPresetName = customPresetName;
            return true;
        }
    }
    return false;
}

juce::String AetherUIAudioProcessor::getCurrentPresetDisplayName() const
{
    if (customPresetName.isNotEmpty())
        return customPresetName;

    const int idx = getCurrentPresetIndex();
    const auto& presets = AetherUI::getFactoryPresets();
    if (idx >= 0 && idx < static_cast<int> (presets.size()))
        return presets[static_cast<size_t> (idx)].name;

    return "Init";
}

//==============================================================================
bool AetherUIAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* AetherUIAudioProcessor::createEditor()
{
    return new AetherUIAudioProcessorEditor (*this);
}

//==============================================================================
void AetherUIAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AetherUIAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
    {
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
    }
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AetherUIAudioProcessor();
}
