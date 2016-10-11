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
#define MOVEMENT_FACTOR 1.0f
#define INITIAL_VELOCITY 7.0f
#define MAXIMUM_DISTANCE 20.0f
#define PARTICLE_SIZE 10.0f
#define PARTICLE_LIFE 1000.0f
#define GRAVITY_CONST 9.81f

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
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Coord;

typedef struct
{
  //GLfloat prevXPos, prevYPos, prevZPos;
  Coord* prevPos;
  Coord* currentPos;
  Coord* nextPos;
  Coord* velocity;
  Coord* acceleration;
  
  //Particle* next;
} Particle;

Particle* particle;

///////////////////////////////////////////////

double myRandom()
//Return random double within range [0,1]
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////
void calculateNewXPosition(Particle* p, float dt){
    p->nextPos->x = (p->currentPos->x + p->velocity->x*(dt));
	p->prevPos->x = p->currentPos->x;
	p->currentPos->x = p->nextPos->x;
	p->nextPos->x = 0.0f;
}
void calculateNewYPosition(Particle* p, float dt){
    p->nextPos->y = p->velocity->y + (1/2*(p->acceleration->y)*pow(dt,2));
	p->prevPos->y = p->currentPos->y;
	p->currentPos->y = p->nextPos->y;
	p->nextPos->y = 0.0f;
}
void calculateNewZPosition(Particle* p, float dt){
	p->nextPos->z = (p->currentPos->z + p->velocity->z*(dt));
	p->prevPos->z = p->currentPos->z;
	p->currentPos->z = p->nextPos->z;
	p->nextPos->z = 0.0f;
}
void calculateNewXVelocity(Particle* p, float dt) {
	p->velocity->x += p->acceleration->x * (dt);
}
void calculateNewYVelocity(Particle* p, float dt) {
	p->velocity->y += p->acceleration->y * (dt);
}
void calculateNewZVelocity(Particle* p, float dt) {
	p->velocity->y += p->acceleration->y * (dt);
}
void resetParticle(Particle* p) {
	particle->prevPos->x = .0f;
	particle->prevPos->y = .0f;
	particle->prevPos->z = .0f;
	particle->currentPos->x = .0f;
	particle->currentPos->y = .0f;
	particle->currentPos->z = .0f;
	particle->nextPos->x = .0f;
	particle->nextPos->y = .0f;
	particle->nextPos->z = .0f;
	particle->velocity->y = .0f;
	currentTime = .0f;

}
float calculateNewVelocity(float distance){
    return (distance/currentTime);
}

///////////////////////////////////////////////
void calculateLookpoint() {
	double deltaX = cos(DEG_TO_RAD_CONV*latitude)*sin(DEG_TO_RAD_CONV*longitude);
	double deltaY = sin(DEG_TO_RAD_CONV*-latitude);
	double deltaZ = cos(DEG_TO_RAD_CONV*latitude)*cos(DEG_TO_RAD_CONV*longitude);

	centerX = eyeX + deltaX;
	centerY = eyeY + deltaY;
	centerZ = eyeZ + deltaZ;
}
///////////////////////////////////////////////
/*
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
*/
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
  if (currentTime < PARTICLE_LIFE){/* && delta < MAXIMUM_DISTANCE){*/
  glPointSize(PARTICLE_SIZE);
  calculateNewXVelocity(particle, currentTime - 0.1);
  calculateNewYVelocity(particle, currentTime - 0.1);
  calculateNewZVelocity(particle, currentTime - 0.1);
  calculateNewXPosition(particle, currentTime - 0.1);
  calculateNewYPosition(particle, currentTime - 0.1);
  calculateNewZPosition(particle, currentTime - 0.1);
  
  printf("[X,Y] = [%f,%f]\n", particle->currentPos->x, particle->currentPos->y);
  glBegin(GL_POINTS);
  glVertex2f(particle->currentPos->x, particle->currentPos->y);/*, particle->currentZPos);*/
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
    eyeY -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 115:
    eyeY += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 120:
    eyeZ -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 122:
    eyeZ += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 32:
    resetParticle(particle);
    break;
 }
  glutPostRedisplay();
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
  eyeY = 0.0;
  eyeZ = 10.0;
  upX = 0.0;
  upY = -1.0;
  upZ = 0.0;
  latitude = 0.0;
  longitude = 0.0;
  /*************************/
  /*INITIAL PARTICLE VARIABLES*/
   particle = (Particle*)malloc(sizeof(Particle));
   particle->prevPos = (Coord*)malloc(sizeof(Coord));
   particle->currentPos = (Coord*)malloc(sizeof(Coord));
   particle->nextPos = (Coord*)malloc(sizeof(Coord));
   particle->velocity = (Coord*)malloc(sizeof(Coord));
   particle->acceleration = (Coord*)malloc(sizeof(Coord));
   particle->velocity->x = 1.5f;
   particle->velocity->y = .0f;
   particle->velocity->z = 1.5f;
   particle->acceleration->x = .0f;
   particle->acceleration->y = GRAVITY_CONST;
   particle->acceleration->z = .0f;
   currentTime = .0f;
   //particle->next = NULL;
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

