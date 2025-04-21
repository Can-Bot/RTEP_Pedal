#include "Filter.h"
#include "EffectRegistration.h"
#include <any>

Filter::Filter()
{
    IsActive = true;
    Setting = 500.0f; // Default cutoff
}

Filter::~Filter()
{
    std::cout << "[Filter] Filter destroyed cleanly\n";
}

void Filter::parseConfig(const Config &config)
{
    IsActive = config.contains("cutoff");

    sampleRate = config.get<float>("sampleRate", 44100.0f);
    cutoff = config.get<float>("cutoff", 500.0f);

    try
    {
        Setting = cutoff;
        lowpass.setup(sampleRate, cutoff);
        configured = true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Filter] Config error: " << e.what() << "\n";
        configured = false;
    }
}

float Filter::process(float sample)
{
    if (!IsActive || !configured)
        return sample;

    try
    {
        return lowpass.filter(sample);
    }
    catch (...)
    {
        std::cerr << "[Filter] Failed to filter sample\n";
        return sample;
    }
}

REGISTER_EFFECT_AUTO(Filter);
