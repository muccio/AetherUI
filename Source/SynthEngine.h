#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>
#include <cmath>

namespace AetherUI
{

//==============================================================================
/** Audio-thread parameter snapshot passed cleanly to the DSP engine. */
struct SynthParams
{
    // Master
    float masterGain = 0.707f;     // Linear gain
    float masterPan = 0.0f;        // -1.0 to 1.0
    float stereoWidth = 1.0f;      // 0.0 to 2.0
    float velocitySens = 0.7f;     // 0.0 to 1.0
    float modWheel = 0.0f;         // 0.0 to 1.0
    float pitchBendSemitones = 0.0f; // -2.0 to 2.0

    // Envelopes: Global ADSR & Attacks
    float envAttackMs = 0.1f;      // 0.05 to 2000 ms
    float envDecayMs = 300.0f;     // 1 to 4000 ms
    float envSustain = 0.0f;       // 0.0 to 1.0
    float envReleaseMs = 120.0f;   // 1 to 3000 ms
    float transAttackMs = 0.05f;   // 0.01 to 30 ms
    float fmAttackMs = 0.5f;       // 0.1 to 500 ms

    // Transient
    bool transEnable = true;
    int transType = 0;             // 0: Dirac, 1: White, 2: Pink, 3: Crackle
    float transDecayMs = 4.0f;     // 0.5 to 60.0 ms
    int transFilterType = 0;       // 0: BP, 1: HP, 2: LP
    float transFilterFreq = 3800.0f; // 100 to 16000 Hz
    float transFilterQ = 3.5f;     // 0.5 to 25.0
    float transLevel = 0.8f;

    // Dual-Op FM
    bool fmEnable = true;
    float fmRatio = 2.41f;         // 0.25 to 16.0
    float fmDepth = 2.0f;          // 0.0 to 10.0
    float fmDecayMs = 180.0f;      // 1.0 to 2500.0 ms
    float fmFeedback = 0.15f;      // 0.0 to 1.0
    float fmCarrierDecayMs = 250.0f; // 5.0 to 3000.0 ms
    float fmPitchEnvDepth = 0.0f;  // -48 to +48 st
    float fmPitchEnvDecayMs = 25.0f; // 1.0 to 1000.0 ms
    float fmLevel = 0.8f;

    // Resonator
    bool resEnable = true;
    int resTuneMode = 0;           // 0: MIDI Note Track, 1: Fixed
    float resFreqOffsetSemi = 0.0f;// -24 to +24 st
    float resDamping = 0.3f;       // 0.0 to 1.0
    float resFeedback = 0.6f;      // 0.0 to 0.99
    float resMix = 0.5f;           // 0.0 to 1.0

    // Shaper
    float shaperDrive = 1.0f;      // Linear factor
    int shaperType = 0;            // 0: Tanh Soft, 1: Polynomial Hard-Knee

    // Output FX: Bitcrusher
    bool crushEnable = false;
    float crushBits = 16.0f;       // 4 to 16 bits
    int crushDownsample = 1;       // 1 to 32
    float crushMix = 0.0f;         // 0.0 to 1.0

    // Output FX: Stereo Delay
    bool delayEnable = false;
    float delayTimeMs = 140.0f;    // 1 to 800 ms
    float delayFeedback = 0.4f;    // 0.0 to 0.95
    float delayDamping = 0.3f;     // 0.0 to 1.0
    float delayPingPong = 0.5f;    // 0.0 to 1.0
    float delayMix = 0.0f;         // 0.0 to 1.0

    // Output FX: Reverb
    bool reverbEnable = false;
    float reverbSize = 0.5f;       // 0.0 to 1.0
    float reverbDamping = 0.4f;    // 0.0 to 1.0
    float reverbWidth = 1.0f;      // 0.0 to 1.0
    float reverbMix = 0.0f;        // 0.0 to 1.0
};

//==============================================================================
/** Lightweight, lock-free XorShift32 pseudo-random generator for audio thread. */
class FastRNG
{
public:
    FastRNG (uint32_t seed = 3141592653u) : state (seed != 0 ? seed : 123456789u) {}

    void setSeed (uint32_t seed) noexcept
    {
        state = (seed != 0 ? seed : 123456789u);
    }

    uint32_t nextUInt() noexcept
    {
        uint32_t x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        state = x;
        return x;
    }

    float nextFloat() noexcept
    {
        return static_cast<float> (nextUInt()) * 2.3283064365386963e-10f;
    }

    float nextBipolar() noexcept
    {
        return nextFloat() * 2.0f - 1.0f;
    }

private:
    uint32_t state;
};

//==============================================================================
/** Multi-stage envelope generator (Attack, Decay, Sustain, Release). */
class MultiStageEnvelope
{
public:
    enum class State { Idle, Attack, Decay, Sustain, Release };

