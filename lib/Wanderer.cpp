#include "Wanderer.h"

Wanderer::Wanderer(int duration) {
    this->journey = (Point**) calloc(duration, sizeof(Point**));
}

Point *Wanderer::atFrame(int frame) {
    return this->journey[frame-1];
}

void Wanderer::setFrame(int frame, Point *point) {
    this->journey[frame-1] = point;
}

void Wanderer::printJourney() {
    int duration = sizeof(this->journey) / sizeof(this->journey[0]);
    for (int i = 0; i < duration; i++) {
        std::cout << '(';
        std::cout << this->journey[i]->getX();
        std::cout << ',';
        std::cout << this->journey[i]->getY();
        std::cout << ',';
        std::cout << i;
        std::cout << ')';
    }

    std::cout << std::endl;
}
