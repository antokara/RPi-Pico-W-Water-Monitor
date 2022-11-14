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
 * @brief the max voltage an analog pin will need,
 *        to reach the MAX_ANALOG_PIN_RANGE
 */
#define MAX_ANALOG_PIN_RANGE_VOLTAGE 3.3

/**
 * @brief the number of which we need to multiply voltage by,
 *        in order to get the expected input pin value, when we know the expected voltage.
 *        This is useful in finding the min/max/current input pin value, a sensor should provide.
 *
 * @example with MAX_ANALOG_PIN_RANGE 1023
 *          and MAX_ANALOG_PIN_RANGE_VOLTAGE 3.3
 *          analogInputValueMultiplier will be 310
 *
 *          let's say our sensor reports a minimum of 0.53v at its 0 and 3.33v max at its 100.
 *          to get our adjusted pin input value, we will do:
 *          adjustedMinInputValue = analogInputValueMultiplier * 0.53 = 164.3
 *          adjustedInputValue = inputValue - adjustedMinInputValue = 164.3 - 164.3 = 0
 *          that way, our adjusted input will now be 0, which should align with the min of the sensor.
 */
const float analogInputValueMultiplier = float(MAX_ANALOG_PIN_RANGE / MAX_ANALOG_PIN_RANGE_VOLTAGE);

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