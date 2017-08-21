#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Coordinate {
 public:
  Coordinate() {
    this->x = 0;
    this->y = 0;
  }

  double x;
  double y;
};

Coordinate *readPlayerCoordinates(const string &lineBuffer, int duration);

int main(int argc, char *argv[]) {
  ifstream fileStream;
  string lineBuffer;
  string ratio;
  int biggestDuration = 0, curr = 0, playersCount = 0;

  fileStream.open("../Paths_D.txt");

  // reading ratio
  getline(fileStream, lineBuffer);
  ratio = stod(lineBuffer.substr(1, lineBuffer.length() - 2));

  // count number of actors and longest duration
  while (fileStream >> curr) {
    if (curr > biggestDuration) {
      biggestDuration = curr;
    }

    playersCount++;
    fileStream.ignore(numeric_limits<streamsize>::max(), '\n');
  }

  // reset file reading
  fileStream.close();
  fileStream.open("../Paths_D.txt");
  // ignore first line
  fileStream.ignore(numeric_limits<streamsize>::max(), '\n');

  auto ** matrix = new Coordinate*[playersCount];

  while (fileStream && getline(fileStream, lineBuffer)) {
    matrix[playersCount] = readPlayerCoordinates(lineBuffer, biggestDuration);
  }

  for (int i = 0; i < playersCount; i++) {
    for (int j = 0; j < biggestDuration; j++) {
      cout << "(" << matrix[i][j].x << "," << matrix[i][j].y << ") ";
    }

    cout << endl;
  }

  return 0;
}

/**
 *
 * @param lineBuffer
 */
Coordinate *readPlayerCoordinates(const string &lineBuffer, int duration) {
  auto * playersCoordinate = new Coordinate[duration];
  string storage, limit;
  int number = 0, i = 0;

  istringstream ss(lineBuffer);
  ss >> limit;

  while (! ss.eof()) {
    auto c = (char) ss.peek();
    if (c == '(' || c == ')' || c == ',' || c == '\t') {
      ss.ignore();
      continue;
    }

    ss >> number;
    int index = i/3;
    if (i%3 == 0) {
      playersCoordinate[index].x = number;
    } else if (i%3 == 1) {
      playersCoordinate[index].y = number;
    }
    i++;
  }

  return playersCoordinate;
}