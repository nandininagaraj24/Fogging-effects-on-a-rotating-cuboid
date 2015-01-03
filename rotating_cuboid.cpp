#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <glut.h>

static int useRGB = 1;
static int useLighting = 1;
static int useFog = 0;
static int useDB = 1;
static int useQuads = 1;
static int tick = -1;
static int moving = 1;

#define RED		1
#define GREEN	2
#define BLUE	3
#define CYAN	4
#define MAGENTA	5
#define YELLOW	6
#define BLACK	7

static float materialColor[8][4] =
{
  {0.8, 0.8, 0.8, 1.0},
  {0.8, 0.0, 0.0, 1.0},
  {0.0, 0.8, 0.0, 1.0},
  {0.0, 0.0, 0.8, 1.0},
  {0.0, 0.8, 0.8, 1.0},
  {0.8, 0.0, 0.8, 1.0},
  {0.8, 0.8, 0.0, 1.0},
  {0.0, 0.0, 0.0, 0.6},
};

static float lightPos[4] =
{2.0, 4.0, 2.0, 1.0};
static float lightAmb[4] =
{0.2, 0.2, 0.2, 1.0};
static float lightDiff[4] =
{0.8, 0.8, 0.8, 1.0};
static float lightSpec[4] =
{0.4, 0.4, 0.4, 1.0};

static float groundPlane[4] =
{0.0, 1.0, 0.0, 1.499};
static float backPlane[4] =
{0.0, 0.0, 1.0, 0.899};

static float fogColor[4] =
{0.0, 0.0, 0.0, 0.0};
//static float fogIndex[1] =
//{0.0};

static unsigned char shadowPattern[128] =
{
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,  
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
  0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55
};

static float cube_vertices[6][4][4] =
{
  {
    {-1.0, -1.0, -1.0, 1.0},
    {-1.0, -1.0, 1.0, 1.0},
    {-1.0, 1.0, 1.0, 1.0},
    {-1.0, 1.0, -1.0, 1.0}},

  {
    {1.0, 1.0, 1.0, 1.0},
    {1.0, -1.0, 1.0, 1.0},
    {1.0, -1.0, -1.0, 1.0},
    {1.0, 1.0, -1.0, 1.0}},

  {
    {-1.0, -1.0, -1.0, 1.0},
    {1.0, -1.0, -1.0, 1.0},
    {1.0, -1.0, 1.0, 1.0},
    {-1.0, -1.0, 1.0, 1.0}},

  {
    {1.0, 1.0, 1.0, 1.0},
    {1.0, 1.0, -1.0, 1.0},
    {-1.0, 1.0, -1.0, 1.0},
    {-1.0, 1.0, 1.0, 1.0}},

  {
    {-1.0, -1.0, -1.0, 1.0},
    {-1.0, 1.0, -1.0, 1.0},
    {1.0, 1.0, -1.0, 1.0},
    {1.0, -1.0, -1.0, 1.0}},

  {
    {1.0, 1.0, 1.0, 1.0},
    {-1.0, 1.0, 1.0, 1.0},
    {-1.0, -1.0, 1.0, 1.0},
    {1.0, -1.0, 1.0, 1.0}}
};

static float cube_normals[6][4] =
{
  {-1.0, 0.0, 0.0, 0.0},
  {1.0, 0.0, 0.0, 0.0},
  {0.0, -1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0, 0.0},
  {0.0, 0.0, -1.0, 0.0},
  {0.0, 0.0, 1.0, 0.0}
};

void buildColormap(void)
{
	return;
}

static void setColor(int c)
{
  if (useLighting) 
  {
	  glMaterialfv(GL_FRONT_AND_BACK,
        GL_AMBIENT_AND_DIFFUSE, &materialColor[c][0]);
  }
  else 
  {
	  glColor4fv(&materialColor[c][0]);
  }
}

static void drawCube(int color)
{
  int i;
  setColor(color);
  for (i = 0; i < 6;++i) 
  {
    glNormal3fv(&cube_normals[i][0]);
    glBegin(GL_POLYGON);
    glVertex4fv(&cube_vertices[i][0][0]);
    glVertex4fv(&cube_vertices[i][1][0]);
    glVertex4fv(&cube_vertices[i][2][0]);
    glVertex4fv(&cube_vertices[i][3][0]);
    glEnd();
  }
}

