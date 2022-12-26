#include <Arduino.h>
#include <Wire.h>
#include "Motor.h"
#include "IMU.h"
#include "Ball.h"

Motor motor;

void setup() {
   Serial.begin(9600);
   Wire.begin();
   Gyro_init();
   motor.free();
   IR_init();
   while(1) {
      IRUpdate();
      Serial.println(BallAngle);
   }
}

void loop() {
   IRUpdate();
   Serial.println(BallAngle);
   speed = 100;

   IRUpdate();
   motor.turnFront();

   if(!isNoBall) {
      if(BallStr > 450) {
         // 回り込み
         if(isBallFront) motor.run(0);
         else {
            float add = 0.2;
            motor.run(BallAngle + BallAngle*add);
         }
      }
      else {
         motor.run(BallAngle);
      }
   }
}