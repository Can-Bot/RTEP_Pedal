// ForceEffects.cpp
#include <iostream>

extern "C" void ForceLink_Harmonizer_Effect();
extern "C" void ForceLink_Fuzz_Effect();
extern "C" void ForceLink_Gain_Effect();
extern "C" void ForceLink_Filter_Effect();

void ForceAllEffects() {
    std::cout << "[ForceEffects] Linking Harmonizer\n";
    ForceLink_Harmonizer_Effect();
    std::cout << "[ForceEffects] Linking Fuzz\n";
    ForceLink_Fuzz_Effect();
    std::cout << "[ForceEffects] Linking Gain\n";
    ForceLink_Gain_Effect();
    std::cout << "[ForceEffects] Linking Filter\n";
    ForceLink_Filter_Effect();
    // Future effects get added here 
    
}
