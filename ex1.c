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
#include <windows.h>

#ifdef MACOSX
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#define DEG_TO_RAD_CONV 0.017453293f
#define PARTICLE_NUMBER 1000000
#define PARTICLE_LIFE .00001f
#define EMITTER_NUMBER 20
#define GRASS_GROWTH_RATE 0.0001
#define MAXIMUM_GRASS_HEIGHT 2.0

// Global viewpoint/camera variables 
GLfloat latitude, longitude;
GLfloat eyeX, eyeY, eyeZ;
GLfloat centerX, centerY, centerZ;
GLfloat upX, upY, upZ;
GLfloat xRotation, yRotation = 0.0;
GLint mouseX, mouseY;
// Display list for coordinate axis 
GLuint axisList;

int AXIS_SIZE= 200;
int axisEnabled= 1;
//Global Time Variable
GLfloat currentTime;

GLfloat MOVEMENT_FACTOR = 2.5f;
GLfloat TIME_STEP = .000005f;
GLfloat GRAVITY_CONST = -2.0f;
GLfloat PARTICLE_SIZE = 1.0f;
static int rootMenuId;
static int subAxisMenuId;
static int subTerrainMenuId;
static int gravitySubMenuId;
static int emitterPositionSubMenuId;
static int fountainSubMenuId;
static int particleSizeSubMenuId;
static int window;
static int menuOpen = 0;
static int val = 0;
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
  GLfloat particleTime;
  Color* color;
  GLfloat grassLength;
  GLfloat grassColor;
  GLfloat chanceOfFlowering;
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

ParticleSystem *pSystem;
///////////////////////////////////////////////

double myRandom()
//Return random double within range [0,1]
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////
void calculateNewXPosition(Particle* p, GLfloat dt){
    p->nextPos->x = p->velocity->x*(dt);
	p->prevPos->x = p->currentPos->x;
	p->currentPos->x += p->nextPos->x;
	p->nextPos->x = 0.0f;
}
void calculateNewYPosition(Particle* p, GLfloat dt){
	if (p->currentPos->y < 0.0) {
		if (p->velocity->y < 0.001) { p->velocity->y = 0.0f; }
		p->velocity->x *= 0.99;
		p->velocity->z *= 0.99;
	}
    p->nextPos->y = p->velocity->y + (float)(1/2*(GRAVITY_CONST)*pow(dt,2));
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
	if (p->currentPos->y > .5) {
		if (myRandom() <= .5)
			p->velocity->x += 15.8;
		else
			p->velocity->x -= 15.8;
	}
}
void calculateNewYVelocity(Particle* p, GLfloat dt) {
	p->velocity->y += p->acceleration->y * (dt);
}
void calculateNewZVelocity(Particle* p, GLfloat dt) {
	p->velocity->z += p->acceleration->z * (dt);
	if (p->currentPos->y > .5) {
		if (myRandom() <= .5)
			p->velocity->z += 15.8;
		else
			p->velocity->z -= 15.8;
	}
}

