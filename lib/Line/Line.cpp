#include "Line.h"
#include "Arduino.h"

int LinePins[4][2] = {{2,3},{0,0},{0,0},{0,0}};

Line::Line() {
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 2; j++) {
         pinMode(LinePins[i][j],input);
      }
   }
}

Line::isOn(int dir) {
   
}