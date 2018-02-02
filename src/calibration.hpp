#ifndef CALIB_H
#define CALIB_H

#include "settings.hpp"
#include "multipga.hpp"
#include <array>

namespace calibration
{
/**
     * @brief Calibrate a single channel.
     * 
     * @param mux_channel is the target to calibrate (not the analog pin, but 0-19!)
     * @param gain_vec is a reference to a std::array containing the requested gains and offsets for that particular channel
     * @return int for success (0) or failure (1)
     * 
     * @note We *could* return a bool, but int may give us flexibility (e.g. return 2 for bad fine adjustment?)
     */
int calibrateChannel(uint8_t mux_channel, std::array<float, 6> &gain_vec, MultiPGA &multi_pga);

/**
     * @brief Calls calibration::calibrateChannel on all channels.
     * 
     * @param pga_settings is a reference to a PGASettings instantiation, which is a 5x4x6 nested array.
     * @return int for success (0) or failure (1).
     * 
     * @note Return currently does not do anything useful.
     */
int calibrateAllChannels(PGASettings &pga_settings, MultiPGA &multi_pga);

/**
     * @brief Estimate the input voltage, given the output voltage and gain/offset settings.
     * 
     * @param val is the measured output voltage, in millivolts
     * @param gain_vec is a reference to a std::array containing the requested gains and offsets for that particular channel
     * @return the estimated input voltage as a float (in millivolts)
     */
float inverseTransferFunction(float val, std::array<float, 6> &gain_vec);
};
#endif
