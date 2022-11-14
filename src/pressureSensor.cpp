#include <ArduinoHA.h>
#include "device.h"
#include "pressureSensor.h"

// the adjusted/actual minimum/max input value the pressure sensor pin can provide,
const float adjustedMinPressureSensorInputValue = analogInputValueMultiplier * MIN_PRESSURE_SENSOR_VOLTAGE;
const float adjustedMaxPressureSensorInputValue = analogInputValueMultiplier * MAX_PRESSURE_SENSOR_VOLTAGE;

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
        Serial.print("raw: ");
        Serial.println(rawPressureSensorInputValue);
        Serial.print("PSI: ");
        Serial.println(psi);
        // only send a minimum of zero PSI
        // to not mess up the statistics/logs
        if (psi > 0)
        {
            psiSensor.setValue(psi);
        }
        else
        {
            psiSensor.setValue(float(0.0));
        }
    }
}