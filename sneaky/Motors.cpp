#include "Arduino.h"
#include "Motors.h"

Motors::Motors(int aIn1, int aIn2, int aPwm, int bIn1, int bIn2, int bPwm, int stb)
{
  _aIn1 = aIn1;
  _aIn2 = aIn2;
  _aPwm = aPwm;
  _bIn1 = bIn1;
  _bIn2 = bIn2;
  _bPwm = bPwm;
  _stb = stb;
}

void Motors::init()
{
  pinMode(_aIn1, OUTPUT);
  pinMode(_aIn2, OUTPUT);
  pinMode(_bIn1, OUTPUT);
  pinMode(_bIn2, OUTPUT);
  pinMode(_stb, OUTPUT);
  digitalWrite(_stb, HIGH);
}

void Motors::moveA(int speed)
{
  if (speed == 0)
  {
    digitalWrite(_aIn1, LOW);
    digitalWrite(_aIn2, LOW);
    analogWrite(_aPwm, 0);
  }
  else if (speed > 0)
  {
    digitalWrite(_aIn1, LOW);
    digitalWrite(_aIn2, HIGH);
    analogWrite(_aPwm, speed);
  }
  else
  {
    digitalWrite(_aIn1, HIGH);
    digitalWrite(_aIn2, LOW);
    analogWrite(_aPwm, -speed);
  }
}

void Motors::moveB(int speed)
{
  if (speed == 0)
  {
    digitalWrite(_bIn1, LOW);
    digitalWrite(_bIn2, LOW);
    analogWrite(_bPwm, 0);
  }
  else if (speed > 0)
  {
    digitalWrite(_bIn1, HIGH);
    digitalWrite(_bIn2, LOW);
    analogWrite(_bPwm, speed);
  }
  else
  {

    digitalWrite(_bIn1, LOW);
    digitalWrite(_bIn2, HIGH);
    analogWrite(_bPwm, -speed);
  }
}
