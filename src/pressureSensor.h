#ifndef PRESSURE_SENSOR
#define PRESSURE_SENSOR

#include <ArduinoHA.h>

/**
 * @brief the MQTT topic for debugging this sensor
 *
 */
#define PRESSURE_SENSOR_DEBUG_MQTT_TOPIC "debug:waterMonitor:pressureSensor"

/**
 * @brief the delta that the pressure sensor needs to have between previous and current value,
 * in order to quality to be sent to the controller, during a water leak test mode.
 */
#define PRESSURE_SENSOR_DELTA_WATER_LEAK_TEST_ACTIVE 0.1

/**
 * @brief the time frequency in milliseconds that needs to pass from the last time
 * we sent the pressure to the controller, in order to quality for sending the new value,
 * during a water leak test mode.
 */
#define PRESSURE_SENSOR_SEND_FREQUENCY_WATER_LEAK_TEST_ACTIVE 5000

/**
 * @brief the delta that the pressure sensor needs to have between previous and current value,
 * in order to quality to be sent to the controller, during normal operation mode.
 */
#define PRESSURE_SENSOR_DELTA 2

/**
 * @brief the time frequency in milliseconds that needs to pass from the last time
 * we sent the pressure to the controller, in order to quality for sending the new value,
 * during normal operation mode.
 */
#define PRESSURE_SENSOR_SEND_FREQUENCY 15000

// the (analog) pin that we connect the pressure sensor output
// you may use A0-A2
#define PRESSURE_SENSOR_PIN A0

// the max PSI the sensor itself can measure
#define MAX_PRESSURE_SENSOR_PSI 100.0

/**
 * @brief how much to multiply the PSI number we come up with,
 * to match the calibrated and expected reading from other sensors.
 * when we calibrate (take voltage measurements from the sensor stepped down),
 *
 *  according to the docs:
 *  The ADC draws current (about 150μA if the temperature sense diode is disabled, which can vary between chips);
 *  there will be an inherent offset of about 150μA*200 = ~30mV. There is a small difference in current draw when the
 *  ADC is sampling (about +20μA), so that offset will also vary with sampling as well as operating temperature.

 */
#define PRESSURE_SENSOR_PSI_CALIBRATION_MULTIPLIER 1.22

// calibration measurements after the step-down.
// 170 @ 0 PSI 0.4v
// 615 @ 50 PSI 2v
// 895 @ 80 PSI 2.49v
// 980 @ 90 PSI 2.76v
// 1023 @ 100 PSI 3.3v

// specs of sensor
// 0 PSI = 0.5v
// 50 PSI = 2.5v
// 100 PSI = 4.5v

// measured output
// 0 PSI = 0.79v instead of the 0.5v spec

// when at 0 PSI
#define MIN_PRESSURE_SENSOR_VOLTAGE 0.3

// when at 100 PSI
#define MAX_PRESSURE_SENSOR_VOLTAGE 3.3

class PressureSensor
{
public:
    static float pressureDelta;
    static unsigned int sendPressureFrequency;
    static const float adjustedMinPressureSensorInputValue;
    static const float adjustedMaxPressureSensorInputValue;
    static const float adjustedPressureSensorInputValueMultiplier;
    static float psi;
    static float prevPsi;
    static unsigned long lastPressureSendTime;
    static HASensorNumber psiSensor;

    // methods
    static bool shouldSendPSI();
    static void setup();
    static void loop();
};

#endif // PRESSURE_SENSOR