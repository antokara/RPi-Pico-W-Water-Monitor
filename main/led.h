#ifndef LED
#define LED

#define LED_PIN LED_BUILTIN

class Led
{
public:
    static void setup();
    static void onCommand(bool state, HASwitch *sender);
};

#endif // LED