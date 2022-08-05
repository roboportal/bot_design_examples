#include "Arduino.h"
#include "Motors.h"

#define DIR_0_R 12
#define DIR_1_R 10

#define PWM_0_R 9
#define PWM_1_R 6

#define DIR_0_L 4
#define DIR_1_L 7

#define PWM_0_L 3
#define PWM_1_L 5

#define STBY 8

Motors::Motors()
{
  pinMode(DIR_0_L, OUTPUT);
  pinMode(DIR_1_L, OUTPUT);

  pinMode(DIR_0_R, OUTPUT);
  pinMode(DIR_1_R, OUTPUT);

  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);
}

void Motors::moveL(int speed)
{
  if (speed == 0)
  {
    digitalWrite(DIR_0_L, LOW);
    digitalWrite(DIR_1_L, LOW);

    analogWrite(PWM_0_L, 0);
    analogWrite(PWM_1_L, 0);
  }
  else if (speed > 0)
  {
    digitalWrite(DIR_0_L, LOW);
    digitalWrite(DIR_1_L, HIGH);

    analogWrite(PWM_0_L, speed);
    analogWrite(PWM_1_L, speed);
  }
  else
  {
    digitalWrite(DIR_0_L, HIGH);
    digitalWrite(DIR_1_L, LOW);

    analogWrite(PWM_0_L, -speed);
    analogWrite(PWM_1_L, -speed);
  }
}

void Motors::moveR(int speed)
{
  if (speed == 0)
  {
    digitalWrite(DIR_0_R, LOW);
    digitalWrite(DIR_1_R, LOW);

    analogWrite(PWM_0_R, 0);
    analogWrite(PWM_1_R, 0);
  }
  else if (speed > 0)
  {
    digitalWrite(DIR_0_R, LOW);
    digitalWrite(DIR_1_R, HIGH);

    analogWrite(PWM_0_R, speed);
    analogWrite(PWM_1_R, speed);
  }
  else
  {
    digitalWrite(DIR_0_R, HIGH);
    digitalWrite(DIR_1_R, LOW);

    analogWrite(PWM_0_R, -speed);
    analogWrite(PWM_1_R, -speed);
  }
}
