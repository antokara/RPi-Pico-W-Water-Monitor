#ifndef SWITCHES
#define SWITCHES
#include <ArduinoHA.h>

class Switches
{
public:
    // properties
    static bool isWaterLeakTestActive;
    static bool isDebugActive;
    static HASwitch waterLeakTestSwitch;
    static HASwitch debugSwitch;
    static bool firstLoop;

    // methods
    static void setup();
    static void loop();
    static void onWaterLeakTestSwitchCommand(bool state, HASwitch *sender);
    static void onWaterMonitorDebugSwitchCommand(bool state, HASwitch *sender);

private:
    static void setIsWaterLeakTestActive(bool state);
    static void setIsDebugActive(bool state);
};

#endif // SWITCHES