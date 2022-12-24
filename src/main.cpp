// #include "Arduino.h"
// #include <Wire.h>
// #include "Motor.h"
// #include "IMU.h"
// // #include "Ball.h"

// Motor motor;

// char BallDebug[64];
// int nearAngle, BallZeroQTY, speed, BallAngle, BallStr, BallAngle_UC = 0;
// bool isNoBall = false;

// int LinePins[4][2] = {{2,3},{0,0},{0,0},{0,0}};
// int BallPins[16] = {23,32,11,38,20,31,12,39,21,30,36,28,22,33,37,29};
// int MotorPins[4][2] = {{4,5},{2,3},{6,7},{8,9}};
// int tmpBallStr[16] = {0};

// void IRUpdate() {
//    int __BallZeroQTY = 0;
//    double VectorX, VectorY = 0;
//    for(int i = 0; i < 16; i++) {
//       int strength = pulseIn(BallPins[i],LOW,833);
//       float sensorDeg = 22.5 * (PI / 180) * i;
//       VectorX += cos(sensorDeg) * strength;
//       VectorY += sin(sensorDeg) * strength;
//       tmpBallStr[i] = strength;
//       if(strength == 0) __BallZeroQTY++;
//    }
//    BallZeroQTY = __BallZeroQTY;
//    VectorX *= -1, VectorY *= -1;
//    float tmpBallAngle = (atan2(VectorX,VectorY) * (180 / PI)) + 180;
//    if(tmpBallAngle < 0) tmpBallAngle += 360;
//    BallAngle = (int)tmpBallAngle;
//    int maxBallStr = 0, maxSensorID = 0;
//    for(int i = 0; i < 16; i++) {
//       if(tmpBallStr[i] > maxBallStr) {
//          maxBallStr = tmpBallStr[i];
//          maxSensorID = i;
//       }
//    }
//    BallStr = tmpBallStr[maxSensorID];
//    isNoBall = ((BallAngle == 0) && (maxBallStr == 0))? true: false;
//     sprintf(BallDebug,"角度:%d 距離:%d",BallAngle,BallStr);
//  }

// void setup() {
//    Serial.begin(9600);
//    Wire.begin();
//    Gyro_init();
//       for(int i = 0; i < 16; i++) {
//        pinMode(BallPins[i],INPUT);
//     }
//    while(1){
//     Serial.println(BallAngle);

//    }
// }

// void loop() {
//    // speed = 100;

//    IRUpdate();
//    motor.turnFront();
//    bool isBallFront = (nearAngle == 4)? true: false;

//    if(!isNoBall) {
//       if(BallStr > 450) {
//          // 回り込み
//          if(isBallFront) motor.run(0);
//          else {
//             float add = 0.2;
//             motor.run(BallAngle + BallAngle*add);
//          }
//       }
//       else {
//          motor.run(BallAngle);
//       }
//    }
// }


#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Wire.h>
MPU6050 mpu;
 static uint8_t mpuIntStatus;
 static bool dmpReady = false;
 static uint16_t packetSize;

 int16_t Gyro_Now, Gyro, Gyro_Offset = 0;
 uint16_t fifoCount;
 uint8_t fifoBuffer[64];
 Quaternion q;

 VectorFloat gravity;
float ypr[3];
 int Gyro_X = -112, Gyro_Y = 63, Gyro_Z = -13, Accel_Z = 1579;

 char BallDebug[64];
 int nearAngle, BallZeroQTY, speed, BallAngle, BallStr, BallAngle_UC = 0;
bool isNoBall = false;

 int LinePins[4][2] = {{2,3},{0,0},{0,0},{0,0}};
