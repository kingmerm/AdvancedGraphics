////////////////////////////////////////////////////////////////
// School of Computer Science
// The University of Manchester
//
// This code is licensed under the terms of the Creative Commons 
// Attribution 2.0 Generic (CC BY 3.0) License.
//
// Skeleton code for COMP37111 coursework, 2013-14
//
// Authors: Arturs Bekasovs and Toby Howard
//
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#ifdef MACOSX
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#define DEG_TO_RAD_CONV 0.017453293
#define MOVEMENT_FACTOR 0.12
#define INITIAL_VELOCITY 0
#define PARTICLE_SIZE 10.0f
#define GRAVITY_CONST 9.81


// Global viewpoint/camera variables 
GLdouble latitude, longitude;
GLfloat eyeX, eyeY, eyeZ;
GLfloat centerX, centerY, centerZ;
GLfloat upX, upY, upZ;
GLfloat particleX0, particleY0, particleZ0;
GLfloat particleXPos, particleYPos, particleZPos;
GLfloat currentTime;


// Display list for coordinate axis 
GLuint axisList;

int AXIS_SIZE= 200;
int axisEnabled= 1;

///////////////////////////////////////////////

double myRandom()
//Return random double within range [0,1]
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////
float calculateNewPosition(float t){
    return (INITIAL_VELOCITY*t + 1/2*(GRAVITY_CONST)*t*t);
}
///////////////////////////////////////////////

void makeParticle(float t){
  glColor3f(0.0f, 25.0f, 50.0f);
  glPointSize(PARTICLE_SIZE);
  particleXPos = particleXPos + calculateNewPosition(t);
  particleYPos = particleYPos + calculateNewPosition(t);
  particleZPos = particleZPos + calculateNewPosition(t);
  /*glBegin(GL_POINTS);
    glVertex3f(particleXPos, particleYPos, particleZPos);
  glEnd();
  */
}

void display()
{
  glLoadIdentity();
  calculateLookpoint();
  gluLookAt(eyeX, eyeY, eyeZ,
            centerX, centerY, centerZ,
            upX, upY, upZ);
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT);
  // If enabled, draw coordinate axis
  if(axisEnabled) glCallList(axisList);
  /**
  THIS QUAD WAS USED FOR TESTING KEY-MAPPINGSw
  glBegin(GL_QUADS);
    glVertex3f(0.0,6.0,4.0);
    glVertex3f(0.0,8.0,0.0);
    glVertex3f(8.0, 6.0, 0.0);
    glVertex3f(8.0, 3.0, 0.0);
    glVertex3f(6.0,0.0,5.0);
    glVertex3f(2.0,0.0,5.0);
  glEnd();**/
  if (currentTime < 20.0){
  glPointSize(PARTICLE_SIZE);
  glBegin(GL_POINTS);
  glVertex3f(particleXPos+currentTime, particleYPos+currentTime, particleZPos+currentTime);
  glEnd();
  glutSwapBuffers();
  glutPostRedisplay();
  currentTime+=0.1f;
  }
}

///////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y)
{
 switch (key)
 {
    case 27:
    exit(0);
    break;
    case 97:
    eyeX += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 100:
    eyeX -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 119:
    eyeZ += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 115:
    eyeZ -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 120:
    eyeY -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 122:
    eyeY += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
 }
  glutPostRedisplay();
}

///////////////////////////////////////////////
void calculateLookpoint() {
  double deltaX = cos(DEG_TO_RAD_CONV*latitude)*sin(DEG_TO_RAD_CONV*longitude);
  double deltaY = sin(DEG_TO_RAD_CONV*latitude);
  double deltaZ = cos(DEG_TO_RAD_CONV*latitude)*cos(DEG_TO_RAD_CONV*longitude);

  centerX = eyeX + deltaX;
  centerY = eyeY + deltaY;
  centerZ = eyeZ + deltaZ;
}
///////////////////////////////////////////////

void reshape(int width, int height)
{
  glClearColor(0.9, 0.9, 0.9, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
}

///////////////////////////////////////////////

void makeAxes() {
// Create a display list for drawing coord axis
  axisList = glGenLists(1);
  glNewList(axisList, GL_COMPILE);
      glLineWidth(2.0);
      glBegin(GL_LINES);
      glColor3f(1.0, 0.0, 0.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, AXIS_SIZE);
    glEnd();
  glEndList();
}
///////////////////////////////////////////////
void initGraphics(int argc, char *argv[])
{
  /*INITIAL NAVIGATIONAL VARIABLES*/
  eyeX = 3.0;
  eyeY = 5.0;
  eyeZ = -10.0;
  upX = 0.0;
  upY = 1.0;
  upZ = 0.0;
  latitude = 0.0;
  longitude = 0.0;
  /*************************/
  /*INITIAL PARTICLE VARIABLES*/
   particleX0 = 0.0f;
   particleY0 = 0.0f;
   particleZ0 = 0.0f;
   currentTime = 0.0f; // initial t = 0
  /****************************/
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("COMP37111 Particles");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  makeAxes();
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  double f;
  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glutMainLoop();
}

