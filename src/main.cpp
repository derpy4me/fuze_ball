#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>

struct Button
{
  int PIN;
  int numKeyPresses;
  bool currentState;
  bool lastState;
  int lastPushTime;
};

struct Beam
{
  int PIN;
  bool currentState;
  bool lastState;
  int lastPushTime;
};

const char *ssid = "therealslimshady";
const char *password = "pleasestandup";

WebSocketsClient socket;

#define USE_SERIAL Serial
const int DEBOUNCE_TIME = 1000; // Debounce time in milliseconds

int latchPin = 12;                         // Pin connected to ST_CP of 74HC595
int clockPin = 13;                         // Pin connected to SH_CP of 74HC595
int dataPin = 15;                          // Pin connected to DS of 74HC595
Button greenButton = {18, 0, false, false, 0}; // Pin connected to reset/start game button
Button redButton = {5, 0, false, false, 0};    // Pin connected to point undo button
Beam homeBeam = {16, false, false, 0};
Beam awayBeam = {17, false, false, 0};

// Define the encoding of characters 0-F of the common-anode 7-segment Display
// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, b, C, d, E, F
// byte num[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e, 0x00};
// byte num[] = {0xff, 0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
byte num[] = {0xfe, 0xdf, 0xef, 0xf7, 0xfb, 0xfd};

Button debounceButton(Button button)
{
  int now = millis();
  int timeGap = now - button.lastPushTime;

  bool state = digitalRead(button.PIN);

  if (timeGap > DEBOUNCE_TIME)
  {
    if (!state)
    {
      button.currentState = true;
    }
    else
    {
      button.currentState = false;
    }
  }

  button.lastState = button.currentState;
  button.lastPushTime = now;
  return button;
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

  USE_SERIAL.println(length);
  USE_SERIAL.println(type);

  switch (type)
  {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

    // send message to server when Connected
    socket.sendTXT("Connected");
    break;
  case WStype_TEXT:
    USE_SERIAL.printf("[WSc] get text: %s\n", payload);

    // send message to server
    // webSocket.sendTXT("message here");
    break;
  case WStype_BIN:
    USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    // hexdump(payload, length);

    // send data to server
    // webSocket.sendBIN(payload, length)
    break;
  case WStype_ERROR:
  case WStype_PING:
  case WStype_PONG:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
}

Beam debounceBeam(Beam beam)
{
  int now = millis();
  int timeGap = now - beam.lastPushTime;

  bool state = digitalRead(beam.PIN);

  if (timeGap > DEBOUNCE_TIME)
  {
    if (!state)
    {
      beam.currentState = true;
    }
    else
    {
      beam.currentState = false;
    }
  }

  beam.lastState = beam.currentState;
  beam.lastPushTime = now;
  return beam;
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
  pinMode(homeBeam.PIN, INPUT_PULLUP);
  pinMode(awayBeam.PIN, INPUT_PULLUP);

  Serial.begin(115200);

  USE_SERIAL.println("Start Setup");

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  WiFi.mode(WIFI_AP);

  WiFi.begin(ssid, password);

  // WiFi.disconnect();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  socket.begin("67.213.228.103", 8000, "/table/ws/Testing");

  // event handler
  socket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  socket.setReconnectInterval(5000);
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

  redButton = debounceButton(redButton);
  greenButton = debounceButton(greenButton);

  if (redButton.currentState)
  {
    Serial.println("Red button pressed");
  }
  if (greenButton.currentState)
  {
    Serial.println("Green Button pressed");
  }

  Serial.println(greenButton.currentState);
  Serial.println(greenButton.lastState);

  homeBeam = debounceBeam(homeBeam);
  awayBeam = debounceBeam(awayBeam);


  if (homeBeam.currentState && !homeBeam.lastState)
  {
    Serial.println("Home Beam Activated");
  }

  if (awayBeam.currentState && !awayBeam.lastState)
  {
    Serial.println("Away Beam Activated");
  }

  socket.loop();
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