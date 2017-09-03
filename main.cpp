#include <iostream>
#include <fstream>
#include <sstream>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

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

class Window {
  public:
    Window() {
        this->minX = numeric_limits<double>::max();
        this->minY = numeric_limits<double>::max();
        this->maxX = numeric_limits<double>::min();
        this->maxY = numeric_limits<double>::min();
    }

    double minX;
    double minY;
    double maxX;
    double maxY;
};

Coordinate *readPlayerCoordinates(const string &lineBuffer, int duration, Window* window);
void drawCallback(void);
void keyboardCallback(unsigned char key, int x, int y);
void initWindowSize(double left, double right, double bottom, double top);
void printMatrix(int biggestDuration, int playersCount, Coordinate *const *matrix);

/**
 * Application startup
 *
 * @param argc
 * @param argv
 * @return
 */
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
    auto * window = new Window;

    int i=0;
    while (fileStream && getline(fileStream, lineBuffer)) {
        matrix[i++] = readPlayerCoordinates(lineBuffer, biggestDuration, window);
    }
    //printMatrix(biggestDuration, playersCount, matrix);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    //glutInitWindowSize(static_cast<int>(maxX-minX), static_cast<int>(maxY-minY));
    glutInitWindowSize(500, 500);
    glutCreateWindow("Análise de movimentação");
    // Registra a função callback de redesenho da janela de visualização
    glutDisplayFunc(drawCallback);
    // Registra a função callback para tratamento das teclas ASCII
    glutKeyboardFunc(keyboardCallback);
    // Chama a função responsável por fazer as inicializações
    //initWindowSize(minX, maxX, minY, maxY);
    initWindowSize(0, 1500, 0, 1500);
    // Inicia o processamento e aguarda interações do usuário
    glutMainLoop();

    return 0;
}

/**
 * @todo: pass declared object instead of its size
 * @param lineBuffer
 */
Coordinate *readPlayerCoordinates(const string &lineBuffer, int duration, Window* window) {
    auto * playersCoordinate = new Coordinate[duration];
    Coordinate *point = nullptr;
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
        if (duration == index) {
            break;
        }
        if (i%3 == 0) {
            point = new Coordinate;
            point->x = number;
        } else if (i%3 == 1) {
            point->y = number;
        } else {
            playersCoordinate[number] = *point;

            if (point->x > window->maxX) {
                window->maxX = point->x;
            }
            if (point->x < window->minX) {
                window->minX = point->x;
            }
            if (point->y > window->maxY) {
                window->maxY = point->y;
            }
            if (point->y < window->minY) {
                window->minY = point->y;
            }
        }

        i++;
    }

    return playersCoordinate;
}

/**
 * @param void
 */
void drawCallback(void) {
    // Limpa a janela de visualização com a cor branca
    glClearColor(1,1,1,0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Define a cor de desenho: azul
    glColor3f(0,0,1);

    // Desenha um triângulo no centro da janela
    glBegin(GL_LINES);
    glVertex2f(500, 500);
    glVertex2f(750, 500);
    glEnd();

    //Executa os comandos OpenGL
    glFlush();
}

/**
 *
 * @param left
 * @param right
 * @param bottom
 * @param top
 */
void initWindowSize(double left, double right, double bottom, double top) {
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(left, right, bottom, top);
    glMatrixMode(GL_MODELVIEW);
}

/**
 *
 * @param key
 * @param x
 * @param y
 */
void keyboardCallback(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    }
}

/**
 *
 * @param biggestDuration
 * @param playersCount
 * @param matrix
 */
void printMatrix(int biggestDuration, int playersCount, Coordinate *const *matrix) {
    for (int i = 0; i < playersCount; i++) {
        for (int j = 0; j < biggestDuration; j++) {
            auto point = matrix[i][j];
            cout << '(';
            cout << point.x;
            cout << ',';
            cout << point.y;
            cout << ',';
            cout << j;
            cout << ')';
        }

        cout << endl;
    }
}