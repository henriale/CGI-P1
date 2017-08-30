#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
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

Coordinate *readPlayerCoordinates(const string &lineBuffer, int duration);
void drawCallback(void);
void keyboardCallback(unsigned char key, int x, int y);
void initWindowSize(void);

/**
 * Application startup
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_3_2_CORE_PROFILE);
  glutInitWindowSize(500,500);
  glutCreateWindow("Desenho de um triângulo em 2D");

  // Registra a função callback de redesenho da janela de visualização
  glutDisplayFunc(drawCallback);
  // Registra a função callback para tratamento das teclas ASCII
  glutKeyboardFunc(keyboardCallback);
  // Chama a função responsável por fazer as inicializações
  initWindowSize();
  // Inicia o processamento e aguarda interações do usuário
  glutMainLoop();

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

  int i=0;
  while (fileStream && getline(fileStream, lineBuffer)) {
    matrix[i++] = readPlayerCoordinates(lineBuffer, biggestDuration);
  }

  for (int i = 0; i < playersCount; i++) {
    for (int j = 0; j < biggestDuration; j++) {
        auto point = matrix[i][j];
        cout << '(';
        cout << point.x;
        cout << ',';
        cout << point.y;
        cout << ')';
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
    if (duration == index) {
      break;
    }
    if (i%3 == 0) {
      playersCoordinate[index].x = number;
    } else if (i%3 == 1) {
      playersCoordinate[index].y = number;
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
  glBegin(GL_TRIANGLES);
  glVertex3f(-0.5,-0.5,0);
  glVertex3f( 0.0, 0.5,0);
  glVertex3f( 0.5,-0.5,0);
  glEnd();

  //Executa os comandos OpenGL
  glFlush();
}

/**
 * @param void
 */
void initWindowSize(void) {
  // Define a janela de visualização 2D
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(-1.0,1.0,-1.0,1.0);
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
