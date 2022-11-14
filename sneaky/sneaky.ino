#include "ArduinoJson.h"
#include "arduino-timer.h"
#include "Motors.h"

#define A_IN1 11
#define A_IN2 10
#define A_PWM 5

#define B_IN1 13
#define B_IN2 A0
#define B_PWM 6

#define STBY 12

#define BAT A1

#define LEFT_DIR 1
#define RIGHT_DIR -1

#define VOLTAGE_CONVERSION_FACTOR 157.49
Timer<1> timer;
Motors motors(A_IN1, A_IN2, A_PWM, B_IN1, B_IN2, B_PWM, STBY);

String inputString = "";
bool stringComplete = false;

bool sendBatteryVoltage(void *argument)
{
  StaticJsonDocument<200> doc;

  float voltage = analogRead(BAT) / VOLTAGE_CONVERSION_FACTOR;

  battery["min"] = 3.2;
  battery["max"] = 4.2;
  battery["uom"] = "V";
  battery["charging"] = false;
  battery["value"] = voltage;

  String telemetry = "";

  serializeJson(doc, telemetry);

  Serial.println(telemetry);

  return true;
}

void setup()
{
  inputString.reserve(200);
  Serial.begin(115200);
  while (!Serial)
  {
    continue;
  }
  motors.init();
  timer.every(2000, sendBatteryVoltage);
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

      uint8_t address = (uint8_t(doc["address"]) & 0b11) + 1;

      uint8_t left = doc["controls"]["l"];
      uint8_t right = doc["controls"]["r"];
      uint8_t forward = doc["controls"]["f"];
      uint8_t backward = doc["controls"]["b"];

      uint8_t lift = doc["controls"]["lift"];

      uint16_t leftSpeed = 0;
      uint16_t rightSpeed = 0;

      if (forward == 100 && left == 0 && right == 0)
      {
        leftSpeed = 255;
        rightSpeed = 255;
      }
      else if (forward == 100 && left == 100)
      {
        leftSpeed = 127;
        rightSpeed = 255;
      }
      else if (forward == 100 && right == 100)
      {
        leftSpeed = 255;
        rightSpeed = 127;
      }
      else if (forward == 0 && backward == 0 && left == 100)
      {
        leftSpeed = -255;
        rightSpeed = 255;
      }
      else if (forward == 0 && backward == 0 && right == 100)
      {
        leftSpeed = 255;
        rightSpeed = -255;
      }
      else if (backward == 100 && left == 0 && right == 0)
      {
        leftSpeed = -255;
        rightSpeed = -255;
      }
      else if (backward == 100 && left == 100)
      {
        leftSpeed = -127;
        rightSpeed = -255;
      }
      else if (backward == 100 && right == 100)
      {
        leftSpeed = -255;
        rightSpeed = -127;
      }
      else if (forward == 0 && left == 0 && right == 0 && backward == 0)
      {
        leftSpeed = 0;
        rightSpeed = 0;
      }

      motors.moveA(leftSpeed * LEFT_DIR);
      motors.moveB(rightSpeed * RIGHT_DIR);
    }
    inputString = "";
    stringComplete = false;
  }
  timer.tick();
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
