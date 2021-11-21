#ifndef Motors_h
#define Motors_h

#include "Arduino.h"

class Motors
{
public:
  Motors(int aIn1, int aIn2, int aPwm, int bIn1, int bIn2, int bPwm, int stb);
  void init();
  void moveA(int speed);
  void moveB(int speed);

private:
  int _aIn1;
  int _aIn2;
  int _aPwm;
  int _bIn1;
  int _bIn2;
  int _bPwm;
  int _stb;
};

#endif
