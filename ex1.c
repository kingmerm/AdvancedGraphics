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

#define DEG_TO_RAD_CONV 0.017453293f
#define TIME_STEP 0.0001f
#define MOVEMENT_FACTOR 2.5f;
#define INITIAL_VELOCITY 7.0f
#define MAXIMUM_DISTANCE 20.0f
#define PARTICLE_SIZE 10.0f
#define PARTICLE_LIFE 10.0f
#define GRAVITY_CONST 9.81f

// Global viewpoint/camera variables 
GLfloat latitude, longitude;
GLfloat eyeX, eyeY, eyeZ;
GLfloat centerX, centerY, centerZ;
GLfloat upX, upY, upZ;

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
  GLfloat currentTime;
  //Particle* next;
} Particle;
Particle* particleSet[10];

///////////////////////////////////////////////

double myRandom()
//Return random double within range [0,1]
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////
void calculateNewXPosition(Particle* p, GLfloat dt){
    p->nextPos->x = (p->currentPos->x + p->velocity->x*(dt));
	p->prevPos->x = p->currentPos->x;
	p->currentPos->x = p->nextPos->x;
	p->nextPos->x = 0.0f;
}
void calculateNewYPosition(Particle* p, GLfloat dt){
    p->nextPos->y = p->velocity->y + (float)(1/2*(p->acceleration->y)*pow(dt,2));
	p->prevPos->y = p->currentPos->y;
	p->currentPos->y = p->nextPos->y;
	p->nextPos->y = 0.0f;
}
void calculateNewZPosition(Particle* p, GLfloat dt){
	p->nextPos->z = (p->currentPos->z + p->velocity->z*(dt));
	p->prevPos->z = p->currentPos->z;
	p->currentPos->z = p->nextPos->z;
	p->nextPos->z = 0.0f;
}
void calculateNewXVelocity(Particle* p, GLfloat dt) {
	p->velocity->x += p->acceleration->x * (dt);
}
void calculateNewYVelocity(Particle* p, GLfloat dt) {
	p->velocity->y += p->acceleration->y * (dt);
}
void calculateNewZVelocity(Particle* p, GLfloat dt) {
	p->velocity->y += p->acceleration->y * (dt);
}
void resetParticle(Particle* p) {
	p->prevPos->x = .0f;
	p->prevPos->y = .0f;
	p->prevPos->z = .0f;
	p->currentPos->x = .0f;
	p->currentPos->y = .0f;
	p->currentPos->z = .0f;
	p->nextPos->x = .0f;
	p->nextPos->y = .0f;
	p->nextPos->z = .0f;
	p->velocity->y = .0f;
	p->currentTime = .0f;

}
///////////////////////////////////////////////
void calculateLookpoint() {
	GLfloat deltaX = cosf(DEG_TO_RAD_CONV*latitude)*sinf(DEG_TO_RAD_CONV*longitude);
	GLfloat deltaY = sinf(DEG_TO_RAD_CONV*-latitude);
	GLfloat deltaZ = cosf(DEG_TO_RAD_CONV*latitude)*cosf(DEG_TO_RAD_CONV*longitude);

	centerX = eyeX + deltaX;
	centerY = eyeY + deltaY;
	centerZ = eyeZ + deltaZ;
}
///////////////////////////////////////////////
/*
void makeParticle(Particle* particle){
  
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
int i = 0;
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

  if (particleSet[i]->currentTime < PARTICLE_LIFE) {
	glPointSize(PARTICLE_SIZE);
	calculateNewXVelocity(particleSet[i], particleSet[i]->currentTime - TIME_STEP);
	calculateNewYVelocity(particleSet[i], particleSet[i]->currentTime - TIME_STEP);
	calculateNewZVelocity(particleSet[i], particleSet[i]->currentTime - TIME_STEP);
	calculateNewXPosition(particleSet[i], particleSet[i]->currentTime - TIME_STEP);
	calculateNewYPosition(particleSet[i], particleSet[i]->currentTime - TIME_STEP);
	calculateNewZPosition(particleSet[i], particleSet[i]->currentTime - TIME_STEP);
	printf("[X,Y] = [%f,%f]\n", particleSet[i]->currentPos->x, particleSet[i]->currentPos->y);
	glBegin(GL_POINTS);
	glVertex3f(particleSet[i]->currentPos->x, particleSet[i]->currentPos->y, particleSet[i]->currentPos->z);
	glEnd();
	particleSet[i]->currentTime += TIME_STEP;
	glutSwapBuffers();
	glutPostRedisplay();
  }//if
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
    resetParticle(particleSet[0]);
    break;
 }
  glutPostRedisplay();
}

///////////////////////////////////////////////

void reshape(int width, int height)
{
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
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
      glVertex3f((float) AXIS_SIZE, 0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, (float)AXIS_SIZE, 0.0);
      glColor3f(0.0, 0.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, (float)AXIS_SIZE);
    glEnd();
  glEndList();
}
///////////////////////////////////////////////
void initGraphics(int argc, char *argv[])
{
  /*INITIAL NAVIGATIONAL VARIABLES*/
  eyeX = 50.0;
  eyeY = 5.0;
  eyeZ = -500.0;
  upX = 0.0;
  upY = -1.0;
  upZ = 0.0;
  latitude = 0.0;
  longitude = 0.0;
  /*************************/
  /*INITIAL PARTICLE VARIABLES*/
  int i;
  for (i = 0; i < 100; i++) {
	  Particle* particle = (Particle*)malloc(sizeof(Particle));
	  particle->prevPos = (Coord*)malloc(sizeof(Coord));
	  particle->currentPos = (Coord*)malloc(sizeof(Coord));
	  particle->nextPos = (Coord*)malloc(sizeof(Coord));
	  particle->velocity = (Coord*)malloc(sizeof(Coord));
	  particle->acceleration = (Coord*)malloc(sizeof(Coord));
	  particle->velocity->x = 7.5f;
	  particle->velocity->y = .0f;
	  particle->velocity->z = 7.5f;
	  particle->acceleration->x = .0f;
	  particle->acceleration->y = GRAVITY_CONST;
	  particle->acceleration->z = .0f;
	  particle->currentTime = .0f;
	  particleSet[i] = particle;
  }
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

