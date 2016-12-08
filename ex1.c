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

#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define DEG_TO_RAD_CONV 0.017453293f
#define PARTICLE_NUMBER 1000000
#define PARTICLE_LIFE .0001f
#define DAY_TIME 24
#define EMITTER_NUMBER 20
#define GRASS_GROWTH_RATE 0.0001f
#define MAXIMUM_GRASS_HEIGHT 2.0f

// Global viewpoint/camera variables
GLfloat latitude, longitude;
GLfloat eyeX, eyeY, eyeZ;
GLfloat centerX, centerY, centerZ;
GLfloat upX, upY, upZ;
GLfloat xRotation, yRotation = 0.0;
GLint mouseX, mouseY;
// Display list for coordinate axis
GLuint axisList;

int AXIS_SIZE = 200;
int axisEnabled = 1;
// Global Time Variable
GLfloat currentTime;

GLfloat MOVEMENT_FACTOR = 2.5f;
GLfloat TIME_STEP = 0.000005f;
GLfloat DAY_TIME_STEP = 0.0001f;
GLfloat GRAVITY_CONST = -2.0f;
GLfloat PARTICLE_SIZE = 1.0f;

static int rootMenuId;
static int axisSubMenuId;
static int terrainSubMenuId;
static int gravitySubMenuId;
static int timestepSubMenuId;
static int emitterPositionSubMenuId;
static int fountainSubMenuId;
static int particleSizeSubMenuId;
static int seasonalSubMenuId;
static int waterScheduleSubMenuId;
static int waterDuringDayEnabled = 0;
static int noScheduleEnabled = 1;
static int seasonalMenuEnabled = 0;
static int springEnabled = 1;
static int summerEnabled = 0;
static int autumnEnabled = 0;
static int winterEnabled = 0;
static int terrainMenuEnabled = 1;
static int fountainEnabled = 1;
static int window;
static int menuOpen = 0;
static int val = 0;
typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat z;
} Coord;

typedef struct {
  GLfloat R;
  GLfloat G;
  GLfloat B;
  GLfloat A;
} Color;

typedef struct {
  // GLfloat prevXPos, prevYPos, prevZPos;
  Coord *prevPos;
  Coord *currentPos;
  Coord *nextPos;
  Coord *velocity;
  Coord *acceleration;
  GLfloat particleTime;
  Color *color;
  GLfloat grassLength;
  GLfloat grassColor;
  GLfloat chanceOfFlowering;
  struct Particle *next;
} Particle;

typedef struct {
  Particle *particleSet[PARTICLE_NUMBER];
  int currentParticles;
  Coord *position;
} Emitter;

Emitter *emitter;
typedef struct {
  Color *color;
  GLfloat currentTimeOfDay;
} Sky;

Sky *sky;
///////////////////////////////////////////////

double myRandom()
// Return random double within range [0,1]
{
  return (rand() / (double)RAND_MAX);
}

///////////////////////////////////////////////
void calculateNewXPosition(Particle *p, GLfloat dt) {
  p->nextPos->x = p->velocity->x * (dt);
  p->prevPos->x = p->currentPos->x;
  p->currentPos->x += p->nextPos->x;
  p->nextPos->x = 0.0f;
}
void calculateNewYPosition(Particle *p, GLfloat dt) {
  if (p->currentPos->y < 0.0) {
    if (p->velocity->y < 0.001) {
      p->velocity->y = 0.0f;
    }
    p->velocity->x *= 0.75;
    p->velocity->z *= 0.75;
    if (p->velocity->x <= 0.1) {
      p->acceleration->x = 0.0;
    }
    if (p->velocity->z <= 0.1) {
      p->acceleration->z = 0.0;
    }
  }
  p->nextPos->y = p->velocity->y + (float)(1 / 2 * (GRAVITY_CONST)*pow(dt, 2));
  p->prevPos->y = p->currentPos->y;
  p->currentPos->y += p->nextPos->y;
  p->nextPos->y = 0.0f;
}
void calculateNewZPosition(Particle *p, GLfloat dt) {
  p->nextPos->z = p->velocity->z * (dt);
  p->prevPos->z = p->currentPos->z;
  p->currentPos->z += p->nextPos->z;
  p->nextPos->z = 0.0f;
}
void calculateNewXVelocity(Particle *p, GLfloat dt) {
  p->velocity->x += p->acceleration->x * (dt);
}
void calculateNewYVelocity(Particle *p, GLfloat dt) {
  p->velocity->y += p->acceleration->y * (dt);
}
void calculateNewZVelocity(Particle *p, GLfloat dt) {
  p->velocity->z += p->acceleration->z * (dt);
}

