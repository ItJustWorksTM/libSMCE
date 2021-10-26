void setup() {
    // Set up the serial uart on Arduino, the speed.
    Serial.begin(9600);
}

void loop() {
    if(Serial.available() > 0)
        Serial.print(Serial.readString());
}
