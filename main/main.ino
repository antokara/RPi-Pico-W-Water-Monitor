#include <ArduinoHA.h>
#include "device.h"
#include "led.h"
#include "pulseSensor.h"

// HASensorNumber psiSensor("psi", HASensorNumber::PrecisionP2);

//     statusSensor.setValue("ok");
//     gpmSensor.setValue(gpm);
//     psiSensor.setValue(psi);
//     gallonsSensor.setValue(gallons);

void setup()
{
    Device::setup();
    Led::setup();

    // psiSensor.setName("Water Pressure");
    // psiSensor.setIcon("mdi:gauge");
    // psiSensor.setDeviceClass("pressure");
    // psiSensor.setUnitOfMeasurement("psi");
    Device::connectoMQTT();
}

void loop()
{
    Device::loop();
}