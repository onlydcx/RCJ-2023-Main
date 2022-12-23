#include "IMU.h"
#include <MPU6050_6Axis_MotionApps20.h>

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

void Gyro_init() {
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