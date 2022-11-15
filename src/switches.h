#ifndef SWITCHES
#define SWITCHES
#include <ArduinoHA.h>

class Switches
{
public:
    // properties
    static bool isWaterLeakTestActive;
    static HASwitch waterLeakTestSwitch;
    static bool firstLoop;

    // methods
    static void setup();
    static void loop();
    static void onWaterLeakTestSwitchCommand(bool state, HASwitch *sender);
};

#endif // SWITCHES