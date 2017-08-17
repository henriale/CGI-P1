#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

int main(int argc, char* argv[])
{
    std::ifstream fileStream("../Paths_D.txt");
    std::string lineBuffer;
    std::string ratio;
    int x, y;

    std::getline(fileStream, lineBuffer);
    ratio = lineBuffer;

    std::stack<char> brackets;

    std::string number;
    int converted;

    while (fileStream && std::getline(fileStream, lineBuffer)) {
        for (char &c : lineBuffer) {
            if (c == '(') {
                brackets.push(c);
                continue;
            }

            if (brackets.empty()) {
                continue;
            }

            if (c == ',') {
                converted = std::stoi(number);
                std::cout << converted << '\n';

                if (1 /* first number */) {

                } else if (2 /* second number */) {

                } else  /* third number */ {

                }

                number = "";
                continue;
            }
            number += c;

            if (c == ')') {
                brackets.pop();
                continue;
            }

            if (c == '\t') {
                // maybe it's not necessary
                continue;
            }
        }
    }

    return 0;
}