static void drawCheck(int w, int h, int evenColor, int oddColor)
{
  static int initialized = 0;
  static int usedLighting = 0;
  static GLuint checklist = 0;
  if (!initialized || (usedLighting != useLighting)) 
  {
    static float square_normal[4] =
    {0.0, 0.0, 1.0, 0.0};
    static float square[4][4];
    int i, j;
	if (!checklist) 
	{
      checklist = glGenLists(1);
    }
    glNewList(checklist, GL_COMPILE_AND_EXECUTE);

    if (useQuads) 
	{
      glNormal3fv(square_normal);
      glBegin(GL_QUADS);
    }
    for (j = 0; j < h; ++j) 
	{
      for (i = 0; i < w;++i) 
	  {
        square[0][0] = -1.0 + 2.0 / w * i;
        square[0][1] = -1.0 + 2.0 / h * (j + 1);
        square[0][2] = 0.0;
        square[0][3] = 1.0;

        square[1][0] = -1.0 + 2.0 / w * i;
        square[1][1] = -1.0 + 2.0 / h * j;
        square[1][2] = 0.0;
        square[1][3] = 1.0;

        square[2][0] = -1.0 + 2.0 / w * (i + 1);
        square[2][1] = -1.0 + 2.0 / h * j;
        square[2][2] = 0.0;
        square[2][3] = 1.0;

        square[3][0] = -1.0 + 2.0 / w * (i + 1);
        square[3][1] = -1.0 + 2.0 / h * (j + 1);
        square[3][2] = 0.0;
        square[3][3] = 1.0;

        if (i & 1 ^ j & 1) 
		{
          setColor(oddColor);
        } 
		else 
		{
          setColor(evenColor);
        }

        if (!useQuads) 
		{
          glBegin(GL_POLYGON);
        }
        glVertex4fv(&square[0][0]);
        glVertex4fv(&square[1][0]);
        glVertex4fv(&square[2][0]);
        glVertex4fv(&square[3][0]);
        if (!useQuads) 
		{
          glEnd();
        }
      }
    }

    if (useQuads) 
	{
      glEnd();
    }
    glEndList();

    initialized = 1;
    usedLighting = useLighting;
  } 
  else 
  {
    glCallList(checklist);
  }
}

static void myShadowMatrix(float ground[4], float light[4])
{
  float dot;
  float shadowMat[4][4];

  dot = ground[0] * light[0] +
    ground[1] * light[1] +
    ground[2] * light[2] +
    ground[3] * light[3];

  shadowMat[0][0] = dot - light[0] * ground[0];
  shadowMat[1][0] = 0.0 - light[0] * ground[1];
  shadowMat[2][0] = 0.0 - light[0] * ground[2];
  shadowMat[3][0] = 0.0 - light[0] * ground[3];

  shadowMat[0][1] = 0.0 - light[1] * ground[0];
  shadowMat[1][1] = dot - light[1] * ground[1];
  shadowMat[2][1] = 0.0 - light[1] * ground[2];
  shadowMat[3][1] = 0.0 - light[1] * ground[3];

  shadowMat[0][2] = 0.0 - light[2] * ground[0];
  shadowMat[1][2] = 0.0 - light[2] * ground[1];
  shadowMat[2][2] = dot - light[2] * ground[2];
  shadowMat[3][2] = 0.0 - light[2] * ground[3];

  shadowMat[0][3] = 0.0 - light[3] * ground[0];
  shadowMat[1][3] = 0.0 - light[3] * ground[1];
  shadowMat[2][3] = 0.0 - light[3] * ground[2];
  shadowMat[3][3] = dot - light[3] * ground[3];

  glMultMatrixf((const GLfloat *) shadowMat);
}
void idle(void)
{
  tick++;
  if (tick >= 120) 
  {
    tick = 0;
  }
  glutPostRedisplay();
}


void keyboard(unsigned char ch, int x, int y)
{
  switch (ch) 
  {

  case 'L':
  case 'l':
    useLighting = !useLighting;
    useLighting ? glEnable(GL_LIGHTING) :
      glDisable(GL_LIGHTING);
    glutPostRedisplay();
    break;
  case 'F':
  case 'f':
    useFog = !useFog;
    useFog ? glEnable(GL_FOG) : glDisable(GL_FOG);
    glutPostRedisplay();
    break;
  case '1':
    glFogf(GL_FOG_MODE, GL_LINEAR);
    glutPostRedisplay();
    break;
  case '2':
    glFogf(GL_FOG_MODE, GL_EXP);
    glutPostRedisplay();
    break;
  case '3':
    glFogf(GL_FOG_MODE, GL_EXP2);
    glutPostRedisplay();
    break;
  case 's':
    if (!moving) 
	{
      idle();
      glutPostRedisplay();
    }
  }
}

