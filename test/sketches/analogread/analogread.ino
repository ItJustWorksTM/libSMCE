void setup() {
    pinMode(0, INPUT);
    pinMode(2, OUTPUT);
    analogWrite(2, analogRead(0));
}

void loop() {
    delayMicroseconds(1000);
}