void initParticle(Particle* particle) {
	particle->prevPos = (Coord*)malloc(sizeof(Coord));
	particle->currentPos = (Coord*)malloc(sizeof(Coord));
	particle->nextPos = (Coord*)malloc(sizeof(Coord));
	particle->velocity = (Coord*)malloc(sizeof(Coord));
	particle->acceleration = (Coord*)malloc(sizeof(Coord));
	particle->color = (Color*)malloc(sizeof(Color));
	particle->currentPos->x =emitter->position->x;
	particle->currentPos->y =emitter->position->y;
	particle->currentPos->z =emitter->position->z;
	if (myRandom() < 0.5) {
		particle->velocity->x = -4.0f*myRandom();
	}
	else
	{
		particle->velocity->x = 4.0f*myRandom();
	}
	particle->velocity->y = 0.025;
	if (myRandom() < 0.5) {
		particle->velocity->z = -4.0f*myRandom();
	}
	else
	{
		particle->velocity->z = 4.0f*myRandom();
	}
	particle->acceleration->x = 3.0f;
	particle->acceleration->y = GRAVITY_CONST;
	particle->acceleration->z = 3.0f;
	particle->particleTime = .0f;
	particle->next = NULL;
	particle->color->G = 0.0f;
	particle->color->B = myRandom()*2.0;
	particle->color->R = 0.0f;
	if (particle->color->B == 1.0) {
		particle->color->R = 1.0f;
		particle->color->G = 1.0f;
	}
	particle->color->A = 1.0f;
	particle->grassColor = (GLfloat) myRandom();
	particle->grassLength = 0.01f*myRandom();
	particle->chanceOfFlowering = myRandom();
}
///////////////////////////////////////////////
void initEmitter(Emitter* emitter) {
	//initialise particle set
	Emitter* freshEmitter = (Emitter*)malloc(sizeof(Emitter));
	freshEmitter->position = (Coord*)malloc(sizeof(Coord));
	freshEmitter->particlesPerSec = 3;
	freshEmitter->position->x = 0;
	freshEmitter->position->y = 0;
	freshEmitter->position->z = 0;
	Particle* particle = (Particle*)malloc(sizeof(Particle));
	freshEmitter->particleSet[0] = particle;
	freshEmitter->currentParticles = 1;
	memcpy(emitter, freshEmitter, sizeof(Emitter));	
}
////////////////////////////////////////////////
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
	calculateNewXVelocity(particle, TIME_STEP);
	calculateNewYVelocity(particle, TIME_STEP);
	calculateNewZVelocity(particle, TIME_STEP);
	calculateNewXPosition(particle, TIME_STEP);
	calculateNewYPosition(particle, TIME_STEP);
	calculateNewZPosition(particle, TIME_STEP);
	//	particle = particle->next;
	//}
}
void drawParticles() {
		Particle* particle =emitter->particleSet[0];
		Particle* tmp = (Particle*)malloc(sizeof(Particle));
		memcpy(tmp, particle, sizeof(Particle));
		glPointSize(PARTICLE_SIZE);
		int i = 0;
		while (tmp != NULL) {
			calculateNextPositions(tmp);
			if (abs(tmp->velocity->x) > 0.0 && abs(tmp->velocity->z) > 0.0) {
				glBegin(GL_POINTS);
				glColor4f(tmp->color->R, tmp->color->G, tmp->color->B, tmp->color->A);
				glVertex3f(tmp->currentPos->x, tmp->currentPos->y, tmp->currentPos->z);
				glEnd();
				if (tmp->particleTime > PARTICLE_LIFE && tmp->color->A > 0) {
					tmp->color->A -= .0001f;
				}
			}
			else
			{
				if (abs(tmp->currentPos->x) < 5 && abs(tmp->currentPos->z) < 5)
				{
					glBegin(GL_POINTS);
					glColor4f(1.0, 1.0, tmp->color->B, tmp->color->A);
					glVertex3f(tmp->currentPos->x, tmp->currentPos->y, tmp->currentPos->z);
					glEnd();
					if (tmp->color->A > 0) {
						tmp->color->A -= .0075f;
					}
				}
				else
				{
					glBegin(GL_LINES);
					glVertex3f(tmp->currentPos->x, tmp->currentPos->y, tmp->currentPos->z);
					if (tmp->grassLength > 0.2 && tmp->chanceOfFlowering < 0.1) {
						glColor4f(.8, tmp->grassColor, 0.0, 0.9);
					}
					else
					{
						glColor4f(0.0, tmp->grassColor, 0.0, 0.6);
					}

					glVertex3f(tmp->currentPos->x + 0.01*cos(myRandom()), tmp->currentPos->y + tmp->grassLength, tmp->currentPos->z);
					glEnd();
					if (tmp->grassLength < MAXIMUM_GRASS_HEIGHT)
						tmp->grassLength += GRASS_GROWTH_RATE;
				}
			}
			tmp->particleTime += TIME_STEP;
			emitter->particleSet[i] = tmp;
			tmp = tmp->next;
			i++;
		}
		currentTime += TIME_STEP;
		glutSwapBuffers();
}
//////////////////////////////////////////////
int leftMouseDown = 0;
void mouseMotion(int x, int y) {
	// Called when mouse moves
	if(leftMouseDown) {
		xRotation += (y - mouseY);	mouseY = y;
		yRotation += (x - mouseX);	mouseX = x;
		// keep all rotations between 0 and 360.
		if (xRotation > 360.0) xRotation -= 360.0;
		if (xRotation < 0.0)   xRotation += 360.0;
		if (yRotation > 360.0) yRotation -= 360.0;
		if (yRotation < 0.0)   yRotation += 360.0;
	}
	// ask for redisplay
	glutPostRedisplay();
}


