#include <ArduinoHA.h>
#include "device.h"
#include "led.h"
// #include "pulseSensor.h"
// #include "initialize.cpp"
// #include "led.cpp"

// HASensor statusSensor("status");
// HASensorNumber gpmSensor("gpm", HASensorNumber::PrecisionP2);
// HASensorNumber psiSensor("psi", HASensorNumber::PrecisionP2);
// HASensorNumber gallonsSensor("gallons", HASensorNumber::PrecisionP0);

float gpm = 123.4567;
float psi = 234.5678;
unsigned long gallons = 1;

//     statusSensor.setValue("ok");
//     gpmSensor.setValue(gpm);
//     psiSensor.setValue(psi);
//     gallonsSensor.setValue(gallons);

void setup()
{
    Device::setup();
    Led::setup();

    // set sensors
    // statusSensor.setName("Status");
    // statusSensor.setIcon("mdi:check-circle");

    // gpmSensor.setName("Water Flow");
    // gpmSensor.setIcon("mdi:water");
    // gpmSensor.setUnitOfMeasurement("gpm");

    // psiSensor.setName("Water Pressure");
    // psiSensor.setIcon("mdi:gauge");
    // psiSensor.setDeviceClass("pressure");
    // psiSensor.setUnitOfMeasurement("psi");

    // gallonsSensor.setName("Gallons Counter");
    // gallonsSensor.setIcon("mdi:counter");
    // gallonsSensor.setDeviceClass("water");
    // gallonsSensor.setUnitOfMeasurement("gal");

    Device::connectoMQTT();
}

void loop()
{
    Device::loop();
}