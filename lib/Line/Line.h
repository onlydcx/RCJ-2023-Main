#ifndef __LINE_H_
#define __LINE_H_

extern int LinePins[4][2];
extern int thlesholds[4][2];

#define right 0
#define front 1
#define left  2
#define back  3
#define in 0
#define out 1

class Line {
   public:
      Line();
      bool isOn(int dir,int inout);
      void thUpdate();
};

#endif