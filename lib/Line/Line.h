#ifndef __LINE_H_
#define __LINE_H_

extern int thresholds[4][2];
extern int isOnAny;

class Line {
   public:
      Line();
      bool isOnLine(int dir);
};

#endif