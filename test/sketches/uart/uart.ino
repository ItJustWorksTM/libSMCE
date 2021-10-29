void setup() {
    // Set up the serial uart on Arduino, the speed.
    Serial.begin(9600);
}

void loop() {
    if(Serial.available() == 11)
        Serial.print(Serial.readString());

    if(Serial.available() == 12) {
        const char expected[] = "HELLO WORLD\0";
        char toWrite[12];
        if(Serial.availableForWrite() == 64) {
            for (int i = 0; i < sizeof(expected); i++) {
                if (Serial.peek() == expected[i]) {
                    toWrite[i] = Serial.read();
                }
            }
        }
        Serial.print(toWrite);
    }
}
