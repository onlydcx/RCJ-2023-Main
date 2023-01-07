#include "Line.h"
#include "Arduino.h"
// 01 23
int LinePins[4][2] = {{13,12},{10,11},{0,1},{2,3}};
// int thlesholds[4][2] = {{300,300},{300,300},{300,300},{300,300}};
int thlesholds[4][2] = {1023};
bool isOnLine[4][2] = {false};
bool isOnAny = false, inCount = false, outCount = false;
// right front left back

// 00 奥 01 手前
// 10 手前 11 奥
// 20 !!!　後ろ奥 21 後ろ手前
// 30 左奥 31 左手前

Line::Line() {
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 2; j++) {
         pinMode(LinePins[i][j],INPUT);
      }
   }
}

int startCnt = 0;
bool isFinished = false;

void Line::thUpdate() {
   int MinVal[4][2] = {1023};
   int MaxVal[4][2] = {0};
   if(!isFinished) Serial.println("\nキーを入力...");
   while(Serial.available() == 0 && startCnt == 0) {
      if(Serial.available() > 0) {
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
               Serial.println("測定終了\n");
               break;
            }
         }
         Serial.print("int thlesholds[4][2] = {");
         for(int i = 0; i < 4; i++) {
            Serial.print("{");
            for(int j = 0; j < 2; j++) {
               thlesholds[i][j] = MinVal[i][j] + ((MaxVal[i][j] - MinVal[i][j]) / 2);
               Serial.print(thlesholds[i][j]);
               if(j < 1) Serial.print(",");
            }
            Serial.print((i < 3)? ("},"): ("}"));
         }
         Serial.println("};\n");
         Serial.println("終了");
         isFinished = !isFinished;
         break;
      }
   }
}

bool Line::isOn(int dir,int inout) {
   // return (analogRead(LinePins[dir][inout]) > 300)? true: false;
   return (analogRead(LinePins[dir][inout]) > (thlesholds[dir][inout]))? true: false;
}

void Line::check() {
   int cnt = 0;
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 2; j++) {
         if(thlesholds[i][j] < analogRead(LinePins[i][j])) {
            isOnLine[i][j] = true;
            cnt++;
         }
         else {
            isOnLine[i][j] = false;
         }
      }
   }
   if(cnt > 0) isOnAny = true;
}