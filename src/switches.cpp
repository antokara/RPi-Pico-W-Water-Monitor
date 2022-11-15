#include <ArduinoHA.h>
#include "switches.h"
#include "pressureSensor.h"

HASwitch Switches::debugSwitch("waterMonitorDebug");
HASwitch Switches::waterLeakTestSwitch("waterMonitorLeakTest");

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
    Switches::debugSwitch.setIcon("mdi:test-tube");
    Switches::debugSwitch.setName("Water Monitor Debug");
    Switches::debugSwitch.onCommand(Switches::onSwitchCommand);

    Switches::waterLeakTestSwitch.setIcon("mdi:water-alert-outline");
    Switches::waterLeakTestSwitch.setName("Water Leak Test");
    Switches::waterLeakTestSwitch.onCommand(Switches::onSwitchCommand);
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
        Switches::debugSwitch.setState(Switches::isDebugActive);
        Switches::setIsWaterLeakTestActive(Switches::isWaterLeakTestActive);
        Switches::waterLeakTestSwitch.setState(Switches::isWaterLeakTestActive);
    }
}