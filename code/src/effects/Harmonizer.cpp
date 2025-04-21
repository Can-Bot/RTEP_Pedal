#include "Harmonizer.h"
#include "EffectRegistration.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <algorithm>

/**
 * @brief Harmonizer constructor for offline use with file paths and semitone values.
 * @param inputWav Input WAV filename (relative to assets/).
 * @param outputWav Output WAV filename (relative to assets/).
 * @param semitones List of semitone shifts to apply.
 */

Harmonizer::Harmonizer(const std::string &inputWav, const std::string &outputWav, const std::vector<int> &semitones)
    : inputWav("assets/" + inputWav), // Input WAV used for offline processing (IE Wav Files)
      outputWav("assets/" + outputWav), // The Output Wav file
      semitones(semitones) // The number of semitones to shift by
{
    IsActive = false;
}

/**
 * @brief Updates the input/output filenames and semitone values.
 * @param in New input file path.
 * @param out New output file path.
 * @param newSemitones New list of semitone shift values.
 */

void Harmonizer::updateInputs(const std::string &in, const std::string &out, const std::vector<int> &newSemitones)
{
    inputWav = in;
    outputWav = out;
    semitones = newSemitones;
}

/**
 * @brief Initializes stretch processors and buffers for real-time audio processing.
 * Only called once, on-demand.
 */

void Harmonizer::initRealtimeStretch()
{
    if (stretchInitialized) return; // Only initialize once

    stretches.resize(semitones.size()); // Create stretch processors for each semitone
    outputBuffers.resize(semitones.size());
    for (size_t i = 0; i < semitones.size(); ++i) /// Initialize each stretch processor
    {
        stretches[i].presetDefault(1, sampleRate);
        outputBuffers[i].assign(blockSize, 0.0f);
    }

    inputBuffer.resize(blockSize); // Initialize input buffer
    inputWriteIndex = 0;
    outputReadIndex = blockSize;
    stretchInitialized = true; // Mark as initialized
}

/**
 * @brief Processes a single audio sample in real time.
 * Applies pitch shifting and returns the mixed output sample.
 * @param sample The incoming audio sample.
 * @return The processed (or passthrough) audio sample.
 */

float Harmonizer::process(float sample)
{
    if (!IsActive) 
    {
        return sample; // If not active, return the sample as is
    }
    initRealtimeStretch();

    if (samplesProcessed++ == 0)
    {
        realtimeStart = std::chrono::high_resolution_clock::now();
    }

    inputBuffer[inputWriteIndex++] = sample; // Write sample to input buffer

    if (outputReadIndex < blockSize) 
    {
        float mixed = 0.0f; // Mix the output buffers
        for (const auto &buffer : outputBuffers)
        {
            mixed += buffer[outputReadIndex];
        }
        ++outputReadIndex; // Increment read index
        return mixed / outputBuffers.size();
    }

    if (inputWriteIndex >= blockSize)
    {

        float *inputs[1] = {inputBuffer.data()};
        for (size_t i = 0; i < semitones.size(); ++i)
        {
            stretches[i].setTransposeSemitones(semitones[i], tonality / sampleRate); // Set semitone for each stretch processor
            float *outputs[1] = {outputBuffers[i].data()}; // Output buffer for each stretch processor
            stretches[i].process(inputs, blockSize, outputs, blockSize); // Process the input buffer
        }

        inputWriteIndex = 0;
        outputReadIndex = 0;

        float mixed = 0.0f;
        for (const auto &buffer : outputBuffers)
        {
            mixed += buffer[outputReadIndex];
        }
        ++outputReadIndex;
        return mixed / outputBuffers.size();
    }

    return 0.0f;
}

/**
 * @brief Sets up the stretch processor for offline processing.
 * @param currentSemitone The semitone shift to apply for the current iteration.
 */

