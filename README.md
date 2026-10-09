# AetherUI Synth

> **Production-Ready Micro-Tactile Synthesizer (VST3 / Standalone)**  
> *Crafted for modern tactile UI/UX sound design and microsound / ambient glitch electronic music.*

[![Platform](https://img.shields.io/badge/platform-macOS%20(Apple%20Silicon%20%2F%20Intel)-brightgreen.svg)](#)
[![Format](https://img.shields.io/badge/format-VST3%20%7C%20Standalone-blue.svg)](#)
[![Standard](https://img.shields.io/badge/C%2B%2B-20-purple.svg)](#)
[![Framework](https://img.shields.io/badge/JUCE-7%20%2F%208-orange.svg)](#)

---

## Overview

**AetherUI** is a specialized polyphonic synthesizer engineered to synthesize modern tactile UI/UX feedback sounds—haptic clicks, glass confirmations, telemetry bleeps, soft error drops, and raster micro-percussions—tailored to blend seamlessly into ambient glitch, IDM, and microsound music.

Built with **C++20** and the **JUCE** framework, AetherUI guarantees strict real-time audio thread safety (zero dynamic memory allocations and lock-free parameter updates).

---

## Key Features & DSP Architecture

Each of the 16 polyphonic voices combines three distinct synthesis blocks with sub-millisecond precision:

### 1. Transient / Haptic Micro-Impulse Engine
- Ultra-short clicks, ticks, and micro-textures.
- **Skins & Sources**: Bandlimited Dirac synthetic impulse, White noise, Pink noise, and Crackle dust.
- **Envelope**: Exponential decay range from `0.5 ms` to `60 ms` with adjustable attack.
- **Pre-Filter**: 2-pole resonant Bandpass, Highpass, and Lowpass filter (`100 Hz` to `16 kHz`) with Q factor shaping.

### 2. Dual-Operator FM Engine (Crystal / Chime / Glass)
- 2 Sine Oscillators (Carrier & Modulator) with audio-rate phase modulation.
- Non-integer inharmonic ratios (`0.25` to `16.0`) for metallic, crystalline, and glass overtones.
- Modulator self-feedback (`0.0` to `1.0`).
- Dedicated sub-millisecond Pitch Envelope sweep (`-48` to `+48` semitones) for drop confirmations and telemetry chirps.

### 3. Tuned Modal / Comb Resonator
- Micro-tuned comb filter simulating physical bodies (glass plates, hollow ceramic, metallic tines).
- Modes: chromatic MIDI Note tracking or fixed microtonal resonance.
- Frequency offset, damping, and feedback controls.

### 4. Global & Module Envelope Modulation
- **Global Amp ADSR**: Sub-millisecond attack (`0.05 ms` to `2000 ms`), exponential decay, sustain, and release.
- **Interactive Vector Envelope Editor**: Tabbed vector display in the GUI (`AMP ADSR`, `TRANSIENT`, `FM MOD`, `PITCH`) with dynamic glow, gradient fills, and vertex handles.

### 5. Master Output FX Rack
- **Bitcrusher & Decimator**: Variable bit depth (`4` to `16` bits) and sample-rate downsampling (`1x` to `32x`).
- **Stereo Ping-Pong Delay**: Interpolated delay buffer (`1` to `800 ms`), high-frequency damping, feedback, and cross-channel bouncing.
- **Spatial Reverb**: High-density stereo room simulator with room size, high-frequency damping, and stereo width controls.
- **Output Shaper**: Tanh soft saturation and polynomial hard-knee limiting with up to `+24 dB` drive.

### 6. Embedded Preset System & Utilities
- **36 Factory Presets** across 5 categories:
  - *Haptics & Micro-Clicks*
  - *Glass & Soft Confirmations*
  - *Telemetry & Data Streams*
  - *Ambient Microsound & Space*
  - *Dismiss & Low States*
- **Smart Parameter Randomization**: Instant generation of musically calibrated UI sounds.
- **File Management**: Save and load custom `.aetherpreset` XML files.
- **Live Vector Scope**: Real-time oscilloscope with peak level indicator.

---

## User Interface

The GUI is designed with a dark-mode minimalist aesthetic inspired by Teenage Engineering and Figma:

- **Dimensions**: `1120 x 780` px vector canvas.
- **Header**: Preset dropdown, `RND` button, `SAVE` / `LOAD` buttons, active polyphony monitor, and Master bus controls.
- **Card 01**: Transient Impulse Generator.
- **Card 02**: Dual-Op FM Engine.
- **Card 03**: Modal Comb Resonator.
- **Card 04**: Envelope Modulator with tabbed vector graph.
- **Card 05**: Output FX Rack (Crusher, Delay, Reverb, Shaper).
- **Card 06**: Real-time Micro-Oscilloscope.

---

## Building from Source

### Prerequisites
- CMake 3.22+
- Clang / Apple Clang supporting C++20
- JUCE 7.0+ (or JUCE 8)

### Build Commands

```bash
# Clone the repository
git clone https://github.com/muccio/AetherUI.git
cd AetherUI

# Configure build directory
cmake -B build -DCMAKE_BUILD_TYPE=Release -DJUCE_PATH=/path/to/JUCE

# Build VST3, Standalone, and Verification Tests
cmake --build build --config Release
```

The compiled targets will be located in:
- `build/AetherUI_artefacts/Release/VST3/AetherUI.vst3`
- `build/AetherUI_artefacts/Release/Standalone/AetherUI.app`
- `build/AetherUIVerificationTest`

---

## Automated Verification Suite

Run the built-in test runner to verify DSP real-time safety, preset stability, and vector GUI rendering:

```bash
./build/AetherUIVerificationTest
```

Expected output: `ALL VERIFICATION TESTS PASSED SUCCESSFULLY! (12/12)`.

---

## License

MIT License. Designed & Developed by Mario Salvucci.
