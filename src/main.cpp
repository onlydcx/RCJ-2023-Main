#include <Arduino.h>
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
   // for(int i = 0; i < 16; i++) {
   //    pinMode(BallPins[i],INPUT);
   // }
}

void loop() {
   IRUpdate();
   Serial.println(BallAngle);
   // speed = 100;

   // IRUpdate();
   // motor.turnFront();
   // bool isBallFront = (nearAngle == 4)? true: false;

   // if(!isNoBall) {
   //    if(BallStr > 450) {
   //       // 回り込み
   //       if(isBallFront) motor.run(0);
   //       else {
   //          float add = 0.2;
   //          motor.run(BallAngle + BallAngle*add);
   //       }
   //    }
   //    else {
   //       motor.run(BallAngle);
   //    }
   // }
}