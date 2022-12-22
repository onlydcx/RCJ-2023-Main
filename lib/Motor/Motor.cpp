#include "Motor.h"
#include "Arduino.h"
#include "IMU.h"

int MotorPins[4][2] = {{4,5},{2,3},{6,7},{8,9}};
int speed = 100;

Motor::Motor() {
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 2; j++) {
         pinMode(MotorPins[i][j],OUTPUT);
         analogWriteFrequency(MotorPins[i][j],2000000);
      }
   }
}

void Motor::roll(int num, int speed) {
   if(speed < 0) {
      analogWrite(MotorPins[num-1][0],abs(speed));
      analogWrite(MotorPins[num-1][1],0);
   }
   else if (speed > 0) {
      analogWrite(MotorPins[num-1][0],0);
      analogWrite(MotorPins[num-1][1],abs(speed)); // 2022.12.21 修正 abs付け加え
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
   float MPwrVector[4] = {0}, MPwrMag[4] = {0};
   angle = 450 - angle;
   for(int i = 0; i < 4; i++) {
      float __Angle = (MotorAngle[i] + angle) * (PI / 180);
      MPwrVector[i] = cos(__Angle);
      if((i == 1) || (i == 3)) MPwrVector[i] *= -1;
      MPwrMag[i] = abs(MPwrVector[i]);
      // Motor(i+1, MPwrVector[i] * speed + addP);
   }
   float MPwrMax = 0;
   for(int i = 0; i < 4; i++) {
      if(MPwrMag[i] > MPwrMax) MPwrMax = MPwrMag[i];
   }
   if(MPwrMax < 1) {
      for(int i = 0; i < 4; i++) {
         MPwrVector[i] *= (1 / MPwrMax);
      }
   }
   int diff = 3;
   int gy = GyroGet(), addPower = 0;
   if((gy >= diff) && (gy < 180)) addPower = -(speed / 20);
   else if ((gy < (360-diff)) && (gy >= 180)) addPower = (speed / 20);
   for(int i = 0; i < 4; i++) {
      Serial.print(MPwrVector[i]);
      Serial.print("  ");
      Motor::roll(i+1, speed * MPwrVector[i] + addPower);
   }
   Serial.println("");
}