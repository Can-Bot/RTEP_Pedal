#include "Harmonizer.h"
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <thread>

int main() {
    Harmonizer harmonizer("input2.wav", "output.wav", 2);
    harmonizer.process();
    // harmonizer.updateInputs("input.wav", "output2.wav", 3);
    // harmonizer.process();
    return 0;
}
