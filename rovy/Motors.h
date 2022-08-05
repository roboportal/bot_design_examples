#ifndef Motors_h
#define Motors_h

#include "Arduino.h"

#define DIR_0_L 12
#define DIR_1_L 10

#define PWM_0_L 9
#define PWM_1_L 6

#define DIR_0_R 4
#define DIR_1_R 7

#define PWM_0_R 3
#define PWM_1_R 5

#define STBY 8

class Motors
{
  public:
    Motors(void);
    void init();
    void moveL(int speed);
    void moveR(int speed);
};

#endif
