# RPi Pico W Water Monitor

A Home Assistant monitor for water flow and pressure for leaks, with an optional water shutoff valve in case of emergencies. Powered by the [Raspberry Pi Pico W board](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html).

## prerequisites

### Mosquitto MQTT broker

#### Mosquitto Installation

[install the Mosquitto MQTT broker on HA](https://github.com/home-assistant/addons/blob/master/mosquitto/DOCS.md)

#### Mosquitto Options

```yaml
    logins: []
    require_certificate: false
    certfile: fullchain.pem
    keyfile: privkey.pem
    customize:
    active: false
    folder: mosquitto
```

#### Mosquitto User

go to HA user management and create a dedicated user (not admin), that will be used by Mosquitto.

### VS Code

1. [install the PlatformIO extension](https://platformio.org/platformio-ide)
1. 

## troubleshooting

## references

1. [Raspberry Pi Pico W Home Assistant Starter Project Using arduino-pico](https://github.com/daniloc/PicoW_HomeAssistant_Starter)
1. [Arduino-Pico repo](https://github.com/earlephilhower/arduino-pico)
1. [ArduinoHA repo](https://github.com/dawidchyrzynski/arduino-home-assistant/)
1. [ArduinoHA documentation](https://dawidchyrzynski.github.io/arduino-home-assistant/)