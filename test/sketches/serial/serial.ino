void setup() {
    // Set up the serial uart on Arduino, the speed.
    Serial.begin(9600);
}

void loop() {
    if(Serial.availableForWrite() > 0) {
        if (Serial.peek() == 'H')
            Serial.print(Serial.readString());
    }
}