    void reset() noexcept
    {
        state = State::Idle;
        currentLevel = 0.0f;
    }

    void start (float attackSec, float decaySec, float sustainLvl, float releaseSec, float sampleRate) noexcept
    {
        attackRate = (attackSec > 0.0001f) ? (1.0f / (attackSec * sampleRate)) : 1.0f;
        decayCoeff = std::exp (-1.0f / (std::max (0.001f, decaySec) * sampleRate));
        sustainLevel = juce::jlimit (0.0f, 1.0f, sustainLvl);
        releaseCoeff = std::exp (-1.0f / (std::max (0.001f, releaseSec) * sampleRate));

        if (attackSec <= 0.0001f)
        {
            currentLevel = 1.0f;
            state = State::Decay;
        }
        else
        {
            currentLevel = 0.0f;
            state = State::Attack;
        }
    }

    void release() noexcept
    {
        if (state != State::Idle)
            state = State::Release;
    }

    float process() noexcept
    {
        switch (state)
        {
            case State::Attack:
                currentLevel += attackRate;
                if (currentLevel >= 1.0f)
                {
                    currentLevel = 1.0f;
                    state = State::Decay;
                }
                break;

            case State::Decay:
                currentLevel = sustainLevel + (currentLevel - sustainLevel) * decayCoeff;
                if (std::abs (currentLevel - sustainLevel) < 1.0e-4f)
                {
                    currentLevel = sustainLevel;
                    state = State::Sustain;
                }
                break;

            case State::Sustain:
                break;

            case State::Release:
                currentLevel *= releaseCoeff;
                if (currentLevel < 1.0e-5f)
                {
                    currentLevel = 0.0f;
                    state = State::Idle;
                }
                break;

            case State::Idle:
                currentLevel = 0.0f;
                break;
        }
        return currentLevel;
    }

    bool isActive() const noexcept { return state != State::Idle; }
    State getState() const noexcept { return state; }
    float getLevel() const noexcept { return currentLevel; }

private:
    State state = State::Idle;
    float currentLevel = 0.0f;
    float attackRate = 1.0f;
    float decayCoeff = 0.99f;
    float sustainLevel = 0.0f;
    float releaseCoeff = 0.99f;
};

//==============================================================================
/** TPT 2-Pole State Variable Filter. */
class TptSVF
{
public:
    void reset() noexcept
    {
        s1 = 0.0f;
        s2 = 0.0f;
    }

    void updateCoefficients (float cutoffHz, float Q, float sampleRate) noexcept
    {
        const float clampedCutoff = juce::jlimit (20.0f, sampleRate * 0.48f, cutoffHz);
        const float clampedQ = juce::jmax (0.1f, Q);

        const float g = std::tan (juce::MathConstants<float>::pi * clampedCutoff / sampleRate);
        const float k = 1.0f / clampedQ;

        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
        damping = k;
    }

    float process (float inSample, int filterType) noexcept
    {
        const float v0 = inSample;
        const float v1 = a1 * s1 + a2 * (v0 - s2);
        const float v2 = s2 + a3 * (v0 - s2) + (a1 * s1) * (a3 / a2);
        const float v3 = v0 - damping * v1 - v2;

        s1 = 2.0f * v1 - s1;
        s2 = 2.0f * v2 - s2;

        if (filterType == 0) return v1;
        if (filterType == 1) return v3;
        return v2;
    }

private:
    float s1 = 0.0f;
    float s2 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
    float a3 = 0.0f;
    float damping = 1.0f;
};

//==============================================================================
/** Tuned Comb / Modal Resonator with fractional delay and damping. */
class TunedCombResonator
{
public:
    void prepare (double sampleRate)
    {
        currentSampleRate = static_cast<float> (sampleRate);
        const int maxDelaySamples = static_cast<int> (sampleRate / 15.0) + 16;
        buffer.assign (static_cast<size_t> (maxDelaySamples), 0.0f);
        writeIndex = 0;
        dampState = 0.0f;
    }

