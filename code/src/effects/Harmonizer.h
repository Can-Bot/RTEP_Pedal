#ifndef HARMONIZER_H
#define HARMONIZER_H

#include <string>
#include <vector>
#include <chrono>
#include <sndfile.h>
#include "Effect.h"
#include "../lib/signalsmith-stretch/signalsmith-stretch.h"
#include "../lib/signalsmith-stretch/cmd/util/stopwatch.h"
#include "../lib/signalsmith-stretch/cmd/util/wav.h"

/**
 * @class Harmonizer
 * @brief An audio effect that applies pitch shifting using the SignalsmithStretch algorithm.
 *
 * This class supports both offline (block-based) and real-time (sample-by-sample) pitch shifting.
 * It can also layer multiple pitch-shifted voices to form chords and export the result as a WAV file.
 */
class Harmonizer : public Effect {
public:
    /**
     * @brief Constructs a harmonizer with specified input/output WAV files and pitch semitone values.
     * @param inputWav Input WAV filename relative to "assets/".
     * @param outputWav Output WAV filename relative to "assets/".
     * @param semitones List of semitone shifts to apply (can be used for chords).
     */
    Harmonizer(const std::string& inputWav = "input.wav", 
               const std::string& outputWav = "output.wav", 
               const std::vector<int>& semitones = {0});

    /**
     * @brief Updates the input/output filenames and semitone values without recreating the object.
     * @param inputWav New input WAV path.
     * @param outputWav New output WAV path.
     * @param semitones New pitch shifts to apply.
     */
    void updateInputs(const std::string& inputWav = "input.wav", 
                      const std::string& outputWav = "output.wav", 
                      const std::vector<int>& semitones = {0});
    
    /**
     * @brief Processes the current input WAV and writes the result to the output.
     * @param iteration Index of the semitone to apply from the semitones vector.
     * @return True on success.
     */
    bool process(int iteration = 0);

    /**
     * @brief Generates a chord by layering multiple pitch-shifted versions of the input WAV.
     * @return True on success.
     */

    /**
     * @brief Processes an individual sample in real-time mode.
     * @param sample The input sample.
     * @param setting Semitone shift (float value).
     * @return The pitch-shifted output sample.
     */
    float process(float sample) override;
    
    ~Harmonizer();
private:
    // === Real-time processing state ===
    bool stretchInitialized = false;
    float lastSemitoneSetting = std::numeric_limits<float>::quiet_NaN();
    std::vector<float> inputBuffer;
    std::vector<std::vector<float>> outputBuffers;
    size_t inputWriteIndex = 0;
    size_t outputReadIndex = 0;
    size_t blockSize = 64;
    int sampleRate = 44100;

    std::chrono::high_resolution_clock::time_point realtimeStart;
    size_t samplesProcessed = 0;

    /**
     * @brief Initializes internal buffers and the stretch object for real-time streaming.
     */
    void initRealtimeStretch();

    // === Offline processing configuration ===
    std::string inputWav;
    std::string outputWav;
    std::vector<int> semitones;
    std::vector<std::chrono::high_resolution_clock::time_point> realtimeSampleTimestamps;

    int currentSemitone = 0;
    double tonality = 8000.0;  //< Controls tonality detection for pitch shifting.
    double time = 1.0;         //< Stretch ratio (1.0 = no stretch).
    bool exactLength = false;

    // === Audio data structures ===
    Wav inWav;
    Wav outWav;
    signalsmith::stretch::SignalsmithStretch<float> stretch;

    // === Profiling & Memory ===
    signalsmith::Stopwatch stopwatch;

    // === Internal helpers ===
    void setupStretch(int currentSemitone);
    void processAudio();
    void reportMemoryUsage();
    void reportProcessingStats(double processSeconds, double processRate, double processPercent);
    void data_processing(double* data, int count, int channels);
    std::vector<signalsmith::stretch::SignalsmithStretch<float>> stretches;


    /**
     * @brief Merges two WAV files into one by blending their contents.
     * @param infilename First input WAV.
     * @param infilename2 Second input WAV.
     * @param outfilename Destination merged WAV.
     * @return Path to the resulting merged file.
     */

protected:
    void parseConfig(const Config &config) override;
};

#endif // HARMONIZER_H
