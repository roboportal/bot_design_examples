#include <SPI.h>
#include <Servo.h>

#include "nRF24L01.h"
#include "RF24.h"

#include "Motors.h"


#define A_IN1 7
#define A_IN2 8
#define A_PWM 6

#define B_IN1 3
#define B_IN2 2
#define B_PWM 5

#define STBY 4

#define SRV 9

#define CSN A1
#define CE 10

#define LEFT_DIR 1
#define RIGHT_DIR -1

#define LIFT_DOWN  5
#define LIFT_UP  100

#define BOT_ADDRESS 1

Motors motors(A_IN1, A_IN2, A_PWM, B_IN1, B_IN2, B_PWM, STBY);
RF24 radio(CE, CSN);
Servo lift;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    continue;
  }
  pinMode(LED_BUILTIN, OUTPUT);

  motors.init();
  lift.attach(SRV);

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
  radio.openReadingPipe(1, BOT_ADDRESS);
  radio.startListening();

  lift.write(LIFT_DOWN);
}

void loop() {
  uint8_t pipe;
  if (radio.available(&pipe)) {
    uint8_t bytes = radio.getPayloadSize();
    uint8_t data[2];
    radio.read((uint8_t *)(&data), 2);
    uint16_t result = data[1] + (data[0] << 8);

    uint8_t leftDir = result >> 13 & 0b1;
    int left = (leftDir ? LEFT_DIR : -LEFT_DIR) * map((result >> 8) & 0b11111, 0, 31, 0, 255);

    uint8_t rightDir = result >> 7 & 0b1;
    int right = (rightDir ? RIGHT_DIR : -RIGHT_DIR) * map((result >> 2) & 0b11111, 0, 31, 0, 255);

    uint8_t fire = uint8_t(result & 0b1);

    if (fire) {
      lift.write(LIFT_UP);
    } else {
      lift.write(LIFT_DOWN);
    }
    motors.moveA(left);
    motors.moveB(right);
  }
}
