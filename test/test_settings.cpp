#include "../src/settings.hpp"
#include <iostream>
// g++ test_settings.cpp ../src/settings.cpp -std=c++0x -o temp.exe
int main() {
    Settings settings(420, true, false);
    std::cout << settings.getSamplingFrequency() << "\n";
    std::cout << settings.sampling_period_us << "\n";
    std::cout << settings.getGain(1, 3, 2) << "\n";
    settings.setGain(3, -1, 3, 69);
    std::cout << settings.getGain(3, 1, 0) << "\n";
}