#include "constants.hpp"
#include "settings.hpp"
#include "i2c_t3.h"
#include "analog.hpp"
#include "communication.hpp"

std::array<uint8_t, 64> buffer_rx;      // uint8_t == "byte"
std::array<uint8_t, 64> buffer_tx;      // transfer buffer
int err_code = 0;                       /// HID communication status
bool is_sampling = false;               /// false is settings, true is sampling
Settings settings(100, false, false);   // default to 100 hz, "raw" mode, verbosity off
std::array<uint16_t, 20> recent_values; /// mildly strong assumption that we're always reading 16-bit ints
std::array<float, 15> converted_recent_values;

elapsedMillis adc_data_timestamp;
unsigned long timestamp = 0;
elapsedMicros between_adc_readings_timer;
int deviation = 0;

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

#ifndef NOHARDWARE /// disable communication via I2C (allows us to develop without the full device)
    Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
    Wire2.setDefaultTimeout(200000);
// setup PGA
// setup multiplexer
// run calibration
#endif
    analog::setupADC();
    adc_data_timestamp = 0;
    between_adc_readings_timer = 0;
}

void loop()
{
    if (is_sampling)
    {
        // take single read of all channels
        // get timestamp here
        analog::readAllOnce(recent_values);

        if (settings.getGameMode())
        { // if in "game" mode, perform rotation and send data
            analog::applyRotation(recent_values, converted_recent_values);
            communication::sendSample(converted_recent_values, buffer_tx);
        }
        else
        { // send data immediately
            communication::sendSample(timestamp, deviation, recent_values, buffer_tx);
        }
    }
    // check for data *after* read, so the time it takes to do that
    // is folded into the busy wait
    // if there is a new message, our timing will be borked anyway
    err_code = communication::receiveData(buffer_rx); // Check for any new messages from host
    if (err_code > 0)                                 // Deal with parsing apart the message and evaluate state changes
    {
        handleInput(is_sampling, buffer_rx, settings);
    }

    if (is_sampling)
    {
        // busy wait until the remained of the period has elapsed
    }
}
