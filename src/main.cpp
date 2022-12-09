#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>

MPU6050 mpu;
static uint8_t mpuIntStatus;
static bool dmpReady = false;
static uint16_t packetSize;
int16_t Gyro_Now = 0, Gyro = 0, Gyro_Offset = 0;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
Quaternion q;
VectorFloat gravity;
float ypr[3];
int Gyro_X = -112, Gyro_Y = 63, Gyro_Z = -13, Accel_Z = 1579;

char BallDebug[64];

int BallAngle = 0, BallStr = 0;
int BallAngle_UC = 0;

int BallPins[16] = {
   23,32,11,38,20,31,12,39,21,30,36,28,22,33,37,29
};

int MotorPins[4][2] = {
   {4,5}, {2,3}, {6,7}, {8,9}
};

int tmpBallStr[16] = {0};

void IRUpdate() {
   int fixconst = -5;
   double VectorX = 0, VectorY = 0;
   for(int i = 0; i < 16; i++) {
      int strength = pulseIn(BallPins[i],LOW,833);
      float sensorDeg = 22.5 * (PI / 180) * i;
      VectorX += cos(sensorDeg) * strength;
      VectorY += sin(sensorDeg) * strength;
      tmpBallStr[i] = strength;
   }
   VectorX *= -1, VectorY *= -1;
   float tmpBallAngle = atan2(VectorX,VectorY) * (180 / PI);
   tmpBallAngle += 180;
   if(tmpBallAngle < 0) tmpBallAngle += 360;
   BallAngle = (int)tmpBallAngle + fixconst;
   BallStr = tmpBallStr[(int)(tmpBallAngle / 22.5)];
   int order = (360 - BallAngle);
   BallAngle_UC = (order > 360)? order -= 360: order;
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

void MotorFree(int num) {
   analogWrite(MotorPins[num-1][0],0);
   analogWrite(MotorPins[num-1][1],0);
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
      if (Gyro < 0) {
         Gyro += 360;
      }
      if (Gyro > 359) {
         Gyro -= 360;
      }
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

void turnFront() {
   int GY = GyroGet();
   int speed = -100;
   int diff = 3;
   if((diff < GY) && (180 > GY)) {
      Motor(1,speed); // 左前
      Motor(2,speed); // 右前
      Motor(3,speed); // 左後
      Motor(4,speed); // 右後
   }
   else if ((180 <= GY) && ((360 - diff) > GY)) {
      Motor(1,-speed); // 左前
      Motor(2,-speed); // 右前
      Motor(3,-speed); // 左後
      Motor(4,-speed); // 右後
   }
   else {
      Motor(1,0); // 左前
      Motor(2,0); // 右前
      Motor(3,0); // 左後
      Motor(4,0); // 右後
   }
}

void Motor(int angle) {
   int MotorAngle[4] = {40, 140, 225, 315};
   float MPwrVector[4] = {0};
   angle += 90;

   for(int i = 0; i < 4; i++) {
      float __Angle = (MotorAngle[i] + angle) * (PI / 180);
      MPwrVector[i] = cos(__Angle);
      if((i == 1) || (i == 3)) MPwrVector[i] *= -1;
      Motor(i+1, MPwrVector[i] * 100);
   }
}

void setup() {
   Serial.begin(9600);
   Wire.begin();
   Gryo_init();
   for(int i = 0; i < 16; i++) {
      pinMode(BallPins[i],INPUT);
   }
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 2; j++) {
         pinMode(MotorPins[i][j],OUTPUT);
         analogWriteFrequency(MotorPins[i][j],200000);
      }
      MotorFree(i);
   }
}

void loop() {
   // IRUpdate();
   // Serial.println(BallDebug);
   // Serial.println(pulseIn(23,LOW));
}