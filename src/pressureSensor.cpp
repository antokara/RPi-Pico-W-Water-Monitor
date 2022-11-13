#include <ArduinoHA.h>
#include "device.h"
#include "pressureSensor.h"

// the number of which we need to multiply voltage by, in order to get the expected input pin value
const float pressureSensorInputValueMultiplier = float(MAX_ANALOG_PIN_RANGE / MAX_PRESSURE_SENSOR_PIN_RANGE_VOLTAGE);

// the adjusted/actual minimum input value the pressure sensor pin can provide,
// to match the minimum pressure sensor PSI
// ie. this should be around 65, which means around 0 PSI (with some fault tolerance)
const float adjustedMinPressureSensorInputValue = pressureSensorInputValueMultiplier * MIN_PRESSURE_SENSOR_VOLTAGE;
const float adjustedMaxPressureSensorInputValue = pressureSensorInputValueMultiplier * MAX_PRESSURE_SENSOR_VOLTAGE;

// the adjusted/actual number we need to multiply the input value - adjustedMinPressureSensorInputValue,
// in order to get the true PSI of the sensor
const float adjustedPressureSensorInputValueMultiplier = MAX_PRESSURE_SENSOR_PSI / adjustedMaxPressureSensorInputValue * PRESSURE_SENSOR_PSI_CALIBRATION_MULTIPLIER;

// current PSI
float psi = 0.0;

// previous PSI (so we only send changes)
float prevPsi = 0.0;

// last time we sent the pressure
unsigned long lastPressureSendTime = 0;

// the water pressure sensor
HASensorNumber psiSensor("psi", HASensorNumber::PrecisionP2);

void PressureSensor::setup()
{
    psiSensor.setName("Water Pressure");
    psiSensor.setIcon("mdi:gauge");
    psiSensor.setDeviceClass("pressure");
    psiSensor.setUnitOfMeasurement("psi");
}

/**
 * @brief if we should send the pressure to the controller.
 * a separate throttler to keep the unsolicited updated in lifeline prioritize the flow.
 *
 * @return true
 * @return false
 */
bool PressureSensor::shouldSendPSI()
{
    return abs(long(millis() - lastPressureSendTime)) > SEND_PRESSURE_FREQUENCY;
}

void PressureSensor::loop()
{
    int rawPressureSensorInputValue = analogRead(PRESSURE_SENSOR_PIN); // read the input pin
    psi = (rawPressureSensorInputValue - adjustedMinPressureSensorInputValue) * adjustedPressureSensorInputValueMultiplier;
    if (abs(psi - prevPsi) >= PRESSURE_DELTA && PressureSensor::shouldSendPSI())
    {
        prevPsi = psi;
        lastPressureSendTime = millis();
        psiSensor.setValue(psi);
    }
}