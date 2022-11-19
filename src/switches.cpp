#include <ArduinoHA.h>
#include "switches.h"
#include "pressureSensor.h"

HASwitch Switches::waterLeakTestSwitch("waterMonitorLeakTest");
HASwitch Switches::debugSwitch("waterMonitorDebug");

/**
 * @brief controls the Water Leak Test mode of the device
 *        if true, the test mode is enabled.
 */
bool Switches::isWaterLeakTestActive = false;

/**
 * @brief controls the water monitor debug mode on MQTT
 */
bool Switches::isDebugActive = false;

/**
 * @brief to keep track of the first loop iteration
 */
bool Switches::firstLoop = true;

void Switches::setIsWaterLeakTestActive(bool state)
{
    // keep our local state
    Switches::isWaterLeakTestActive = state;

    if (state)
    {
        // test mode needs high accuracy and refresh rate
        PressureSensor::pressureDelta = PRESSURE_SENSOR_DELTA_WATER_LEAK_TEST_ACTIVE;
        PressureSensor::sendPressureFrequency = PRESSURE_SENSOR_SEND_FREQUENCY_WATER_LEAK_TEST_ACTIVE;
    }
    else
    {
        // normal mode does not need high accuracy or refresh rate
        PressureSensor::pressureDelta = PRESSURE_SENSOR_DELTA;
        PressureSensor::sendPressureFrequency = PRESSURE_SENSOR_SEND_FREQUENCY;
    }
}

void Switches::setIsDebugActive(bool state)
{
    // keep our local state
    Switches::isDebugActive = state;
}

/**
 * @brief called when state changes remotely, from the controller
 *
 * @param state
 */
void Switches::onSwitchCommand(bool state, HASwitch *sender)
{
    if (sender == &Switches::waterLeakTestSwitch)
    {
        Switches::setIsWaterLeakTestActive(state);
    }
    else if (sender == &Switches::debugSwitch)
    {
        Switches::setIsDebugActive(state);
    }

    // report state back to the Home Assistant
    sender->setState(state);
}

void Switches::setup()
{
    Switches::waterLeakTestSwitch.setIcon("mdi:water-alert-outline");
    Switches::waterLeakTestSwitch.setName("Water Leak Test");
    Switches::waterLeakTestSwitch.onCommand(Switches::onSwitchCommand);

    Switches::debugSwitch.setIcon("mdi:test-tube");
    Switches::debugSwitch.setName("Water Monitor Debug");
    Switches::debugSwitch.onCommand(Switches::onSwitchCommand);
}

void Switches::loop()
{
    /**
     * @brief only on the first loop, reset the state,
     * in case there was a previous state that is now invalid.
     */
    if (Switches::firstLoop)
    {
        Switches::firstLoop = false;
        Switches::setIsWaterLeakTestActive(Switches::isWaterLeakTestActive);
        Switches::waterLeakTestSwitch.setState(Switches::isWaterLeakTestActive);
        Switches::debugSwitch.setState(Switches::isDebugActive);
    }
}