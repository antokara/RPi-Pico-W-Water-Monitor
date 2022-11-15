#include <ArduinoHA.h>
#include "switches.h"
#include "pressureSensor.h"

HASwitch Switches::waterLeakTestSwitch("waterLeakTest");

/**
 * @brief controls the Water Leak Test mode of the device
 *        if true, the test mode is enabled.
 */
bool Switches::isWaterLeakTestActive = false;

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

void Switches::onWaterLeakTestSwitchCommand(bool state, HASwitch *sender)
{
    // report state back to the Home Assistant
    sender->setState(state);

    // make the changes necessary to enable this mode
    Switches::setIsWaterLeakTestActive(state);
}

void Switches::setup()
{
    Switches::waterLeakTestSwitch.setIcon("mdi:water-alert-outline");
    Switches::waterLeakTestSwitch.setName("Water Leak Test");
    Switches::waterLeakTestSwitch.onCommand(Switches::onWaterLeakTestSwitchCommand);
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
        Switches::waterLeakTestSwitch.setState(Switches::isWaterLeakTestActive);
        Switches::setIsWaterLeakTestActive(Switches::isWaterLeakTestActive);
    }
}