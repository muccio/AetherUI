#pragma once

#include <array>

namespace AetherUI
{

struct Preset
{
    const char* name;
    const char* category;

    // Master
    float masterGainDb;
    float stereoWidth;
    float velocitySens;

    // Envelopes (Global ADSR & Module Attacks)
    float envAttackMs;
    float envDecayMs;
    float envSustain;
    float envReleaseMs;
    float transAttackMs;
    float fmAttackMs;

    // Transient Engine
    bool transEnable;
    int transType;         // 0: Dirac/Impulse, 1: White Noise, 2: Pink Noise, 3: Crackle Dust
    float transDecayMs;
    int transFilterType;   // 0: Bandpass, 1: Highpass, 2: Lowpass
    float transFilterFreq;
    float transFilterQ;
    float transLevel;

    // Dual-Op FM Engine
    bool fmEnable;
    float fmRatio;
    float fmDepth;
    float fmDecayMs;
    float fmFeedback;
    float fmCarrierDecayMs;
    float fmPitchEnvDepth;
    float fmPitchEnvDecayMs;
    float fmLevel;

    // Modal / Comb Resonator
    bool resEnable;
    int resTuneMode;       // 0: Track MIDI Note, 1: Fixed Microtonal
    float resFreqOffsetSemi;
    float resDamping;
    float resFeedback;
    float resMix;

    // Output Shaper Stage
    float shaperDriveDb;
    int shaperType;        // 0: Tanh Soft, 1: Hard-Knee Polynomial

    // Output FX: Bitcrusher
    bool crushEnable;
    float crushBits;
    int crushDownsample;
    float crushMix;

    // Output FX: Stereo Delay
    bool delayEnable;
    float delayTimeMs;
    float delayFeedback;
    float delayDamping;
    float delayPingPong;
    float delayMix;