void mousePress(int button, int state, int x, int y) {
	// When left mouse button is pressed, save the mouse(x,y)
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		leftMouseDown = 1;
		mouseX = x;
		mouseY = y;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		leftMouseDown = 0;
	}
}
void display()
{
  glLoadIdentity();
  glRotatef(xRotation, 1.0, 0.0, 0.0);
  glRotatef(yRotation, 0.0, 1.0, 0.0);
  glPushMatrix();
  calculateLookpoint();
  gluLookAt(eyeX, eyeY, eyeZ,
            centerX, centerY, centerZ,
            upX, upY, upZ);
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT);
  if (val != 5) {
	  glBegin(GL_QUADS);
	  glColor3f(0.52f, 0.37f, 0.26f);
	  glVertex3f(-100.0, 0.0, -100.0);
	  glVertex3f(-100.0, 0.0, 100.0);
	  glVertex3f(100.0, 0.0, 100.0);
	  glVertex3f(100.0, 0.0, -100.0);
	  glEnd();
	  glBegin(GL_QUADS);
	  glColor4f(0.0, .16f, 0.8f, .4f);
	  glVertex3f(-5.0, 0.0, -5.0);
	  glColor4f(0.0, .16f, .5f, .8f);
	  glVertex3f(-5.0, 0.0, 5.0);
	  glColor4f(0.0, 0.0, .70f, .4f);
	  glVertex3f(5.0, 0.0, 5.0);
	  glColor4f(1.0f, 1.0f, 1.0f, .8f);
	  glVertex3f(5.0, 0.0, -5.0);
	  glEnd();
  }
  //check to see if fountain is on
  if (val != 14/*fountain is on*/) {
	  if (emitter->currentParticles < PARTICLE_NUMBER) {
		  //Add new particle
		  emitter->particleSet[emitter->currentParticles - 1]->next = (Particle*)malloc(sizeof(Particle));
		  //initialise new particle
		  initParticle(emitter->particleSet[emitter->currentParticles - 1]->next);
		  //increase number of particles
		  emitter->currentParticles += 1;
	  }
  }
	// If enabled, draw coordinate axis
	if (axisEnabled) glCallList(axisList);
    drawParticles();
	glPopMatrix();
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
	eyeX += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 100:
    eyeX -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
    case 119:
    eyeY += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
	case 115: {
		if (eyeY >= MOVEMENT_FACTOR) {
			eyeY -= MOVEMENT_FACTOR;
		}
		calculateLookpoint();
	}
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
	{
		initEmitter(emitter);
		initParticle(emitter->particleSet[0]);
	}
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
      glColor4f(1.0, 0.0, 0.0, 1.0);       // X axis - red
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f((float) AXIS_SIZE, 0.0, 0.0);
      glColor4f(0.0, 1.0, 0.0, 1.0);       // Y axis - green
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, (float)AXIS_SIZE, 0.0);
      glColor4f(0.0, 0.0, 1.0, 1.0);       // Z axis - blue
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(0.0, 0.0, (float)AXIS_SIZE);
    glEnd();
  glEndList();
}
///////////////////////////////////////////////
void menu(int value) {
	menuOpen = 1;
	switch (value) {
	case 0:
	{
		glutDestroyWindow(window);
		exit(0);
		glutPostRedisplay();
		break;
	}
	case 2: {
		axisEnabled = 1;
		glutPostRedisplay();
		printf("AXIS SHOWING \n");
		break;
	}
	case 3:
	{
		axisEnabled = 0;
		glutPostRedisplay();
		printf("AXIS REMOVED \n");
		break;
	}
	case 6: {
		GRAVITY_CONST *= 0.25f;
		glutPostRedisplay();
		printf("GRAVITY IS: %f \n", GRAVITY_CONST);
		break;
	}
	case 7: {
		GRAVITY_CONST *= 0.5f;
		printf("GRAVITY IS: %f \n", GRAVITY_CONST);
		glutPostRedisplay();
		break;
	}
	case 8: {
		GRAVITY_CONST *= 1.5;
		printf("GRAVITY IS: %f \n", GRAVITY_CONST);
		glutPostRedisplay();
		break;
	}
	case 9: {
		GRAVITY_CONST *= 2.0;
		printf("GRAVITY IS: %f \n", GRAVITY_CONST);
		glutPostRedisplay();
		break;
	}
	case 10:
	{
		GRAVITY_CONST = -2.0;
		printf("GRAVITY RESET, GRAVITY IS: %f \n", GRAVITY_CONST);
		glutPostRedisplay();
		break;
	}
	case 11:
	{
		initEmitter(emitter);
		emitter->position->x = 10.0f*myRandom();
		emitter->position->y = 10.0f*myRandom();
		emitter->position->z = 10.0f*myRandom();
		initParticle(emitter->particleSet[0]);
		glutPostRedisplay();
		break;
	}
	case 12:
	{
		initEmitter(emitter);
		emitter->position->x = 0.0f;
		emitter->position->y = 0.0f;
		emitter->position->z = 0.0f;
		initParticle(emitter->particleSet[0]);
		glutPostRedisplay();
		break;
	}
	case 15:{
		PARTICLE_SIZE = 1.0f;
		glutPostRedisplay();
		printf("PARTICLE SIZE IS: %f \n", PARTICLE_SIZE);
		break;
	}
	case 16: {
		PARTICLE_SIZE = 3.0f;
		glutPostRedisplay();
		printf("PARTICLE SIZE IS: %f \n", PARTICLE_SIZE);
		break;
	}
	case 17: {
		PARTICLE_SIZE = 5.0f;
		glutPostRedisplay();
		printf("PARTICLE SIZE IS: %f \n", PARTICLE_SIZE);
		break;
	}
	default:
	{
		val = value;
		glutPostRedisplay();
		break;
	}
	}
}
void createMenu(void) {
	subAxisMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("Show", 2);
	glutAddMenuEntry("Remove", 3);
	subTerrainMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("Show", 4);
	glutAddMenuEntry("Remove", 5);
	gravitySubMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("x0.25", 6);
	glutAddMenuEntry("x0.5", 7);
	glutAddMenuEntry("x1.5", 8);
	glutAddMenuEntry("x2.0", 9);
	glutAddMenuEntry("RESET", 10);
	emitterPositionSubMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("Random", 11);
	glutAddMenuEntry("Origin", 12);
	fountainSubMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("Start", 13);
	glutAddMenuEntry("Stop", 14);
	particleSizeSubMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("Small", 15);
	glutAddMenuEntry("Medium", 16);
	glutAddMenuEntry("Large", 17);
	rootMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("QUIT", 0);
	glutAddSubMenu("Axis", subAxisMenuId);
	glutAddSubMenu("Fountain", fountainSubMenuId);
	glutAddSubMenu("Ground & Water", subTerrainMenuId);
	glutAddSubMenu("Gravity", gravitySubMenuId);
	glutAddSubMenu("Move Emitter", emitterPositionSubMenuId);
	glutAddSubMenu("Particle Size", particleSizeSubMenuId);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void initGraphics(int argc, char *argv[])
{
  /*INITIAL NAVIGATIONAL VARIABLES*/
  centerX = 0;
  centerY = 76;
  centerZ = 1;
  eyeX = .5;
  eyeY = 0.5;
  eyeZ = -50.0;
  upX = 0.0;
  upY = 1.0;
  upZ = 0.0;
  latitude = 0.0;
  longitude = 0.0;
  /******************************/
 pSystem = (ParticleSystem*)malloc(sizeof(ParticleSystem));
  /*INITIAL EMITTER VARIABLES*/
 emitter = (Emitter*)malloc(sizeof(Emitter));
 initEmitter(emitter);
 initParticle(emitter->particleSet[0]);
 // int i;
  /*
  for (i = 0; i < EMITTER_NUMBER; i++) {
	  Emitter* emitter = (Emitter*)malloc(sizeof(Emitter));
	  emitter->position = (Coord*)malloc(sizeof(Coord));
	  emitter->particlesPerSec = 1;
	  emitter->position->x = 100 * myRandom();
	  emitter->position->y = 100 * myRandom();
	  emitter->position->z = 100 * myRandom();
	  pSystem->emitterSet[i] = emitter;
  }*/
  /**************************/
  /*
  for (i = 0; i < EMITTER_NUMBER; i++) {
	  Particle* particle = (Particle*)malloc(sizeof(Particle));
	  initParticle(particle);
	 emitter->particleSet[0] = particle;
	 emitter->currentParticles = 1;
  }
  */
  /****************************/
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE);
  window = glutCreateWindow("COMP37111 Particles");
  //Enable alpha blending
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glShadeModel(GL_SMOOTH);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mousePress);
  glutMotionFunc(mouseMotion);
  glutReshapeFunc(reshape);
  createMenu();
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

