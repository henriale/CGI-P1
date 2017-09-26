#ifndef CGI_P1_POINT_H
#define CGI_P1_POINT_H

class Point {
  public:
    Point();
    Point(int x, int y);

    void setX(double x);
    void setY(double y);
    double getX();
    double getY();
    double distanceToPoint(Point* p2);
    bool isOnScreen();

    inline bool operator ==(Point* that);
    inline bool operator !=(Point* that);

  private:
    double x;
    double y;
};

#endif //CGI_P1_POINT_H
