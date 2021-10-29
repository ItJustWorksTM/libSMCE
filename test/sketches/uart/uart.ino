void setup() {
    // Set up the serial uart on Arduino, the speed.
    Serial.begin(9600);
}

void loop() {
    if(Serial.available() == 11)
        Serial.print(Serial.readString());

    if(Serial.available() == 12) {
        const char expected[12] = "HELLO WORLD";
        char toWrite[12];
        if(Serial.availableForWrite() == 64) {
            for (int i = 0; i < sizeof(expected); i++) {
                if (Serial.peek() == expected[i]) {
                    toWrite[i] = Serial.read();
                }
            }
            for (int i = 0; i < sizeof(toWrite); i++){
                Serial.print(toWrite[i]);
            }
        }
    }
}
