#ifndef PULSE_SENSOR
#define PULSE_SENSOR

/**
 * @brief frequency in milliseconds,
 * to allow sending of gallons to the controller
 *
 * 3600000 = 1 hour
 * 300000 = 5 minutes
 */
#define SEND_GALLONS_COUNTER_FREQUENCY 300000

// #define SEND_GPM_FREQUENCY 1000

// the (digital) pin that we need to connect the water meter pulse switch.
// the other end, needs to go the ground (GND) pin
#define PULSE_SENSOR_PIN D2

// the (analog) pin that we connect to the
// InfraRed AO (analog output) pin of the sensor
// you may use A0-A2
#define IR_SENSOR_PIN A1

// the delta we must calculate between two infrared sensor values
// in order to be considered an actual change/motion
// (true, when greater than)
//
// 2 is sensitive enough, with the IR_COUNTS_THRESHOLD
// to not produce false positive flow but
// it can produce false negative flow.
#define IR_DELTA_THRESHOLD 2

// time in milliseconds that a delta lasts
//
// 2500 does not produce false positive flow but produces false negative flow, at low GPM
// 3500 no false positives but false negatives only at high GPM >6
// 5000 no false positives but intermittent false negatives, again only at high GPM >6
// 8000 no false positives, no false negatives
#define IR_TIMEOUT 8000

// number of delta counts that need to happen within the timeout period
// for the IR sensor to be considered ON (to avoid potential noise)
// (true, when greater than)
#define IR_COUNTS_THRESHOLD 1

// minimum gallons per minute that the water meter can detect.
// this helps us detect no-flow, by calculating a "time-out" when
// too much time has passed since a new pulse.
#define MIN_GPM 0.1

// time in milliseconds for our target rate
// 60000msecs = 60secs = 1minute rate for GPM
#define TARGET_RATE_TIME 60000.0

// frequency to update the flow, in milliseconds (between pulse and timeout only)
#define UPDATE_FLOW_FREQUENCY 2500

// number of pulses per gallon (Pulse/Gallon)
#define PULSE_RATE 1.0

class PulseSensor
{
public:
    static bool shouldSendGallonsCounter();
    static void checkGallonsCounter();
    static void increaseGallonsCounter();
    static void updateIrSensorActive();
    static unsigned long timePassedSinceLastPulse();
    static void updateGPM();
    static void updateGPM(float newValue);
    static void sendGPM();
    static bool isPulseSensorActive();
    static void setup();
    static void loop();
};

#endif // PULSE_SENSOR