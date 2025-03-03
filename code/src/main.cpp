#include <cstdio>
#include <iostream>
#include <vector>
#include <sndfile.h>
#include "../lib/stretch/signalsmith-stretch.h"

#include "../lib/stretch/cmd/util/stopwatch.h"
#include "../lib/stretch/cmd/util/memory-tracker.hxx"

#include <iostream>
#define LOG_EXPR(expr) std::cout << #expr << " = " << (expr) << "\n";

#include "../lib/stretch/cmd/util/simple-args.h"
#include "../lib/stretch/cmd/util/wav.h"

std::string pitchShift(const std::string& inputWav, const float semitones, const float exactLength) {
    signalsmith::Stopwatch stopwatch;

    signalsmith::stretch::SignalsmithStretch<float/*, std::mt19937*/> stretch;
    Wav inWav;
    if (!inWav.read(inputWav).warn()) std::cerr << "failed to read WAV";
    size_t inputLength = inWav.samples.size() / inWav.channels;

    Wav outWav;
    outWav.channels = inWav.channels;
    outWav.sampleRate = inWav.sampleRate;
    
    float timeFactor = 1.0f; 

    int outputLength = std::round(inputLength * timeFactor);  

    stretch.presetDefault(inWav.channels, inWav.sampleRate);
    stretch.setTransposeSemitones(semitones, 1);

    size_t paddedInputLength = inputLength + stretch.inputLatency();
    inWav.samples.resize(paddedInputLength * inWav.channels);

    int tailSamples = exactLength ? stretch.outputLatency() : (stretch.outputLatency() + stretch.inputLatency());
    int paddedOutputLength = outputLength + tailSamples;
    outWav.samples.resize(paddedOutputLength * outWav.channels);

    signalsmith::MemoryTracker processMemory;

    stopwatch.start();
    stretch.seek(inWav, stretch.inputLatency(), 1.0f / timeFactor);
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
        if (processMemory) std::cerr << "allocated during process()";
    }

    if (!outWav.write("output.wav").warn()) std::cerr << "failed to write WAV";

    return "penis";
}


int main(int argc, char* argv[]) {
	pitchShift("input.wav", 3, 0);
}