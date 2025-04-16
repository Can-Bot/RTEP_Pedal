// ForceEffects.cpp
#include <iostream>

extern "C" void ForceLink_OctaveDoubler_Effect();
extern "C" void ForceLink_Harmonizer_Effect();
extern "C" void ForceLink_Fuzz_Effect();
extern "C" void ForceLink_Gain_Effect();

void ForceAllEffects() {
    std::cout << "[ForceEffects] Linking OctaveDoubler\n";
    ForceLink_OctaveDoubler_Effect();
    std::cout << "[ForceEffects] Linking Harmonizer\n";
    ForceLink_Harmonizer_Effect();
    std::cout << "[ForceEffects] Linking Fuzz\n";
    ForceLink_Fuzz_Effect();
    std::cout << "[ForceEffects] Linking Gain\n";
    ForceLink_Gain_Effect();
    // Future effects just get auto-added here (or auto-discovered in a generator)
}