void Harmonizer::setupStretch(int currentSemitone)
{
    if (!inWav.read(inputWav).warn())
    {
        std::cerr << "Error reading input WAV file!" << std::endl;
        return;
    }

    const size_t inputLength = inWav.samples.size() / inWav.channels; // Get the number of samples
    outWav.channels = inWav.channels;
    outWav.sampleRate = inWav.sampleRate;

    const int outputLength = std::round(inputLength * time); // Calculate the output length
    stretch.presetDefault(inWav.channels, inWav.sampleRate); // Initialize the stretch processor
    stretch.setTransposeSemitones(currentSemitone, tonality / inWav.sampleRate); // Set the semitone shift

    inWav.samples.resize((inputLength + stretch.inputLatency()) * inWav.channels); // Resize input buffer
    const int tailSamples = exactLength ? stretch.outputLatency() : (stretch.outputLatency() + stretch.inputLatency());
    outWav.samples.resize((outputLength + tailSamples) * outWav.channels);
}

/**
 * @brief Performs the full offline audio stretch process.
 * Handles seeking, processing, flushing and final output offset.
 */

void Harmonizer::processAudio()
{
    const size_t inputLength = inWav.samples.size() / inWav.channels; //
    const size_t outputLength = std::round(inputLength * time);

    stretch.seek(inWav, stretch.inputLatency(), 1 / time);
    inWav.offset += stretch.inputLatency();

    stretch.process(inWav, inputLength, outWav, outputLength);
    outWav.offset += outputLength;
    stretch.flush(outWav, stretch.outputLatency());
    outWav.offset -= outputLength;
}

/**
 * @brief Processes the given semitone shift and writes result to WAV.
 * @param iteration Index of the semitone shift to apply.
 * @return True if processing succeeded.
 */

bool Harmonizer::process(int iteration)
{
    setupStretch(semitones[iteration]);
    std::cout << "Processing " << inputWav << " with " << semitones[iteration] << " semitones\n";

    processAudio();

    outputWav = "assets/output" + std::to_string(iteration) + ".wav";
    outWav.write(outputWav);
    return true;
}

/**
 * @brief Placeholder function for modifying raw audio data.
 * Currently applies a gain of 1.0.
 * @param data Audio sample buffer.
 * @param count Total number of values.
 * @param channels Number of audio channels.
 */

void Harmonizer::data_processing(double *data, int count, int channels)
{
    for (int ch = 0; ch < channels; ++ch)
        for (int i = ch; i < count; i += channels)
            data[i] *= 1.0;
}

/**
 * @brief Parses semitone shift configuration from user config.
 * Enables the harmonizer if "harmonizer" is found in config.
 * @param config The loaded configuration object.
 */

void Harmonizer::parseConfig(const Config &config)
{
    IsActive = config.contains("harmonizer");

    std::string intervalsStr = config.get<std::string>("harmonizer", "0");
    std::stringstream ss(intervalsStr);
    std::string token;
    semitones.clear();

    std::cout << "[Harmonizer] Raw interval string: \"" << intervalsStr << "\"\n";

    while (ss >> token) // space-separated
    {
        try
        {
            int val = std::stoi(token);
            semitones.push_back(val);
            std::cout << "[Harmonizer] Parsed semitone: " << val << "\n";
        }
        catch (...)
        {
            std::cerr << "[Harmonizer] Warning: invalid interval \"" << token << "\"\n";
        }
    }

    std::cout << "[Harmonizer] Total intervals loaded: " << semitones.size() << "\n";
}

/**
 * @brief Destructor that reports timing stats if real-time processing was used.
 * Prints samples processed, elapsed time, per-sample latency, and real-time ratio.
 */

Harmonizer::~Harmonizer()
{
    using namespace std::chrono;

    if (samplesProcessed > 0)
    {
        auto end = high_resolution_clock::now();
        double elapsed = duration<double>(end - realtimeStart).count();
        double perSampleUs = (elapsed / samplesProcessed) * 1e6;
        double realtimeRatio = (samplesProcessed / static_cast<double>(sampleRate)) / elapsed;

        std::cout << "[Harmonizer] Real-time stretch stats:\n";
        std::cout << "\tSamples processed: " << samplesProcessed << "\n";
        std::cout << "\tElapsed time: " << elapsed << " s\n";
        std::cout << "\tPer sample: " << perSampleUs << " µs\n";
        std::cout << "\tRealtime factor: " << realtimeRatio << "x\n";
    }
}

REGISTER_EFFECT_AUTO(Harmonizer);
