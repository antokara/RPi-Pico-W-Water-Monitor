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

// the actual max voltage the pin would need to reach that range
// meaning, even though the pin can only accept a max of 3volts and
// even though, the max range of the pin is 1023, it would theoritically
// reach that 1023 if we supplied it with 5volts,
// which would can't without damaging the board. Z-uno's documentation
// should have mentioned that but they do not state anything about this.
#define MAX_PRESSURE_SENSOR_PIN_RANGE_VOLTAGE 3.3

// the max PSI the sensor itself can measure
#define MAX_PRESSURE_SENSOR_PSI 100.0

// how much to multiply the PSI number we come up with
// to match the calibrated and expected reading from other sensors
//
// when connected to the USB of a computer, the 5v voltage, reports 4.7v and
// with that voltage, the calibration multiplier is 1.2
//
// when connected to the USB of a dedicated USB power supply, the 5v voltage, reports 5.1v and
// with that voltage, the calibration multiplier is 1.1
//
// the reason for that is 4.7/5.1=0.92*1.2=1.10
// changed to directly the voltage multiplier
#define PRESSURE_SENSOR_PSI_CALIBRATION_MULTIPLIER 0.92

// the min/max Voltage the sensor itself outputs, depending on the pressure
#define MIN_PRESSURE_SENSOR_VOLTAGE 0.333
#define MAX_PRESSURE_SENSOR_VOLTAGE 3

class PressureSensor
{
public:
    static bool shouldSendPSI();
    static void setup();
    static void loop();
};

#endif // PRESSURE_SENSOR