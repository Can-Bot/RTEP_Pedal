#include "Harmonizer.h"

int main() {
    Harmonizer harmonizer("input.wav", "output.wav", 2);
    harmonizer.process();
    harmonizer.updateInputs("input.wav", "output2.wav", 3);
    harmonizer.process();
    return 0;
}