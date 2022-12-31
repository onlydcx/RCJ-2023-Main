#include "Motor.h"
#include "Arduino.h"
#include "IMU.h"

int MotorPins[4][2] = {{4,5},{2,3},{6,7},{8,9}};
int speed = 0;

Motor::Motor() {
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 2; j++) {
         pinMode(MotorPins[i][j],OUTPUT);
         analogWriteFrequency(MotorPins[i][j],1000000);
         /// 100khzでは？？？

      }
   }
}

void Motor::roll(int num, int power) {
   if(power < 0) {
      analogWrite(MotorPins[num-1][0],abs(power));
      analogWrite(MotorPins[num-1][1],0);
   }
   else if (speed > 0) {
      analogWrite(MotorPins[num-1][0],0);
      analogWrite(MotorPins[num-1][1],abs(power)); // 2022.12.21 修正 abs付け加え
   }
   else {
      analogWrite(MotorPins[num-1][0],255);
      analogWrite(MotorPins[num-1][1],255);
   }
}

void Motor::free() {
   for(int i = 0; i < 4; i++) {
      analogWrite(MotorPins[i-1][0],0);
      analogWrite(MotorPins[i-1][1],0);
   }
}

void Motor::stop() {
   for(int i = 0; i < 4; i++) {
      Motor::roll(i+1,0);
   }
}

void Motor::turnFront() {
   int GY = GyroGet();
   int speed = 80;
   int diff = 15;
   while((GY > diff) || ((360 - diff) > GY)) {
      if(180 > GY) speed = -abs(speed);
      else speed = abs(speed);
      for(int i = 0; i < 4; i++) {
         Motor::roll(i+1,speed);
      }
      GY = GyroGet();
      if((GY < diff) || ((360 - diff) < GY)) {
         Motor::stop();
         delay(100);
         break;
      }
   }
}

void Motor::turn(int dir, bool absolute) {
   int diff = 5, power = 80;
   if(absolute) {
      if(dir <= 180) {
         while((GyroGet() < (dir - diff))) {
            for(int i = 0; i < 4; i++) {
               Motor::roll(i+1,power);
            }
            if(GyroGet() > (dir - diff)) break;
         }
      }
      else if (dir > 180) {
         while(GyroGet() >= (dir + diff)) {
            for(int i = 0; i < 4; i++) {
               Motor::roll(i+1,-power);
            }
            if(GyroGet() <= (dir + diff)) break;
         }
      }
      for(int i = 0; i < 4; i++) {
         Motor::roll(i+1,0);
      }
   }
}

void Motor::run(int angle) {
   int MotorAngle[4] = {40, 140, 225, 315};
   float MPwrVector[4] = {0};
   float MPwrMax = 0;
   angle = 450 - angle;
   if(angle > 360) angle -= 360;
   for(int i = 0; i < 4; i++) {
      MPwrVector[i] = -sin((angle - MotorAngle[i]) * (PI / 180));
      if(MPwrMax < MPwrVector[i]) MPwrMax = MPwrVector[i];
   }
   if(MPwrMax < 1) {
      for(int i = 0; i < 4; i++) {
         MPwrVector[i] *= (1 / MPwrMax);
      }
   }
   int diff = 5;
   int gy = GyroGet(), addPower = 0;
   // 30 good
   if((gy >= diff) && (gy < 180)) addPower = -(speed / 10);
   else if ((gy < (360-diff)) && (gy >= 180)) addPower = (speed / 10);
   for(int i = 0; i < 4; i++) {
      int finalPower = speed * MPwrVector[i] + addPower;
      // Motor::roll(i+1, finalPower);
      Serial.print(finalPower);
      Serial.print(" ");
      // プラスとかマイナスとかじゃなくて掛け算にしてみては？？ 2022.12/29
   }
   Serial.println("");
}