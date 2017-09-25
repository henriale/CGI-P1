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

#include <cmath>
#include <vector>
#include <algorithm>

#include "lib/Point.h"
#include "lib/Wanderer.h"
#include "lib/Window.h"
#include "lib/Reader.h"
#include "lib/RGB.h"
#include "lib/SpaceVector.h"

#define LINE_THICKNESS 1.5

using namespace std;

template<typename T>
inline static bool operator ==(const vector<T>& v, const T& elem)
{
    return (find(v.begin(), v.end(), elem) != v.end());
}

template<typename T>
inline static bool operator !=(const vector<T>& v, const T& elem)
{
    return !(find(v.begin(), v.end(), elem) != v.end());
}

Reader* reader = nullptr;
Wanderer** wanderers = nullptr;
RGB** palette = nullptr;

void drawCallback(void);
void keyboardCallback(unsigned char key, int x, int y);
void keyboardSpecialCallback(int key, int x, int y);
void setupOrthographicMatrix();
void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius);
void closestWanderers(int frame, int wandererIndex, vector<int>* group);
void drawConnectiveLine(Point *p1, Point *p2);
void analyseGroup();
RGB** colorPalette(int size);

#define FPS 5
#define SMOOTH_INDEX 10
#define PI 3.1415926535897932384626433832795

#define GROUP_MIN_MEMBERS 3
#define GROUP_MAX_DISTANCE 100
#define GROUP_MIN_FRAMES 40

/**
 * Application startup
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[]) {

    SpaceVector* v1 = new SpaceVector(new Point(2,2), new Point(5,5));
    v1->unit();
    string filename = "../dataset/" + string(argv[1]) + ".txt";
    try {
        reader = new Reader(filename);
    } catch (char const* error) {
        cout << error;
        exit(EXIT_FAILURE);
    }

    // TODO:
    // reader->readWindowDimensions();
    reader->dimensions();


    // TODO:
    // CollorPalette::generate();
    palette = colorPalette(reader->bodyCount);

    // TODO:
    // wanderers = reader->readWanderersJourney();
    wanderers = (Wanderer **) calloc(reader->maxDuration * reader->bodyCount, sizeof(Wanderer**));

    // TODO:
    // remove this block of code
    for (int i=0; reader->hasNext(); i++) {
        wanderers[i] = reader->nextWandererJourney();
    }




    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Analise de movimento");
    glutDisplayFunc(drawCallback);
    glutKeyboardFunc(keyboardCallback);
    glutSpecialFunc(keyboardSpecialCallback);
    setupOrthographicMatrix();
    glutMainLoop();

    free(wanderers);
    free(reader);

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

    GLfloat thickness = LINE_THICKNESS;

    for (int i=0; i<reader->bodyCount; i++) {
        Point* lastPoint = nullptr;
        glColor3f(palette[i]->r, palette[i]->g, palette[i]->b);

        glLineWidth(thickness);
        glBegin(GL_LINE_STRIP);
        for (int j=1; j<reader->duration; j=(j+SMOOTH_INDEX<reader->duration)?j+SMOOTH_INDEX:reader->duration) {
            if (wanderers[i]->atFrame(j) != nullptr &&
                (lastPoint == nullptr
                || wanderers[i]->atFrame(j)->getX() != lastPoint->getX()
                || wanderers[i]->atFrame(j)->getY() != lastPoint->getY())
            ) {
                glVertex2f((GLfloat) wanderers[i]->atFrame(j)->getX(), (GLfloat) wanderers[i]->atFrame(j)->getY());
                //cout << "plotting[" << i << "][" << j << "] " << wanderers[i]->atFrame(j)->getX() << ", " << wanderers[i]->atFrame(j)->getY() << '\n';
            }
            lastPoint = wanderers[i]->atFrame(j);
        }

        glEnd();

    }

    analyseGroup();

    glFlush();
}

/**
 * @return void
 */
