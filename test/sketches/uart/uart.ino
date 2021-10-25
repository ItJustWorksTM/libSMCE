void setup() {
    Serial.begin(9600);
}

void loop() {
    myTime = millis();
    Time = micros();
    if(Serial.available() > 0)
        Serial.print(Serial.readString());
}
