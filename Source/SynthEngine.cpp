#include "SynthEngine.h"

namespace AetherUI
{

//==============================================================================
void OutputEffectsRack::prepare (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = static_cast<float> (sampleRate > 1000.0 ? sampleRate : 44100.0);

    // Prepare Bitcrusher
    crushHoldL = 0.0f;
    crushHoldR = 0.0f;
    crushCounter = 0;

    // Prepare Stereo Ping-Pong Delay (1.0 second max buffer)
    const int maxDelaySamples = static_cast<int> (currentSampleRate * 1.0f) + 64;
    delayBufferL.assign (static_cast<size_t> (maxDelaySamples), 0.0f);
    delayBufferR.assign (static_cast<size_t> (maxDelaySamples), 0.0f);
    delayWritePos = 0;
    delayDampStateL = 0.0f;
    delayDampStateR = 0.0f;

    // Prepare Reverb
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = currentSampleRate;
    spec.maximumBlockSize = static_cast<uint32_t> (samplesPerBlock > 0 ? samplesPerBlock : 512);
    spec.numChannels = 2;
    reverb.prepare (spec);

    wetBuffer.setSize (2, std::max (samplesPerBlock, 512));
    wetBuffer.clear();
}

void OutputEffectsRack::reset()
{
    crushHoldL = 0.0f;
    crushHoldR = 0.0f;
    crushCounter = 0;

    std::fill (delayBufferL.begin(), delayBufferL.end(), 0.0f);
    std::fill (delayBufferR.begin(), delayBufferR.end(), 0.0f);
    delayWritePos = 0;
    delayDampStateL = 0.0f;
    delayDampStateR = 0.0f;

    reverb.reset();
}

void OutputEffectsRack::process (juce::AudioBuffer<float>& buffer, const SynthParams& params)
{
    const int numSamples = buffer.getNumSamples();
    if (numSamples <= 0 || buffer.getNumChannels() < 1)
        return;

    auto* left = buffer.getWritePointer (0);
    auto* right = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : left;

    // -------------------------------------------------------------
    // Stage 1: Bitcrusher & Sample-Rate Decimator
    // -------------------------------------------------------------
    if (params.crushEnable && params.crushMix > 0.001f)
    {
        const float bits = juce::jlimit (4.0f, 16.0f, params.crushBits);
        const float step = 1.0f / std::pow (2.0f, bits);
        const int downsample = juce::jlimit (1, 32, params.crushDownsample);
        const float mix = juce::jlimit (0.0f, 1.0f, params.crushMix);

        for (int i = 0; i < numSamples; ++i)
        {
            if ((crushCounter % downsample) == 0)
            {
                crushHoldL = std::floor (left[i] / step + 0.5f) * step;
                crushHoldR = std::floor (right[i] / step + 0.5f) * step;
            }
            ++crushCounter;

            left[i]  = (1.0f - mix) * left[i]  + mix * crushHoldL;
            right[i] = (1.0f - mix) * right[i] + mix * crushHoldR;
        }
    }

    // -------------------------------------------------------------
    // Stage 2: Stereo Ping-Pong / Glitch Micro-Delay
    // -------------------------------------------------------------
    if (params.delayEnable && params.delayMix > 0.001f && ! delayBufferL.empty())
    {
        const auto bufSize = static_cast<int> (delayBufferL.size());
        const float delayBaseSamples = juce::jlimit (16.0f, static_cast<float> (bufSize - 16),
                                                     params.delayTimeMs * 0.001f * currentSampleRate);
        const float pingPong = juce::jlimit (0.0f, 1.0f, params.delayPingPong);
        const float feedback = juce::jlimit (0.0f, 0.95f, params.delayFeedback);
        const float damping  = juce::jlimit (0.0f, 0.95f, params.delayDamping);
        const float mix      = juce::jlimit (0.0f, 1.0f, params.delayMix);

        auto readInterp = [bufSize] (const std::vector<float>& buf, float rPos) -> float
        {
            while (rPos < 0.0f)
                rPos += static_cast<float> (bufSize);
            const int i0 = static_cast<int> (rPos) % bufSize;
            const int i1 = (i0 + 1) % bufSize;
            const float frac = rPos - std::floor (rPos);
            return buf[static_cast<size_t> (i0)] * (1.0f - frac)
                 + buf[static_cast<size_t> (i1)] * frac;
        };

        for (int i = 0; i < numSamples; ++i)
        {
            const float rPosL = static_cast<float> (delayWritePos) - delayBaseSamples;
            const float rPosR = static_cast<float> (delayWritePos) - delayBaseSamples * (1.0f + pingPong * 0.35f);

            const float delayedL = readInterp (delayBufferL, rPosL);
            const float delayedR = readInterp (delayBufferR, rPosR);

            delayDampStateL = delayedL * (1.0f - damping) + delayDampStateL * damping;
            delayDampStateR = delayedR * (1.0f - damping) + delayDampStateR * damping;

            // Ping-Pong cross-feedback
            delayBufferL[static_cast<size_t> (delayWritePos)] = left[i]  + delayDampStateR * feedback;
            delayBufferR[static_cast<size_t> (delayWritePos)] = right[i] + delayDampStateL * feedback;

            delayWritePos = (delayWritePos + 1) % bufSize;

            left[i]  += delayedL * mix;
            right[i] += delayedR * mix;
        }
    }

    // -------------------------------------------------------------
    // Stage 3: Spatial Reverb / Diffusion
    // -------------------------------------------------------------
    if (params.reverbEnable && params.reverbMix > 0.001f)
    {
        juce::dsp::Reverb::Parameters rParams;
        rParams.roomSize = juce::jlimit (0.0f, 1.0f, params.reverbSize);
        rParams.damping  = juce::jlimit (0.0f, 1.0f, params.reverbDamping);
        rParams.width    = juce::jlimit (0.0f, 1.0f, params.reverbWidth);
        rParams.dryLevel = 0.0f;
        rParams.wetLevel = 1.0f;
        reverb.setParameters (rParams);

        if (wetBuffer.getNumSamples() < numSamples)
            wetBuffer.setSize (2, numSamples, false, false, true);

        for (int ch = 0; ch < 2; ++ch)
            wetBuffer.copyFrom (ch, 0, buffer, ch, 0, numSamples);

        juce::dsp::AudioBlock<float> block (wetBuffer.getArrayOfWritePointers(), 2, 0, static_cast<size_t> (numSamples));
        juce::dsp::ProcessContextReplacing<float> context (block);
        reverb.process (context);

        const float revMix = juce::jlimit (0.0f, 1.0f, params.reverbMix);
        for (int i = 0; i < numSamples; ++i)
        {
            left[i]  = left[i]  * (1.0f - revMix * 0.35f) + wetBuffer.getSample (0, i) * revMix;
            right[i] = right[i] * (1.0f - revMix * 0.35f) + wetBuffer.getSample (1, i) * revMix;
        }
    }
}

//==============================================================================
SynthVoice::SynthVoice (int voiceIndex)
    : voiceId (voiceIndex),
      rng (static_cast<uint32_t> (voiceIndex * 1337 + 1013904223u))
{
    voicePanSpread = ((static_cast<float> (voiceIndex % 8) - 3.5f) / 4.0f) * 0.35f;
}

void SynthVoice::prepare (double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = (newSampleRate > 1000.0) ? newSampleRate : 44100.0;
    resonator.prepare (sampleRate);
    transFilter.reset();
    globalAmpEnv.reset();

    pinkB0 = pinkB1 = pinkB2 = pinkB3 = pinkB4 = pinkB5 = pinkB6 = 0.0f;
    carrierPhase = 0.0f;
    modulatorPhase = 0.0f;
    lastModSample = 0.0f;
    isPlaying = false;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity,
                            juce::SynthesiserSound* /*sound*/,
                            int currentPitchWheelPosition)
{
    isPlaying = true;
    currentNote = midiNoteNumber;
    currentVelocity = juce::jlimit (0.01f, 1.0f, velocity);
    pitchWheelNorm = static_cast<float> (currentPitchWheelPosition - 8192) / 8192.0f;

    if (currentParams == nullptr)
        return;

    const float velSens = currentParams->velocitySens;
    const float velScale = (1.0f - velSens) + velSens * currentVelocity;
    const float srFloat = static_cast<float> (sampleRate);

    // 1. Global Amp Envelope
    globalAmpEnv.start (currentParams->envAttackMs * 0.001f,
                        currentParams->envDecayMs * 0.001f,
                        currentParams->envSustain,
                        currentParams->envReleaseMs * 0.001f,
                        srFloat);

    // 2. Transient Module
    transFilter.reset();
    transTriggered = true;
    if (currentParams->transAttackMs > 0.05f)
    {
        transEnv = 0.0f;
        transInAttack = true;
        transAttackRate = velScale / (currentParams->transAttackMs * 0.001f * srFloat);
    }
    else
    {
        transEnv = velScale;
        transInAttack = false;
        transAttackRate = 1.0f;
    }
    const float transDecaySec = juce::jmax (0.0005f, currentParams->transDecayMs * 0.001f);
    transDecayCoeff = std::exp (-1.0f / (transDecaySec * srFloat));

    // 3. FM Module
    carrierPhase = 0.0f;
    modulatorPhase = 0.0f;
    lastModSample = 0.0f;

    const float targetFmDepth = currentParams->fmDepth * velScale;
    if (currentParams->fmAttackMs > 0.1f)
    {
        fmDepthEnv = 0.0f;
        fmDepthInAttack = true;
        fmDepthAttackRate = targetFmDepth / (currentParams->fmAttackMs * 0.001f * srFloat);
    }
    else
    {
        fmDepthEnv = targetFmDepth;
        fmDepthInAttack = false;
        fmDepthAttackRate = 1.0f;
    }
    const float fmDecaySec = juce::jmax (0.001f, currentParams->fmDecayMs * 0.001f);
    fmDepthDecayCoeff = std::exp (-1.0f / (fmDecaySec * srFloat));

    fmCarrierEnv = velScale;
    const float fmCarrierDecaySec = juce::jmax (0.005f, currentParams->fmCarrierDecayMs * 0.001f);
    fmCarrierDecayCoeff = std::exp (-1.0f / (fmCarrierDecaySec * srFloat));

    fmPitchEnv = 1.0f;
    const float fmPitchDecaySec = juce::jmax (0.0005f, currentParams->fmPitchEnvDecayMs * 0.001f);
    fmPitchEnvDecayCoeff = std::exp (-1.0f / (fmPitchDecaySec * srFloat));

    // 4. Resonator Reset
    resonator.reset();
}

void SynthVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (! allowTailOff)
    {
        isPlaying = false;
        clearCurrentNote();
    }
    else
    {
        globalAmpEnv.release();
    }
}

