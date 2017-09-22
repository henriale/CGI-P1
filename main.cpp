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

#include "lib/Point.h"
#include "lib/Wanderer.h"
#include "lib/Window.h"
#include "lib/Reader.h"
#include "lib/RGB.h"
#include "lib/SpaceVector.h"

using namespace std;

class EventObserver {
  public:

  private:
    EventObserver() {}
};

Reader* reader = nullptr;
Wanderer** wanderers = nullptr;
RGB** palette = nullptr;

void drawCallback(void);
void keyboardCallback(unsigned char key, int x, int y);
void keyboardSpecialCallback(int key, int x, int y);
void setupOrthographicMatrix();
RGB** colorPalette();

#define FPS 5
#define SMOOTH_INDEX 10

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
    palette = colorPalette();

    // TODO:
    // wanderers = reader->readWanderersJourney();
    wanderers = (Wanderer **) calloc(reader->maxDuration * reader->bodyCount, sizeof(Wanderer**));

    // TODO:
    // remove this block of code
    for (int i=0; reader->hasNext(); i++) {
        wanderers[i] = reader->nextWandererJourney();
    }

    for (int i=0; i<reader->bodyCount; i++) {
        for (int j=1; j<reader->duration; j=(j+SMOOTH_INDEX<reader->duration)?j+SMOOTH_INDEX:reader->duration) {

        }
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

    GLfloat thickness = 2.0;

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

RGB **colorPalette() {
    auto palette = (RGB **) calloc(reader->bodyCount, sizeof(RGB));
    for (int i=0; i<reader->bodyCount; i++) {
        palette[i] = new RGB();
    }

    return palette;
}
