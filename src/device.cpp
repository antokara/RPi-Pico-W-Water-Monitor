#include <ArduinoHA.h>
// TODO: use def for OTA
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "secrets.h"
#include "device.h"

/**
 * @author Antonios Karagiannis (antokarag@gmail.com)
 * @brief definitions for the Device that will be added to the Home Assistant
 * @version 0.1
 * @date 2022-11-08
 *
 * @copyright Copyright (c) 2022
 *
 */

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
const float Device::analogInputValueMultiplier = float(MAX_ANALOG_PIN_RANGE / MAX_ANALOG_PIN_RANGE_VOLTAGE);

int Device::wifiStatus;

WiFiClient Device::client;
HADevice Device::device(DEVICE_ID);

// increase the device types limit, otherwise, some of the sensors/switches will not get registered
// @see https://dawidchyrzynski.github.io/arduino-home-assistant/documents/library/device-types.html#limitations
HAMqtt Device::mqtt(Device::client, Device::device, 7);

/**
 * @brief a status string sensor
 * for the general status/health of the device
 */
HASensor Device::statusSensor("waterMonitorStatus");

// flag to keep track of the first loop
bool Device::firstLoop = true;

void Device::connectToWifi()
{
  // get our WiFi's mac address
  byte mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);

  // connect to WiFi
  while (Device::wifiStatus != WL_CONNECTED)
  {
#ifdef SERIAL_DEBUG
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_SSID);
#endif
    Device::wifiStatus = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // wait to connect...
    delay(WAIT_FOR_WIFI);
  }

#ifdef SERIAL_DEBUG
  Serial.print("Connected to: ");
  Serial.print(WIFI_SSID);
  Serial.print(", with IP: ");
  Serial.print(WiFi.localIP());
  Serial.print(" and mac: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
#endif
}

/**
 * @brief connects to the MQTT broker.
 *        it should be called after the device, controls and sensors have been defined.
 *
 */
void Device::connectoMQTT()
{
#ifdef SERIAL_DEBUG
  Serial.print("Connecting to MQTT\n");
#endif
  if (mqtt.begin(BROKER_ADDR, BROKER_PORT, BROKER_USERNAME, BROKER_PASSWORD) == true)
  {
#ifdef SERIAL_DEBUG
    Serial.print("Connected to MQTT broker");
#endif
  }
  else
  {
#ifdef SERIAL_DEBUG
    Serial.print("Could not connect to MQTT broker");
#endif
  }
}

/**
 * @brief enables OTA (over the air updates)
 *
 */
void Device::setupOTA()
{
  ArduinoOTA.setHostname(DEVICE_ID);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]()
                     {
                       String type;
                       if (ArduinoOTA.getCommand() == U_FLASH)
                       {
                         type = "sketch";
                       }
                       else
                       { // U_FS
                         type = "filesystem";
                       }

// NOTE: if updating FS this would be the place to unmount FS using FS.end()
#ifdef SERIAL_DEBUG
                       Serial.println("Start updating " + type);
#endif
                     });
  ArduinoOTA.onEnd([]()
                   {
#ifdef SERIAL_DEBUG
                     Serial.println("\nEnd");
#endif
                   });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        {
#ifdef SERIAL_DEBUG
                          Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
                        });
  ArduinoOTA.onError([](ota_error_t error)
                     {
#ifdef SERIAL_DEBUG
                       Serial.printf("Error[%u]: ", error);
                       if (error == OTA_AUTH_ERROR)
                       {
                         Serial.println("Auth Failed");
                       }
                       else if (error == OTA_BEGIN_ERROR)
                       {
                         Serial.println("Begin Failed");
                       }
                       else if (error == OTA_CONNECT_ERROR)
                       {
                         Serial.println("Connect Failed");
                       }
                       else if (error == OTA_RECEIVE_ERROR)
                       {
                         Serial.println("Receive Failed");
                       }
                       else if (error == OTA_END_ERROR)
                       {
                         Serial.println("End Failed");
                       }
#endif
                     });
  ArduinoOTA.begin();
}

/**
 * @brief should be called once, from the main setup() function
 *
 */
void Device::setup()
{
  // @see https://arduino-pico.readthedocs.io/en/latest/analog.html#void-analogreadresolution-int-bits
  analogReadResolution(ANALOG_READ_RESOLUTION);

#ifdef SERIAL_DEBUG
  Serial.begin(9600);
  delay(500); // Give the serial terminal a chance to connect, if present
  Serial.print("Device::Setup()");
#endif

  Device::connectToWifi();

  // set device's details
  Device::device.setName(DEVICE_NAME);
  Device::device.setSoftwareVersion(FIRMWARE_VERSION);

  // enable MQTT LWT feature. If device will lose connection
  // to the broker, all device types related to it will be marked as offline in
  // the Home Assistant Panel.
  Device::device.enableLastWill();

  // set the status sensor details
  Device::statusSensor.setName("Status");
  Device::statusSensor.setIcon("mdi:check-circle");

  // enable OTA
  Device::setupOTA();
}

/**
 * @brief should be called on every iteration of the main loop() function
 *
 */
void Device::loop()
{
  // process any pending mqtt messages
  Device::mqtt.loop();

  // process any incoming OTA requests
  ArduinoOTA.handle();

  /**
   * @brief only on the first loop,
   *        swap the status to force an update of the status sensor,
   *        in order to have a record of the time the device rebooted...
   *
   */
  if (Device::firstLoop)
  {
    Device::firstLoop = false;
    Device::statusSensor.setValue("connected");
    // allow mqtt to send the "connected" value before changing it to "ready"
    mqtt.loop();
    delay(250);
    Device::statusSensor.setValue("ready");
  }
  // TODO: check WiFi and reconnect if dropped;; TEST it
}