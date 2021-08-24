#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include "ArduinoJson.h"

#define CSN 10
#define CE 9

RF24 radio(CE, CSN);

String inputString = "";
bool stringComplete = false;

uint16_t addMovement(uint16_t lSpeeed, uint16_t lDirection, uint16_t rSpeeed, uint16_t rDirection) {
  return (rSpeeed << 2) + (rDirection << 7) + (lSpeeed << 8) + (lDirection << 13);
}

void setup() {
  inputString.reserve(200);
  Serial.begin(115200);
  while (!Serial) {
    continue;
  }
  pinMode(LED_BUILTIN, OUTPUT);
  if (!radio.begin()) {
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(2);
  radio.stopListening();
}

void loop() {
  if (stringComplete) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, inputString);
    if (!error) {
      uint16_t command = 0;

      uint8_t address = (uint8_t(doc["address"]) & 0b11) + 1;

      uint8_t left = doc["controls"]["l"];
      uint8_t right = doc["controls"]["r"];
      uint8_t forward = doc["controls"]["f"];
      uint8_t backward = doc["controls"]["b"];

      uint8_t lift = doc["controls"]["lift"];

      uint16_t m_speed = 31;

      if (forward == 100 && left == 0 && right == 0) {
        command += addMovement(m_speed, 1, m_speed, 1);
      } else if (forward == 100 && left == 100) {
        command += addMovement(m_speed / 2, 1, m_speed, 1);
      } else if (forward == 100 && right == 100) {
        command += addMovement(m_speed, 1, m_speed / 2, 1);
      } else if (forward == 0 && backward == 0 && left == 100) {
        command += addMovement(m_speed, 0, m_speed, 1);
      } else if (forward == 0 && backward == 0 && right == 100) {
        command += addMovement(m_speed, 1, m_speed, 0);
      } else if (backward == 100 && left == 0 && right == 0) {
        command += addMovement(m_speed, 0, m_speed, 0);
      } else if (backward == 100 && left == 100) {
        command += addMovement(m_speed / 2, 0, m_speed, 0);
      } else if (backward == 100 && right == 100) {
        command += addMovement(m_speed, 0, m_speed / 2, 0);
      } else if (forward == 0 && left == 0 && right == 0 && backward == 0) {
        command += addMovement(0, 0, 0, 0);
      }

      if (lift == 100) {
        command += 1;
      }

      uint8_t data[] = { (command >> 8) & 0b11111111, command & 0b11111111 };

      radio.openWritingPipe(address);
      radio.write(&data, sizeof(data));
    }
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
