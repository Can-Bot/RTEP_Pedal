#include "Fuzz.h"
#include "EffectRegistration.h"
#include <cmath>
#include <iomanip>
#include <iostream>

Fuzz::Fuzz()
{
    IsActive = true;
    Setting = 1.0f; // Default threshold
}

float Fuzz::process(float sample)
{
    if (!IsActive)
    {
        return sample;
    }

    float threshold = 1024.0f;
    try
    {
        gain = 1 + std::any_cast<float>(Setting); //add setting to gain
        threshold = (1 -  std::any_cast<float>(Setting)) * 1024; //inversely proportional to setting
    }
    catch (...)
    {
        threshold = 1.0f; // Fallback threshold
    }
    sample *= gain; //Multiple by gain factor
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
