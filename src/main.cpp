#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Wire.h>
#include "IMU.h"

char BallDebug[64];
int nearAngle, BallZeroQTY, speed, BallAngle, BallStr, BallAngle_UC = 0;
bool isNoBall = false;

int LinePins[4][2] = {{2,3},{0,0},{0,0},{0,0}};
int BallPins[16] = {23,32,11,38,20,31,12,39,21,30,36,28,22,33,37,29};
int MotorPins[4][2] = {{4,5},{2,3},{6,7},{8,9}};
int tmpBallStr[16] = {0};

void IRUpdate() {
   int __BallZeroQTY = 0;
   double VectorX, VectorY = 0;
   for(int i = 0; i < 16; i++) {
      int strength = pulseIn(BallPins[i],LOW,833);
      float sensorDeg = 22.5 * (PI / 180) * i;
      VectorX += cos(sensorDeg) * strength;
      VectorY += sin(sensorDeg) * strength;
      tmpBallStr[i] = strength;
      if(strength == 0) __BallZeroQTY++;
   }
   BallZeroQTY = __BallZeroQTY;
   VectorX *= -1, VectorY *= -1;
   float tmpBallAngle = (atan2(VectorX,VectorY) * (180 / PI)) + 180;
   if(tmpBallAngle < 0) tmpBallAngle += 360;
   BallAngle = (int)tmpBallAngle;
   int maxBallStr = 0, maxSensorID = 0;
   for(int i = 0; i < 16; i++) {
      if(tmpBallStr[i] > maxBallStr) {
         maxBallStr = tmpBallStr[i];
         maxSensorID = i;
      }
   }
   BallStr = tmpBallStr[maxSensorID];
   isNoBall = ((BallAngle == 0) && (maxBallStr == 0))? true: false;
   sprintf(BallDebug,"角度:%d 距離:%d",BallAngle,BallStr);
}

void __motor(int num, int speed) {
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



void MotorFree() {
   for(int i = 0; i < 4; i++) {
      analogWrite(MotorPins[i-1][0],0);
      analogWrite(MotorPins[i-1][1],0);
   }
}

void MotorStop() {
   for(int i = 0; i < 4; i++) {
      __motor(i+1,0);
   }
}

void turnFront() {
   int GY = GyroGet();
   int speed = 80;
   int diff = 15;
   while((GY > diff) || ((360 - diff) > GY)) {
      if(180 > GY) speed = -abs(speed);
      else speed = abs(speed);
      for(int i = 0; i < 4; i++) {
         __motor(i+1,speed);
      }
      GY = GyroGet();
      if((GY < diff) || ((360 - diff) < GY)) {
         MotorStop();
         delay(100);
         break;
      }
   }
}

void turn(int dir, bool absolute) {
   int diff = 5, power = 80;
   if(absolute) {
      if(dir <= 180) {
         while((GyroGet() < (dir - diff))) {
            for(int i = 0; i < 4; i++) {
               __motor(i+1,power);
            }
            if(GyroGet() > (dir - diff)) break;
         }
      }
      else if (dir > 180) {
         while(GyroGet() >= (dir + diff)) {
            for(int i = 0; i < 4; i++) {
               __motor(i+1,-power);
            }
            if(GyroGet() <= (dir + diff)) break;
         }
      }
      for(int i = 0; i < 4; i++) {
         __motor(i+1,0);
      }
   }
}

void Motor(int angle) {
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
      __motor(i+1, speed * MPwrVector[i] + addPower);
   }
   Serial.println("");
}

void send32U4(int data) {
   data /= 4;
   Wire.beginTransmission(8);
   Wire.write(data);
   Wire.endTransmission();
}

void setup() {
   Serial.begin(9600);
   Wire.begin();
   // Wire.setClock(400000);
   Gyro_init();
   for(int i = 0; i < 16; i++) {
      pinMode(BallPins[i],INPUT);
   }
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 2; j++) {
         pinMode(MotorPins[i][j],OUTPUT);
         analogWriteFrequency(MotorPins[i][j],200000);
      }
   }
   MotorFree();
   while(1) {
      Serial.println(GyroGet());
   }
}

void loop() {
  //   Wire.requestFrom(8, 1);
  // while (Wire.available()) {
  //   byte data = Wire.read();
  //   Serial.println(data);
  // }
   speed = 100;
   IRUpdate();
   turnFront();
   bool isBallFront = (nearAngle == 4)? true: false;

   if(!isNoBall) {
      if(BallStr > 450) {
         // 回り込み
         if(isBallFront) Motor(0);
         else {
            float add = 0.2;
            Motor(BallAngle + BallAngle*add);
         }
      }
      else {
         Motor(BallAngle);
      }
   }
}
