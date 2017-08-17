#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[])
{
    std::ifstream fileStream("../Paths_D.txt");
    std::string lineBuffer;
    std::string ratio;

    std::getline(fileStream, lineBuffer);
    ratio = lineBuffer;

    while (fileStream && std::getline(fileStream, lineBuffer)) {
        for (char &c : lineBuffer) {
            std::cout << c;
        }

        std::cout << '\n';
    }

    return 0;
}