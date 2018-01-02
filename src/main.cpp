#include "constants.hpp"
#include "settings.hpp"
#include "i2c_t3.h"

uint8_t buffer_rx[64]; // uint8_t == "byte"
int hid_code = 0; /// HID communication status
bool is_sampling = false; /// false is settings, true is sampling
Settings settings(100, false, false); // default to 100 hz, "raw" mode, verbosity off
std::array<uint8_t, 20> recent_values;

void setup()
{
    // put all the analog pins in INPUT mode
    for (const auto& pin : constants::pin::sensor_pins) {
        pinMode(pin, INPUT);
    }
    pinMode(LED_BUILTIN, OUTPUT); // Toggle onboard LED
    pinMode(constants::pin::led_for_calibration, OUTPUT); // Toggle calibration LED
    pinMode(constants::pin::led_for_adc, OUTPUT); // Toggle ADC LED
    // Ensure LEDs are off by default
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(constants::pin::led_for_calibration, LOW);
    digitalWrite(constants::pin::led_for_adc, LOW);

    #ifndef NOHARDWARE
    Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
    Wire2.setDefaultTimeout(200000);
    // setup PGA
    // setup multiplexer
    // run calibration
    #endif
    // setup ADCs
}

void loop()
{
    hid_code = RawHID.recv(buffer_rx, 0); // Check for any new messages from host
    if (hid_code > 0) // Deal with parsing apart the message and evaluate state changes
    {
        handleInput(is_sampling, buffer_rx, settings);
    }

    if (is_sampling) {
        // take single read of all channels
        // if in "game" mode, perform rotation
        // send data
        // busy wait until period elapsed

    }
}

void handleInput(bool &is_sampling, uint8_t buffer_rx[64], Settings &settings) {
    // buffer_rx is always 64 elements
    // sampling_now can mutate here

    if (is_sampling) { // sampling
        if (buffer_rx[0] == 'i') {
            is_sampling = false;
        }

    } else { // idle
        switch (buffer_rx[0]) {
            case 'a': is_sampling = true; runCalibration(); break;
            case 's': {
                switch (buffer_rx[1]) { // check setting type
                    case 'f': settings.setSamplingFrequency(100.0); break; // sampling frequency
                    case 'g': settings.setGain(200, buffer_rx[5]); break; // gain
                } break; }
            case 'g': {
                switch (buffer_rx[1]) { // check setting type
                    case 'f': settings.getSamplingFrequency; break; // sampling frequency
                    case 'g': settings.finger_gains[buffer_rx[2]]; break; // gain
            } break; }
        } // end switch
    } // end idle
} // end function
