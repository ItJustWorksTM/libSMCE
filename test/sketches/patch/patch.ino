extern int foobar();

void setup() {
    pinMode(0, OUTPUT);
    pinMode(2,INPUT);
    analogWrite(0, foobar());
    analogRead(2);
}

void loop() {
    delay(1);
    delayMicroseconds(50);
}