void initParticle(Particle *particle) {
  // initialise particle attributes
  particle->prevPos = (Coord *)malloc(sizeof(Coord));
  particle->currentPos = (Coord *)malloc(sizeof(Coord));
  particle->nextPos = (Coord *)malloc(sizeof(Coord));
  particle->velocity = (Coord *)malloc(sizeof(Coord));
  particle->acceleration = (Coord *)malloc(sizeof(Coord));
  particle->color = (Color *)malloc(sizeof(Color));
  // set emission position to emitter position
  particle->currentPos->x = emitter->position->x;
  particle->currentPos->y = emitter->position->y;
  particle->currentPos->z = emitter->position->z;
  // vary x and z velocities to cause fanned out sprinkler motion
  if (myRandom() <= 0.5) {
    particle->velocity->x = -40.0f * (GLfloat)myRandom();
  } else {
    particle->velocity->x = 40.0f * (GLfloat)myRandom();
  }
  if (myRandom() <= 0.5) {
    particle->velocity->z = -40.0f * (GLfloat)myRandom();
  } else {
    particle->velocity->z = 40.0f * (GLfloat)myRandom();
  }
  // set same initial y-velocity for all particles
  particle->velocity->y = 0.2f;
  // set accelerations for x,y,z
  particle->acceleration->x = .05f;
  particle->acceleration->y = GRAVITY_CONST;
  particle->acceleration->z = .05f;
  particle->particleTime = .0f;
  particle->next = NULL;
  // set watery colour for particles while in fountain
  particle->color->G = 0.0f;
  particle->color->B = (GLfloat)myRandom();
  particle->color->R = 0.0f;
  // setting random amount of particles to white for water spray effect
  if (myRandom() < 0.25) {
    particle->color->B = 1.0f;
    particle->color->R = 1.0f;
    particle->color->G = 1.0f;
  }
  particle->color->A = 1.5f * (GLfloat)myRandom();
  // setting grass attributes for each particle
  particle->grassColor = (GLfloat)myRandom();
  particle->grassLength = 0.01f * (GLfloat)myRandom();
  particle->chanceOfFlowering = (GLfloat)myRandom();
}
///////////////////////////////////////////////
void initEmitter(Emitter *emitter) {
  // create a fresh emitter
  Emitter *freshEmitter = (Emitter *)malloc(sizeof(Emitter));
  // setup position of fresh emitter
  freshEmitter->position = (Coord *)malloc(sizeof(Coord));
  freshEmitter->position->x = 0.0f;
  freshEmitter->position->y = 0.0f;
  freshEmitter->position->z = 0.0f;
  // initialise particle set
  Particle *particle = (Particle *)malloc(sizeof(Particle));
  freshEmitter->particleSet[0] = particle;
  freshEmitter->currentParticles = 1;
  // copy freshEmitter into emitter
  memcpy(emitter, freshEmitter, sizeof(Emitter));
}
////////////////////////////////////////////////
void calculateLookpoint() {
  GLfloat deltaX =
      cosf(DEG_TO_RAD_CONV * latitude) * sinf(DEG_TO_RAD_CONV * longitude);
  GLfloat deltaY = sinf(DEG_TO_RAD_CONV * -latitude);
  GLfloat deltaZ =
      cosf(DEG_TO_RAD_CONV * latitude) * cosf(DEG_TO_RAD_CONV * longitude);

  centerX = eyeX + deltaX;
  centerY = eyeY + deltaY;
  centerZ = eyeZ + deltaZ;
}
///////////////////////////////////////////////
void calculateNextPositions(Particle *particle) {
  if (abs(particle->velocity->x) > 0 && abs(particle->velocity->z) > 0) {
    calculateNewXVelocity(particle, particle->particleTime - TIME_STEP);
    calculateNewYVelocity(particle, particle->particleTime - TIME_STEP);
    calculateNewZVelocity(particle, particle->particleTime - TIME_STEP);
    calculateNewXPosition(particle, particle->particleTime - TIME_STEP);
    calculateNewYPosition(particle, particle->particleTime - TIME_STEP);
    calculateNewZPosition(particle, particle->particleTime - TIME_STEP);
  }
}
///////////////////////////////////////////////
void drawParticles() {
  Particle *particle = emitter->particleSet[0];
  Particle *tmp = (Particle *)malloc(sizeof(Particle));
  memcpy(tmp, particle, sizeof(Particle));
  glPointSize(PARTICLE_SIZE);
  int i = 0;
  while (tmp != NULL) {
    calculateNextPositions(tmp);
    if (abs(tmp->velocity->x) > 0 && abs(tmp->velocity->z) > 0) {
      glBegin(GL_LINES);
      glColor4f(tmp->color->R, tmp->color->G, tmp->color->B, tmp->color->A);
      glVertex3f(tmp->prevPos->x, tmp->prevPos->y, tmp->prevPos->z);
      glVertex3f(tmp->currentPos->x, tmp->currentPos->y, tmp->currentPos->z);
      glEnd();
      if (tmp->particleTime > PARTICLE_LIFE && tmp->color->A > 0.0f) {
        tmp->color->A -= .001f;
      }
    } else {
      if (abs(tmp->currentPos->x - emitter->position->x) < 5 &&
          abs(tmp->currentPos->z - emitter->position->z) < 5) {
        glBegin(GL_POINTS);
        glColor4f(1.0f, 1.0f, tmp->color->B, tmp->color->A);
        glVertex3f(tmp->currentPos->x, tmp->currentPos->y, tmp->currentPos->z);
        glEnd();
        if (tmp->particleTime > PARTICLE_LIFE && tmp->color->A > 0.0f) {
          tmp->color->A -= .75f;
        }
      } else {
        glBegin(GL_LINES);
        glVertex3f(tmp->currentPos->x, tmp->currentPos->y, tmp->currentPos->z);
        if (tmp->grassLength > 0.2f && tmp->chanceOfFlowering < 0.1f) {
          if (seasonalMenuEnabled) {
            if (springEnabled /*SPRING*/) {
              glColor4f(0.1f, 0.7f, 0.05f, 1.0f);
            }
            if (summerEnabled /*SUMMER*/) {
              glColor4f(0.8f, 0.9f, 0.0f, 0.9f);
            }
            if (autumnEnabled /*AUTUMN*/) {
              glColor4f(0.8f, 0.6f, 0.0f, 0.9f);
            }
            if (winterEnabled /*WINTER*/) {
              glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
            }
          } else {
            glColor4f(0.1f, tmp->grassColor, 0.05f, 1.0f);
          }
        } else {
          if (seasonalMenuEnabled) {
            if (springEnabled /*SPRING*/) {
              glColor4f(0.0f, tmp->grassColor, 0.0f, 1.0f);
            }
            if (summerEnabled /*SUMMER*/) {
              glColor4f(0.1f, tmp->grassColor, 0.1f, 1.0f);
            }
            if (autumnEnabled /*AUTUMN*/) {
              glColor4f(0.4f, tmp->grassColor, 0.0f, 0.7f);
            }
            if (winterEnabled /*WINTER*/) {
              if (tmp->chanceOfFlowering < .1) {
                glColor4f(.9f, 1.0f, .9f, 0.3f);
              } else {
                glColor4f(0.0f, 0.4f, 0.0f, 0.1f);
              }
            }
          } else {
            glColor4f(0.0f, tmp->grassColor, 0.0f, 1.0f);
          }
        }

        glVertex3f(tmp->currentPos->x +
                       0.1f * (GLfloat)cos(tmp->chanceOfFlowering),
                   tmp->currentPos->y + tmp->grassLength, tmp->currentPos->z);
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
  if (leftMouseDown) {
    xRotation += (y - mouseY);
    mouseY = y;
    yRotation += (x - mouseX);
    mouseX = x;
    // keep all rotations between 0 and 360.
    if (xRotation > 360.0f)
      xRotation -= 360.0f;
    if (xRotation < 0.0f)
      xRotation += 360.0f;
    if (yRotation > 360.0f)
      yRotation -= 360.0f;
    if (yRotation < 0.0f)
      yRotation += 360.0f;
  }
  // ask for redisplay
  glutPostRedisplay();
}
///////////////////////////////////////////////
void mousePress(int button, int state, int x, int y) {
  // When left mouse button is pressed, save the mouse(x,y)
  if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
    leftMouseDown = 1;
    mouseX = x;
    mouseY = y;
  } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    leftMouseDown = 0;
  }
}
///////////////////////////////////////////////
void display(void) {
  glLoadIdentity();
  glRotatef(xRotation, 1.0, 0.0, 0.0);
  glRotatef(yRotation, 0.0, 1.0, 0.0);
  glPushMatrix();
  calculateLookpoint();
  gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
  // Clear the screen
  if ((int)sky->currentTimeOfDay % (DAY_TIME) == 0) {
    sky->color->R -= 2.0f * DAY_TIME_STEP;
    sky->color->G -= 2.0f * DAY_TIME_STEP;
    sky->color->B -= DAY_TIME_STEP;
  } else {
    sky->color->R += 2.0f * DAY_TIME_STEP;
    sky->color->G += 2.0f * DAY_TIME_STEP;
    sky->color->B += DAY_TIME_STEP;
    if (sky->color->R > 0.9f && sky->color->G > 0.9f && sky->color->B > 0.8f) {
      sky->currentTimeOfDay = 0.0f;
    }
  }
  sky->currentTimeOfDay += DAY_TIME_STEP;
  glClearColor(sky->color->R, sky->color->G, sky->color->B, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  if (terrainMenuEnabled) {
    glBegin(GL_QUADS);
    glColor3f(0.42f, 0.27f, 0.16f);
    glVertex3f(-400.0f, 0.0f, -400.0f);
    glVertex3f(-400.0f, 0.0f, 400.0f);
    glVertex3f(400.0f, 0.0f, 400.0f);
    glVertex3f(400.0f, 0.0f, -400.0f);
    glEnd();
    glBegin(GL_QUADS);
    glColor4f(0.0f, .16f, 0.8f, .4f);
    glVertex3f(emitter->position->x - 5.0f, 0.0f, emitter->position->z - 5.0f);
    glColor4f(0.0f, .16f, .5f, .8f);
    glVertex3f(emitter->position->x - 5.0f, 0.0f, emitter->position->z + 5.0f);
    glColor4f(0.0f, 0.0f, .70f, .4f);
    glVertex3f(emitter->position->x + 5.0f, 0.0f, emitter->position->z + 5.0f);
    glColor4f(1.0f, 1.0f, 1.0f, .8f);
    glVertex3f(emitter->position->x + 5.0f, 0.0f, emitter->position->z - 5.0f);
    glEnd();
  }
  // check to see if fountain is on
  if (fountainEnabled /*fountain is on*/) {
    if (waterDuringDayEnabled && !noScheduleEnabled) {
      if (sky->color->R > 0.1f && sky->color->G > 0.1f &&
          emitter->currentParticles < PARTICLE_NUMBER) {
        // Add new particle
        emitter->particleSet[emitter->currentParticles - 1]->next =
            (Particle *)malloc(sizeof(Particle));
        // initialise new particle
        initParticle(emitter->particleSet[emitter->currentParticles - 1]->next);
        // increase number of particles
        emitter->currentParticles += 1;
      }
    } else if (!waterDuringDayEnabled && !noScheduleEnabled) {
      if (sky->color->R <= 0.1f && sky->color->G <= 0.1f &&
          emitter->currentParticles < PARTICLE_NUMBER) {
        // Add new particle
        emitter->particleSet[emitter->currentParticles - 1]->next =
            (Particle *)malloc(sizeof(Particle));
        // initialise new particle
        initParticle(emitter->particleSet[emitter->currentParticles - 1]->next);
        // increase number of particles
        emitter->currentParticles += 1;
      }
    } else if (noScheduleEnabled) {
      if (emitter->currentParticles < PARTICLE_NUMBER) {
        // Add new particle
        emitter->particleSet[emitter->currentParticles - 1]->next =
            (Particle *)malloc(sizeof(Particle));
        // initialise new particle
        initParticle(emitter->particleSet[emitter->currentParticles - 1]->next);
        // increase number of particles
        emitter->currentParticles += 1;
      }
    }
  }
  // If enabled, draw coordinate axis
  if (axisEnabled)
    glCallList(axisList);
  drawParticles();
  glPopMatrix();
  glutPostRedisplay();
}

///////////////////////////////////////////////
void keyboard(unsigned char key, int x, int y) {
  switch (key) {
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
    if (eyeY > 1.5 * MOVEMENT_FACTOR) {
      eyeY -= MOVEMENT_FACTOR;
    }
    calculateLookpoint();
  } break;
  case 120:
    eyeZ -= MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
  case 122:
    eyeZ += MOVEMENT_FACTOR;
    calculateLookpoint();
    break;
  case 32: {
    initEmitter(emitter);
    initParticle(emitter->particleSet[0]);
  } break;
  }
  glutPostRedisplay();
}

///////////////////////////////////////////////

void reshape(int width, int height) {
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
  glColor4f(1.0, 0.0, 0.0, 1.0); // X axis - red
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f((float)AXIS_SIZE, 0.0, 0.0);
  glColor4f(0.0, 1.0, 0.0, 1.0); // Y axis - green
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, (float)AXIS_SIZE, 0.0);
  glColor4f(0.0, 0.0, 1.0, 1.0); // Z axis - blue
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, (float)AXIS_SIZE);
  glEnd();
  glEndList();
}
///////////////////////////////////////////////
/********MENU********/
void menu(int value) {
  menuOpen = 1;
  switch (value) {
  case 0: {
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
  case 3: {
    axisEnabled = 0;
    glutPostRedisplay();
    printf("AXIS REMOVED \n");
    break;
  }
  case 4: {
    terrainMenuEnabled = 1;
    glutPostRedisplay();
    printf("TERRAIN SHOWING\n");
    break;
  }
  case 5: {
    terrainMenuEnabled = 0;
    glutPostRedisplay();
    printf("TERRAIN REMOVED\n");
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
    GRAVITY_CONST *= 1.5f;
    printf("GRAVITY IS: %f \n", GRAVITY_CONST);
    glutPostRedisplay();
    break;
  }
  case 9: {
    GRAVITY_CONST *= 2.0f;
    printf("GRAVITY IS: %f \n", GRAVITY_CONST);
    glutPostRedisplay();
    break;
  }
  case 10: {
    GRAVITY_CONST = -2.0f;
    printf("GRAVITY RESET, GRAVITY IS: %f \n", GRAVITY_CONST);
    glutPostRedisplay();
    break;
  }
  case 11: {
    initEmitter(emitter);
    emitter->position->x = 10.0f * (GLfloat)myRandom();
    emitter->position->z = 10.0f * (GLfloat)myRandom();
    initParticle(emitter->particleSet[0]);
    glutPostRedisplay();
    break;
  }
  case 12: {
    initEmitter(emitter);
    emitter->position->x = 0.0f;
    emitter->position->y = 0.0f;
    emitter->position->z = 0.0f;
    initParticle(emitter->particleSet[0]);
    glutPostRedisplay();
    break;
  }
  case 13: {
    fountainEnabled = 1;
    val = 13;
    glutPostRedisplay();
    break;
  }
  case 14: {
    fountainEnabled = 0;
    val = 14;
    glutPostRedisplay();
    break;
  }
  case 15: {
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
  case 18: {
    seasonalMenuEnabled = 1;
    springEnabled = 1;
    summerEnabled = 0;
    autumnEnabled = 0;
    winterEnabled = 0;
    glutPostRedisplay();
    printf("SEASON: SPRING\n");
    break;
  }
  case 19: {
    seasonalMenuEnabled = 1;
    springEnabled = 0;
    summerEnabled = 1;
    autumnEnabled = 0;
    winterEnabled = 0;
    glutPostRedisplay();
    printf("SEASON: SUMMER\n");
    break;
  }
  case 20: {
    seasonalMenuEnabled = 1;
    springEnabled = 0;
    summerEnabled = 0;
    autumnEnabled = 1;
    winterEnabled = 0;
    ;
    glutPostRedisplay();
    printf("SEASON: AUTUMN\n");
    break;
  }
  case 21: {
    seasonalMenuEnabled = 1;
    springEnabled = 0;
    summerEnabled = 0;
    autumnEnabled = 0;
    winterEnabled = 1;
    glutPostRedisplay();
    printf("SEASON: WINTER\n");
    break;
  }
  case 22: {
    seasonalMenuEnabled = 0;
    springEnabled = 0;
    summerEnabled = 0;
    autumnEnabled = 0;
    winterEnabled = 0;
    glutPostRedisplay();
    printf("SEASON: DEFAULT\n");
    break;
  }
  case 23: {
    TIME_STEP *= 0.25f;
    glutPostRedisplay();
    printf("TIME-STEP IS: %fs \n", TIME_STEP);
    break;
  }
  case 24: {
    TIME_STEP *= 0.5f;
    glutPostRedisplay();
    printf("TIME-STEP IS: %fs \n", TIME_STEP);
    break;
  }
  case 25: {
    TIME_STEP *= 1.5f;
    glutPostRedisplay();
    printf("TIME-STEP IS: %fs \n", TIME_STEP);
    break;
  }
  case 26: {
    TIME_STEP *= 2.0f;
    glutPostRedisplay();
    printf("TIME-STEP IS: %fs \n", TIME_STEP);
    break;
  }
  case 27: {
    TIME_STEP = 0.001f;
    glutPostRedisplay();
    printf("TIME-STEP RESET, TIME-STEP IS: %fs \n", TIME_STEP);
    break;
  }
  case 28: {
    waterDuringDayEnabled = 1;
    noScheduleEnabled = 0;
    val = 28;
    glutPostRedisplay();
    printf("WATERING DURING THE DAY-TIME\n");
    break;
  }
  case 29: {
    waterDuringDayEnabled = 0;
    noScheduleEnabled = 0;
    val = 29;
    glutPostRedisplay();
    printf("WATERING DURING THE NIGHT-TIME\n");
    break;
  }
  case 30: {
    noScheduleEnabled = 1;
    val = 30;
    glutPostRedisplay();
    printf("WATERING ALL THE TIME!\n");
    break;
  }
  default: {
    val = value;
    glutPostRedisplay();
    break;
  }
  }
}
void createMenu(void) {
  axisSubMenuId = glutCreateMenu(menu);
  glutAddMenuEntry("Show", 2);
  glutAddMenuEntry("Remove", 3);
  terrainSubMenuId = glutCreateMenu(menu);
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
  seasonalSubMenuId = glutCreateMenu(menu);
  glutAddMenuEntry("Spring", 18);
  glutAddMenuEntry("Summer", 19);
  glutAddMenuEntry("Autumn", 20);
  glutAddMenuEntry("Winter", 21);
  glutAddMenuEntry("DEFAULT", 22);
  timestepSubMenuId = glutCreateMenu(menu);
  glutAddMenuEntry("x0.25", 23);
  glutAddMenuEntry("x0.5", 24);
  glutAddMenuEntry("x1.5", 25);
  glutAddMenuEntry("x2.0", 26);
  glutAddMenuEntry("RESET", 27);
  waterScheduleSubMenuId = glutCreateMenu(menu);
  glutAddMenuEntry("Day", 28);
  glutAddMenuEntry("Night", 29);
  glutAddMenuEntry("DEFAULT", 30);
  rootMenuId = glutCreateMenu(menu);
  glutAddMenuEntry("QUIT", 0);
  glutAddSubMenu("Axis", axisSubMenuId);
  glutAddSubMenu("Fountain", fountainSubMenuId);
  glutAddSubMenu("Fountain Schedule", waterScheduleSubMenuId);
  glutAddSubMenu("Ground & Water", terrainSubMenuId);
  glutAddSubMenu("Gravity", gravitySubMenuId);
  glutAddSubMenu("Time-step", timestepSubMenuId);
  glutAddSubMenu("Move Emitter", emitterPositionSubMenuId);
  glutAddSubMenu("Particle Size", particleSizeSubMenuId);
  glutAddSubMenu("Season", seasonalSubMenuId);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}
///////////////////////////////////////////////
/******INIT GRAPHICS AND MAIN******/
void initGraphics(int argc, char *argv[]) {
  /*INITIAL NAVIGATIONAL VARIABLES*/
  centerX = 0.0f;
  centerY = 76.0f;
  centerZ = 1.0f;
  eyeX = .5f;
  eyeY = 2.5f;
  eyeZ = -50.0f;
  upX = 0.0f;
  upY = 1.0f;
  upZ = 0.0f;
  latitude = 0.0f;
  longitude = 0.0f;
  /*INITIAL EMITTER VARIABLES*/
  emitter = (Emitter *)malloc(sizeof(Emitter));
  initEmitter(emitter);
  initParticle(emitter->particleSet[0]);
  /*INITAL SKY VARIABLES*/
  sky = (Sky *)malloc(sizeof(Sky));
  sky->color = (Color *)malloc(sizeof(Color));
  sky->color->R = 0.980392f;
  sky->color->G = 0.980392f;
  sky->color->B = 0.823529f;
  sky->currentTimeOfDay = 0.0f;
  /*SETUP*/
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE);
  window = glutCreateWindow("COMP37111 Particles");
  // Enable alpha blending
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

int main(int argc, char *argv[]) {
  double f;
  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glutMainLoop();
}
