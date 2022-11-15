#include <ArduinoHA.h>
#include "switches.h"
#include "pressureSensor.h"

HASwitch Switches::waterLeakTestSwitch("waterLeakTest");
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
        PressureSensor::pressureDelta = 0.1;
        PressureSensor::sendPressureFrequency = 5000;
    }
    else
    {
        // normal mode does not need high accuracy or refresh rate
        PressureSensor::pressureDelta = 1;
        PressureSensor::sendPressureFrequency = 30000;
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
void Switches::onWaterLeakTestSwitchCommand(bool state, HASwitch *sender)
{
    // report state back to the Home Assistant
    sender->setState(state);

    // make the changes necessary to enable this mode
    Switches::setIsWaterLeakTestActive(state);
}

/**
 * @brief called when state changes remotely, from the controller
 *
 * @param state
 */
void Switches::onWaterMonitorDebugSwitchCommand(bool state, HASwitch *sender)
{
    // report state back to the Home Assistant
    sender->setState(state);

    // make the changes necessary to enable this mode
    Switches::setIsDebugActive(state);
}

void Switches::setup()
{
    Switches::waterLeakTestSwitch.setIcon("mdi:water-alert-outline");
    Switches::waterLeakTestSwitch.setName("Water Leak Test");
    Switches::waterLeakTestSwitch.onCommand(Switches::onWaterLeakTestSwitchCommand);

    Switches::debugSwitch.setIcon("mdi:test-tube");
    Switches::debugSwitch.setName("Debug");
    Switches::debugSwitch.onCommand(Switches::onWaterMonitorDebugSwitchCommand);
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