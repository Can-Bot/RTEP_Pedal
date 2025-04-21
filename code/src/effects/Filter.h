#pragma once

#include "Effect.h"
#include "Iir.h"
#include <iostream>

/**
 * @class Filter
 * @brief A real-time Butterworth low-pass filter effect.
 *
 * Configurable parameters via Config:
 * - "cutoff": cutoff frequency in Hz (float)
 * - "sampleRate": sample rate in Hz (float)
 */
class Filter : public Effect
{
public:
    Filter();
    ~Filter();

    float process(float sample) override;

protected:
    void parseConfig(const Config &config) override;

private:
    static constexpr int order = 4;
    Iir::Butterworth::LowPass<order> lowpass;

    float sampleRate = 44100.0f;
    float cutoff = 500.0f;
    bool configured = false;
};
