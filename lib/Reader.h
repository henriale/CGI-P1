#ifndef CGI_P1_READER_H
#define CGI_P1_READER_H

#include <string>
#include <fstream>
#include <sstream>

#include "Wanderer.h"
#include "Window.h"

using namespace std;

class Reader {
  public:
    int bodyCount = 0;
    int maxDuration = 0;
    ifstream fileStream;
    string path;

    explicit Reader(string path);
    string next();
    void dimensions();
    void restart();
    bool hasNext();
    Wanderer* nextWandererJourney();
};

#endif //CGI_P1_READER_H
