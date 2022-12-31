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
      speed = 100;
      motor.run(0);
      // IRUpdate();
      // Serial.println(GyroGet());
   }
}

void loop() {
   speed = 150;
   IRUpdate();
   if(isNoBall) {
      motor.free();
   }
   else {
      int toMove = 0;
      if(BallStr < 450) {
         toMove = BallAngle;
      }
      else {
         if(BallAngle <= 15 || BallAngle >= 345) {
            toMove = 0;
         }
         else {
            // スピード落とせ！！
            int param = BallAngle;
            if(param > 180) param = 360 - param;
            if(param > 50) param = 50;
            if(BallAngle > 15 && BallAngle < 180) {
               toMove = BallAngle + param;
            }
            else {
               toMove = BallAngle - param;
            }
         }
      }
      motor.run(toMove);
   }
   // speed = 150;
   // IRUpdate();
   // int toMove = 0;
   // float s = (float)BallStr / 550;
   // if(s >= 1) s = 1.0;
   // s = pow(s,2);
   // int dir = BallAngle;
   // if(dir > 180) dir = 360 - dir;
   // if(dir > 50) dir = 45;
   // dir = dir*s*2;
         
   // if((BallAngle > 10) && (BallAngle <= 180)) {
   //    // Motor(BallAngle + dir);
   //    toMove = BallAngle + dir;
   // }
   // else if ((BallAngle > 180) && (BallAngle < 350)) {
   //    // Motor(BallAngle - dir);
   //    toMove = BallAngle - dir;
   // }
   // else {
   //    // Motor(0);
   //    toMove = 0;
   // }
      


   // if(BallStr > 400) {
   //    if(isBallFront) {
   //       while(isBallFront) {
   //          // Motor(0);
   //          toMove = 0;
   //          if(!isBallFront) break;
   //       }
   //    }
   //     else {
   //   int dir = BallAngle;
   // if(dir > 180) dir = 360 - dir;
   // if(dir > 50) dir = 45;
      // if((BallAngle > 10) && (BallAngle <= 135)) {
      //       // Motor(BallAngle + dir);
      //       toMove = BallAngle + dir;
      //    }
      //    else if ((BallAngle > 225) && (BallAngle < 350)) {
      //       // Motor(BallAngle - dir);
      //       toMove = BallAngle - dir;
      //    }
      //    else if (BallAngle > 135 && BallAngle < 180) {
      //       toMove = 225;
      //    }
      //    else if (BallAngle >= 180 && BallAngle <= 225) {
      //       toMove = 135;
      //    }
      //    else {
      //       // Motor(0);
      //       toMove = 0;
      //    }


   // }
   // else {
   //    toMove = BallAngle;
   //    // Motor(BallAngle);
   // }
   // motor.run(toMove);
   // char deb[64];
   // sprintf(deb,"ボール：%d  行く方向：%d",BallAngle,toMove);
   
   // Serial.println(s);
}