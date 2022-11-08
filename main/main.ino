#include <ArduinoHA.h>
#include <WiFi.h>
#include "secrets.h"
#include "device.h"

#define LED_PIN LED_BUILTIN

int wifiStatus;

WiFiClient client;
HADevice device(DEVICE_ID);
HAMqtt mqtt(client, device);

HASwitch led("testLed");

HASensor statusSensor("status");
HASensorNumber gpmSensor("gpm", HASensorNumber::PrecisionP2);
HASensorNumber psiSensor("psi", HASensorNumber::PrecisionP2);
HASensorNumber gallonsSensor("gallons", HASensorNumber::PrecisionP0);

float gpm = 123.4567;
float psi = 234.5678;
unsigned long gallons = 1;

void onSwitchCommand(bool state, HASwitch *sender)
{
    Serial.print("onSwitchCommand");
    Serial.println(state);
    digitalWrite(LED_PIN, (state ? HIGH : LOW));
    sender->setState(state); // report state back to the Home Assistant

    statusSensor.setValue("ok");
    gpmSensor.setValue(gpm);
    psiSensor.setValue(psi);
    gallonsSensor.setValue(gallons);
}

void setup()
{
    Serial.begin(9600);
    delay(1000); // Give the serial terminal a chance to connect, if present

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

    // confirm and report our IP
    if (wifiStatus == WL_CONNECTED)
    {
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

    // prepate LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // set device's details (optional)
    device.setName(DEVICE_NAME);
    device.setSoftwareVersion(FIRMWARE_VERSION);

    // set icon (optional)
    led.setIcon("mdi:lightbulb");
    led.setName("My Test LED");

    // set sensors
    statusSensor.setName("Status");
    statusSensor.setIcon("mdi:check-circle");

    gpmSensor.setName("Water Flow");
    gpmSensor.setIcon("mdi:water");
    gpmSensor.setUnitOfMeasurement("gpm");

    psiSensor.setName("Water Pressure");
    psiSensor.setIcon("mdi:gauge");
    psiSensor.setDeviceClass("pressure");
    psiSensor.setUnitOfMeasurement("psi");

    gallonsSensor.setName("Gallons Counter");
    gallonsSensor.setIcon("mdi:counter");
    gallonsSensor.setDeviceClass("water");
    gallonsSensor.setUnitOfMeasurement("gal");

    // handle switch state
    led.onCommand(onSwitchCommand);

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

void loop()
{
    mqtt.loop();

    // TODO: check WiFi and reconnect if dropped;; TEST it

    // You can also change the state at runtime as shown below.
    // This kind of logic can be used if you want to control your switch using a button connected to the device.
    // led.setState(true); // use any state you want
}