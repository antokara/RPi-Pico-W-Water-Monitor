#ifndef PRESSURE_SENSOR
#define PRESSURE_SENSOR

/**
 * @brief the delta in PSI that must be great or equal to,
 * between the previous reading and the current,
 * in order to consider updating it and sending it to the controller
 */
#define PRESSURE_DELTA 0.1

/**
 * @brief frequency in milliseconds,
 * to allow sending of the water pressure to the controller,
 * even if the delta is greater.
 */
#define SEND_PRESSURE_FREQUENCY 5000

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
#define PRESSURE_SENSOR_PSI_CALIBRATION_MULTIPLIER 1.05

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
    static bool shouldSendPSI();
    static void setup();
    static void loop();
};

#endif // PRESSURE_SENSOR