
#include <Servo.h>

#include "ArduinoJson.h"
#include "arduino-timer.h"

#include "IP5306.h"
#include "Motors.h"

#define SERVO 11

#define LEFT_DIR -1
#define RIGHT_DIR 1

#define MIN_CAM_POSITION 20
#define MAX_CAM_POSITION 80
#define DEFAULT_CAM_POSITION 60

IP5306 ip5306;
Motors motors;

Servo servo;
Timer<1> batteryTimer;
Timer<1> servoTimer;

int currentServoPosition = DEFAULT_CAM_POSITION;
int servoDirection = 0;

String inputString = "";
bool stringComplete = false;
bool areControlsEnabled = false;

bool moveServo(void *argument)
{
  currentServoPosition = currentServoPosition + servoDirection;

  if (currentServoPosition <= MIN_CAM_POSITION)
  {
    currentServoPosition = MIN_CAM_POSITION;
  }

  if (currentServoPosition >= MAX_CAM_POSITION)
  {
    currentServoPosition = MAX_CAM_POSITION;
  }

  servo.write(currentServoPosition);

  return true;
}

bool sendBatteryVoltage(void *argument)
{
  StaticJsonDocument<200> doc;

  doc["id"] = 0;

  JsonObject battery = doc.createNestedObject("battery");

  battery["min"] = 0;
  battery["max"] = 100;
  battery["uom"] = "%";
  battery["charging"] = ip5306.check_charging_status() ? true : false;
  battery["value"] = ip5306.get_battery_level();

  String telemetry = "";

  serializeJson(doc, telemetry);

  Serial.println(telemetry);

  return true;
}

void setup()
{
  Serial.begin(115200);
  ip5306.begin();

  delay(100);

  ip5306.set_battery_voltage(BATT_VOLTAGE_0);
  ip5306.charger_under_voltage(VOUT_5);
  ip5306.end_charge_current(CURRENT_400);
  ip5306.set_charging_stop_voltage(CUT_OFF_VOLTAGE_3);
  ip5306.set_light_load_shutdown_time(SHUTDOWN_64s);
  ip5306.low_battery_shutdown(ENABLE);
  ip5306.boost_after_vin(ENABLE);
  ip5306.power_on_load(ENABLE);
  ip5306.boost_mode(ENABLE);

  servo.attach(SERVO);
  delay(200);
  servo.write(DEFAULT_CAM_POSITION);

  batteryTimer.every(2000, sendBatteryVoltage);
  servoTimer.every(15, moveServo);

  delay(100);
}

// {"address":0,"controls":{"cu":0,"cd":0,"l":100,"r":0,"f":100,"b":0}}
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

      uint8_t cameraUp = doc["controls"]["cu"];
      uint8_t cameraDown = doc["controls"]["cd"];

      uint16_t leftSpeed = 0;
      uint16_t rightSpeed = 0;

      bool stop = doc["controls"]["stop"];
      bool start = doc["controls"]["start"];

      if (start)
      {
        areControlsEnabled = true;
      }

      if (stop || !areControlsEnabled)
      {
        servoDirection = 0;
        leftSpeed = 0;
        rightSpeed = 0;
        areControlsEnabled = false;
      }
      else
      {
        if (cameraUp == 0 && cameraDown == 0)
        {
          servoDirection = 0;
        }
        else if (cameraUp == 100 && cameraDown == 0)
        {
          servoDirection = 1;
        }
        else if (cameraUp == 0 && cameraDown == 100)
        {
          servoDirection = -1;
        }

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
      }

      motors.moveL(leftSpeed * LEFT_DIR);
      motors.moveR(rightSpeed * RIGHT_DIR);
    }
    inputString = "";
    stringComplete = false;
  }
  batteryTimer.tick();
  servoTimer.tick();
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
