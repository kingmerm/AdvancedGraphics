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
#define MOVEMENT_FACTOR 0.5
#define INITIAL_VELOCITY 7.0f
#define MAXIMUM_DISTANCE 20.0f
#define PARTICLE_SIZE 10.0f
#define PARTICLE_LIFE 1000.0f
#define GRAVITY_CONST -105.81f

// Global viewpoint/camera variables 
GLdouble latitude, longitude;
GLfloat eyeX, eyeY, eyeZ;
GLfloat centerX, centerY, centerZ;
GLfloat upX, upY, upZ;
GLfloat currentTime;

// Display list for coordinate axis 
GLuint axisList;

int AXIS_SIZE= 200;
int axisEnabled= 1;

typedef struct
{
  GLfloat prevXPos, prevYPos, prevZPos;
  GLfloat currentXPos, currentYPos, currentZPos;
  GLfloat nextXPos, nextYPos, nextZPos;
  GLfloat acceleration;
  GLfloat velocity;
  //Particle* next;
} Particle;

///////////////////////////////////////////////

double myRandom()
//Return random double within range [0,1]
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////
void calculateNewXPosition(Particle* p, float t){
    p->nextXPos = (p->currentXPos + p->velocity*(currentTime-t));
}
void calculateNewYPosition(Particle* p, float t){
    p->nextYPos = p->currentYPos - p->prevYPos + (1/2*(GRAVITY_CONST)*pow(currentTime-t,2));
}
void calculateNewZPosition(Particle* p, float t){
    p->nextZPos = (p->currentZPos + p->velocity*(currentTime-t));;
}
float calculateNewVelocity(float distance){
    return (distance/currentTime);
}
///////////////////////////////////////////////

void makeParticle(float t){
  
  glColor3f(0.0f, 25.0f, 50.0f);
  glPointSize(PARTICLE_SIZE);
  particleXPos = particleXPos + calculateNewXPosition(t);
  particleYPos = particleYPos + calculateNewYPosition(t);
  particleZPos = particleZPos + calculateNewZPosition(t);
  glBegin(GL_POINTS);
    glVertex3f(particleXPos, particleYPos, particleZPos);
  glEnd();
  
}

float deltaX = 0.0;
float deltaY = 0.0;
float deltaZ = 0.0;
float distance = 0.0;
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

  calculateNewXPosition(particle,0.1);
  calculateNewYPosition(particle,0.1);
  calculateNewZPosition(particle,0.1);
  if (currentTime < PARTICLE_LIFE){/* && delta < MAXIMUM_DISTANCE){*/
  glPointSize(PARTICLE_SIZE);
  glBegin(GL_POINTS);
  glVertex3f(particle->currentXPos,particle->currentYPos, particle->currentZPos);
  glEnd();
  currentTime+=0.1f;
  }
  glutSwapBuffers();
  glutPostRedisplay();
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
    eyeX -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 100:
    eyeX += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 119:
    eyeZ -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 115:
    eyeZ += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 120:
    eyeY += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 122:
    eyeY -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 32:
    currentTime = 0;
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
  eyeX = 30.0;
  eyeY = 50.0;
  eyeZ = -100.0;
  upX = 0.0;
  upY = -1.0;
  upZ = 0.0;
  latitude = 0.0;
  longitude = 0.0;
  /*************************/
  /*INITIAL PARTICLE VARIABLES*/
   Particle* particle = (Particle*)malloc(sizeof(Particle));
   particle->prevXPos = 0.0f;
   particle->prevYPos = 0.0f;
   particle->prevZPos = 0.0f;
   particle->currentXPos = 0.0f;
   particle->currentYPos = 0.0f;
   particle->currentZPos = 0.0f;
   particle->nextXPos = 0.0f;
   particle->nextYPos = 0.0f;
   particle->nextZPos = 0.0f;
   particle->velocity = 1.5f;
   particle->acceleration = -9.81f;
   //particle->next = NULL;
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

