#include <MQTT.h>
#include <WiFi.h>

void setup() {
    WiFi.begin("foo", "bar");
    MQTTClient clt;
    WiFiClient net;
    clt.begin(net);
}

void loop() { delay(1); }
