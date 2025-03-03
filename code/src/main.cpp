#include <cstdio>
#include <iostream>
#include <vector>
#include <sndfile.h>
#include "../lib/stretch/signalsmith-stretch.h"

int main() {
    // WAV Setup
    const char *inputFile = "/home/alake/RTEP_Pedal/code/input.wav";
    SF_INFO sfInfo;
    SNDFILE *inFile = sf_open(inputFile, SFM_READ, &sfInfo);
    if (!inFile) {
        std::cerr << "Error: Could not open input file!" << std::endl;
        return -1;
    }
    if (sfInfo.channels < 1 || sfInfo.channels > 2) {
        std::cerr << "Error: Only mono and stereo WAV files are supported!" << std::endl;
        sf_close(inFile);
        return -1;
    }
    const char *outputFile = "output.wav";
    SF_INFO outSfInfo = sfInfo;
    outSfInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    SNDFILE *outFile = sf_open(outputFile, SFM_WRITE, &outSfInfo);
    if (!outFile) {
        std::cerr << "Error: Could not open output file!" << std::endl;
        sf_close(inFile);
        return -1;
    }

    const int bufferSize = 1024;
    std::vector<float> inputBuffer(bufferSize * sfInfo.channels);
    std::vector<float> outputBuffer(bufferSize * sfInfo.channels);
    std::vector<float*> inputChannels(sfInfo.channels);
    std::vector<float*> outputChannels(sfInfo.channels);
    for (int c = 0; c < sfInfo.channels; ++c) {
        inputChannels[c] = inputBuffer.data() + c;
        outputChannels[c] = outputBuffer.data() + c;
    }

    // Pitch Shift
    using Stretch = signalsmith::stretch::SignalsmithStretch<float>;
    Stretch stretch;
    int sampleRate = sfInfo.samplerate;
    stretch.presetDefault(channels, sampleRate);
    stretch.setTransposeSemitones(4);  
    while (true) {
        int framesRead = sf_readf_float(inFile, inputBuffer.data(), bufferSize);
        if (framesRead == 0) break;  // End of file
        stretch.process(inputChannels.data(), framesRead, outputChannels.data(), framesRead);
        sf_writef_float(outFile, outputBuffer.data(), framesRead);
    }

    sf_close(inFile);
    sf_close(outFile);

    std::cout << "Pitch shift completed - Output saved as 'output.wav'." << std::endl;
    return 0;
}