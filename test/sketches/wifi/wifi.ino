#include <WiFi.h>
#include <MQTT.h>

void setup() {
#ifndef __SMCE__
    WiFi.begin("foo", "bar");
#endif
    MQTTClient clt;
    clt.begin(WiFi);
}

void loop() { delay(1); }