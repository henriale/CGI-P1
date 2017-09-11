#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <utility>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

using namespace std;

class Point {
  public:
    Point() {
        this->x = 0;
        this->y = 0;
        this->touched = false;
    }

    void setX(double x) {
        this->x = x;
        this->touch();
    }

    void setY(double y) {
        this->y = y;
        this->touch();
    }

    void touch() {
        this->touched = true;
    }

    double getX() {
        return this->x;
    }

    double getY() {
        return this->y;
    }

    bool isTouched() {
        return this->touched;
    }

  private:
    double x;
    double y;
    bool touched;
};

typedef Point** Scene;
Scene scene = nullptr;

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

Window* window = new Window;

class Reader {
  public:
    explicit Reader(string path) {
        this->path = std::move(path);
        fileStream.open(this->path);

        if (fileStream.fail()) {
            throw "File doesn't exists";
        }

        this->next();
    }

    bool isHeader() {
        // TODO
        return false;
    }

    string next() {
        string lineBuffer;
        getline(fileStream, lineBuffer);

        return lineBuffer;
    }

    void readDimensions() {
        for (int number=0; fileStream >> number; bodyCount++) {
            if (number > maxDuration) {
                maxDuration = number;
            }

            fileStream.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        this->restart();
    }

    void restart() {
        fileStream.clear();
        fileStream.close();
        fileStream.open(this->path);
        this->next();
    }

    int bodyCount = 0;
    int maxDuration = 0;
    ifstream fileStream;
    string path;

    bool hasNext() {
        return (bool) fileStream;
    }
};

Reader* reader = nullptr;

Point* readBodyMoves(const string &lineBuffer, int duration, Window *window);
void drawCallback(void);
void keyboardCallback(unsigned char key, int x, int y);
void setupOrthographicMatrix(double left, double right, double bottom, double top);
void printMatrix(int biggestDuration, int bodyCount, Point** matrix);


/**
 * Application startup
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]) {
    reader = new Reader("../Paths_D.txt");
    reader->readDimensions();

    scene = new Point*[reader->bodyCount];

    for (int i=0; reader->hasNext(); i++) {
        scene[i] = readBodyMoves(reader->next(), reader->maxDuration, window);
    }

    //printMatrix(reader->maxDuration, reader->bodyCount, scene);

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
    setupOrthographicMatrix(window->minX, window->maxX, window->maxY, window->minY);
    //setupOrthographicMatrix(0, 1500, 0, 1500);
    // Inicia o processamento e aguarda interações do usuário
    glutMainLoop();

    return 0;
}

/**
 * @todo: pass declared object instead of its size
 * @param lineBuffer
 */
Point* readBodyMoves(const string &lineBuffer, int duration, Window *window) {
    auto * playersCoordinate = new Point[duration + 1];
    Point* point = nullptr;
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
            point = new Point;
            point->setX(number);
            //point->x = number;
        } else if (i%3 == 1) {
            point->setY(number);
            //point->y = number;
        } else {
            playersCoordinate[number] = *point;

            if (point->getX() > window->maxX) {
                window->maxX = point->getX();
            }
            if (point->getX() < window->minX) {
                window->minX = point->getX();
            }
            if (point->getY() > window->maxY) {
                window->maxY = point->getY();
            }
            if (point->getY() < window->minY) {
                window->minY = point->getY();
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

    GLfloat thickness = 1.0;

    for (int i=0; i<reader->bodyCount; i++) {
        Point* lastPoint = nullptr;

        glColor3f(((float)(rand()%100))/100, ((float)(rand()%100))/100, ((float)(rand()%100))/100);

        glLineWidth(thickness);
        glBegin(GL_LINE_STRIP);
        for (int j=1; j<reader->maxDuration; j++) {
            if (scene[i][j].isTouched() &&
                (lastPoint == nullptr
                || scene[i][j].getX() != lastPoint->getX()
                || scene[i][j].getY() != lastPoint->getY())
            ) {
                glVertex2f((GLfloat) scene[i][j].getX(), (GLfloat) scene[i][j].getY());
                cout << "plotting " << scene[i][j].getX() << ", " << scene[i][j].getY() << '\n';
            }
            lastPoint = &scene[i][j];
        }

        glEnd();

        cout << " | ending line | \n";

    }

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
void setupOrthographicMatrix(double left, double right, double bottom, double top) {
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
 * @param bodyCount
 * @param matrix
 */
void printMatrix(int biggestDuration, int bodyCount, Point** matrix) {
    for (int i = 0; i < bodyCount; i++) {
        for (int j = 0; j < biggestDuration; j++) {
            auto point = matrix[i][j];
            cout << '(';
            cout << point.getX();
            cout << ',';
            cout << point.getY();
            cout << ',';
            cout << j;
            cout << ')';
        }

        cout << endl;
    }
}