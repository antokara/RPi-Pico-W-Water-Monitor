#ifndef DEVICE
#define DEVICE

#include <ArduinoHA.h>
// TODO: use def for OTA
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define DEVICE_ID "waterMonitorTest"
#define DEVICE_NAME "Water Monitor Test"
#define FIRMWARE_VERSION "1.0.0"

/**
 * @brief the bits to use for analog pin resolution
 * @see https://arduino-pico.readthedocs.io/en/latest/analog.html#void-analogreadresolution-int-bits
 */
#define ANALOG_READ_RESOLUTION 10

/**
 * @brief the max value (range) an analog pin can achieve.
 *        this is directly related to the ANALOG_READ_RESOLUTION defined above.
 *        it must be the result of 2^ANALOG_READ_RESOLUTION-1
 * @see https://arduino-pico.readthedocs.io/en/latest/analog.html#int-analogread-pin-size-t-pin-a0-a3
 */
#define MAX_ANALOG_PIN_RANGE 1023

/**
 * @brief the max voltage an analog pin will need,
 *        to reach the MAX_ANALOG_PIN_RANGE
 */
#define MAX_ANALOG_PIN_RANGE_VOLTAGE 3.3

/**
 * @brief time in milliseconds to wait for the WiFi to connect
 *
 */
#define WAIT_FOR_WIFI 5000

class Device
{
public:
    // properties
    static const float analogInputValueMultiplier;
    static int wifiStatus;
    static WiFiClient client;
    static HADevice device;
    static HAMqtt mqtt;
    static HASensor statusSensor;
    static bool firstLoop;

    // methods
    static void connectoMQTT();
    static void connectToWifi();
    static void setupOTA();
    static void setup();
    static void loop();
};

#endif // DEVICE