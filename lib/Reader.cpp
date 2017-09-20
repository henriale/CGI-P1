#include "Reader.h"

Reader::Reader(string path) {
    this->path = std::move(path);
    fileStream.open(this->path);

    if (fileStream.fail()) {
        throw "File doesn't exists";
    }

    this->next();
}

void Reader::dimensions() {
    for (int number=0; fileStream >> number; bodyCount++) {
        if (number > maxDuration) {
            maxDuration = number;
        }

        fileStream.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    this->restart();
}

string Reader::next() {
    string lineBuffer;
    getline(fileStream, lineBuffer);

    return lineBuffer;
}

void Reader::restart() {
    fileStream.clear();
    fileStream.close();
    fileStream.open(this->path);
    this->next();
}

bool Reader::hasNext() {
    return (bool) fileStream;
}

Wanderer *Reader::nextWandererJourney() {
    auto wandererJourney = new Wanderer(this->maxDuration + 1);
    Point* point = nullptr;

    string storage, limit;
    int aux = 0, i = 0;

    istringstream ss(this->next());
    ss >> limit;

    while (! ss.eof()) {
        auto c = (char) ss.peek();
        if (c == '(' || c == ')' || c == ',' || c == '\t') {
            ss.ignore();
            continue;
        }

        ss >> aux;
        int index = i/3;
        if (this->maxDuration == index) {
            break;
        }
        if (i%3 == 0) {
            point = new Point;
            point->setX(aux);
        } else if (i%3 == 1) {
            point->setY(aux);
        } else {
            wandererJourney->setFrame(aux, point);

            if (point->getX() > Window::maxX) {
                Window::maxX = point->getX();
            }
            if (point->getX() < Window::minX) {
                Window::minX = point->getX();
            }
            if (point->getY() > Window::maxY) {
                Window::maxY = point->getY();
            }
            if (point->getY() < Window::minY) {
                Window::minY = point->getY();
            }
        }

        i++;
    }

    return wandererJourney;
}
