#include "Point.h"

Point::Point() {
    this->x = 0;
    this->y = 0;
}

void Point::setX(double x) {
    this->x = x;
}

void Point::setY(double y) {
    this->y = y;
}

double Point::getX() {
    return this->x;
}

double Point::getY() {
    return this->y;
}