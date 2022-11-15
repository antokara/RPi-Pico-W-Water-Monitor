#include <ArduinoHA.h>
#include "pulseSensor.h"

// holds the last pulse sensor isActive state
boolean PulseSensor::lastPulseSensorIsActive = false;

// formula for getting GPM, using pulse rate and duration between pulses
// [target rate time] / [duration between pulses] / [pulse rate] = Gallons Per Rate
// examples:
// target rate: 60 seconds to get (GPM)
// pulse rate: 1
// 60secs / 60secs / 1 pulse per gallon = 1 GPM
// 60 / 120 / 1 = 0.5
// pulse rate: 2
// 60 / 60 / 2 = 0.5
// 60 / 30 / 2 = 1

// formula for getting duration between pulses, using target rate time, Gallons Per Rate and pulse rate
// [target rate time] / [Gallons Per Rate] / [pulse rate] = [duration between pulses]
// Normal Flow Range: 0.25 - 15 GPM
// Pulse Rate is 1 Pulse/Gallon

// last time we had a pulse
unsigned long PulseSensor::lastPulseTime = 0;

// time passed between previous pulse and the current one
// TODO: rename last/prev/current to clear things up
unsigned long PulseSensor::prevTimePassedSinceLastPulse = 0;

// current gallons per minute
float PulseSensor::gpm = 0.0;

// last flow GPM value we sent (to avoid let's say sending 0.0 twice in a row)
float PulseSensor::lastGpmSent = 0.0;

// last time we sent the gpm
unsigned long PulseSensor::lastGpmSendTime = 0;

// time that must pass without a pulse, in order to be considered no-flow
unsigned int PulseSensor::flowTimeout = 0;

// last time we got an infrared delta
unsigned long PulseSensor::lastIrTime = 0;

// the "first" time we got an infrared delta
unsigned long PulseSensor::fistIrTime = 0;

// previous infrared value we had (since the last delta)
int PulseSensor::prevIrValue = -1;

// number of delta counts that are happening, within the timeout period
// @see IR_COUNTS_THRESHOLD
unsigned int PulseSensor::irCounts = 0;

// current value
bool PulseSensor::isIrSensorActive = false;

// gallons to increase the water meter by
// defaults to -1 in order to send 0 at boot, in case it rebooted while last sent a value > 0
long PulseSensor::gallonsCounter = -1;

// buffer for  the gallons to increase the water meter by.
// this is the internal counter, before we update and send the new value to the controller.
long PulseSensor::gallonsCounterBuffer = 0;

// last time we sent the gallons counter
unsigned long PulseSensor::lastGallonsCounterSendTime = 0;

// flag to keep track of the first loop
bool PulseSensor::firstLoop = true;

// the water flow GPM sensor
HASensorNumber PulseSensor::gpmSensor("gpm", HASensorNumber::PrecisionP2);

// the water gallons counter sensor
HASensorNumber PulseSensor::gallonsSensor("gallons", HASensorNumber::PrecisionP0);

/**
 * @brief if we should send the pressure to the controller.
 * a separate throttler to keep the unsolicited updated in lifeline prioritize the flow.
 *
 * @return true
 * @return false
 */
bool PulseSensor::shouldSendGallonsCounter()
{
    return abs(long(millis() - PulseSensor::lastGallonsCounterSendTime)) > SEND_GALLONS_COUNTER_FREQUENCY;
}

/**
 * @brief checks if the gallons counter needs to be set or reset and then sent to the controller.
 *        this should be called on every loop iteration (in case we need to reset the counter).
 *
 */
void PulseSensor::checkGallonsCounter()
{
    if (PulseSensor::shouldSendGallonsCounter())
    {
        bool send = false;
        if (PulseSensor::gallonsCounter == 0)
        {
            // don't update when both counters are set to zero
            if (PulseSensor::gallonsCounterBuffer != 0)
            {
                // set the exposed counter
                PulseSensor::gallonsCounter = PulseSensor::gallonsCounterBuffer;
                // reset the buffer
                PulseSensor::gallonsCounterBuffer = 0;
                send = true;
            }
        }
        else
        {
            // reset the exposed counter
            PulseSensor::gallonsCounter = 0;
            send = true;
        }

        // only send when there's a new value
        if (send)
        {
            // send the new value
            PulseSensor::lastGallonsCounterSendTime = millis();
            PulseSensor::gallonsSensor.setValue(PulseSensor::gallonsCounter);
        }
    }
}

/**
 * @brief this should only be called, every time we get a new pulse.
 *        it increases the gallonsCounterBuffer by one
 */
void PulseSensor::increaseGallonsCounter()
{
    PulseSensor::gallonsCounterBuffer++;
}

/**
 * @brief checks if the infrared sensor is changing,
 * which means, movement is taking place on the spinning dial.
 *
 * when true, it sets irSensorValue to true and false when not.
 * it debounces the sensor properly and on the first reading, it leaves it at false.
 *
 * @see IR_SENSOR_PIN
 * @see IR_DELTA_THRESHOLD
 *
 * @return true
 * @return false
 */
