#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <vector>

void readLine(std::string &lineBuffer);

int main(int argc, char *argv[]) {
  std::ifstream fileStream("../Paths_D.txt");
  std::string lineBuffer;
  std::string ratio;
  std::stack<double> brackets;
  std::getline(fileStream, lineBuffer);
  ratio = lineBuffer;

  while (fileStream && std::getline(fileStream, lineBuffer)) {
    readLine(lineBuffer);
  }

  return 0;
}

/**
 *
 * @param lineBuffer
 */
void readLine(std::string &lineBuffer) {
  std::string storage = "";
  bool isStoring = false;
  int number = 0;

  for (char &c : lineBuffer) {
    if (c == '(') {
      isStoring = true;
      continue;
    }

    if (c == ')') {
      isStoring = false;

      std::istringstream strStream(storage);
      while(strStream >> number) {
        strStream.ignore();
        std::cout << number << '\n';
      }

      storage = "";
      continue;
    }

    if (c == '\t') {
      // maybe it's not necessary
      continue;
    }

    if (isStoring) {
      storage += c;
    }
  }
}