#ifndef ANALOG_H
#define ANALOG_H
#include <array>

namespace analog
{
void setupADC();
void readAllOnce(std::array<uint16_t, 20> &recent_values);
float readChannelMillivolt(unsigned char channel);
void applyRotation(std::array<uint16_t, 20> &recent_values, std::array<float, 15> &converted_values);
};

#endif
