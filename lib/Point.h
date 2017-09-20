#ifndef CGI_P1_POINT_H
#define CGI_P1_POINT_H

class Point {
  public:
    Point();
    void setX(double x);
    void setY(double y);
    double getX();
    double getY();

  private:
    double x;
    double y;
};

#endif //CGI_P1_POINT_H
