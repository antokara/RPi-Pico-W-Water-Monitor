#include <ArduinoHA.h>
#include "device.h"
#include "pulseSensor.h"
#include "pressureSensor.h"

void setup()
{
    Device::setup();
    PulseSensor::setup();
    PressureSensor::setup();
    // after everything is setup...
    Device::connectoMQTT();
}

void loop()
{
    Device::loop();
    PulseSensor::loop();
    PressureSensor::loop();
}