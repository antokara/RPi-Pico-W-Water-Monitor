#include <ArduinoHA.h>
#include <WiFi.h>
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
        delay(5000);
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
 * @brief should be called once, from the main setup() function
 *
 */
void Device::setup()
{
    Serial.begin(9600);
    delay(1000); // Give the serial terminal a chance to connect, if present

    Device::connectToWifi();

    // set device's details
    device.setName(DEVICE_NAME);
    device.setSoftwareVersion(FIRMWARE_VERSION);
}

/**
 * @brief should be called on every iteration of the main loop() function
 *
 */
void Device::loop()
{
    mqtt.loop();
    // TODO: check WiFi and reconnect if dropped;; TEST it
}