    // Output FX: Spatial Reverb
    bool reverbEnable;
    float reverbSize;
    float reverbDamping;
    float reverbWidth;
    float reverbMix;
};

inline const std::array<Preset, 36>& getFactoryPresets() noexcept
{
    static const std::array<Preset, 36> presets = {{
        // =============================================================
        // Category 1: Haptics & Micro-Clicks (Dry, tactile, snappy)
        // =============================================================
        {
            "Haptic Tap (Ceramic)", "Haptics & Micro-Clicks",
            -2.0f, 1.0f, 0.8f,
            0.1f, 15.0f, 0.0f, 20.0f, 0.05f, 0.5f,
            true, 0, 1.2f, 0, 3800.0f, 16.0f, 0.95f,
            true, 1.0f, 0.2f, 8.0f, 0.05f, 14.0f, 0.0f, 10.0f, 0.2f,
            true, 0, 0.0f, 0.85f, 0.35f, 0.38f,
            3.0f, 0,
            false, 16.0f, 1, 0.0f,
            false, 120.0f, 0.3f, 0.5f, 0.0f, 0.0f,
            false, 0.3f, 0.5f, 1.0f, 0.0f
        },
        {
            "Optic Switch", "Haptics & Micro-Clicks",
            -1.5f, 1.8f, 0.7f,
            0.1f, 25.0f, 0.0f, 20.0f, 0.05f, 0.2f,
            true, 1, 2.0f, 1, 6500.0f, 2.2f, 0.90f,
            true, 4.0f, 0.8f, 8.0f, 0.0f, 12.0f, 0.0f, 10.0f, 0.35f,
            false, 0, 0.0f, 0.90f, 0.10f, 0.10f,
            2.0f, 0,
            false, 16.0f, 1, 0.0f,
            false, 80.0f, 0.2f, 0.4f, 0.8f, 0.0f,
            false, 0.2f, 0.6f, 1.0f, 0.0f
        },
        {
            "Sub Thud UI", "Haptics & Micro-Clicks",
            0.0f, 0.8f, 0.85f,
            0.2f, 95.0f, 0.0f, 40.0f, 0.1f, 0.5f,
            true, 0, 3.5f, 2, 220.0f, 1.8f, 0.85f,
            true, 0.5f, 1.6f, 65.0f, 0.1f, 95.0f, -24.0f, 18.0f, 0.95f,
            true, 1, -24.0f, 0.40f, 0.50f, 0.55f,
            4.0f, 0,
            false, 16.0f, 1, 0.0f,
            false, 140.0f, 0.2f, 0.8f, 0.0f, 0.0f,
            false, 0.3f, 0.5f, 0.8f, 0.0f
        },
        {
            "Micro Needle", "Haptics & Micro-Clicks",
            -2.5f, 1.2f, 0.9f,
            0.05f, 10.0f, 0.0f, 10.0f, 0.02f, 0.2f,
            true, 0, 0.6f, 1, 9500.0f, 9.0f, 1.00f,
            false, 1.0f, 0.0f, 5.0f, 0.0f, 10.0f, 0.0f, 5.0f, 0.0f,
            true, 0, 12.0f, 0.92f, 0.22f, 0.30f,
            4.5f, 0,
            false, 16.0f, 1, 0.0f,
            false, 60.0f, 0.1f, 0.9f, 0.0f, 0.0f,
            false, 0.2f, 0.8f, 1.0f, 0.0f
        },
        {
            "Carbon Click", "Haptics & Micro-Clicks",
            -2.0f, 1.1f, 0.75f,
            0.1f, 18.0f, 0.0f, 20.0f, 0.05f, 0.3f,
            true, 1, 1.5f, 0, 4200.0f, 5.5f, 0.90f,
            true, 2.0f, 0.5f, 12.0f, 0.0f, 18.0f, 5.0f, 8.0f, 0.30f,
            true, 0, 7.0f, 0.75f, 0.45f, 0.30f,
            3.5f, 0,
            false, 16.0f, 1, 0.0f,
            false, 90.0f, 0.25f, 0.5f, 0.5f, 0.0f,
            false, 0.25f, 0.6f, 1.0f, 0.0f
        },
        {
            "Silicon Relay", "Haptics & Micro-Clicks",
            -1.5f, 1.3f, 0.8f,
            0.1f, 22.0f, 0.0f, 15.0f, 0.05f, 0.2f,
            true, 0, 1.8f, 0, 5600.0f, 8.0f, 0.95f,
            true, 3.14f, 1.2f, 14.0f, 0.15f, 20.0f, -8.0f, 10.0f, 0.40f,
            true, 0, 0.0f, 0.82f, 0.38f, 0.35f,
            2.5f, 0,
            false, 16.0f, 1, 0.0f,
            false, 100.0f, 0.3f, 0.6f, 0.6f, 0.0f,
            false, 0.3f, 0.5f, 1.0f, 0.0f
        },
        {
            "Piezo Blip", "Haptics & Micro-Clicks",
            -2.0f, 1.0f, 0.9f,
            0.05f, 12.0f, 0.0f, 15.0f, 0.02f, 0.1f,
            true, 0, 0.9f, 0, 7200.0f, 12.0f, 1.0f,
            true, 1.0f, 0.4f, 6.0f, 0.0f, 10.0f, 12.0f, 6.0f, 0.5f,
            false, 0, 0.0f, 0.7f, 0.1f, 0.1f,
            3.0f, 0,
            false, 16.0f, 1, 0.0f,
            false, 75.0f, 0.15f, 0.7f, 0.0f, 0.0f,
            false, 0.2f, 0.7f, 1.0f, 0.0f
        },
        {
            "Membrane Key", "Haptics & Micro-Clicks",
            -1.0f, 0.9f, 0.7f,
            0.1f, 35.0f, 0.0f, 25.0f, 0.05f, 0.5f,
            true, 2, 4.0f, 2, 850.0f, 2.5f, 0.85f,
            true, 0.5f, 0.8f, 25.0f, 0.0f, 35.0f, 0.0f, 10.0f, 0.60f,
            true, 0, -5.0f, 0.60f, 0.45f, 0.40f,
            1.5f, 0,
            false, 16.0f, 1, 0.0f,
            false, 120.0f, 0.2f, 0.5f, 0.0f, 0.0f,
            false, 0.2f, 0.6f, 0.8f, 0.0f
        },

        // =============================================================
        // Category 2: Glass & Soft Confirmations (FM overtones & Chimes)
        // =============================================================
        {
            "Glass Accept", "Glass & Soft Confirmations",
            -3.0f, 1.2f, 0.75f,
            0.2f, 450.0f, 0.0f, 120.0f, 0.1f, 0.5f,
            true, 0, 2.0f, 0, 4800.0f, 4.0f, 0.45f,
            true, 2.41f, 3.2f, 350.0f, 0.08f, 520.0f, 0.0f, 20.0f, 0.85f,
            true, 0, 0.0f, 0.25f, 0.72f, 0.60f,
            1.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 180.0f, 0.25f, 0.4f, 0.5f, 0.15f,
            true, 0.45f, 0.35f, 1.2f, 0.25f
        },
        {
            "Ethereal Notification", "Glass & Soft Confirmations",
            -3.5f, 1.4f, 0.7f,
            0.5f, 850.0f, 0.0f, 250.0f, 0.2f, 1.0f,
            true, 2, 8.0f, 0, 2400.0f, 2.5f, 0.30f,
            true, 3.0f, 1.8f, 600.0f, 0.05f, 850.0f, 0.0f, 30.0f, 0.80f,
            true, 0, 7.0f, 0.18f, 0.82f, 0.75f,
            0.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 220.0f, 0.40f, 0.3f, 0.8f, 0.30f,
            true, 0.70f, 0.30f, 1.4f, 0.40f
        },
        {
            "Quantum Ping", "Glass & Soft Confirmations",
            -2.5f, 1.3f, 0.8f,
            0.1f, 250.0f, 0.0f, 80.0f, 0.05f, 0.5f,
            true, 1, 3.5f, 0, 8000.0f, 12.0f, 0.70f,
            true, 5.18f, 4.5f, 120.0f, 0.35f, 280.0f, 0.0f, 15.0f, 0.85f,
            true, 0, 12.0f, 0.30f, 0.78f, 0.65f,
            2.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 130.0f, 0.35f, 0.5f, 0.7f, 0.20f,
            true, 0.50f, 0.40f, 1.2f, 0.30f
        },
        {
            "Plexiglass Pop", "Glass & Soft Confirmations",
            -2.0f, 1.0f, 0.8f,
            0.1f, 130.0f, 0.0f, 50.0f, 0.05f, 0.5f,
            true, 0, 4.0f, 2, 1800.0f, 3.0f, 0.80f,
            true, 1.5f, 2.0f, 80.0f, 0.0f, 140.0f, 14.0f, 15.0f, 0.75f,
            true, 0, -12.0f, 0.45f, 0.65f, 0.55f,
            1.0f, 0,
            false, 16.0f, 1, 0.0f,
            false, 110.0f, 0.2f, 0.6f, 0.0f, 0.0f,
            true, 0.35f, 0.5f, 0.9f, 0.15f
        },
        {
            "Crystal Marimba", "Glass & Soft Confirmations",
            -2.5f, 1.2f, 0.85f,
            0.1f, 320.0f, 0.0f, 90.0f, 0.05f, 0.5f,
            true, 0, 2.5f, 0, 3200.0f, 6.0f, 0.75f,
            true, 3.76f, 2.5f, 160.0f, 0.04f, 350.0f, 0.0f, 20.0f, 0.85f,
            true, 0, 4.0f, 0.22f, 0.75f, 0.70f,
            1.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 160.0f, 0.3f, 0.4f, 0.5f, 0.18f,
            true, 0.40f, 0.3f, 1.1f, 0.22f
        },
        {
            "Orbital Chime", "Glass & Soft Confirmations",
            -3.0f, 1.5f, 0.75f,
            0.4f, 650.0f, 0.0f, 180.0f, 0.1f, 0.8f,
            true, 1, 4.0f, 0, 6800.0f, 5.0f, 0.40f,
            true, 2.0f, 2.2f, 400.0f, 0.05f, 700.0f, 7.0f, 40.0f, 0.80f,
            true, 0, 12.0f, 0.25f, 0.80f, 0.68f,
            1.0f, 0,
            false, 16.0f, 1, 0.0f,
            true, 240.0f, 0.45f, 0.35f, 0.9f, 0.35f,
            true, 0.65f, 0.25f, 1.3f, 0.38f
        },
        {
            "Luminous Token", "Glass & Soft Confirmations",
            -2.0f, 1.1f, 0.8f,
            0.1f, 180.0f, 0.0f, 60.0f, 0.05f, 0.3f,
            true, 0, 1.5f, 0, 5200.0f, 8.0f, 0.65f,
            true, 4.0f, 3.0f, 95.0f, 0.1f, 200.0f, -5.0f, 15.0f, 0.85f,
            true, 0, 0.0f, 0.35f, 0.70f, 0.50f,
            2.0f, 0,
            false, 16.0f, 1, 0.0f,
            true, 140.0f, 0.25f, 0.5f, 0.6f, 0.15f,
            true, 0.35f, 0.45f, 1.0f, 0.20f
        },
        {
            "Celestial Badge", "Glass & Soft Confirmations",
            -3.0f, 1.3f, 0.75f,
            0.3f, 500.0f, 0.0f, 140.0f, 0.1f, 0.5f,
            true, 2, 5.0f, 0, 4100.0f, 4.0f, 0.50f,
            true, 6.0f, 2.4f, 280.0f, 0.05f, 550.0f, 12.0f, 30.0f, 0.80f,
            true, 0, 7.0f, 0.20f, 0.78f, 0.65f,
            1.2f, 0,
            false, 16.0f, 1, 0.0f,
            true, 200.0f, 0.35f, 0.4f, 0.8f, 0.25f,
            true, 0.55f, 0.35f, 1.2f, 0.30f
        },

        // =============================================================
        // Category 3: Telemetry & Data Streams (Sci-Fi, bleeps, glitch)
        // =============================================================
        {
            "Data Stream Blip", "Telemetry & Data Streams",
            -2.0f, 1.1f, 0.7f,
            0.05f, 65.0f, 0.0f, 30.0f, 0.02f, 0.2f,
            true, 0, 1.0f, 0, 4000.0f, 5.0f, 0.50f,
            true, 3.5f, 4.0f, 40.0f, 0.20f, 65.0f, -12.0f, 20.0f, 0.90f,
            false, 0, 0.0f, 0.50f, 0.0f, 0.0f,
            2.0f, 0,
            false, 16.0f, 1, 0.0f,
            false, 90.0f, 0.2f, 0.5f, 0.0f, 0.0f,
            false, 0.2f, 0.6f, 1.0f, 0.0f
        },
        {
            "Microsound Scan", "Telemetry & Data Streams",
            -3.0f, 1.5f, 0.8f,
            0.1f, 110.0f, 0.0f, 45.0f, 0.05f, 0.3f,
            true, 3, 25.0f, 0, 6200.0f, 6.0f, 0.85f,
            true, 7.23f, 2.8f, 90.0f, 0.15f, 110.0f, 24.0f, 45.0f, 0.70f,
            true, 0, 0.0f, 0.50f, 0.55f, 0.45f,
            3.0f, 0,
            true, 12.0f, 2, 0.25f,
            true, 75.0f, 0.35f, 0.6f, 0.8f, 0.20f,
            false, 0.3f, 0.5f, 1.0f, 0.0f
        },
        {
            "Buffer Glitch Tick", "Telemetry & Data Streams",
            -2.0f, 1.6f, 0.85f,
            0.05f, 50.0f, 0.0f, 20.0f, 0.02f, 0.2f,
            true, 1, 3.0f, 0, 5200.0f, 18.0f, 0.80f,
            true, 11.37f, 5.5f, 35.0f, 0.45f, 50.0f, 7.0f, 12.0f, 0.85f,
            true, 0, 3.5f, 0.60f, 0.60f, 0.40f,
            4.0f, 1,
            true, 8.0f, 3, 0.40f,
            false, 60.0f, 0.2f, 0.7f, 0.0f, 0.0f,
            false, 0.2f, 0.7f, 1.0f, 0.0f
        },
        {
            "Telemetry Pulse", "Telemetry & Data Streams",
            -1.0f, 1.0f, 0.9f,
            0.05f, 14.0f, 0.0f, 15.0f, 0.02f, 0.1f,
            true, 0, 0.8f, 0, 2000.0f, 2.0f, 0.25f,
            true, 1.0f, 0.0f, 10.0f, 0.0f, 14.0f, 0.0f, 5.0f, 0.95f,
            false, 0, 0.0f, 0.50f, 0.0f, 0.0f,
            0.5f, 0,
            false, 16.0f, 1, 0.0f,
            false, 80.0f, 0.1f, 0.7f, 0.0f, 0.0f,
            false, 0.2f, 0.6f, 1.0f, 0.0f
        },
        {
            "Radar Chirp", "Telemetry & Data Streams",
            -1.5f, 1.2f, 0.8f,
            0.05f, 45.0f, 0.0f, 25.0f, 0.02f, 0.2f,
            true, 0, 1.2f, 0, 5500.0f, 6.0f, 0.6f,
            true, 2.0f, 1.5f, 30.0f, 0.05f, 45.0f, 36.0f, 25.0f, 0.90f,
            false, 0, 0.0f, 0.5f, 0.0f, 0.0f,
            1.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 120.0f, 0.3f, 0.5f, 0.6f, 0.20f,
            false, 0.2f, 0.6f, 1.0f, 0.0f
        },
        {
            "Quantum Packet", "Telemetry & Data Streams",
            -2.5f, 1.4f, 0.85f,
            0.05f, 75.0f, 0.0f, 30.0f, 0.02f, 0.3f,
            true, 1, 2.5f, 1, 7500.0f, 4.0f, 0.7f,
            true, 4.25f, 5.0f, 50.0f, 0.3f, 75.0f, -18.0f, 30.0f, 0.85f,
            true, 0, 0.0f, 0.45f, 0.65f, 0.45f,
            3.0f, 0,
            true, 10.0f, 2, 0.30f,
            true, 65.0f, 0.4f, 0.4f, 0.8f, 0.25f,
            false, 0.3f, 0.5f, 1.0f, 0.0f
        },
        {
            "Binary Tracer", "Telemetry & Data Streams",
            -2.0f, 1.2f, 0.8f,
            0.05f, 35.0f, 0.0f, 20.0f, 0.02f, 0.1f,
            true, 0, 0.9f, 0, 8500.0f, 14.0f, 0.8f,
            true, 8.0f, 2.5f, 25.0f, 0.1f, 35.0f, 0.0f, 10.0f, 0.75f,
            true, 0, 12.0f, 0.70f, 0.50f, 0.40f,
            2.0f, 0,
            false, 16.0f, 1, 0.0f,
            true, 85.0f, 0.35f, 0.6f, 0.7f, 0.25f,
            false, 0.2f, 0.7f, 1.0f, 0.0f
        },
        {
            "Synapse Spark", "Telemetry & Data Streams",
            -2.0f, 1.5f, 0.9f,
            0.05f, 40.0f, 0.0f, 20.0f, 0.02f, 0.2f,
            true, 3, 12.0f, 0, 4800.0f, 10.0f, 0.9f,
            true, 5.5f, 4.0f, 30.0f, 0.4f, 40.0f, 15.0f, 15.0f, 0.80f,
            true, 0, -7.0f, 0.55f, 0.55f, 0.35f,
            4.0f, 1,
            true, 6.0f, 4, 0.35f,
            true, 70.0f, 0.3f, 0.5f, 0.9f, 0.20f,
            false, 0.25f, 0.6f, 1.0f, 0.0f
        },

        // =============================================================
        // Category 4: Ambient Microsound & Space (Diffused, textural)
        // =============================================================
        {
            "Deep Horizon Bell", "Ambient Microsound & Space",
            -3.5f, 1.5f, 0.75f,
            0.5f, 1200.0f, 0.1f, 400.0f, 0.2f, 1.5f,
            true, 0, 3.0f, 0, 1800.0f, 4.0f, 0.4f,
            true, 2.76f, 2.5f, 800.0f, 0.05f, 1200.0f, 0.0f, 50.0f, 0.85f,
            true, 0, 0.0f, 0.15f, 0.85f, 0.75f,
            0.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 280.0f, 0.55f, 0.3f, 0.9f, 0.40f,
            true, 0.85f, 0.25f, 1.5f, 0.45f
        },
        {
            "Frozen Vapor Drop", "Ambient Microsound & Space",
            -3.0f, 1.6f, 0.8f,
            0.2f, 750.0f, 0.0f, 250.0f, 0.1f, 0.5f,
            true, 1, 4.0f, 1, 7200.0f, 3.0f, 0.5f,
            true, 3.14f, 3.0f, 400.0f, 0.15f, 750.0f, 12.0f, 60.0f, 0.80f,
            true, 0, 7.0f, 0.25f, 0.75f, 0.60f,
            1.0f, 0,
            false, 16.0f, 1, 0.0f,
            true, 220.0f, 0.45f, 0.4f, 0.8f, 0.35f,
            true, 0.75f, 0.30f, 1.4f, 0.40f
        },
        {
            "Subsurface Resonator", "Ambient Microsound & Space",
            -2.5f, 1.3f, 0.8f,
            0.3f, 900.0f, 0.2f, 350.0f, 0.1f, 1.0f,
            true, 2, 6.0f, 2, 650.0f, 3.5f, 0.6f,
            true, 0.5f, 1.5f, 500.0f, 0.05f, 900.0f, -12.0f, 80.0f, 0.90f,
            true, 0, -12.0f, 0.20f, 0.88f, 0.80f,
            1.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 320.0f, 0.50f, 0.5f, 0.7f, 0.35f,
            true, 0.80f, 0.35f, 1.3f, 0.40f
        },
        {
            "Atmospheric Shimmer", "Ambient Microsound & Space",
            -3.5f, 1.8f, 0.7f,
            1.0f, 1500.0f, 0.3f, 500.0f, 0.5f, 2.0f,
            true, 1, 8.0f, 0, 5500.0f, 2.0f, 0.3f,
            true, 5.0f, 1.8f, 900.0f, 0.04f, 1500.0f, 0.0f, 100.0f, 0.75f,
            true, 0, 12.0f, 0.12f, 0.92f, 0.85f,
            0.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 350.0f, 0.60f, 0.25f, 1.0f, 0.45f,
            true, 0.90f, 0.20f, 1.6f, 0.50f
        },
        {
            "Glitch Cloud Bloom", "Ambient Microsound & Space",
            -3.0f, 1.6f, 0.85f,
            0.2f, 800.0f, 0.0f, 300.0f, 0.1f, 0.8f,
            true, 3, 30.0f, 0, 4200.0f, 8.0f, 0.8f,
            true, 7.0f, 3.5f, 350.0f, 0.35f, 800.0f, -7.0f, 50.0f, 0.80f,
            true, 0, 5.0f, 0.30f, 0.75f, 0.65f,
            2.5f, 1,
            true, 10.0f, 3, 0.30f,
            true, 180.0f, 0.50f, 0.4f, 0.9f, 0.35f,
            true, 0.65f, 0.35f, 1.3f, 0.35f
        },
        {
            "Astral Prism Drift", "Ambient Microsound & Space",
            -3.5f, 1.7f, 0.75f,
            0.8f, 1600.0f, 0.2f, 600.0f, 0.3f, 1.5f,
            true, 2, 10.0f, 0, 3600.0f, 3.0f, 0.35f,
            true, 2.414f, 2.0f, 850.0f, 0.08f, 1600.0f, 5.0f, 120.0f, 0.80f,
            true, 0, 19.0f, 0.14f, 0.90f, 0.80f,
            0.8f, 0,
            false, 16.0f, 1, 0.0f,
            true, 260.0f, 0.55f, 0.3f, 0.95f, 0.40f,
            true, 0.88f, 0.22f, 1.5f, 0.48f
        },

        // =============================================================
        // Category 5: Dismiss & Low States (Errors, cancellations, low)
        // =============================================================
        {
            "Soft Decline", "Dismiss & Low States",
            -2.0f, 1.0f, 0.8f,
            0.1f, 260.0f, 0.0f, 80.0f, 0.05f, 0.4f,
            true, 0, 2.5f, 2, 1200.0f, 1.2f, 0.45f,
            true, 0.75f, 1.2f, 180.0f, 0.0f, 260.0f, -18.0f, 120.0f, 0.85f,
            true, 0, -7.0f, 0.70f, 0.40f, 0.35f,
            1.5f, 0,
            false, 16.0f, 1, 0.0f,
            false, 120.0f, 0.2f, 0.6f, 0.0f, 0.0f,
            true, 0.30f, 0.6f, 0.8f, 0.15f
        },
        {
            "Magnetic Void", "Dismiss & Low States",
            -1.5f, 1.2f, 0.85f,
            0.2f, 320.0f, 0.0f, 100.0f, 0.05f, 0.5f,
            true, 2, 15.0f, 2, 450.0f, 4.0f, 0.70f,
            true, 0.33f, 3.0f, 220.0f, 0.1f, 320.0f, -24.0f, 160.0f, 0.90f,
            true, 0, -12.0f, 0.55f, 0.75f, 0.65f,
            3.5f, 0,
            false, 16.0f, 1, 0.0f,
            true, 160.0f, 0.3f, 0.7f, 0.5f, 0.20f,
            true, 0.45f, 0.5f, 0.9f, 0.25f
        },
        {
            "Out of Memory", "Dismiss & Low States",
            -2.5f, 1.4f, 0.9f,
            0.05f, 90.0f, 0.0f, 40.0f, 0.02f, 0.3f,
            true, 3, 18.0f, 0, 1500.0f, 8.0f, 0.95f,
            true, 2.77f, 6.0f, 75.0f, 0.60f, 90.0f, -9.0f, 35.0f, 0.85f,
            true, 0, -5.0f, 0.65f, 0.58f, 0.50f,
            12.0f, 1,
            true, 6.0f, 6, 0.60f,
            false, 80.0f, 0.2f, 0.8f, 0.0f, 0.0f,
            false, 0.2f, 0.7f, 1.0f, 0.0f
        },
        {
            "Glass Reject", "Dismiss & Low States",
            -2.0f, 1.2f, 0.8f,
            0.1f, 130.0f, 0.0f, 50.0f, 0.05f, 0.3f,
            true, 0, 1.5f, 1, 4200.0f, 6.0f, 0.60f,
            true, 1.414f, 5.0f, 90.0f, 0.30f, 130.0f, 5.0f, 20.0f, 0.85f,
            true, 0, 6.0f, 0.88f, 0.65f, 0.55f,
            2.0f, 0,
            false, 16.0f, 1, 0.0f,
            false, 100.0f, 0.2f, 0.6f, 0.0f, 0.0f,
            true, 0.30f, 0.5f, 0.9f, 0.15f
        },
        {
            "Thermal Throttle", "Dismiss & Low States",
            -2.0f, 1.3f, 0.85f,
            0.1f, 180.0f, 0.0f, 60.0f, 0.05f, 0.4f,
            true, 1, 10.0f, 0, 2200.0f, 4.0f, 0.75f,
            true, 0.88f, 3.5f, 110.0f, 0.2f, 180.0f, -14.0f, 70.0f, 0.85f,
            true, 0, -8.0f, 0.60f, 0.50f, 0.40f,
            6.0f, 1,
            true, 7.0f, 4, 0.45f,
            false, 90.0f, 0.2f, 0.7f, 0.0f, 0.0f,
            false, 0.25f, 0.6f, 1.0f, 0.0f
        },
        {
            "System Fault Drop", "Dismiss & Low States",
            -2.5f, 1.4f, 0.9f,
            0.05f, 160.0f, 0.0f, 60.0f, 0.02f, 0.3f,
            true, 0, 2.0f, 0, 1800.0f, 5.0f, 0.85f,
            true, 1.732f, 5.5f, 100.0f, 0.4f, 160.0f, -32.0f, 90.0f, 0.90f,
            true, 0, -10.0f, 0.68f, 0.60f, 0.45f,
            8.0f, 1,
            true, 5.0f, 5, 0.50f,
            true, 110.0f, 0.3f, 0.6f, 0.7f, 0.20f,
            true, 0.35f, 0.5f, 0.9f, 0.15f
        }
    }};

    return presets;
}

} // namespace AetherUI
