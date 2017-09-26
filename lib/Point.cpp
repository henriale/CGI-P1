#include "Point.h"
#include "SpaceVector.h"

Point::Point() {
    this->x = 0;
    this->y = 0;
}

Point::Point(int x, int y) {
    this->x = x;
    this->y = y;
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

double Point::distanceToPoint(Point* p2) {
    SpaceVector* vector = new SpaceVector(this, p2);

    return vector->length();
}

bool Point::isOnScreen() {
    return !((this == nullptr) || this->x < 1 || this->y < 1);
}

bool Point::operator==(Point *that) {
    return this->getX() == that->getX() && this->getY() == that->getY();
}

bool Point::operator!=(Point *that) {
    return ! (this->getX() == that->getX() && this->getY() == that->getY());
}