int BallPins[16] = {23,32,11,38,20,31,12,39,21,30,36,28,22,33,37,29};
int MotorPins[4][2] = {{4,5},{2,3},{6,7},{8,9}};
int tmpBallStr[16] = {0};
void IRUpdate() {
   int fixconst = 0;
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
    BallAngle = (int)tmpBallAngle + fixconst;

    // 2022.12.21 追記分 (距離の正確性)
    int maxBallStr = 0, maxSensorID = 0;
    int __BallStrID[4] = {0};
    bool isSort = false;
    for(int i = 0; i < 16; i++) {
       if(tmpBallStr[i] > maxBallStr) {
          maxBallStr = tmpBallStr[i];
          maxSensorID = i;
       }
    }
       BallStr = tmpBallStr[maxSensorID];
   ((BallAngle == 0) && (maxBallStr == 0))? isNoBall = true: isNoBall = false;
    sprintf(BallDebug,"角度:%d 距離:%d",BallAngle,BallStr);
}

    void Motor(int num, int speed) {
   if(speed < 0) {
      analogWrite(MotorPins[num-1][0],abs(speed));
      analogWrite(MotorPins[num-1][1],0);
   }
   else if (speed > 0) {
      analogWrite(MotorPins[num-1][0],0);
      analogWrite(MotorPins[num-1][1],speed);
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
int GyroGet(void) {
   mpuIntStatus = false;
   mpuIntStatus = mpu.getIntStatus();
   fifoCount = mpu.getFIFOCount();
   if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
      mpu.resetFIFO();
   }
   else if (mpuIntStatus & 0x02) {
      while (fifoCount < packetSize) {
         fifoCount = mpu.getFIFOCount();
      }
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      fifoCount -= packetSize;
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      Gyro_Now = degrees(ypr[0]) + 180;
      Gyro = Gyro_Now + Gyro_Offset - 180;
      if (Gyro < 0) Gyro += 360;
      if (Gyro > 359) Gyro -= 360;
   }
   return Gyro;
}
void Gryo_init() {
   mpu.initialize();
   if (mpu.testConnection() != true) {
      Serial.println("MPU disconection");
   }
   if (mpu.dmpInitialize() != 0) {
      Serial.println("MPU break");
   }
   mpu.setXGyroOffset(Gyro_X);
   mpu.setYGyroOffset(Gyro_Y);
   mpu.setZGyroOffset(Gyro_Z);
   mpu.setZAccelOffset(Accel_Z);
   mpu.setDMPEnabled(true);
   mpuIntStatus = mpu.getIntStatus();
   dmpReady = true;
   packetSize = mpu.dmpGetFIFOPacketSize();
}
void roll(int dir) {
   for(int i = 0; i < 4; i++) {
      Motor(i+1,-dir);
   }
}

void turnFront() {
   int GY = GyroGet();
   speed = 150;
   int diff = 90;
   while((GY > diff) || ((360 - diff) < GY)) {
      // if(GY < 180) {
      //    Motor(1,speed);
      //    Motor(2,speed);
      //    Motor(3,speed);
      //    Motor(4,speed);
      // }
      // else {
      //    Motor(1,-speed);
      //    Motor(2,-speed);
      //    Motor(3,-speed);
      //    Motor(4,-speed);
      // }
      if(GY < 180) speed *= -1;
      for(int i = 0; i < 4; i++) {
         Motor(i+1,speed);
      }
      if((GyroGet() < diff) || (GyroGet() > (360 - diff))) break;
   }
}
void Motor(int angle) {
   int MotorAngle[4] = {40, 140, 225, 315};
   float MPwrVector[4] = {0}, MPwrMag[4] = {0};
   angle = 450 - angle;
   // 懸念点：：angle>360 の処理は？？ 2022.12.23
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
      Motor(i+1, speed * MPwrVector[i] + addPower);
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
   Gryo_init();
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
   //  while(1) {
   //    IRUpdate();
   //    Serial.println(BallStr > 400);
   //  }
 }

void loop() {
   speed = 220;
   IRUpdate();
   bool isBallFront = (nearAngle == 4)? true: false;
   if(BallStr > 400) {
      if(isBallFront) {
         Motor(0);
      }
      else {
         int dir = BallAngle;
         if(dir > 180); dir = 360 - dir;
         if(dir > 50) dir = 50;
         if((BallAngle > 10) && (BallAngle <= 180)) {
            Motor(BallAngle + dir);
         }
         else if ((BallAngle > 180) && (BallAngle < 350)) {
            Motor(BallAngle - dir);
         }
         else {
            Motor(0);
         }
      }
   }
   else {
      Motor(BallAngle);
   }
}