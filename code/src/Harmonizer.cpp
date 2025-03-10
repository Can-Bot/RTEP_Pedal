#include "Harmonizer.h"
#include <iostream>
#include <cmath>

Harmonizer::Harmonizer(const std::string& inputFile, int semitones)
    : inputWav(inputFile), semitones(semitones) {}

void Harmonizer::process() {
    double tonality = 8000;
    double time = 1;
    bool exactLength = false;

    Wav inWav;
    if (!inWav.read(inputWav).warn());
    size_t inputLength = inWav.samples.size() / inWav.channels;

    Wav outWav;
    outWav.channels = inWav.channels;
    outWav.sampleRate = inWav.sampleRate;
    int outputLength = std::round(inputLength * time);

    signalsmith::MemoryTracker initMemory;
    signalsmith::Stopwatch stopwatch;

    stretch.presetDefault(inWav.channels, inWav.sampleRate);
    stretch.setTransposeSemitones(semitones, tonality / inWav.sampleRate);
    initMemory = initMemory.diff();

    size_t paddedInputLength = inputLength + stretch.inputLatency();
    inWav.samples.resize(paddedInputLength * inWav.channels);
    int tailSamples = exactLength ? stretch.outputLatency() : (stretch.outputLatency() + stretch.inputLatency());
    int paddedOutputLength = outputLength + tailSamples;
    outWav.samples.resize(paddedOutputLength * outWav.channels);

    signalsmith::MemoryTracker processMemory;

    stopwatch.start();
    stretch.seek(inWav, stretch.inputLatency(), 1 / time);
    inWav.offset += stretch.inputLatency();
    stretch.process(inWav, inputLength, outWav, outputLength);
    outWav.offset += outputLength;
    stretch.flush(outWav, tailSamples);
    outWav.offset -= outputLength;

    double processSeconds = stopwatch.seconds(stopwatch.lap());
    double processRate = (inWav.length() / inWav.sampleRate) / processSeconds;
    double processPercent = 100 / processRate;
    processMemory = processMemory.diff();
    std::cout << "Process:\n\t" << processSeconds << "s, " << processRate << "x realtime, " << processPercent << "% CPU\n";
    if (processMemory.implemented) {
        std::cout << "\tallocated " << (processMemory.allocBytes / 1000) << "kB, freed " << (processMemory.freeBytes / 1000) << "kB\n";
    }
    outWav.write(outputWav);
}