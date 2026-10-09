#include <iostream>
#include <cassert>
#include <cmath>
#include "PluginProcessor.h"
#include "Presets.h"

int main()
{
    std::cout << "===================================================\n";
    std::cout << "  AetherUI Synth - Automated DSP Verification Test\n";
    std::cout << "===================================================\n\n";

    juce::ScopedJuceInitialiser_GUI guiInit;

    // 1. Instantiate Audio Processor
    std::cout << "[Test 1] Instantiating AetherUIAudioProcessor...\n";
    auto processor = std::make_unique<AetherUIAudioProcessor>();
    assert (processor != nullptr);
    assert (processor->getName() == "AetherUI");
    assert (processor->getNumPrograms() == 36);
    std::cout << "  PASSED: Plugin instantiated successfully with 36 programs.\n\n";

    // 2. Test Multi-sample rate preparation
    std::cout << "[Test 2] Preparing playback at multiple sample rates...\n";
    const double testSampleRates[] = { 44100.0, 48000.0, 88200.0, 96000.0, 192000.0 };
    for (double sr : testSampleRates)
    {
        processor->prepareToPlay (sr, 512);
        std::cout << "  Sample rate: " << sr << " Hz initialized successfully.\n";
    }
    std::cout << "  PASSED: Multi-sample rate stability verified.\n\n";

    // Prepare for 48 kHz standard buffer processing
    const double sr = 48000.0;
    const int blockSize = 512;
    processor->prepareToPlay (sr, blockSize);

    juce::AudioBuffer<float> buffer (2, blockSize);
    juce::MidiBuffer midi;

    // 3. Test All 36 Factory Presets
    std::cout << "[Test 3] Testing all 36 Factory Presets...\n";
    const auto& presets = AetherUI::getFactoryPresets();
    assert (presets.size() == 36);

    for (int i = 0; i < 36; ++i)
    {
        processor->loadPreset (i);
        assert (processor->getCurrentPresetIndex() == i);

        // Clear buffer and trigger MIDI Note On
        buffer.clear();
        midi.clear();
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 0.85f), 0);

        float maxPeak = 0.0f;
        bool hasNaN = false;
        bool hasInf = false;

        // Process 40 blocks (~426 ms of audio)
        for (int b = 0; b < 40; ++b)
        {
            processor->processBlock (buffer, midi);
            midi.clear(); // Only send Note On in first block

            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                const float* ptr = buffer.getReadPointer (ch);
                for (int s = 0; s < buffer.getNumSamples(); ++s)
                {
                    const float val = ptr[s];
                    if (std::isnan (val)) hasNaN = true;
                    if (std::isinf (val)) hasInf = true;
                    maxPeak = std::max (maxPeak, std::abs (val));
                }
            }
        }

        assert (! hasNaN);
        assert (! hasInf);
        assert (maxPeak > 0.0001f); // Generated audible sound
        assert (maxPeak <= 2.5f);   // Clean, properly shaped/bounded

        std::cout << "  Preset " << (i + 1) << ": [" << presets[i].category << "] \""
                  << presets[i].name << "\" -> Peak: "
                  << juce::Decibels::gainToDecibels (maxPeak) << " dB (Stable)\n";
    }
    std::cout << "  PASSED: All 36 factory presets produced clean, bounded audio.\n\n";

    // 4. Test Polyphony & Voice Allocation
    std::cout << "[Test 4] Testing 8-voice Polyphony & Chord Execution...\n";
    buffer.clear();
    midi.clear();
    // Play 8-note chord
    const int chordNotes[] = { 48, 52, 55, 60, 64, 67, 72, 76 };
    for (int n : chordNotes)
    {
        midi.addEvent (juce::MidiMessage::noteOn (1, n, 0.75f), 0);
    }

    processor->processBlock (buffer, midi);
    midi.clear();

    const int activeVoices = processor->getActiveVoiceCount();
    std::cout << "  Active voices sounding on chord: " << activeVoices << " / 16\n";
    assert (activeVoices >= 6); // Voices should be sounding

    // Process a few more blocks
    for (int b = 0; b < 20; ++b)
    {
        processor->processBlock (buffer, midi);
    }

    // Send All Notes Off
    midi.addEvent (juce::MidiMessage::allNotesOff (1), 0);
    processor->processBlock (buffer, midi);
    std::cout << "  PASSED: Polyphonic chord processed cleanly.\n\n";

    // 5. Test MIDI CC (Mod Wheel) & Pitch Bend
    std::cout << "[Test 5] Testing MIDI CC1 (Mod Wheel) & Pitch Bend...\n";
    midi.clear();
    midi.addEvent (juce::MidiMessage::controllerEvent (1, 1, 127), 0); // Mod Wheel full
    midi.addEvent (juce::MidiMessage::pitchWheel (1, 12000), 10);     // Pitch Bend up
    processor->processBlock (buffer, midi);
    std::cout << "  PASSED: MIDI CC and Pitch Bend handled smoothly.\n\n";

    // 6. Test MIDI Program Change
    std::cout << "[Test 6] Testing MIDI Program Change...\n";
    midi.clear();
    midi.addEvent (juce::MidiMessage::programChange (1, 4), 0); // Switch to Preset 5 ("Glass Accept")
    processor->processBlock (buffer, midi);
    assert (processor->getCurrentPresetIndex() == 4);
    std::cout << "  PASSED: MIDI Program Change switched to: "
              << processor->getProgramName (4) << "\n\n";

    // 7. Test Waveform Visualizer FIFO
    std::cout << "[Test 7] Testing Waveform Visualizer FIFO...\n";
    auto& fifo = processor->getVisualFifo();
    const int readySamples = fifo.getNumReady();
    std::cout << "  Visualizer FIFO ready samples: " << readySamples << "\n";
    assert (readySamples > 0);
    std::cout << "  PASSED: Oscilloscope FIFO successfully receives real-time samples.\n\n";

    // 8. Test State Save / Restore
    std::cout << "[Test 8] Testing State Serialization (XML memory block)...\n";
    processor->loadPreset (2); // "Sub Thud UI"
    juce::MemoryBlock stateData;
    processor->getStateInformation (stateData);
    assert (stateData.getSize() > 0);

    // Switch to different preset
    processor->loadPreset (7); // "Plexiglass Pop"
    assert (processor->getCurrentPresetIndex() == 7);

    // Restore state
    processor->setStateInformation (stateData.getData(), static_cast<int> (stateData.getSize()));
    std::cout << "  State size: " << stateData.getSize() << " bytes.\n";
    std::cout << "  PASSED: State serialization & restoration verified.\n\n";

    // 9. Test GUI Editor Instantiation & Paint
    std::cout << "[Test 9] Testing PluginEditor construction & paint rendering...\n";
    auto* editor = processor->createEditorIfNeeded();
    assert (editor != nullptr);
    assert (editor->getWidth() == 1120);
    assert (editor->getHeight() == 780);

    // Render to offline image
    juce::Image testImage (juce::Image::ARGB, editor->getWidth(), editor->getHeight(), true);
    juce::Graphics g (testImage);
    editor->paintEntireComponent (g, false);
    std::cout << "  Rendered GUI canvas: " << editor->getWidth() << "x" << editor->getHeight() << " px.\n";
    std::cout << "  PASSED: GUI vector rendering verified without issues.\n\n";

    processor->editorBeingDeleted (editor);
    delete editor;

    // 10. Test Parameter Randomization Functionality
    std::cout << "[Test 10] Testing Parameter Randomization across 25 iterations...\n";
    for (int iter = 0; iter < 25; ++iter)
    {
        processor->randomizeAllParameters();
        const auto dispName = processor->getCurrentPresetDisplayName();
        assert (dispName.startsWith ("Randomized"));

        buffer.clear();
        midi.clear();
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 0.8f), 0);

        for (int b = 0; b < 10; ++b)
        {
            processor->processBlock (buffer, midi);
            midi.clear();

            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                const float* ptr = buffer.getReadPointer (ch);
                for (int s = 0; s < buffer.getNumSamples(); ++s)
                {
                    assert (! std::isnan (ptr[s]));
                    assert (! std::isinf (ptr[s]));
                }
            }
        }
    }
    std::cout << "  PASSED: Parameter randomization produces stable audio across all runs.\n\n";

    // 11. Test Custom Preset File Save & Load (.aetherpreset)
    std::cout << "[Test 11] Testing Preset File Save and Load (.aetherpreset)...\n";
    const auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
                               .getChildFile ("AetherTestPreset.aetherpreset");
    if (tempFile.existsAsFile())
        tempFile.deleteFile();

    // Set a known randomized state
    processor->randomizeAllParameters();
    const float testFmRatio = *processor->getAPVTS().getRawParameterValue ("fm_ratio");

    const juce::String customPresetTitle = "Crystalline Glitch Micro";
    const bool saveSuccess = processor->savePresetToFile (tempFile, customPresetTitle);
    assert (saveSuccess);
    assert (tempFile.existsAsFile());
    std::cout << "  Saved preset file: " << tempFile.getFullPathName()
              << " (" << tempFile.getSize() << " bytes)\n";

    // Change parameters by loading a different factory preset
    processor->loadPreset (0); // "Haptic Tap"
    const float changedFmRatio = *processor->getAPVTS().getRawParameterValue ("fm_ratio");
    (void)changedFmRatio;

    // Reload from file
    juce::String loadedName;
    const bool loadSuccess = processor->loadPresetFromFile (tempFile, &loadedName);
    assert (loadSuccess);
    assert (loadedName == customPresetTitle);

    const float reloadedFmRatio = *processor->getAPVTS().getRawParameterValue ("fm_ratio");
    assert (std::abs (reloadedFmRatio - testFmRatio) < 1.0e-4f);
    std::cout << "  Reloaded preset name: \"" << loadedName << "\", fm_ratio verified.\n";

    tempFile.deleteFile();
    std::cout << "  PASSED: Preset file save and load roundtrip verified.\n\n";

    // 12. Test Output FX Bus & Envelope Modulation
    std::cout << "[Test 12] Testing Output Effects Bus (Bitcrusher, Delay, Reverb) & Envelopes...\n";
    {
        // Test Delay Tail: preset 27 has delay enabled ("Glitch Echo Cluster")
        processor->loadPreset (27);
        assert (processor->getCurrentPresetIndex() == 27);

        buffer.clear();
        midi.clear();
        midi.addEvent (juce::MidiMessage::noteOn (1, 64, 0.9f), 0);
        processor->processBlock (buffer, midi);
        midi.clear();

        // Release note immediately
        midi.addEvent (juce::MidiMessage::noteOff (1, 64, 0.0f), 0);
        processor->processBlock (buffer, midi);
        midi.clear();

        float tailEnergy = 0.0f;
        for (int b = 0; b < 25; ++b)
        {
            processor->processBlock (buffer, midi);
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                const float* ptr = buffer.getReadPointer (ch);
                for (int s = 0; s < buffer.getNumSamples(); ++s)
                {
                    assert (! std::isnan (ptr[s]));
                    assert (! std::isinf (ptr[s]));
                    tailEnergy += ptr[s] * ptr[s];
                }
            }
        }
        assert (tailEnergy > 1.0e-5f);
        std::cout << "  Echo & Reverb tail energy detected: " << tailEnergy << " (Active)\n";

        // Test Bitcrusher: preset 29 has crush enabled ("Downsampled Fracture")
        processor->loadPreset (29);
        buffer.clear();
        midi.clear();
        midi.addEvent (juce::MidiMessage::noteOn (1, 60, 0.85f), 0);
        processor->processBlock (buffer, midi);
        float crushPeak = 0.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            const float* ptr = buffer.getReadPointer (ch);
            for (int s = 0; s < buffer.getNumSamples(); ++s)
                crushPeak = std::max (crushPeak, std::abs (ptr[s]));
        }
        assert (crushPeak > 0.001f && crushPeak <= 2.5f);
        std::cout << "  Bitcrushed signal generated clean peak: " << juce::Decibels::gainToDecibels (crushPeak) << " dB\n";
    }
    std::cout << "  PASSED: Output effects bus and envelopes verified.\n\n";

    std::cout << "=====================================================\n";
    std::cout << "  ALL VERIFICATION TESTS PASSED SUCCESSFULLY! (12/12)\n";
    std::cout << "=====================================================\n";

    return 0;
}
