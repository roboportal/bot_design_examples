#include "ChiNRF.h"
#include "Motors.h"
#include "PololuLedStrip.h"

#define BOT_ADDR 0

#define A_IN1 A3
#define A_IN2 A2
#define A_PWM 10

#define B_IN1 6
#define B_IN2 4
#define B_PWM 9

#define STB 8

#define LED_D A1

#define WOUND 12
#define LED 13

#define LEFT_DIR -1
#define RIGHT_DIR 1

void onPacketReceived();

ChiNRF radio(chip_RFM75);
Motors motors(A_IN1, A_IN2, A_PWM, B_IN1, B_IN2, B_PWM, STB);

PololuLedStrip<A1> led;
rgb_color colors[1];

void initColor()
{
  switch (BOT_ADDR)
  {
  case 0:
    colors[0].red = 255;
    colors[0].green = 0;
    colors[0].blue = 0;
    break;
  case 1:
    colors[0].red = 0;
    colors[0].green = 255;
    colors[0].blue = 0;
    break;
  case 2:
    colors[0].red = 0;
    colors[0].green = 0;
    colors[0].blue = 255;
    break;
  case 3:
    colors[0].red = 255;
    colors[0].green = 255;
    colors[0].blue = 0;
    break;
  }
}

void setup()
{
  //  initColor();
  //  led.write(colors, 1);

  //  Serial.begin(115200);
  //  while (!Serial) continue;
  motors.init();

  radio.setOnReceive(onPacketReceived);
  radio.begin(2, 14, 16, 15, A5, A4);
  radio.setRxPayloadWidth(0, 2);
  radio.setRxAddressP0(0x1122332211LL);
  radio.setTxAddress(0x1122332210LL);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  pinMode(WOUND, INPUT_PULLUP);
}

void loop()
{
  radio.tick();
  if (!digitalRead(WOUND))
  {
    uint8_t data = 0 | BOT_ADDR | 0b100;
    radio.writeTxPayload((uint8_t *)(&data), 1);
  }
}

void onPacketReceived()
{
  uint8_t data[2];
  radio.readRxPayload((uint8_t *)(&data), 2);
  uint16_t result = data[1] + (data[0] << 8);
  //  Serial.print("data: ");
  //  Serial.println(result, BIN);
  uint8_t botAddr = result & 0b11;

  uint8_t leftDir = result >> 13 & 0b1;
  int left = (leftDir ? LEFT_DIR : -LEFT_DIR) * map((result >> 8) & 0b11111, 0, 31, 0, 255);

  uint8_t rightDir = result >> 7 & 0b1;
  int right = (rightDir ? RIGHT_DIR : -RIGHT_DIR) * map((result >> 2) & 0b11111, 0, 31, 0, 255);

  if (botAddr == BOT_ADDR)
  {
    motors.moveA(left);
    motors.moveB(right);
  }
}