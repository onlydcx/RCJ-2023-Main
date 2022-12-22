#include <Arduino.h>
#include <Wire.h>
#include "IMU.h"
#include "Motor.h"

Motor Motor();

char BallDebug[64];
int nearAngle, BallZeroQTY, speed, BallAngle, BallStr, BallAngle_UC = 0;
bool isNoBall = false;

int LinePins[4][2] = {{2,3},{0,0},{0,0},{0,0}};
int BallPins[16] = {23,32,11,38,20,31,12,39,21,30,36,28,22,33,37,29};
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

void send32U4(int data) {
   data /= 4;
   Wire.beginTransmission(8);
   Wire.write(data);
   Wire.endTransmission();
}

void setup() {
   Serial.begin(9600);
   Wire.begin();
   Gyro_init();
   for(int i = 0; i < 16; i++) {
      pinMode(BallPins[i],INPUT);
   }
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
   // speed = 100;
   // IRUpdate();
   // turnFront();
   // bool isBallFront = (nearAngle == 4)? true: false;

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
