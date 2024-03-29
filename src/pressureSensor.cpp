#include <ArduinoHA.h>
#include "device.h"
#include "switches.h"
#include "pressureSensor.h"

/**
 * @brief the delta in PSI that must be great or equal to,
 * between the previous reading and the current,
 * in order to consider updating it and sending it to the controller
 *
 * note: this changes depending on the mode (ie. water leak test active)
 * @see src/switches.cpp
 */
float PressureSensor::pressureDelta = PRESSURE_SENSOR_DELTA;

/**
 * @brief frequency in milliseconds,
 * to allow sending of the water pressure to the controller,
 * even if the delta is greater.
 *
 * note: this changes depending on the mode (ie. water leak test active)
 * @see src/switches.cpp
 */
unsigned int PressureSensor::sendPressureFrequency = PRESSURE_SENSOR_SEND_FREQUENCY;

// the adjusted/actual minimum/max input value the pressure sensor pin can provide,
const float PressureSensor::adjustedMinPressureSensorInputValue = Device::analogInputValueMultiplier * MIN_PRESSURE_SENSOR_VOLTAGE;
const float PressureSensor::adjustedMaxPressureSensorInputValue = Device::analogInputValueMultiplier * MAX_PRESSURE_SENSOR_VOLTAGE;

// the adjusted/actual number we need to multiply the input value - adjustedMinPressureSensorInputValue,
// in order to get the true PSI of the sensor
const float PressureSensor::adjustedPressureSensorInputValueMultiplier = MAX_PRESSURE_SENSOR_PSI / PressureSensor::adjustedMaxPressureSensorInputValue * PRESSURE_SENSOR_PSI_CALIBRATION_MULTIPLIER;

// current PSI
float PressureSensor::psi = 0.0;

// previous PSI (so we only send changes)
float PressureSensor::prevPsi = 0.0;

// last time we sent the pressure
unsigned long PressureSensor::lastPressureSendTime = 0;

// the water pressure sensor
HASensorNumber PressureSensor::psiSensor("waterMonitorPressure", HASensorNumber::PrecisionP2);

void PressureSensor::setup()
{
    PressureSensor::psiSensor.setName("Water Pressure");
    PressureSensor::psiSensor.setIcon("mdi:gauge");
    PressureSensor::psiSensor.setDeviceClass("pressure");
    PressureSensor::psiSensor.setUnitOfMeasurement("psi");
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
    return abs(long(millis() - PressureSensor::lastPressureSendTime)) > PressureSensor::sendPressureFrequency;
}

void PressureSensor::loop()
{
    int rawPressureSensorInputValue = analogRead(PRESSURE_SENSOR_PIN); // read the input pin
    PressureSensor::psi = (rawPressureSensorInputValue - PressureSensor::adjustedMinPressureSensorInputValue) * PressureSensor::adjustedPressureSensorInputValueMultiplier;
    if (abs(PressureSensor::psi - PressureSensor::prevPsi) >= PressureSensor::pressureDelta && PressureSensor::shouldSendPSI())
    {
        PressureSensor::prevPsi = PressureSensor::psi;
        PressureSensor::lastPressureSendTime = millis();

#ifdef SERIAL_DEBUG
        Serial.print("raw: ");
        Serial.println(rawPressureSensorInputValue);
        Serial.print("PSI: ");
        Serial.println(PressureSensor::psi);
#endif
        if (Switches::isDebugActive)
        {
            Device::mqtt.publish(PRESSURE_SENSOR_DEBUG_MQTT_TOPIC, String("raw PSI input: " + String(rawPressureSensorInputValue) + ", PSI: " + String(PressureSensor::psi)).c_str());
        }

        // only send a minimum of zero PSI
        // to not mess up the statistics/logs
        if (PressureSensor::psi > 0)
        {
            PressureSensor::psiSensor.setValue(PressureSensor::psi);
        }
        else
        {
            PressureSensor::psiSensor.setValue(float(0.0));
        }
    }
    else if (Device::reconnected)
    {
        /**
         * @brief only upon reconnection (the reconnect flag lasts only one loop)
         * send the current GPM to the controller, in case for example, the flow stopped
         * while we were disconnected, so that the controller gets this value "update"...
         */
        PressureSensor::psiSensor.setValue(PressureSensor::psi, true);
    }
}