void display(void)
{
  GLfloat cubeXform[4][4];

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  glTranslatef(0.0, -1.5, 0.0);
  glRotatef(-90.0, 1, 0, 0);
  glScalef(2.0, 2.0, 2.0);

  drawCheck(5, 5, BLUE, GREEN);  /* draw ground */
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, 0.0, -0.9);
  glScalef(2.0, 2.0, 2.0);

  drawCheck(5,5, BLUE,GREEN );  /* draw back */
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, 0.2, 0.0);
  glScalef(0.3, 0.3, 0.3);
  glRotatef((360.0 / (80 * 1)) * tick, 1, 0, 0);
  glRotatef((360.0 / (80 * 2)) * tick, 0, 1, 0);
  glRotatef((360.0 / (80 * 3)) * tick, 0, 0, 1);
  glScalef(1.0, 2.0, 1.0);
  glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) cubeXform);

  drawCube(YELLOW);        /* draw cube */
  glPopMatrix();

  glDepthMask(GL_FALSE);
  //if (useRGB) 
  //{
    glEnable(GL_BLEND);
 // } else 
  //{
    //glEnable(GL_POLYGON_STIPPLE);
  //}
  if (useFog) 
  {
    glDisable(GL_FOG);
  }
  glPushMatrix();
  myShadowMatrix(groundPlane, lightPos);
  glTranslatef(0.0, 0.0, 2.0);
  glMultMatrixf((const GLfloat *) cubeXform);
  drawCube(BLACK);      /* draw ground shadow */
  glPopMatrix();
  glPushMatrix();
  myShadowMatrix(backPlane, lightPos);
  glTranslatef(0.0, 0.0, 2.0);
  glMultMatrixf((const GLfloat *) cubeXform);
  drawCube(BLACK);      /* draw back shadow */
  glPopMatrix();
  glDepthMask(GL_TRUE);
  
  //if (useRGB) 
  //{
    glDisable(GL_BLEND);
 // } 
 // else 
  //{
    //glDisable(GL_POLYGON_STIPPLE);
  //}
  if (useFog) 
  {
    glEnable(GL_FOG);
  }
  if (useDB)
  {
    glutSwapBuffers();
  } 
  else 
  {
    glFlush();
  }
}

void fog_select(int fog)
{
  glFogf(GL_FOG_MODE, fog);
  glutPostRedisplay();
}

void menu_select(int mode)
{
  switch (mode) 
  {
  case 1:
    moving = 1;
    glutIdleFunc(idle);
    break;
  case 2:
    moving = 0;
    glutIdleFunc(NULL);
    break;
  case 3:
    useFog = !useFog;
    useFog ? glEnable(GL_FOG) : glDisable(GL_FOG);
    glutPostRedisplay();
    break;
  case 4:
    useLighting = !useLighting;
    useLighting ? glEnable(GL_LIGHTING) :
      glDisable(GL_LIGHTING);
    glutPostRedisplay();
    break;
  case 5:
    exit(0);
    break;
  }
}

void visible(int state)
{
  if (state == GLUT_VISIBLE) 
  {
    if (moving)
      glutIdleFunc(idle);
  } 
  else 
  {
    if (moving)
      glutIdleFunc(NULL);
  }
}

int main(int argc, char **argv)
{
  int width = 350, height = 350;
  int fog_menu;

  glutInitWindowSize(width, height);
  glutInit(&argc, argv);
  if (useDB) 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  else 
      glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

  glutCreateWindow("Fogging and Lighting on a spinning cube");

  buildColormap();

  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutVisibilityFunc(visible);

  fog_menu = glutCreateMenu(fog_select);
  glutAddMenuEntry("Linear fog", GL_LINEAR);
  glutAddMenuEntry("Exp^2 fog", GL_EXP2);
  glutAddMenuEntry("Exp fog", GL_EXP);
 

  glutCreateMenu(menu_select);
  glutAddMenuEntry("Start motion", 1);
  glutAddMenuEntry("Stop motion", 2);
  glutAddMenuEntry("Toggle fog", 3);
  glutAddMenuEntry("Toggle lighting", 4);
  glutAddSubMenu("Fog type", fog_menu);
  glutAddMenuEntry("Quit", 5);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 3.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -2.0);

  glEnable(GL_DEPTH_TEST);

  if (useLighting) 
  {
    glEnable(GL_LIGHTING);
  }
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

  glEnable(GL_NORMALIZE);

  if (useFog) 
  {
    glEnable(GL_FOG);
  }
  glFogfv(GL_FOG_COLOR, fogColor);
  //glFogfv(GL_FOG_INDEX, fogIndex);
  glFogf(GL_FOG_MODE, GL_EXP);
  glFogf(GL_FOG_DENSITY, 0.5);
  glFogf(GL_FOG_START, 1.0);
  glFogf(GL_FOG_END, 3.0);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glShadeModel(GL_SMOOTH);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 // glPolygonStipple((const GLubyte *) shadowPattern);
  

  glClearColor(0.0, 0.0, 0.0, 1);
  glClearDepth(1);

  glutMainLoop();
  return 0;            
}

