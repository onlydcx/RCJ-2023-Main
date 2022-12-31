#include "Line.h"
#include "Arduino.h"

int LinePins[4][2] = {{0,0},{0,0},{3,2},{1,0}};
int thlesholds[4][2] = {{300,300},{300,300},{300,300},{300,300}};
// right front left back

Line::Line() {
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 2; j++) {
         pinMode(LinePins[i][j],INPUT);
      }
   }
}

int startCnt = 0;

void Line::thUpdate() {
   int MinVal[4][2] = {1023};
   int MaxVal[4][2] = {0};
   Serial.println("キーを入力...");
   while(Serial.available() == 0) {
      if((Serial.available() > 0) && (startCnt == 0)) {
         Serial.println("測定開始...");
         startCnt++;
         while(true) {
            for(int i = 0; i < 4; i++) {
               for(int j = 0; j < 2; j++) {
                  int __tmpVal = analogRead(LinePins[i][j]);
                  if(MinVal[i][j] > __tmpVal) MinVal[i][j] = __tmpVal;
                  if(MaxVal[i][j] < __tmpVal) MaxVal[i][j] = __tmpVal;
               }
            }
            if(Serial.available() > 1) {
               Serial.println("測定終了");
               break;
            }
         }
         Serial.print("int thlesholds[4][2] = ");
         for(int i = 0; i < 4; i++) {
            Serial.print("{");
            for(int j = 0; j < 2; j++) {
               thlesholds[i][j] = MinVal[i][j] + ((MaxVal[i][j] - MinVal[i][j]) / 2);
               Serial.print(thlesholds[i][j]);
               Serial.print(",");
            }
            Serial.print("},");
         }
         Serial.println(";");
         break;
      }
   }
   while(true) {}
}

bool Line::isOn(int dir,int inout) {
   // return (analogRead(LinePins[dir][inout]) > 300)? true: false;
   return (analogRead(LinePins[dir][inout]) > (thlesholds[dir][inout]))? true: false;
}