void PulseSensor::updateIrSensorActive()
{
    int irSensorValue = analogRead(IR_SENSOR_PIN); // read the input pin
    // only when the value has changed
    if (PulseSensor::prevIrValue == -1)
    {
        // during initial run, just set the previous value to the current one
        // isIrSensorActive should already be set to false
        PulseSensor::prevIrValue = irSensorValue;
        Serial.print("IR initial: ");
        Serial.println(irSensorValue);
    }
    else if (abs(irSensorValue - PulseSensor::prevIrValue) > IR_DELTA_THRESHOLD)
    {
        // Serial.print("IR > delta: ");
        // Serial.println(irSensorValue);
        // when the delta is greater than the threshold
        // update the the last time we had a delta
        lastIrTime = millis();

        // purely for debug
        // TODO: place in IFDEF
        int delta = abs(irSensorValue - PulseSensor::prevIrValue);

        // keep the last value
        PulseSensor::prevIrValue = irSensorValue;

        // when the time since the first IR delta
        // is less than the timeout
        if (abs(long(millis() - PulseSensor::fistIrTime)) <= IR_TIMEOUT)
        {
            // increase the counter
            PulseSensor::irCounts++;
        }
        else
        {
            // otherwise, reset
            PulseSensor::irCounts = 0;
            PulseSensor::fistIrTime = millis();
        }

        // when the IR counts have reached the threshold
        if (PulseSensor::irCounts > IR_COUNTS_THRESHOLD)
        {
            // mark our IR sensor as active
            PulseSensor::isIrSensorActive = true;

            Serial.print("irCounts: ");
            Serial.print(PulseSensor::irCounts);
            Serial.print(", IR true with delta: ");
            Serial.println(delta);
        }
        else
        {
            Serial.print("irCounts: ");
            Serial.print(PulseSensor::irCounts);
            Serial.print(", IR delta: ");
            Serial.println(delta);
        }
    }
    else if (PulseSensor::isIrSensorActive && abs(long(millis() - PulseSensor::lastIrTime)) > IR_TIMEOUT)
    {
        Serial.print("IR false with delta: ");
        Serial.println(abs(irSensorValue - PulseSensor::prevIrValue));
        // when the delta is less than the threshold and
        // the timeout period has passed, only then,
        // mark the sensor as inactive.
        // (this is a debouncer of some sort)
        PulseSensor::isIrSensorActive = false;
    }
}

/**
 * @brief if the time passed is negative (ie. due to overflow) or
 * if actual=false and the lastPulseTime is zero (initial pulse) or timePassed <= 0, it will return the flowTimeout,
 * to indicate the lowest possible flow, since we need 2 pulses at least, to calculate the actual flow.
 *
 * @see https://www.arduino.cc/reference/en/language/functions/time/millis/
 * @param actual optional. defaults to false. if true, it will never return the flowTimeout.
 * @return milliseconds since the last pulse
 */
unsigned long PulseSensor::timePassedSinceLastPulse(bool actual = false)
{
    if (PulseSensor::lastPulseTime > 0)
    {
        const unsigned long timePassed = abs(long(millis() - PulseSensor::lastPulseTime));
        if (timePassed > 0)
        {
            return timePassed;
        }
    }

    if (actual)
    {
        return 0;
    }

    return PulseSensor::flowTimeout;
}

/**
 * @brief updates the GPM based on the pulses received
 */
void PulseSensor::updateGPM()
{
    PulseSensor::gpm = TARGET_RATE_TIME / PulseSensor::timePassedSinceLastPulse() / PULSE_RATE;
}

/**
 * @brief sets the GPM to the specified value
 *
 * @param newValue
 */
void PulseSensor::updateGPM(float newValue)
{
    PulseSensor::gpm = newValue;
}

/**
 * @brief sends the water flow GPS to the controller,
 * if the previously sent value is different than the current and
 * if enough time has passed since last time we sent or if force is true.
 *
 * @param force optional. defaults to false. if true, it will ignore the frequency and send the value immediately
 */
void PulseSensor::sendGPM(bool force = false)
{
    if (PulseSensor::lastGpmSent != PulseSensor::gpm && (abs(long(millis() - PulseSensor::lastGpmSendTime)) > SEND_GPM_FREQUENCY || force))
    {
        PulseSensor::lastGpmSent = gpm;
        PulseSensor::lastGpmSendTime = millis();
        PulseSensor::gpmSensor.setValue(gpm);
    }
}

/**
 * @brief
 * handles debouncing on its own, so that it returns true only once.
 * even if it stays true and we call it multiple times, it will be true only once.
 *
 * @return true if the pulse sensor is active (meaning, a Gallon was just metered)
 * @return false when there is no pulse
 */
