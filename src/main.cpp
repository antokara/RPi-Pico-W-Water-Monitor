#include <ArduinoHA.h>
#include "device.h"
#include "pulseSensor.h"
#include "pressureSensor.h"
#include "switches.h"

// uncomment to enable serial.print debug messages
// #define SERIAL_DEBUG

void setup()
{
    Device::setup();
    Switches::setup();
    PulseSensor::setup();
    PressureSensor::setup();
    // after everything is setup...
    Device::connectToMQTT();
}

void loop()
{
    Device::loop();
    Switches::loop();
    PulseSensor::loop();
    PressureSensor::loop();
}