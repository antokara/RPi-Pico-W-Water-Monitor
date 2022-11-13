#ifndef DEVICE
#define DEVICE

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
 * @brief time in milliseconds to wait for the WiFi to connect
 *
 */
#define WAIT_FOR_WIFI 3000

class Device
{
public:
    static void connectoMQTT();
    static void connectToWifi();
    static void setupOTA();
    static void setup();
    static void loop();
};

#endif // DEVICE