void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{
    pitchWheelNorm = static_cast<float> (newPitchWheelValue - 8192) / 8192.0f;
}

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    if (controllerNumber == 1)
    {
        modWheelNorm = static_cast<float> (newControllerValue) / 127.0f;
    }
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                  int startSample, int numSamples)
{
    if (! isPlaying || currentParams == nullptr)
        return;

    const float srFloat = static_cast<float> (sampleRate);
    const float twoPi = juce::MathConstants<float>::twoPi;

    const float baseNoteFreq = 440.0f * std::pow (2.0f, (static_cast<float> (currentNote) - 69.0f) / 12.0f);
    const float pitchWheelFactor = std::pow (2.0f, (pitchWheelNorm * 2.0f) / 12.0f);
    const float pitchedBaseFreq = baseNoteFreq * pitchWheelFactor;

    const float resTuneSemi = currentParams->resFreqOffsetSemi;
    float resFreq = 220.0f;
    if (currentParams->resTuneMode == 0)
        resFreq = pitchedBaseFreq * std::pow (2.0f, resTuneSemi / 12.0f);
    else
        resFreq = 220.0f * std::pow (2.0f, resTuneSemi / 12.0f);

    const float effectiveResFeedback = juce::jlimit (0.0f, 0.992f,
                                                     currentParams->resFeedback + modWheelNorm * 0.08f);

    auto* leftOut = outputBuffer.getWritePointer (0);
    auto* rightOut = (outputBuffer.getNumChannels() > 1) ? outputBuffer.getWritePointer (1) : leftOut;

    const float pan = juce::jlimit (-1.0f, 1.0f, voicePanSpread);
    const float panAngle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
    const float voicePanL = std::cos (panAngle);
    const float voicePanR = std::sin (panAngle);

    const float velScale = (1.0f - currentParams->velocitySens) + currentParams->velocitySens * currentVelocity;

    for (int i = 0; i < numSamples; ++i)
    {
        const int sampleIndex = startSample + i;

        // 1. Global Amplitude Envelope
        const float globalAmp = globalAmpEnv.process();

        // 2. Transient Envelope (Attack & Decay)
        if (transInAttack)
        {
            transEnv += transAttackRate;
            if (transEnv >= velScale)
            {
                transEnv = velScale;
                transInAttack = false;
            }
        }
        else
        {
            transEnv *= transDecayCoeff;
        }

        // 3. FM Depth Envelope (Attack & Decay)
        if (fmDepthInAttack)
        {
            fmDepthEnv += fmDepthAttackRate;
            const float maxFm = currentParams->fmDepth * velScale;
            if (fmDepthEnv >= maxFm)
            {
                fmDepthEnv = maxFm;
                fmDepthInAttack = false;
            }
        }
        else
        {
            fmDepthEnv *= fmDepthDecayCoeff;
        }

        fmCarrierEnv *= fmCarrierDecayCoeff;
        fmPitchEnv *= fmPitchEnvDecayCoeff;

        // 4. Frequency with Pitch Sweep
        const float pitchSweepSemi = currentParams->fmPitchEnvDepth * fmPitchEnv;
        const float carrierFreq = pitchedBaseFreq * std::pow (2.0f, pitchSweepSemi / 12.0f);
        const float modFreq = carrierFreq * currentParams->fmRatio;

        // 5. Transient Generation
        float transSample = 0.0f;
        if (currentParams->transEnable && transEnv > 1.0e-5f)
        {
            float rawImpulse = 0.0f;
            switch (currentParams->transType)
            {
                case 0: // Dirac / Pulse
                    if (transTriggered)
                    {
                        rawImpulse = 1.0f;
                        transTriggered = false;
                    }
                    rawImpulse *= transEnv;
                    break;

                case 1: // White Noise
                    rawImpulse = rng.nextBipolar() * transEnv;
                    break;

                case 2: // Pink Noise
                {
                    const float white = rng.nextBipolar();
                    pinkB0 = 0.99886f * pinkB0 + white * 0.0555179f;
                    pinkB1 = 0.99332f * pinkB1 + white * 0.0750759f;
                    pinkB2 = 0.96900f * pinkB2 + white * 0.1538520f;
                    pinkB3 = 0.86650f * pinkB3 + white * 0.3104856f;
                    pinkB4 = 0.55000f * pinkB4 + white * 0.5329522f;
                    pinkB5 = -0.7616f * pinkB5 - white * 0.0168980f;
                    const float pink = pinkB0 + pinkB1 + pinkB2 + pinkB3 + pinkB4 + pinkB5 + pinkB6 + white * 0.5362f;
                    pinkB6 = white * 0.115926f;
                    rawImpulse = pink * 0.14f * transEnv;
                    break;
                }

                case 3: // Crackle Dust
                {
                    const float u = rng.nextFloat();
                    const float spike = (u > 0.965f) ? (rng.nextBipolar() * 2.2f) : 0.0f;
                    rawImpulse = spike * transEnv;
                    break;
                }

                default:
                    rawImpulse = 0.0f;
                    break;
            }

            transFilter.updateCoefficients (currentParams->transFilterFreq,
                                            currentParams->transFilterQ,
                                            srFloat);
            transSample = transFilter.process (rawImpulse, currentParams->transFilterType) * currentParams->transLevel;
        }

        // 6. Dual-Operator FM
        float fmSample = 0.0f;
        if (currentParams->fmEnable && fmCarrierEnv > 1.0e-5f)
        {
            const float modPhaseEff = modulatorPhase + currentParams->fmFeedback * lastModSample * juce::MathConstants<float>::pi;
            const float modOut = std::sin (modPhaseEff);
            lastModSample = modOut;

            const float carrierPhaseEff = carrierPhase + modOut * fmDepthEnv;
            const float carrierOut = std::sin (carrierPhaseEff);
            fmSample = carrierOut * fmCarrierEnv * currentParams->fmLevel;

            modulatorPhase += twoPi * modFreq / srFloat;
            if (modulatorPhase >= twoPi)
                modulatorPhase -= twoPi;

            carrierPhase += twoPi * carrierFreq / srFloat;
            if (carrierPhase >= twoPi)
                carrierPhase -= twoPi;
        }

        // 7. Modal / Comb Resonator
        float resSample = 0.0f;
        if (currentParams->resEnable)
        {
            const float excite = transSample * 0.75f + fmSample * 0.45f;
            resSample = resonator.process (excite, resFreq, currentParams->resDamping, effectiveResFeedback)
                        * currentParams->resMix;
        }

        // 8. Voice Sum & Shaper Stage (scaled by Global Amp Envelope)
        const float voiceRaw = (transSample + fmSample + resSample) * globalAmp;
        const float driven = voiceRaw * currentParams->shaperDrive;

        float voiceShaped = 0.0f;
        if (currentParams->shaperType == 0)
        {
            voiceShaped = std::tanh (driven);
        }
        else
        {
            if (driven > 1.15f)
                voiceShaped = 1.0f - 1.0f / (1.0f + (driven - 1.15f));
            else if (driven < -1.15f)
                voiceShaped = -1.0f + 1.0f / (1.0f + (-driven - 1.15f));
            else
                voiceShaped = driven - (driven * driven * driven) * 0.16f;
        }

        // 9. Accumulate with stereo spread
        leftOut[sampleIndex]  += voiceShaped * voicePanL;
        rightOut[sampleIndex] += voiceShaped * voicePanR;

        // 10. Voice Lifecycle
        if (! globalAmpEnv.isActive() && transEnv < 1.0e-4f && fmCarrierEnv < 1.0e-4f && std::abs (resSample) < 1.0e-4f)
        {
            isPlaying = false;
            clearCurrentNote();
            break;
        }
    }
}

