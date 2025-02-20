#ifndef PULSE_SENSOR
#define PULSE_SENSOR

/**
 * @brief the MQTT topic for debugging this sensor
 *
 */
#define PULSE_SENSOR_DEBUG_MQTT_TOPIC "debug:waterMonitor:pulseSensor"

/**
 * @brief frequency in milliseconds,
 * to allow sending of gallons to the controller
 *
 * 300000 = 5 minutes
 */
#define SEND_GALLONS_COUNTER_FREQUENCY 300000

/**
 * @brief frequency in milliseconds,
 * to allow sending of water flow GPM to the controller
 * when there is a change in the existing flow.
 *
 * this frequency does not apply to flow start/stop events.
 */
#define SEND_GPM_FREQUENCY 1000

/**
 * @brief frequency in milliseconds,
 * to allow re-sending of water flow GPM to the controller
 * after the GPM drops to zero. This is an attempt to have
 * better chances of the controller receiving and processing the new flow.
 * (mostly for the stop-flow event).
 */
#define RESEND_GPM_FREQUENCY 5000

/**
 * @brief how many times to resend. (0 will be no resend, just the original send)
 * this only applies if/while the value has not changed.
 */
#define RESEND_GPM_TIMES 1

// the (digital) pin that we need to connect the water meter pulse switch.
// the other end, needs to go the ground (GND) pin
// you may use D0-D22 which correlates to GP0-GP22
#define PULSE_SENSOR_PIN D2

// the (analog) pin that we connect to the
// InfraRed AO (analog output) pin of the sensor
// you may use A0-A2
#define IR_SENSOR_PIN A1

/**
 * @brief the delta we must calculate between two infrared sensor values
 * in order to be considered an actual change/motion
 * (true, when greater than)
 *
 * when on computer USB/power:
 *  with 4, we get 15-20 counts within 8 secs, when there's low flow and ~5 with no flow/noise
 *
 * when on stable/clean external power:
 *  with 3, we get 10-20 counts within 8 secs, when there's low flow and ~1 with no flow/noise
 *  with delta 3, timeout 4000 and no flow, we get: 1-7
 *  with delta 3, timeout 4000 and low flow, we get: 10-13
 *  conclusion: delta 3, timeout 4000 and count 10, is probably the lowest and "safe" we can go
 *
 */
#define IR_DELTA_THRESHOLD 3

// TODO: make those parameters dynamic
// Note: something appears to randomly interrupt the loop, causing low count of IR
//       even though, they probably did not really drop... Maybe Wifi reconnect?
//       maybe we need to create a counter module that runs on its own dedicated loop/controller
//       for truly accurate results OR find a way to perform that uninterrupted.

// since there's some inherit noise in the IR "module", we need to increase the "delta duration" timeout
// in order to get a bigger sample and therefore, normalize/avg the noise...
#define IR_TIMEOUT 10000

// number of delta counts that need to happen within the timeout period
// for the IR sensor to be considered ON (to avoid potential noise)
// (true, when greater than)
// 50 with sensor at step 4 distance
#define IR_COUNTS_THRESHOLD 50

//
// at very low flows, the Flow Indicator propeler, spins intermittently.
// that's because the flow falls at the threshold of the Water Meter's minimum flow detection rate
// which causes momentary stops of rotation of about 10 seconds at a time.
//
// therefore, in order to account for those momentary stops, we must increase the timeout
// above them, hence the *3 IR_TIMEOUT...
// the downside to that, is the prolonged time it will now take (~30 secs), to call a no-flow event.
#define IR_TIMEOUT_KEEP_ACTIVE 30000

// number of delta counts that need to happen within the timeout period
// for the IR sensor to be kept ON (to avoid false positive from noise, while already active)
// (true, when greater than)
#define IR_COUNTS_THRESHOLD_KEEP_ACTIVE 90

// minimum gallons per minute that the water meter can detect.
// this helps us detect no-flow, by calculating a "time-out" when
// too much time has passed since a new pulse.
#define MIN_GPM 0.1

// time in milliseconds for our target rate
// 60000msecs = 60secs = 1minute rate for GPM
#define TARGET_RATE_TIME 60000.0

// number of pulses per gallon (Pulse/Gallon)
#define PULSE_RATE 1.0

/**
 * @brief frequency in milliseconds, to debounce the pulses.
 * in case the pulse switch toggles too fast for some reason within the defined
 * period, it will be ignored.
 */
#define PULSE_DEBOUNCE_FREQUENCY 250

class PulseSensor
{
public:
    // properties
    static bool lastPulseSensorIsActive;
    static unsigned long lastPulseTime;
    static unsigned long prevTimePassedSinceLastPulse;
    static float gpm;
    static float lastGpmSent;
    static unsigned long lastGpmSendTime;
    static unsigned long lastGpmResendTime;
    static unsigned int flowTimeout;
    static unsigned int gpmResendTimes;
    static unsigned long lastIrTime;
    static unsigned long fistIrTime;
    static int prevIrValue;
    static unsigned int irCounts;
    static bool isIrSensorActive;
    static long gallonsCounter;
    static long gallonsCounterBuffer;
    static unsigned long lastGallonsCounterSendTime;
    static bool firstLoop;
    static HASensorNumber gpmSensor;
    static HASensorNumber gallonsSensor;
    // for debug of gpm infrared counts
    static bool lastIsDebugActive;
    static unsigned int minIrCounts;
    static unsigned int maxIrCounts;
    static float avgIrCounts;
    static unsigned int deltaRounds;
    static unsigned long loopCycles;
    static bool activeCountsReported;

    // methods
    static bool shouldSendGallonsCounter();
    static void checkGallonsCounter();
    static void checkResendGPM();
    static void increaseGallonsCounter();
    static void updateIrSensorActive();
    static unsigned long timePassedSinceLastPulse(bool actual);
    static void updateGPM();
    static void updateGPM(float newValue);
    static void sendGPM(bool force);
    static bool isPulseSensorActive();
    static void setup();
    static void loop();
};

#endif // PULSE_SENSOR