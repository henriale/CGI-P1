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

class Wanderer {
  public:
    Point* atFrame(int frame) {
        return this->journey[frame];
    }

    void printJourney() {
        cout << "print not implemented yet";
    }

  private:
    Point** journey;
};

class EventObserver {
  public:

  private:
    EventObserver() {}
};

class Window {
  public:
    static double minX;
    static double minY;
    static double maxX;
    static double maxY;
};

double Window::minX = numeric_limits<double>::max();
double Window::minY = numeric_limits<double>::max();
double Window::maxX = numeric_limits<double>::min();
double Window::maxY = numeric_limits<double>::min();

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

    string next() {
        string lineBuffer;
        getline(fileStream, lineBuffer);

        return lineBuffer;
    }

    void dimensions() {
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

Point* readBodyMoves(const string &lineBuffer, int duration);
void drawCallback(void);
void keyboardCallback(unsigned char key, int x, int y);
void setupOrthographicMatrix();
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
    reader->dimensions();

    //Wanderer* wanderers = new Wanderer[reader->bodyCount];
    scene = new Point*[reader->bodyCount];

    for (int i=0; reader->hasNext(); i++) {
        //wanderers[i] = readBodyMoves(reader->next(), reader->maxDuration);
        scene[i] = readBodyMoves(reader->next(), reader->maxDuration);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Analise de movimento");
    glutDisplayFunc(drawCallback);
    glutKeyboardFunc(keyboardCallback);
    setupOrthographicMatrix();
    glutMainLoop();

    return 0;
}

/**
 * @todo: pass declared object instead of its size
 * @param lineBuffer
 */
Point* readBodyMoves(const string &lineBuffer, int duration) {
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
        } else if (i%3 == 1) {
            point->setY(number);
        } else {
            playersCoordinate[number] = *point;

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

    return playersCoordinate;
}

/**
 * @param void
 */
void drawCallback(void) {
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

    }

    glFlush();
}

/**
 *
 * @param left
 * @param right
 * @param bottom
 * @param top
 */
void setupOrthographicMatrix() {
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(Window::minX, Window::maxX, Window::maxY, Window::minY);
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