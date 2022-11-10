#include <ArduinoHA.h>
#include "led.h"

// for test
HASwitch led("testLed");

void Led::onCommand(bool state, HASwitch *sender)
{
    Serial.print("led onCommand");
    Serial.println(state);
    digitalWrite(LED_PIN, (state ? HIGH : LOW));
    sender->setState(state); // report state back to the Home Assistant
}

void Led::setup()
{
    // prepate LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // control info
    led.setIcon("mdi:lightbulb");
    led.setName("My Test LED");
    led.onCommand(Led::onCommand);
}