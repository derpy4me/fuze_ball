struct Button {
  const int PIN;
  int numKeyPresses;
  bool pressed;
};

int latchPin = 12;          // Pin connected to ST_CP of 74HC595（Pin12）
int clockPin = 13;          // Pin connected to SH_CP of 74HC595（Pin11）
int dataPin = 15;           // Pin connected to DS of 74HC595（Pin14）
Button redButton = {0, 0, false};
Button greenButton = {4, 0, false};
int homeBeam = 16;
int awayBeam = 17;

// Define the encoding of characters 0-F of the common-anode 7-segment Display
// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b, C, d, E, F
// byte num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0x00};
// byte num[] = {0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
byte num[] = {0xfe, 0xdf, 0xef, 0xf7, 0xfb, 0xfd};
char inChar;

void IRAM_ATTR redButtonPressed() {
  redButton.numKeyPresses += 1;
  redButton.pressed = true;
}

void IRAM_ATTR greenButtonPressed() {
  greenButton.numKeyPresses += 1;
  greenButton.pressed = true;
}

void setup() {
  // set pins to output
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(redButton.PIN, INPUT);
  attachInterrupt(redButton.PIN, redButtonPressed, FALLING);
  pinMode(greenButton.PIN, INPUT);
  attachInterrupt(greenButton.PIN, greenButtonPressed, FALLING);
  pinMode(homeBeam, INPUT);
  pinMode(awayBeam, INPUT);
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
  if (redButton.pressed) {
    Serial.printf("Red Button has been pressed %u times\n", redButton.numKeyPresses);
    redButton.pressed = false;
  }
  if (greenButton.pressed) {
    Serial.printf("Green Button has been pressed %u times\n", greenButton.numKeyPresses);
    greenButton.pressed = false;
  }
}

void serialEvent() {
  if (Serial.available()) {
      inChar = Serial.read();
      Serial.print("received: ");
      Serial.println(inChar);
    }
}