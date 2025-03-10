#include "Harmonizer.h"
#include <iostream>
#include <cmath>

// Constructor
Harmonizer::Harmonizer(const std::string& inputWav, const std::string& outputWav, int semitones)
    : inputWav(inputWav), outputWav(outputWav), semitones(semitones) {}

void Harmonizer::updateInputs(const std::string& inputWav, const std::string& outputWav, int semitones) {
    this->inputWav = inputWav;
    this->outputWav = outputWav;
    this->semitones = semitones;
}
    

void Harmonizer::setupStretch() {
    if (!inWav.read(inputWav).warn()) { // If it can't read the file...
        std::cerr << "Error reading input WAV file!" << std::endl;
        return;
    }
    
    size_t inputLength = inWav.samples.size() / inWav.channels;
    outWav.channels = inWav.channels;
    outWav.sampleRate = inWav.sampleRate;
    
    int outputLength = std::round(inputLength * time);

    stretch.presetDefault(inWav.channels, inWav.sampleRate);
    stretch.setTransposeSemitones(semitones, tonality / inWav.sampleRate);

    size_t paddedInputLength = inputLength + stretch.inputLatency();
    inWav.samples.resize(paddedInputLength * inWav.channels);

    int tailSamples = exactLength ? stretch.outputLatency() : (stretch.outputLatency() + stretch.inputLatency());
    int paddedOutputLength = outputLength + tailSamples;
    outWav.samples.resize(paddedOutputLength * outWav.channels);
}

void Harmonizer::processAudio() {
    size_t inputLength = inWav.samples.size() / inWav.channels;
    size_t outputLength = std::round(inputLength * time);

    stretch.seek(inWav, stretch.inputLatency(), 1 / time);
    inWav.offset += stretch.inputLatency();
    
    stretch.process(inWav, inputLength, outWav, outputLength);
    outWav.offset += outputLength;
    stretch.flush(outWav, stretch.outputLatency());
    outWav.offset -= outputLength;
}

void Harmonizer::reportMemoryUsage() {
    if (processMemory.implemented) {
        std::cout << "\tallocated " << (processMemory.allocBytes / 1000) << "kB, freed " << (processMemory.freeBytes / 1000) << "kB\n";
    }
}

void Harmonizer::reportProcessingStats(double processSeconds, double processRate, double processPercent) {
    std::cout << "Process:\n\t" << processSeconds << "s, " << processRate << "x realtime, " << processPercent << "% CPU\n";
    reportMemoryUsage();
}

bool Harmonizer::process() {
    setupStretch(); // Setup
    stopwatch.start(); // Used to report on the time taken for pitch shifting, useful for testing
    processAudio();
    double processSeconds = stopwatch.seconds(stopwatch.lap()); // Shows how long it took to process the audio
    double processRate = (inWav.length() / inWav.sampleRate) / processSeconds; // Shows how fast pitch shifting is compared to length of input
    double processPercent = 100 / processRate; // Shows the CPU usage
    // reportProcessingStats(processSeconds, processRate, processPercent); // Displays the stats of the pitch shifting
    outWav.write(outputWav); // Creates the new .Wav file
    return true;
}
