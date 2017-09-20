#ifndef CGI_P1_WANDERER_H
#define CGI_P1_WANDERER_H

#include <cstdlib>
#include <iostream>
#include "Point.h"

class Wanderer {
  public:
    explicit Wanderer(int duration);
    Point* atFrame(int frame);
    void setFrame(int frame, Point *point);
    void printJourney();

  private:
    Point** journey;
};

#endif //CGI_P1_WANDERER_H
