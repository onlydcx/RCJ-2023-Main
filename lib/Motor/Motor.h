#ifndef __MOTOR_H_
#define __MOTOR_H_

extern int speed;

class Motor {
   public:
      Motor();
      void run(int angle);
      void roll(int num, int speed);
      void stop();
      void free();
      void turnFront();
      void turn(int dir, bool absolute);
};

#endif