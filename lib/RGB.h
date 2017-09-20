#ifndef CGI_P1_RGB_H
#define CGI_P1_RGB_H

#include <cstdlib>

class RGB {
  public:
    RGB(double r = (float) (rand() % 85 + 15) / 100, double g = (float) (rand() % 85 + 15) / 100, double b = (float) (rand() % 85 + 15) / 100);

    double r;
    double g;
    double b;
};

#endif //CGI_P1_RGB_H
