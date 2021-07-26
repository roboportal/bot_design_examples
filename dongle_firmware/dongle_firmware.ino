#include "ChiNRF.h"
#include "ArduinoJson.h"

#define LED 13

void onPacketReceived();

ChiNRF radio(chip_RFM75);

String inputString = "";
bool stringComplete = false;

uint16_t addMovement(uint16_t lSpeeed, uint16_t lDirection, uint16_t rSpeeed, uint16_t rDirection)
{
  return (rSpeeed << 2) + (rDirection << 7) + (lSpeeed << 8) + (lDirection << 13);
}

void setup()
{
  inputString.reserve(200);

  Serial.begin(115200);
  while (!Serial)
    continue;

  radio.setOnReceive(onPacketReceived);
  radio.begin(2, 14, 16, 15, A5, A4);
  radio.setTxAddress(0x1122332211LL);
  radio.setRxAddressP0(0x1122332210LL);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}

void loop()
{
  radio.tick();

  if (stringComplete)
  {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, inputString);
    if (!error)
    {
      uint16_t command = 0;

      uint8_t address = uint8_t(doc["address"]) & 0b11; // 0-3
      uint8_t left = doc["controls"]["left"];           // 0-100
      uint8_t right = doc["controls"]["right"];         // 0-100
      uint8_t forward = doc["controls"]["forward"];     // 0-100
      uint8_t backward = doc["controls"]["backward"];   // 0-100

      uint16_t m_speed = 31; // 0-31

      command += address;
      if (forward == 100 && (left == 0 || right == 0))
      {
        command += addMovement(m_speed, 1, m_speed, 1);
      }
      else if (forward == 100 && left == 100)
      {
        command += addMovement(m_speed / 2, 1, m_speed, 1);
      }
      else if (forward == 100 && right == 100)
      {
        command += addMovement(m_speed, 1, m_speed / 2, 1);
      }
      else if (forward == 0 && backward == 0 && left == 100)
      {
        command += addMovement(m_speed, 0, m_speed, 1);
      }
      else if (forward == 0 && backward == 0 && right == 100)
      {
        command += addMovement(m_speed, 1, m_speed, 0);
      }
      else if (backward == 100 && (left == 0 || right == 0))
      {
        command += addMovement(m_speed, 0, m_speed, 0);
      }
      else if (backward == 100 && left == 100)
      {
        command += addMovement(m_speed / 2, 0, m_speed, 0);
      }
      else if (backward == 100 && right == 100)
      {
        command += addMovement(m_speed, 0, m_speed / 2, 0);
      }
      else if (forward == 0 && left == 0 && right == 0 && backward == 0)
      {
        command += addMovement(0, 0, 0, 0);
      }
      uint8_t data[] = {(command >> 8) & 0b11111111, command & 0b11111111};
      radio.writeTxPayload((uint8_t *)(&data), 2);
    }
    inputString = "";
    stringComplete = false;
  }
  if (Serial.available())
  {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n')
    {
      stringComplete = true;
    }
  }
}

void onPacketReceived()
{
  StaticJsonDocument<200> doc;
  uint8_t data;
  radio.readRxPayload((uint8_t *)(&data), 1);
  doc["Address"] = data & 0b11;
  doc["Wound"] = bool(data & 0b100);
  serializeJson(doc, Serial);
  Serial.println("");
}
