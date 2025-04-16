#include <iostream>
#include <iomanip>
#include <string>
#include "MockSamplingModule.h"
#include "MockOutputModule.h"
#include "DigitalSignalChain.h"
#include "Sample.h"
#include "EffectFactory.h"
#include "OctaveDoubler.h"

// Optional linker anchor for safety — may be removed if REGISTER_EFFECT_AUTO is working reliably.
extern void ForceAllEffects();  // defined in ForceEffects.cpp

/**
 * @brief Processes a single audio sample by applying all registered DSP effects,
 *        logging it to console, and writing it to the output module.
 *
 * @param sample The audio sample to process
 * @param dspChain The digital signal chain containing the effects
 * @param mockOutput The output module for writing processed samples
 */
void processSample(Sample& sample, DigitalSignalChain& dspChain, MockOutputModule& mockOutput, float setting = 2.0)
{
    dspChain.applyEffects(sample, setting);

    // std::cout << "[test.cpp] Time: " << sample.getTimeIndex() << "s, PCM: " << std::fixed << std::setprecision(4) << sample.getPcmValue();

    //for (const auto& effect : sample.getAppliedEffects()) {std::cout << " [" << effect << "]";}

    std::cout << "\r" << std::flush;

    mockOutput.writeSample(sample);
}

/**
 * @brief Entry point for the real-time harmoniser pedal test.
 * Loads an input WAV, registers a DSP effect, processes each sample, and writes to output.
 */
int main(int argc, char* argv[])
{
    std::cout << "Real-Time Pedal: Testing Mode\n";

     if (argc < 3) {
        std::cerr << "[Usage] " << argv[0] << " <input.wav> <output.wav>\n";
        return EXIT_FAILURE;
    }

    const std::string inputWavFilePath = argv[1];
    const std::string outputWavFilePath = argv[2];

    MockSamplingModule mockSampler(inputWavFilePath);
    MockOutputModule mockOutput(outputWavFilePath);
    DigitalSignalChain dspChain;

    // Optional: call to anchor OctaveDoubler object file during linking (only needed if effect fails to register)
    ForceAllEffects();

    dspChain.loadEffectsFromFile("code/assets/effects_chain.txt");

    // Process audio sample-by-sample
    float pcm;
    double timeIndex = 0.0;
    constexpr double sampleRate = 44100.0;
    constexpr double timeStep = 1.0 / sampleRate;

    while (mockSampler.getSample(pcm)) {
        Sample sample(pcm, timeIndex);
        processSample(sample, dspChain, mockOutput);
        timeIndex += timeStep;
    }

    std::cout << "\n [test.cpp]  All samples processed. Writing output file...\n";
    mockOutput.saveToFile();

    std::cout << "[test.cpp]  Done.\n";
    return 0;
}
