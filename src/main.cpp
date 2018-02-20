#include <Arduino.h>
#include <array>
#include "constants.hpp"
#include "settings.hpp"
#include "analog.hpp"
#include "multipga.hpp"
#include "calibration.hpp"
#include "ui.hpp"
#include "comm.hpp"

// Required for using the standard library, unclear why
extern "C" {
int _getpid() { return -1; }
int _kill(int pid, int sig) { return -1; }
int _write() { return -1; }
}

// "global" things
std::array<uint8_t, 64> buffer_rx; // uint8_t == "byte"

uint8_t comm_status = 0;                       ///< HID communication status
bool is_sampling = false;                      ///< false is settings, true is sampling
Settings settings(100, false);                 // default to 100 hz, "raw" mode, verbosity off
std::array<uint16_t, 20> recent_values;        ///< mildly strong assumption that we're always reading 16-bit ints
std::array<float, 15> converted_recent_values; ///< X, Y, Z forces that have been fed through applyRotation

elapsedMillis adc_data_timestamp;         ///< Time at the start of acquisition of a given sample, in ms
uint32_t timestamp = 0;                   ///< Stores the result of adc_data_timestamp (which runs away)
elapsedMicros between_adc_readings_timer; ///< Used to time consecutive calls to readAllOnce
int16_t deviation = 0;                    ///< deviation from the expected sampling period, in us

void setup()
{
    // put all the analog pins in INPUT mode
    for (const auto &pin : constants::pin::sensor_pins)
    {
        pinMode(pin, INPUT);
    }
    pinMode(LED_BUILTIN, OUTPUT);                         // Toggle onboard LED
    pinMode(constants::pin::led_for_calibration, OUTPUT); // Toggle calibration LED
    pinMode(constants::pin::led_for_adc, OUTPUT);         // Toggle ADC LED
    // Ensure LEDs are off by default
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(constants::pin::led_for_calibration, LOW);
    digitalWrite(constants::pin::led_for_adc, LOW);
    comm::setupComm(); // Only does something if Serial is being used (HID needs no init)
    analog::setupADC();

#ifndef NOHARDWARE ///< disable communication via I2C (allows us to develop without the full device)
    multipga::init();
    calibration::calibrateAllChannels(settings.pga_settings);
    // TODO: send settings to PGA
#endif
    adc_data_timestamp = 0;
    between_adc_readings_timer = 0;
    buffer_rx.fill(0); // initialize receive buffer
}

void loop()
{
    if (is_sampling)
    {
        // busy wait until the remainder of the period has elapsed
        while (between_adc_readings_timer < settings.sampling_period_us)
        {
        }
        deviation = between_adc_readings_timer - settings.sampling_period_us; // if anything, this will be negative (can never be faster than the period)
        between_adc_readings_timer = 0;
        // take single read of all channels
        timestamp = adc_data_timestamp;
        analog::readAllOnce(recent_values);

        if (settings.getGameMode())
        {                                                                  // if in "game" mode, perform rotation and send data
            analog::applyRotation(recent_values, converted_recent_values); // TODO: move applyRotation somewhere more appropriate
            comm::sendSample(converted_recent_values);
        }
        else
        { // send data immediately
            comm::sendSample(timestamp, deviation, recent_values);
        }
    }
    // check for data *after* read, so the time it takes to do that
    // is folded into the busy wait
    // if there is a new message, our timing will be borked anyway
    comm_status = comm::receiveRawPacket(buffer_rx); // Check for any new messages from host
    if (comm_status > 0)                             // Deal with parsing apart the message and evaluate state changes
    {
        ui::handleInput(is_sampling, buffer_rx, settings); // all args are pass by reference
    }
}
