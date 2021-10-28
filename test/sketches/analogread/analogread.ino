void setup() {
    pinMode(0, INPUT);
    pinMode(2, OUTPUT);
}

void loop() {
    analogWrite(2, analogRead(0));
    delayMicroseconds(1000);
}