//==============================================================================
SynthEngine::SynthEngine()
{
    for (int i = 0; i < kMaxPolyphony; ++i)
    {
        auto* voice = new SynthVoice (i);
        voices[static_cast<size_t> (i)] = voice;
        synth.addVoice (voice);
    }
    synth.addSound (new SynthSound());
}

void SynthEngine::prepare (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    for (auto* voice : voices)
    {
        if (voice != nullptr)
            voice->prepare (sampleRate, samplesPerBlock);
    }
    outputEffects.prepare (sampleRate, samplesPerBlock);
}

void SynthEngine::reset()
{
    synth.allNotesOff (0, false);
    outputEffects.reset();
}

void SynthEngine::process (juce::AudioBuffer<float>& buffer,
                           juce::MidiBuffer& midiMessages,
                           const SynthParams& params)
{
    for (auto* voice : voices)
    {
        if (voice != nullptr)
            voice->setParams (&params);
    }

    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    // Master Bus Output Effects (Bitcrusher, Delay, Reverb)
    outputEffects.process (buffer, params);

    // Mid/Side stereo width processing & master output gain
    const int numSamples = buffer.getNumSamples();
    auto* left = buffer.getWritePointer (0);
    auto* right = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : left;

    const float pan = juce::jlimit (-1.0f, 1.0f, params.masterPan);
    const float panAngle = (pan + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
    const float panL = std::cos (panAngle) * std::sqrt (2.0f);
    const float panR = std::sin (panAngle) * std::sqrt (2.0f);

    const float masterGain = params.masterGain;
    const float width = params.stereoWidth;

    for (int i = 0; i < numSamples; ++i)
    {
        float l = left[i];
        float r = right[i];

        const float mid = 0.5f * (l + r);
        const float side = 0.5f * (l - r) * width;

        float outL = (mid + side) * panL * masterGain;
        float outR = (mid - side) * panR * masterGain;

        if (std::abs (outL) > 1.2f)
            outL = std::tanh (outL);
        if (std::abs (outR) > 1.2f)
            outR = std::tanh (outR);

        left[i] = outL;
        right[i] = outR;
    }
}

int SynthEngine::getActiveVoices() const noexcept
{
    int count = 0;
    for (const auto* voice : voices)
    {
        if (voice != nullptr && voice->isVoiceActive())
            ++count;
    }
    return count;
}

} // namespace AetherUI
