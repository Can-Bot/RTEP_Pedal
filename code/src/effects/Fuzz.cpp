#include "Fuzz.h"
#include "EffectRegistration.h"
#include <cmath>
#include <iostream>

Fuzz::Fuzz()
{
    IsActive = true;
    Setting = 3.0f; // Default threshold
}

float Fuzz::process(float sample)
{
    if (!IsActive)
    {
        return sample;
    }

    float threshold = 0.01f;
    try
    {
        //std::cout << "[Fuzz] Applying Fuzz\n";
        threshold *= std::any_cast<float>(Setting);
    }
    catch (...)
    {
        threshold = 1.0f; // Fallback threshold
    }

    if (sample > threshold)
        return threshold;
    if (sample < -threshold)
        return -threshold;
    return sample;
}

Fuzz::~Fuzz()
{
    std::cout << "[Fuzz] Fuzz destroyed cleanly\n";
}

void Fuzz::parseConfig(const Config &config)
{
    IsActive = config.contains("fuzz");
    Setting = config.get<float>("fuzz", 1.0f);
}

REGISTER_EFFECT_AUTO(Fuzz);
