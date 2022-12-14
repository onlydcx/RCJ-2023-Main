#include "Ball.h"
#include "Arduino.h"

int BallPins[16] = {23,32,11,38,20,31,12,39,21,30,36,28,22,33,37,29};
int tmpBallStr[16] = {0};
char BallDebug[64];
int nearAngle, BallZeroQTY, BallAngle,BallStr, BallAngle_UC = 0;
bool isNoBall = false;
bool isBallFront = (nearAngle == 4)? true: false;

void IR_init() {
   for(int i = 0; i < 16; i++) {
      pinMode(BallPins[i],INPUT);
   }
}

void IRUpdate() {
   int __BallZeroQTY = 0;
   double VectorX = 0, VectorY = 0;
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
   // Serial.println(BallDebug);
}