#include <ArduinoHA.h>
// #include <WiFi.h>
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

int wifiStatus;

WiFiClient client;
HADevice device(DEVICE_ID);
HAMqtt mqtt(client, device);

/**
 * @brief a status string sensor
 * for the general status/health of the device
 */
HASensor statusSensor("status");

// flag to keep track of the first loop
bool firstLoop = true;

void Device::connectToWifi()
{
  // get our WiFi's mac address
  byte mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);

  // connect to WiFi
  while (wifiStatus != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_SSID);
    wifiStatus = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // wait to connect...
    delay(WAIT_FOR_WIFI);
  }

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
}

/**
 * @brief connects to the MQTT broker.
 *        it should be called after the device, controls and sensors have been defined.
 *
 */
void Device::connectoMQTT()
{
  Serial.print("Connecting to MQTT\n");
  if (mqtt.begin(BROKER_ADDR, BROKER_PORT, BROKER_USERNAME, BROKER_PASSWORD) == true)
  {
    Serial.print("Connected to MQTT broker");
  }
  else
  {
    Serial.print("Could not connect to MQTT broker");
  }
}

/**
 * @brief enables OTA (over the air updates)
 *
 */
void Device::setupOTA()
{
  ArduinoOTA.onStart([]()
                     {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    } });
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

  // TODO: use DEBUG to disable
  Serial.begin(9600);
  delay(500); // Give the serial terminal a chance to connect, if present
  Serial.print("Device::Setup()");

  Device::connectToWifi();

  // set device's details
  device.setName(DEVICE_NAME);
  device.setSoftwareVersion(FIRMWARE_VERSION);

  // enable MQTT LWT feature. If device will lose connection
  // to the broker, all device types related to it will be marked as offline in
  // the Home Assistant Panel.
  device.enableLastWill();

  // set the status sensor details
  statusSensor.setName("Status");
  statusSensor.setIcon("mdi:check-circle");

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
  mqtt.loop();

  // process any incoming OTA requests
  ArduinoOTA.handle();

  /**
   * @brief only on the first loop,
   *        swap the status to force an update of the status sensor,
   *        in order to have a record of the time the device rebooted...
   *
   */
  if (firstLoop)
  {
    firstLoop = false;
    statusSensor.setValue("connected");
    // allow mqtt to send the "connected" value before changing it to "ready"
    mqtt.loop();
    delay(250);
    statusSensor.setValue("ready");
  }
  // TODO: check WiFi and reconnect if dropped;; TEST it
}