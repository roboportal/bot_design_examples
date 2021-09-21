#include "ArduinoJson.h"

#define LED 2

#define STOP 0b00000
#define FORWARD 0b10000
#define LEFT 0b01000
#define RIGHT 0b00100
#define BACKWARD 0b00010
#define WEAPON 0b00001

#define A0 0b00
#define A1 0b01
#define A2 0b10
#define A3 0b11

#define FREQ_KHZ 38

#define START_US 1700
#define PAUSE_US 330
#define SHORT_US 330
#define LONG_US 1000

#define TIME_FACTOR 1.2

#define COMMAND_LENGTH 5
#define ADDRESS_LENGTH 2

const float PULSE_US = 500 / FREQ_KHZ / TIME_FACTOR;

const unsigned int START_TICKS = START_US / PULSE_US / 2;
const unsigned int SHORT_TICKS = SHORT_US / PULSE_US / 2;
const unsigned int LONG_TICKS = LONG_US / PULSE_US / 2;

String inputString = "";
bool stringComplete = false;

void makeTicks(unsigned int nTicks)
{
  for (unsigned int i = 0; i < nTicks; i++)
  {
    digitalWrite(LED, HIGH);
    delayMicroseconds(PULSE_US);
    digitalWrite(LED, LOW);
    delayMicroseconds(PULSE_US);
  }
  delayMicroseconds(PAUSE_US);
}

void sendStart()
{
  makeTicks(START_TICKS);
}

void sendSequence(byte seq)
{
  if (seq)
  {
    makeTicks(LONG_TICKS);
  }
  else
  {
    makeTicks(SHORT_TICKS);
  }
}

void sendCommand(byte command, byte address)
{
  sendStart();

  byte byteToCalculateParity = (command << ADDRESS_LENGTH) + address;
  int onesCounter = 0;

  for (int i = COMMAND_LENGTH + ADDRESS_LENGTH - 1; i >= 0; i--)
  {
    if ((byteToCalculateParity >> i) & 1)
    {
      onesCounter++;
    }
  }

  for (int i = COMMAND_LENGTH - 1; i >= 0; i--)
  {
    sendSequence((command >> i) & 1);
  }

  sendSequence(onesCounter % 2);

  for (int i = ADDRESS_LENGTH - 1; i >= 0; i--)
  {
    sendSequence((address >> i) & 1);
  }
}

void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  if (stringComplete)
  {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, inputString);
    if (!error)
    {
      uint16_t command = 0;

      uint8_t address = (uint8_t(doc["address"]) & 0b11);

      uint8_t left = doc["controls"]["l"];
      uint8_t right = doc["controls"]["r"];
      uint8_t forward = doc["controls"]["f"];
      uint8_t backward = doc["controls"]["b"];

      uint8_t lift = doc["controls"]["lift"];

      if (forward == 100 && left == 0 && right == 0)
      {
        sendCommand(FORWARD, address);
      }
      else if (forward == 100 && left == 100)
      {
        sendCommand(LEFT, address);
      }
      else if (forward == 100 && right == 100)
      {
        sendCommand(RIGHT, address);
      }
      else if (forward == 0 && backward == 0 && left == 100)
      {
        sendCommand(LEFT, address);
      }
      else if (forward == 0 && backward == 0 && right == 100)
      {
        sendCommand(RIGHT, address);
      }
      else if (backward == 100 && left == 0 && right == 0)
      {
        sendCommand(BACKWARD, address);
      }
      else if (backward == 100 && left == 100)
      {
        sendCommand(LEFT, address);
      }
      else if (backward == 100 && right == 100)
      {
        sendCommand(RIGHT, address);
      }
      else if (forward == 0 && left == 0 && right == 0 && backward == 0 && lift == 0)
      {
        sendCommand(STOP, address);
      }
      else if (lift == 100)
      {
        sendCommand(WEAPON, address);
      }
    }
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n')
    {
      stringComplete = true;
    }
  }
}