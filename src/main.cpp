#include "Arduino.h"
#include <Wire.h>
#include "Motor.h"
#include "IMU.h"
#include "Ball.h"

Motor motor;

int LinePins[4][2] = {{2,3},{0,0},{0,0},{0,0}};

void setup() {
   Serial.begin(9600);
   Wire.begin();
   Gyro_init();
   IR_init();
   while(1) {
      motor.free();
   }
}

void loop() {
   // speed = 100;
   // IRUpdate();
   // turnFront();

   // if(!isNoBall) {
   //    if(BallStr > 450) {
   //       // 回り込み
   //       if(isBallFront) Motor(0);
   //       else {
   //          float add = 0.2;
   //          Motor(BallAngle + BallAngle*add);
   //       }
   //    }
   //    else {
   //       Motor(BallAngle);
   //    }
   // }
}