    void reset() noexcept
    {
        std::fill (buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
        dampState = 0.0f;
    }

    float process (float input, float targetFreqHz, float damping, float feedback) noexcept
    {
        if (buffer.empty())
            return input;

        const float clampedFreq = juce::jlimit (20.0f, currentSampleRate * 0.45f, targetFreqHz);
        const float delaySamples = currentSampleRate / clampedFreq;
        const auto bufSize = static_cast<int> (buffer.size());

        float readPos = static_cast<float> (writeIndex) - delaySamples;
        while (readPos < 0.0f)
            readPos += static_cast<float> (bufSize);

        const int i0 = static_cast<int> (readPos) % bufSize;
        const int i1 = (i0 + 1) % bufSize;
        const float frac = readPos - std::floor (readPos);

        const float delayedSample = buffer[static_cast<size_t> (i0)] * (1.0f - frac)
                                  + buffer[static_cast<size_t> (i1)] * frac;

        const float clampedDamping = juce::jlimit (0.0f, 0.98f, damping);
        dampState = delayedSample * (1.0f - clampedDamping) + dampState * clampedDamping;

        const float feedbackSample = dampState * juce::jlimit (0.0f, 0.992f, feedback);
        const float newBufferSample = input + feedbackSample;

        buffer[static_cast<size_t> (writeIndex)] = newBufferSample;
        writeIndex = (writeIndex + 1) % bufSize;

        return delayedSample;
    }

private:
    float currentSampleRate = 44100.0f;
    std::vector<float> buffer;
    int writeIndex = 0;
    float dampState = 0.0f;
};

//==============================================================================
/** Master bus Output Effects Rack: Bitcrusher, Stereo Delay, Spatial Reverb. */
class OutputEffectsRack
{
public:
    void prepare (double sampleRate, int samplesPerBlock);
    void reset();
    void process (juce::AudioBuffer<float>& buffer, const SynthParams& params);

private:
    float currentSampleRate = 44100.0f;

    // Bitcrusher
    float crushHoldL = 0.0f;
    float crushHoldR = 0.0f;
    int crushCounter = 0;

    // Stereo Ping-Pong Delay
    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    int delayWritePos = 0;
    float delayDampStateL = 0.0f;
    float delayDampStateR = 0.0f;

    // Reverb
    juce::dsp::Reverb reverb;
    juce::AudioBuffer<float> wetBuffer;
};

//==============================================================================
/** Standard SynthesiserSound descriptor. */
class SynthSound : public juce::SynthesiserSound
{
public:
    SynthSound() = default;
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

//==============================================================================
/** High-performance per-voice DSP synthesis engine. */
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice (int voiceIndex);

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;

    void stopNote (float velocity, bool allowTailOff) override;

    void pitchWheelMoved (int newPitchWheelValue) override;

    void controllerMoved (int controllerNumber, int newControllerValue) override;

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample, int numSamples) override;

    void prepare (double sampleRate, int samplesPerBlock);

    void setParams (const SynthParams* params) noexcept { currentParams = params; }

    bool isVoiceActive() const noexcept override { return isPlaying; }

private:
    const int voiceId;
    double sampleRate = 44100.0;
    const SynthParams* currentParams = nullptr;

    bool isPlaying = false;
    int currentNote = 60;
    float currentVelocity = 1.0f;
    float pitchWheelNorm = 0.0f;
    float modWheelNorm = 0.0f;

    FastRNG rng;

    // Pink noise state
    float pinkB0 = 0.0f, pinkB1 = 0.0f, pinkB2 = 0.0f;
    float pinkB3 = 0.0f, pinkB4 = 0.0f, pinkB5 = 0.0f, pinkB6 = 0.0f;

    // Global Amp Envelope
    MultiStageEnvelope globalAmpEnv;

    // Transient module state
    float transEnv = 0.0f;
    float transAttackRate = 1.0f;
    float transDecayCoeff = 0.99f;
    bool  transInAttack = false;
    bool  transTriggered = false;
    TptSVF transFilter;

    // FM module state
    float carrierPhase = 0.0f;
    float modulatorPhase = 0.0f;
    float lastModSample = 0.0f;

    float fmDepthEnv = 0.0f;
    float fmDepthAttackRate = 1.0f;
    float fmDepthDecayCoeff = 0.99f;
    bool  fmDepthInAttack = false;

    float fmCarrierEnv = 0.0f;
    float fmCarrierDecayCoeff = 0.99f;

    float fmPitchEnv = 0.0f;
    float fmPitchEnvDecayCoeff = 0.99f;

    // Resonator module
    TunedCombResonator resonator;

    // Voice stereo spread
    float voicePanSpread = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthVoice)
};

//==============================================================================
/** Polyphonic Synth Manager hosting 16 voices with Output FX rack. */
class SynthEngine
{
public:
    static constexpr int kMaxPolyphony = 16;

    SynthEngine();
    ~SynthEngine() = default;

    void prepare (double sampleRate, int samplesPerBlock);
    void reset();

    void process (juce::AudioBuffer<float>& buffer,
                  juce::MidiBuffer& midiMessages,
                  const SynthParams& params);

    int getActiveVoices() const noexcept;

private:
    juce::Synthesiser synth;
    std::array<SynthVoice*, kMaxPolyphony> voices {};
    OutputEffectsRack outputEffects;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthEngine)
};

} // namespace AetherUI
