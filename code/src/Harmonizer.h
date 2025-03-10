// Harmonizer.h
#ifndef HARMONIZER_H
#define HARMONIZER_H

#include <string>
#include "../lib/stretch/signalsmith-stretch.h"
#include "../lib/stretch/cmd/util/stopwatch.h"
#include "../lib/stretch/cmd/util/memory-tracker.h"
#include "../lib/stretch/cmd/util/wav.h"

class Harmonizer {
public:
    Harmonizer(const std::string& inputFile, int semitones);
    void process();

private:
    std::string inputWav;
    std::string outputWav = "output.wav";
    int semitones;
    signalsmith::stretch::SignalsmithStretch<float> stretch;
};
#endif