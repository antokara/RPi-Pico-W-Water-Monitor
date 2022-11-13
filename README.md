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

*Note that without a proper certificate, we can't enable the secure ports and we will have to enable and use the insecure 1883, 1884 ports.

#### Mosquitto User

go to HA user management and create a dedicated user, that will be used by our Mosquitto client.

Warning:

- changing the password, because it caches the credentials, requires:
  - reload the Mosquitto broker integration first
  - restart of the *Mosquitto broker* addon second
  - otherwise you will get `Connection error: Connection Refused: not authorised. Error: The connection was refused.`
  - if you get only `Error: Connection refused` it means the port you try to use is not open (probably due to certificate issues), it does not mean the credentials are not correct
- certain special characters are not allowed (ie. the underscore is safe to use) and perhaps even very long passwords as well
- the user should not be an admin
- the user should only have access from local network

### IDE

Until PlatformIO properly [supports pico w](https://github.com/platformio/platform-raspberrypi/pull/36), we will use the original Arduino IDE...

### Arduino IDE

1. [download, extract and run the Linux zip](https://www.arduino.cc/en/software) (do not use the flatpak/snap it has issues with serial ports)
1. [follow guide for library installation](https://dawidchyrzynski.github.io/arduino-home-assistant/documents/getting-started/installation.html#arduino-ide)
1. [follow guide for board installation](https://arduino-pico.readthedocs.io/en/latest/install.html#installing-via-arduino-boards-manager)
1. `tools` -> `Board` -> `Raspberry Pi RP2040 Boards(ver.)` -> `Raspberry PI Pico W`
1. `tools` -> `Flash size` -> `2MB (no FS)` _since we won't be using any file system_
1. `tools` -> `WiFi Region`-> `USA`
1. disable WiFi debug output:
    1. `tools` -> `Debug Level`-> `None`
    1. `tools` -> `Debug Port`-> `Disabled`
    1. note that these changes require a re-upload to take effect
1. very important, to enable auto-reset after the first upload/boot
    1. `sudo usermod -a -G dialout "$USER"`
    1. reboot
    1. `groups` should now list `dialout` [(more info)](https://support.arduino.cc/hc/en-us/articles/360016495679-Fix-port-access-on-Linux)
1. hold the BOOTSEL button down while plugging in the Pico to your computer [(more info)](https://arduino-pico.readthedocs.io/en/latest/install.html#uploading-sketches)
1. connect the board with the USB cable
1. `tools` -> `Port` -> `UF2 Board`
1. `tools` -> `USB Stack` -> `Pico SDK` [(more info)](https://arduino-pico.readthedocs.io/en/latest/usb.html)
1. `tools` -> `Get Board Info` (should not error and return at least the `BN: Raspberry Pi Pico W`)
1. at this point, the sketch example Blink, should compile, upload and make the connected device blink
1. after first upload+boot, the port should automatically change. If not: `tools` -> `Port` -> `ttyAMC0`
    1. if you need to manually reset the port: `sudo stty -F /dev/ttyACM0 1200` and then select the port again

### VS Code

1. install VS Code
1. make sure the *Arduino* and *z-uno* extensions are not installed or at least, *disabled* if installed
1. [install the PlatformIO extension](https://platformio.org/platformio-ide)
1. open Libraries of PIO from side panel, search for `home-assistant-integration` and install it
1. install [99-platformio-udev.rules](https://docs.platformio.org/en/latest/core/installation/udev-rules.html)
    1. `curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/master/scripts/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules`
    1. on Fedora:
        1. `sudo udevadm control --reload-rules && sudo udevadm trigger`
        1. `sudo usermod -a -G dialout $USER`
        1. `sudo usermod -a -G plugdev $USER`
    1. After this file is installed, physically unplug and reconnect your board
1. to initialize the project
    1. `pio project init`
    1. should give `Project has been successfully updated!`
1. to build
    1. `>PlatformIO: Build` or `pio run` or from the bottom left corner of the IDE
    1. should result in `[SUCCESS]`
1. to monitor the serial port for debugging
    1. `>PlatformIO: Serial Monitor` or from the bottom left corner of the IDE
    1. should open up a new Terminal with the serial monitor
1. to upload
    1. `>PlatformIO: Upload` or from the bottom left corner of the IDE
    1. should show progress `Loading into Flash: [====] 100%` and `[SUCCESS]`
    1. if not, make sure you have installed the udev rules properly...

## troubleshooting

### clear arduino compile cache

`rm /tmp/arduino* -rf`

## references

1. [Raspberry Pi Pico W Home Assistant Starter Project Using arduino-pico](https://github.com/daniloc/PicoW_HomeAssistant_Starter)
1. [Arduino-Pico documentation](https://arduino-pico.readthedocs.io/en/latest/)
1. [Arduino-Pico repo](https://github.com/earlephilhower/arduino-pico)
1. [ArduinoHA documentation](https://dawidchyrzynski.github.io/arduino-home-assistant/)
1. [ArduinoHA repo](https://github.com/dawidchyrzynski/arduino-home-assistant/)
