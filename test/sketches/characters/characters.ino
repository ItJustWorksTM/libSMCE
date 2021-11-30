void setup() {
    pinMode(0, OUTPUT);
    pinMode(2, OUTPUT);
}

void loop() {

    // Should write true if all parts works as expected.
    // Correct values are tested here.
    digitalWrite(2, isAlpha('A') && isAlphaNumeric('2') && isAscii('!') && isPrintable('!') && isControl('\n')
                        && isDigit('2') && isGraph('|') && isHexadecimalDigit('F') && isLowerCase('z')
                        && isPunct('.') && isSpace(' ') && isUpperCase('Z') && isWhitespace('\t'));

    // Should write true if all parts works as expected.
    // Incorrect values are tested here.
    digitalWrite(0, !isAlpha('1') && !isAlphaNumeric('\n') && !isControl('1')
                        && !isDigit('a') && !isGraph(' ') && !isHexadecimalDigit('&') && !isLowerCase('Z')
                        && !isPunct('1') && !isSpace('a') && !isUpperCase('z') && !isWhitespace('2'));
    delay(1);
}
