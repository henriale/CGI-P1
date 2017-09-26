#include <cmath>
#include "SpaceVector.h"

SpaceVector::SpaceVector(Point *p1, Point *p2) {
    this->p1 = new Point(*p1);
    this->p2 = new Point(*p2);
}

double SpaceVector::length() {

    return sqrt(pow(p2->getX() - p1->getX(), 2) + pow(p2->getY() - p1->getY(), 2));
}

void SpaceVector::invert() {
    Point* aux = this->p1;
    this->p1 = this->p2;
    this->p2 = aux;
}

// TODO
double SpaceVector::angle() {
    double ang = atan2(this->p2->getX() - this->p1->getX(), this->p1->getY() - this->p2->getY());

    return ang * 180 / 3.1412;
}

// TODO
SpaceVector* SpaceVector::unit() {
    SpaceVector* vector = new SpaceVector(*this);
    return 0;
}
