#include "ArduinoJson.h"
#include "arduino-timer.h"

Timer<1> timer;
String inputString = "";
bool stringComplete = false;

// telemetry callback, called via timer interrupt
bool sendTelemetry(void *argument)
{
  StaticJsonDocument<400> doc;

  doc["id"] = 0;

  // unused nested objects could be removed
  JsonObject battery = doc.createNestedObject("battery");
  JsonObject location = doc.createNestedObject("location");
  JsonObject genericData = doc.createNestedObject("genericData");

  // setup of minimum / maximum battery range and unit of measure to display
  // required to correctly display battery value as progress on widget
  battery["min"] = 3.2;
  battery["max"] = 4.2;
  battery["uom"] = "V";

  // charging status indicator, 'true' when connected to charger
  battery["charging"] = false;

  // this could be measured, for example with ADC or battery management IC
  battery["value"] = 3.9;

  // those values could be obtained from GPS module
  location["lat"] = 49.2766275048836;
  location["lng"] = -123.13133235249737;
  location["headingAngle"] = 90;

  // values obtained from sensors
  genericData["speed"] = "0.1 m/s";
  genericData["temperature"] = "23 C";

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
  // setting up timer to run telemetry callback each 2s
  timer.every(2000, sendTelemetry);
}

void loop()
{
  if (stringComplete)
  {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, inputString);
    if (!error)
    {
      // access controls object
      uint8_t button = doc["controls"]["b"];

      if (button == 100)
      {
        // run motor, servo, etc.
      }
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