bool PulseSensor::isPulseSensorActive()
{
    if (digitalRead(PULSE_SENSOR_PIN) == LOW)
    {
        // when the sensor is in active state
        if (!PulseSensor::lastPulseSensorIsActive && abs(long(millis() - PulseSensor::lastPulseTime)) > PULSE_DEBOUNCE_FREQUENCY)
        {
            // and it just turned active
            PulseSensor::lastPulseSensorIsActive = true;

            // only the first time, return true
            Serial.println("lastPulseSensorIsActive true");
            return PulseSensor::lastPulseSensorIsActive;
        }
    }
    else if (PulseSensor::lastPulseSensorIsActive)
    {
        // it just turned inactive
        PulseSensor::lastPulseSensorIsActive = false;
        Serial.println("lastPulseSensorIsActive false");
    }

    // any other time, return inactive
    return false;
}

void PulseSensor::setup()
{
    // set the water flow sensor details
    PulseSensor::gpmSensor.setName("Water Flow");
    PulseSensor::gpmSensor.setIcon("mdi:water");
    PulseSensor::gpmSensor.setUnitOfMeasurement("gpm");

    // set the water gallons counter sensor details
    PulseSensor::gallonsSensor.setName("Gallons Counter");
    PulseSensor::gallonsSensor.setIcon("mdi:counter");
    PulseSensor::gallonsSensor.setDeviceClass("water");
    PulseSensor::gallonsSensor.setUnitOfMeasurement("gal");

    // calculate how much time must pass without a pulse, in order to consider no-flow
    PulseSensor::flowTimeout = TARGET_RATE_TIME / MIN_GPM / PULSE_RATE;

    // set the mode for the digital pins
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PULSE_SENSOR_PIN, INPUT_PULLUP);
}

void PulseSensor::loop()
{
    /**
     * @brief only on the first loop, reset the flow to zero,
     * in case there was a previous flow that is now invalid.
     *
     */
    if (PulseSensor::firstLoop)
    {
        PulseSensor::firstLoop = false;
        PulseSensor::gpmSensor.setValue(float(0.0));
    }

    // update the value
    PulseSensor::updateIrSensorActive();

    if (PulseSensor::isPulseSensorActive())
    {
        // since we got a pulse, force the IR sensor to be true
        // the pulse is more reliable
        PulseSensor::isIrSensorActive = true;
        PulseSensor::lastIrTime = millis();

        // we got a pulse (this can only happen once, per pulse,
        // even if the meter stops right when the switch is on and the switch remains on)
        PulseSensor::updateGPM();
        if (PulseSensor::gpm < MIN_GPM)
        {
            // when there's pulse but too much time has passed since the last pulse
            // make sure we set a minimum flow.
            // This can happen when water starts flowing after a long period (pulse timeout)
            PulseSensor::updateGPM(MIN_GPM);
        }

        PulseSensor::sendGPM(true);
        digitalWrite(LED_BUILTIN, HIGH);
        PulseSensor::increaseGallonsCounter();

        // keep the time passed, before we update the lastPulseTime
        PulseSensor::prevTimePassedSinceLastPulse = PulseSensor::timePassedSinceLastPulse();

        // reset the timer, after we have used it (with timePassedSinceLastPulse)
        PulseSensor::lastPulseTime = millis();
    }
    else if (PulseSensor::isIrSensorActive)
    {
        const float prevGPM = PulseSensor::gpm;
        if (PulseSensor::timePassedSinceLastPulse(true) > PulseSensor::prevTimePassedSinceLastPulse)
        {
            // when the time that has passed since the last pulse
            // is greater than the time that had passed since the previous to last one and
            // enough time has passed to update the flow

            // when there's no pulse but there's flow (the IR sensor is active)
            // update the current GPM depending on the last pulse time
            PulseSensor::updateGPM();
        }

        if (PulseSensor::gpm < MIN_GPM)
        {
            // when there's no pulse but there's flow (the IR sensor is active) and
            // when the GPM has been set to zero because too much time has passed since the last pulse
            // make sure we set a minimum flow.
            // This can happen when water starts flowing after a long period (pulse timeout)
            // but before a pulse is sent.
            PulseSensor::updateGPM(MIN_GPM);
        }

        if (prevGPM == 0.0 && PulseSensor::gpm > 0.0)
        {
            // turn on the LED, when we just set the gpm > 0 from 0
            digitalWrite(LED_BUILTIN, HIGH);
            PulseSensor::sendGPM(true);
        }
        else
        {
            // existing flow
            PulseSensor::sendGPM();
        }
    }
    else if (PulseSensor::gpm > 0.0)
    {
        // no pulse or flow (the IR sensor is inactive) but there's residual GPM
        // reset everything to 0
        PulseSensor::updateGPM(0.0);
        PulseSensor::sendGPM(true);
        digitalWrite(LED_BUILTIN, LOW);

        Serial.println("gpm stop - no pulse or flow");
    }

    // after all other checks have taken place and
    // any data has been sent, check if we need to set/reset the gallons counter
    PulseSensor::checkGallonsCounter();
}