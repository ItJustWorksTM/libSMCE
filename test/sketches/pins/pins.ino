void setup() {
    pinMode(0, INPUT);
    pinMode(2, OUTPUT);
    pinMode(3, INPUT);
    pinMode(4, OUTPUT);
}

void loop() {
    digitalWrite(2, !digitalRead(0));
    analogWrite(4, analogRead(3));
    delay(1);
}
