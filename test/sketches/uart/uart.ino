void setup() {
    Serial.begin(9600);
}

void loop() {
    if(Serial.available() == 11) {
        char expected[] = "HELLO";
        char toWrite[5];

        if(Serial.availableForWrite() == 64 && Serial.peek() == expected[0]) {
            for (int i = 0; i < sizeof(expected); i++) {
                if (Serial.peek() == expected[i]) {
                    toWrite[i] = Serial.read();
                }
            }
            String rest = Serial.readString();
            for (int i = 0; i < sizeof(toWrite); i++){
                Serial.print(toWrite[i]);
            }
            Serial.print(rest);
        }
        else
            Serial.print(Serial.readString());
    }
    else if (Serial.available() > 0)
        Serial.print(Serial.readString());
}
