#include <iostream>
#define LOG_EXPR(expr) std::cout << #expr << " = " << (expr) << "\n";
#include "../lib/stretch/signalsmith-stretch.h"
#include "../lib/stretch/cmd/util/stopwatch.h" // For testing, can be removed later
#include "../lib/stretch/cmd/util/memory-tracker.hxx"
#include "../lib/stretch/cmd/util/wav.h"

int pitchShift(const std::string& inputWav, int semitones) {
	signalsmith::stretch::SignalsmithStretch<float/*, std::mt19937*/> stretch;

	std::string outputWav = "output.wav";
	double tonality = 8000;
	double time = 1;
	bool exactLength = false;
	Wav inWav;
	if (!inWav.read(inputWav).warn());
	size_t inputLength = inWav.samples.size()/inWav.channels;

	Wav outWav;
	outWav.channels = inWav.channels;
	outWav.sampleRate = inWav.sampleRate;
	int outputLength = std::round(inputLength*time);

	signalsmith::MemoryTracker initMemory;
	signalsmith::Stopwatch stopwatch;

	stretch.presetDefault(inWav.channels, inWav.sampleRate);
	stretch.setTransposeSemitones(semitones, tonality/inWav.sampleRate);
	initMemory = initMemory.diff();

	size_t paddedInputLength = inputLength + stretch.inputLatency();
	inWav.samples.resize(paddedInputLength*inWav.channels);
	int tailSamples = exactLength ? stretch.outputLatency() : (stretch.outputLatency() + stretch.inputLatency());
	int paddedOutputLength = outputLength + tailSamples;
	outWav.samples.resize(paddedOutputLength*outWav.channels);

	signalsmith::MemoryTracker processMemory;

	stopwatch.start();
	stretch.seek(inWav, stretch.inputLatency(), 1/time);
	inWav.offset += stretch.inputLatency();
	stretch.process(inWav, inputLength, outWav, outputLength);
	outWav.offset += outputLength;
	stretch.flush(outWav, tailSamples);
	outWav.offset -= outputLength;

	double processSeconds = stopwatch.seconds(stopwatch.lap());
	double processRate = (inWav.length()/inWav.sampleRate)/processSeconds;
	double processPercent = 100/processRate;
	processMemory = processMemory.diff();
	std::cout << "Process:\n\t" << processSeconds << "s, " << processRate << "x realtime, " << processPercent << "% CPU\n";
	if (processMemory.implemented) {
		std::cout << "\tallocated " << (processMemory.allocBytes/1000) << "kB, freed " << (processMemory.freeBytes/1000) << "kB\n";
		if (processMemory);
	}
	outWav.write(outputWav);
    return 1;
}

int main() {
	pitchShift("input.wav", 4);
}