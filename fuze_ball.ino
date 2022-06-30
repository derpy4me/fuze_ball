/*
  Sketch 17.1.1
  1-digit 7-segment Display

  modified 2016/8/17
  by http://www.freenove.com
*/

int latchPin = 12;          // Pin connected to ST_CP of 74HC595（Pin12）
int clockPin = 13;          // Pin connected to SH_CP of 74HC595（Pin11）
int dataPin = 15;           // Pin connected to DS of 74HC595（Pin14）

// Define the encoding of characters 0-F of the common-anode 7-segment Display
// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b, C, d, E, F
// byte num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0x00};
// byte num[] = {0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
byte num[] = {0xfe, 0xdf, 0xef, 0xf7, 0xfb, 0xfd};
char inChar;

void setup() {
  // set pins to output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // Cycling display 0-F
  for (int i = 0; i <= 0x05; i++) {
    // Output low level to latchPin
    digitalWrite(latchPin, LOW);
    // Send serial data to 74HC595
    shiftOut(dataPin, clockPin, MSBFIRST, num[i]);
    // Output high level to latchPin, and 74HC595 will update the data to the parallel output port.
    digitalWrite(latchPin, HIGH);
    delay(500);
  }
}

void serialEvent() {
  if (Serial.available()) {
      inChar = Serial.read();
      Serial.print("received: ");
      Serial.println(inChar);
    }
}