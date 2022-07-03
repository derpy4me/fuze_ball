struct Button
{
  const int PIN;
  int numKeyPresses;
  bool lastState;
  bool currentState;
  bool lastFlicker;
  int debounceTime;
};

long DEBOUNCE_TIME = 50; // Debounce time in milliseconds

int latchPin = 12;                                    // Pin connected to ST_CP of 74HC595
int clockPin = 13;                                    // Pin connected to SH_CP of 74HC595
int dataPin = 15;                                     // Pin connected to DS of 74HC595
Button greenButton = {18, 0, false, false, false, 0}; // Pin connected to reset/start game button
Button redButton = {5, 0, false, false, false, 0};    // Pin connected to point undo button
int homeBeam = 16;                                    // Pin connected to home side break beam
int awayBeam = 17;                                    // Pin connected to away side break beam

// Define the encoding of characters 0-F of the common-anode 7-segment Display
// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b, C, d, E, F
// byte num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0x00};
// byte num[] = {0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
byte num[] = {0xfe, 0xdf, 0xef, 0xf7, 0xfb, 0xfd};
char inChar;
void debounceCheck(Button button)
{
  if (button.currentState != button.lastFlicker)
  {
    button.debounceTime = millis();
    button.lastFlicker = button.currentState;
  }

  if ((millis() - button.debounceTime) > DEBOUNCE_TIME)
  {
    if (button.lastState && !button.currentState)
    {
      Serial.printf("Pin u% button pressed\n", button.PIN);
      button.numKeyPresses += 1;
    }
    else if (!button.lastState && button.currentState)
    {
      Serial.printf("Pin u% button released\n", button.PIN);
    }

    button.lastState = button.currentState;
  }
}

void IRAM_ATTR redButtonPressed()
{
  // Interrupt for red button press
  debounceCheck(redButton);
}

void IRAM_ATTR greenButtonPressed()
{
  // Interrupt for green button press
  debounceCheck(greenButton);
}

void setup()
{
  // Set output pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Set input pins
  pinMode(redButton.PIN, INPUT_PULLUP);
  attachInterrupt(redButton.PIN, redButtonPressed, RISING);
  pinMode(greenButton.PIN, INPUT_PULLUP);
  attachInterrupt(greenButton.PIN, greenButtonPressed, RISING);
  pinMode(homeBeam, INPUT);
  pinMode(awayBeam, INPUT);

  Serial.begin(115200);
}

void loop()
{
  // Serial.println("Loop is working");
  // Cycling display
  // for (int i = 0; i <= 0x05; i++) {
  //   // Output low level to latchPin
  //   digitalWrite(latchPin, LOW);
  //   // Send serial data to 74HC595
  //   shiftOut(dataPin, clockPin, MSBFIRST, num[i]);
  //   // Output high level to latchPin, and 74HC595 will update the data to the parallel output port.
  //   digitalWrite(latchPin, HIGH);
  //   delay(500);
  // }

  if (redButton.currentState) {
    Serial.printf("Red Button pressed u% times\n", redButton.numKeyPresses);
  }
    if (greenButton.currentState) {
    Serial.printf("Green Button pressed u% times\n", greenButton.numKeyPresses);
  }
}

void serialEvent()
{
  // Serial Interrupt event
  if (Serial.available())
  {
    inChar = Serial.read();
    Serial.print("received: ");
    Serial.println(inChar);
  }
}