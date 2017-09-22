#ifndef CGI_P1_SPACEVECTOR_H
#define CGI_P1_SPACEVECTOR_H

#include "Point.h"

class SpaceVector {
    Point *p2;
    Point *p1;

  public:
    SpaceVector(Point *p1, Point *p2);
    double length();
    double angle();
    SpaceVector* unit();
    void invert();
};

#endif //CGI_P1_SPACEVECTOR_H
