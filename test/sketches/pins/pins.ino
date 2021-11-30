void setup() {
    pinMode(0, INPUT);
    pinMode(2, OUTPUT);
    pinMode(3, INPUT);
    pinMode(4, OUTPUT);
}

void loop() {
    // Writing and reading on digital pins
    digitalWrite(2, !digitalRead(0));

    // Writing and reading on analog pins
    analogWrite(4, analogRead(3));
    delayMicroseconds(1000);
}