void analyseGroup() {
    int startFrame = reader->duration;
    int endFrame = 1;

    for (int frame=1; frame < reader->duration; frame= (frame+SMOOTH_INDEX<reader->duration) ? frame+SMOOTH_INDEX : reader->duration) {
        cout << "[FRAME " << frame << "]" << endl;

        vector<int>* group = new vector<int>();
        for (int i=0; i<reader->bodyCount; i++) {
            if (! wanderers[i]->atFrame(frame)->isOnScreen()) {
                continue;
            }

            closestWanderers(frame, i, group);

            if (group->size() < GROUP_MIN_MEMBERS) {
                group->clear();
            }
        }

        if (group->size() >= GROUP_MIN_MEMBERS) {
            if (startFrame > endFrame) {
                startFrame = frame;
            }
            endFrame = frame;
        }

        if (frame != endFrame) {
            startFrame = reader->duration;
            endFrame = frame;
        }

        if ((endFrame-startFrame) >= GROUP_MIN_FRAMES) {
            cout << "group found!" << endl;
            cout << "[ ";
            for(auto& k : *group) {
                std::cout << k << ' ';
            }
            cout << "]";

            drawHollowCircle(
                (GLfloat) wanderers[group->at(2)]->atFrame((endFrame+startFrame)/2)->getX(),
                (GLfloat) wanderers[group->at(2)]->atFrame((endFrame+startFrame)/2)->getY(),
                GROUP_MIN_FRAMES * 2
            );
        }

        cout << endl;
    }
}

/**
 * Find the other people near the given wanderer
 *
 * @param frame
 * @param wandererIndex
 * @param group
 */
void closestWanderers(int frame, int wandererIndex, vector<int>* group) {
    for (int anotherWandererIndex=wandererIndex+1; anotherWandererIndex < reader->bodyCount; anotherWandererIndex++) {
        auto p1 = wanderers[wandererIndex]->atFrame(frame);
        auto p2 = wanderers[anotherWandererIndex]->atFrame(frame);

        if (! p2->isOnScreen()) {
            continue;
        }

        auto distance = p1->distanceToPoint(p2);

        //cout << "distance: [" << wandererIndex << "] -> [" << anotherWandererIndex << "] = " << distance << endl;
        if (distance < GROUP_MAX_DISTANCE && *group != (anotherWandererIndex)) {
            if (*group != wandererIndex) {
                group->push_back(wandererIndex);
            }

            //cout << "desenhando vetor entre os pontos!" << endl;
            group->push_back(anotherWandererIndex);
            closestWanderers(frame, anotherWandererIndex, group);
            //drawConnectiveLine(p1, p2);
        }
    }
}

/**
 *
 * @param p1
 * @param p2
 */
void drawConnectiveLine(Point *p1, Point *p2) {
    glColor3f(0.9, 0.9, 0.9);
    glLineWidth(1);
    glBegin(GL_LINE_STRIP);
    glVertex2f((GLfloat) p1->getX(), (GLfloat) p1->getY());
    glVertex2f((GLfloat) p2->getX(), (GLfloat) p2->getY());
    glEnd();
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
 * @param key
 * @param x
 * @param y
 */
void keyboardSpecialCallback(int key, int x, int y) {
    int newDuration;

    switch (key) {
        case 100:
            newDuration = reader->duration - FPS;
            reader->duration = newDuration <= FPS ? FPS : newDuration;
            glutPostRedisplay();
            return;

        case 102:
            newDuration = reader->duration + FPS;
            reader->duration = newDuration >= reader->maxDuration ? reader->duration : newDuration;
            glutPostRedisplay();
            return;

        default:break;
    }
}

/**
 * @source gist.github.com
 * @param x
 * @param y
 * @param radius
 */
void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius) {
    int i;
    int lineAmount = 25;

    GLfloat twicePi = 2.0f * PI;

    glLineWidth(5);
    glColor3f(0.8f, 0.4f, 0.4f);

    glBegin(GL_LINE_LOOP);
    for(i = 0; i <= lineAmount;i++) {
        glVertex2f(
            x + (radius * cos(i *  twicePi / lineAmount)),
            y + (radius * sin(i * twicePi / lineAmount))
        );
    }
    glEnd();
}

/**
 * generate a random color palette
 *
 * @return void
 */
RGB **colorPalette(int size) {
    auto palette = (RGB **) calloc(size, sizeof(RGB));
    for (int i=0; i<reader->bodyCount; i++) {
        palette[i] = new RGB();
    }

    return palette;
}
