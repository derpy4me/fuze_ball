#include <Arduino.h>

struct Button
{
  const int PIN;
  int numKeyPresses;
  bool currentState;
  int lastPushTime;
  int timeGap;
};

const int DEBOUNCE_TIME = 50; // Debounce time in milliseconds

int latchPin = 12;                         // Pin connected to ST_CP of 74HC595
int clockPin = 13;                         // Pin connected to SH_CP of 74HC595
int dataPin = 15;                          // Pin connected to DS of 74HC595
Button greenButton = {18, 0, false, 0, 0}; // Pin connected to reset/start game button
Button redButton = {5, 0, false, 0, 0};    // Pin connected to point undo button
int homeBeam = 16;                         // Pin connected to home side break beam
int awayBeam = 17;                         // Pin connected to away side break beam

// Define the encoding of characters 0-F of the common-anode 7-segment Display
// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b, C, d, E, F
// byte num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0x00};
// byte num[] = {0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
byte num[] = {0xfe, 0xdf, 0xef, 0xf7, 0xfb, 0xfd};

bool debounceButton(Button button) {
  int now = millis();
  button.timeGap = now - button.lastPushTime;

  bool state = digitalRead(button.PIN);

  Serial.println();

  if (button.timeGap > DEBOUNCE_TIME) {
    if (!state) {
      button.currentState = true;
    } else {
      button.currentState = false;
    }
  }

  button.lastPushTime = now;
  return button.currentState;
}

void setup()
{
  // Set output pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Set input pins
  pinMode(redButton.PIN, INPUT_PULLUP);
  pinMode(greenButton.PIN, INPUT_PULLUP);
  pinMode(homeBeam, INPUT);
  pinMode(awayBeam, INPUT);

  Serial.begin(115200);
}

void loop()
{
  // Cycling display
  // for (int i = 0; i <= 0x05; i++)
  // {
  //   // Output low level to latchPin
  //   digitalWrite(latchPin, LOW);
  //   // Send serial data to 74HC595
  //   shiftOut(dataPin, clockPin, MSBFIRST, num[i]);
  //   // Output high level to latchPin, and 74HC595 will update the data to the parallel output port.
  //   digitalWrite(latchPin, HIGH);
  //   delay(500);
  // }

  redButton.currentState = debounceButton(redButton);
  greenButton.currentState = debounceButton(greenButton);

  Serial.printf("Green Button %u", greenButton.currentState);

  if (redButton.currentState) {
    Serial.println("Red button pressed");
  }
  if (greenButton.currentState) {
    Serial.println("Green Button pressed");
  }
  delay(100);
}

// void serialEvent()
// {
//   char inChar;
//   // Serial Interrupt event
//   if (Serial.available())
//   {
//     inChar = Serial.read();
//     Serial.print("received: ");
//     Serial.println(inChar);
//   }
// }