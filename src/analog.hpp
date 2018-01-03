#include <array>

namespace analog {
    void setupADC();
    void readSingle(std::array<uint16_t, 20> &recent_values);
    void applyRotation(std::array<uint16_t, 20> &recent_values, std::array<float, 15> &converted_values);
};
