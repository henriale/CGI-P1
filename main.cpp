#include <iostream>
#include <fstream>
#include <sstream>
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
#include <limits>
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

class Wanderer {
  public:
    Wanderer(int duration) {
        this->journey = (Point **) malloc(sizeof(Point) * duration);
    }
    Point* atFrame(int frame) {
        return this->journey[frame-1];
    }

    void printJourney() {
        cout << "print not implemented yet";
    }

    void setFrame(int frame, Point *point) {
        this->journey[frame-1] = point;

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
    int bodyCount = 0;
    int maxDuration = 0;
    ifstream fileStream;
    string path;

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

    bool hasNext() {
        return (bool) fileStream;
    }

    Wanderer* nextWandererJourney() {
        Wanderer* wandererJourney = new Wanderer(this->maxDuration + 1);
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
};

Reader* reader = nullptr;
Wanderer** wanderers = nullptr;

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

    string filename = "../dataset/" + string(argv[1]) + ".txt";
    try {
        reader = new Reader(filename);
    } catch (char const* error) {
        cout << error;
        exit(EXIT_FAILURE);
    }
    reader->dimensions();

    wanderers = new Wanderer*[reader->bodyCount];

    for (int i=0; reader->hasNext(); i++) {
        wanderers[i] = reader->nextWandererJourney();
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

void renderText(const char *text, double x, double y) {
    glColor3f(1, 0, 0);
    glRasterPos2f((GLfloat) x, (GLfloat) y);

    while (*text != '\0') {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int) *text);
        text++;
    }
}

/**
 * @param void
 */
void drawCallback(void) {
    renderText("This text should be printed", Window::minX + 20, Window::minY + 20);

    glClearColor(1,1,1,0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat thickness = 1.0;

    for (int i=0; i<reader->bodyCount; i++) {
        Point* lastPoint = nullptr;

        glColor3f(((float)(rand()%100))/100, ((float)(rand()%100))/100, ((float)(rand()%100))/100);

        glLineWidth(thickness);
        glBegin(GL_LINE_STRIP);
        for (int j=1; j<reader->maxDuration; j++) {
            if (wanderers[i]->atFrame(j) != nullptr &&
                (lastPoint == nullptr
                || wanderers[i]->atFrame(j)->getX() != lastPoint->getX()
                || wanderers[i]->atFrame(j)->getY() != lastPoint->getY())
            ) {
                glVertex2f((GLfloat) wanderers[i]->atFrame(j)->getX(), (GLfloat) wanderers[i]->atFrame(j)->getY());
                cout << "plotting[" << i << "][" << j << "] " << wanderers[i]->atFrame(j)->getX() << ", " << wanderers[i]->atFrame(j)->getY() << '\n';
            }
            lastPoint = wanderers[i]->atFrame(j);
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