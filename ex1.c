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
#define TIME_STEP .001f
#define MOVEMENT_FACTOR 2.5f;
#define INITIAL_VELOCITY 7.0f
#define MAXIMUM_DISTANCE 20.0f
#define PARTICLE_SIZE 5.0f
#define PARTICLE_NUMBER 600
#define PARTICLE_LIFE .10f
#define EMITTER_NUMBER 3
#define GRAVITY_CONST -1.81f

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
	GLdouble R;
	GLdouble G;
	GLdouble B;
	GLdouble A;
}Color;

typedef struct
{
  //GLfloat prevXPos, prevYPos, prevZPos;
  Coord* prevPos;
  Coord* currentPos;
  Coord* nextPos;
  Coord* velocity;
  Coord* acceleration;
  GLfloat currentTime;
  Color* color;
  struct Particle* next;
}Particle;

typedef struct
{
	Particle* particleSet[PARTICLE_NUMBER];
	int particlesPerSec;
	int currentParticles;
	Coord* position;
} Emitter;

Emitter* emitter;

typedef struct
{
	Emitter* emitterSet[EMITTER_NUMBER];
}ParticleSystem;
///////////////////////////////////////////////

double myRandom()
//Return random double within range [0,1]
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////
void calculateNewXPosition(Particle* p, GLfloat dt){
	if (p->currentPos->y < -100.0f) {
		p->velocity->x -= 0.0001f;
    if (p->velocity->x < 0.0f)
    {
        p->velocity->x *= 0.1;
    }
	}
    p->nextPos->x = p->velocity->x*(dt);
	p->prevPos->x = p->currentPos->x;
	p->currentPos->x += p->nextPos->x;
	p->nextPos->x = 0.0f;
}
void calculateNewYPosition(Particle* p, GLfloat dt){
	if (p->currentPos->y < -100.0f) {
		p->velocity->y *= -.10f;
		p->acceleration->y += 0.1;

		//p->velocity->y += 0.1;
		//glColor3f(p->color->R, p->color->G, p->color->B);
	}
    p->nextPos->y = p->velocity->y + (float)(1/2*(p->acceleration->y)*pow(dt,2));
	p->prevPos->y = p->currentPos->y;
	p->currentPos->y += p->nextPos->y;
	p->nextPos->y = 0.0f;
}
void calculateNewZPosition(Particle* p, GLfloat dt){
	p->nextPos->z = p->velocity->z*(dt);
	p->prevPos->z = p->currentPos->z;
	p->currentPos->z += p->nextPos->z;
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
void initParticle(Particle* particle) {
	particle->prevPos = (Coord*)malloc(sizeof(Coord));
	particle->currentPos = (Coord*)malloc(sizeof(Coord));
	particle->nextPos = (Coord*)malloc(sizeof(Coord));
	particle->velocity = (Coord*)malloc(sizeof(Coord));
	particle->acceleration = (Coord*)malloc(sizeof(Coord));
	particle->color = (Color*)malloc(sizeof(Color));
	particle->velocity->x = 12.75f*myRandom();
	particle->velocity->y = 5.0f;
	particle->velocity->z = 1.75f;
	particle->acceleration->x = .0f;
	particle->acceleration->y = GRAVITY_CONST;
	particle->acceleration->z = .0f;
	particle->currentTime = .0f;
	particle->next = NULL;
	particle->color->R = (GLdouble)myRandom()*1.0f;
	particle->color->G = (GLdouble)myRandom()*1.0f;
	particle->color->B = (GLdouble)myRandom()*1.0f;
	particle->color->A = 1.0f;

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
void calculateNextPositions(Particle* particle){
	//Particle* particle = emitter->particleSet[0]; //loop through current particles
	//while (particle != NULL) {
	calculateNewXVelocity(particle, particle->currentTime - TIME_STEP);
	calculateNewYVelocity(particle, particle->currentTime - TIME_STEP);
	calculateNewZVelocity(particle, particle->currentTime - TIME_STEP);
	calculateNewXPosition(particle, particle->currentTime - TIME_STEP);
	calculateNewYPosition(particle, particle->currentTime - TIME_STEP);
	calculateNewZPosition(particle, particle->currentTime - TIME_STEP);
	//	particle = particle->next;
	//}
}
void drawParticles() {
	Particle* particle = emitter->particleSet[0];
	Particle* tmp = (Particle*)malloc(sizeof(Particle));
	memcpy(tmp,particle,sizeof(Particle));
	/*
	if (particle->next == NULL) {
		glPointSize(PARTICLE_SIZE);
		glBegin(GL_POINTS);
		glVertex3f(particle->currentPos->x, particle->currentPos->y, particle->currentPos->z);
		glEnd();
	}
	*/
	glPointSize(PARTICLE_SIZE);
	int i = 0;
	while (tmp != NULL) {
		//glColor3f(particle->color->R, particle->color->G, particle->color->B);
		calculateNextPositions(tmp);
		glColor4f(1.0f, 0.0f, 1.0f, tmp->color->A - TIME_STEP);
		glBegin(GL_POINTS);
		glVertex2f(tmp->currentPos->x, tmp->currentPos->y);/* , tmp->currentPos->z);*/
		glEnd();
		tmp->currentTime += TIME_STEP;
		emitter->particleSet[i] = tmp;
		tmp = tmp->next;
		i++;
	}
	glutSwapBuffers();
}
int i;
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
  glBegin(GL_LINES);
  glVertex3f(0.0, 200, 0.0);
  glVertex3f(2000, 200, 0.0);
  glEnd();
  if(axisEnabled) glCallList(axisList);
  /*THIS LINE IS TO CHECK COLLISIONS WITH Y = 200*/
  	glBegin(GL_LINE);
	glVertex2f(0, 200);
	glVertex2f(1000, 200);
	glEnd();
	//glPushMatrix();
	if (emitter->currentParticles + emitter->particlesPerSec < PARTICLE_NUMBER && emitter->particleSet[emitter->currentParticles - 1]->currentTime >= PARTICLE_LIFE) {
		//Particle* particle = (Particle*)malloc(sizeof(Particle));
		//initParticle(particle);
		emitter->particleSet[emitter->currentParticles-1]->next = (Particle*)malloc(sizeof(Particle));
		//memcpy(emitter->particleSet[emitter->currentParticles]->next,particle, sizeof(Particle*));
		initParticle(emitter->particleSet[emitter->currentParticles-1]->next);
		emitter->currentParticles += 1;
		printf("New Particle Drawn, Now there are %d Particles here! \n", emitter->currentParticles);
	}
	drawParticles();
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
	/*
    case 97:
    glTranslatef(-2.5f,0,0);
    calculateLookpoint();
    break;
    case 100:
    //eyeX += MOVEMENT_FACTOR;
    glTranslatef(2.5f, 0, 0);
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
	*/
    case 32:
    initParticle(emitter->particleSet[0]);
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
  eyeY = -20.0;
  eyeZ = -700.0;
  upX = 0.0;
  upY = 1.0;
  upZ = 0.0;
  latitude = 0.0;
  longitude = 0.0;
  /******************************/
  /*INITIAL EMITTER VARIABLES*/
  emitter = (Emitter*)malloc(sizeof(Emitter));
  emitter->position = (Coord*)malloc(sizeof(Coord));
  emitter->particlesPerSec = 1;
  emitter->position->x = 50.0f;
  emitter->position->y = 50.0f;
  emitter->position->z = 50.0f;
  /**************************/
  /*INITIAL PARTICLE VARIABLES*/
  /*
  for (i = 0; i < 100; i++) {
	  Particle* particle = (Particle*)malloc(sizeof(Particle));
	  initParticle(particle);
	  emitter->particleSet[i] = particle;
  }
  */
  Particle* particle = (Particle*)malloc(sizeof(Particle));
  initParticle(particle);
  emitter->particleSet[0] = particle;
  emitter->currentParticles = 1;
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

