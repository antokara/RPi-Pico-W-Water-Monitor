#ifndef DEVICE
#define DEVICE

#define DEVICE_ID "waterMonitorTest"
#define DEVICE_NAME "Water Monitor Test"
#define FIRMWARE_VERSION "1.0.0"

class Device
{
public:
    static void connectoMQTT();
    static void connectToWifi();
    static void setup();
    static void loop();
};

#endif // DEVICE