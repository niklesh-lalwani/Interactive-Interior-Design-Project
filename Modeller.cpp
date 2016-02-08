/*******************************************************************
	       Scene Modelling Program
********************************************************************/
#pragma warning (disable : 4996)

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "QuadMesh.h"
#include "CubeMesh.h"
#include "Chairs.h"
#include "Avatar.h"
#include "Tables.h"
#include "Sofas.h"
#include "Textures.h"
#include "Shelves.h"

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
VECTOR3D ScreenToWorld(int x, int y);
void door1animate(void);
void door2animate(void);
int CheckCollisionAvatar(void);
int CheckCollisionObjects(VECTOR3D min2, VECTOR3D max2, GLfloat tx1, GLfloat tz1);

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef unsigned short ushort;
typedef unsigned long  ulong;

typedef struct RGB
{
	byte r, g, b;
} RGB;

typedef struct RGBpixmap
{
	int nRows, nCols;
	RGB *pixel;
} RGBpixmap;

GLuint readBMPFile(RGBpixmap *pm, char *file);

RGBpixmap pix[6]; // make six empty pixmaps, one for each side of cube

static int currentButton;
static unsigned char currentKey;

GLfloat cameraX, cameraY, cameraZ;
GLfloat lookATx, lookATy, lookATz;
GLfloat directionX, directionY, directionZ, direction;
int oldmouseX, oldmouseY;
GLfloat theta, phi, radius;
GLfloat thetar, phir, theta_head;

GLfloat light_position0[] = { -8.0, 0.0, 8.0, 0.0 };
GLfloat light_position1[] = { 8.0,  0.0, -8.0, 0.0 };
GLfloat light_position2[] = { -8.0,  0.0, -8.0, 0.0 };
GLfloat light_diffuse[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_ambient[]   = {1.0, 1.0, 1.0, 1.0};

// Interaction State Variable
enum Action {TRANSLATE, ROTATE, SCALE, EXTRUDE};
enum Action currentAction = TRANSLATE;

QuadMesh *floorMesh1 = NULL;
QuadMesh *floorMesh2 = NULL;
QuadMesh *floorMesh3 = NULL;
// Wall Mesh variables here
CubeMesh *wallMesh_L1 = NULL; // Left wall
CubeMesh *wallMesh_B1 = NULL; // Back wall
CubeMesh *wallMesh_R1 = NULL; // Right wall
CubeMesh *wallMesh_R2 = NULL; // Right wall 2
CubeMesh *wallMesh_F1 = NULL; // Front wall 1
CubeMesh *wallMesh_F2 = NULL; // Front wall 2
CubeMesh *wallMesh_M1[3] = { NULL }; // Middle wall 1
CubeMesh *wallMesh_M2[3] = { NULL }; // Middle wall 2
// Doors
CubeMesh *Door1 = NULL;
CubeMesh *Door2 = NULL;

Table *Tables[5] = { NULL };
Sofa *Sofas[5] = { NULL };
Chair *Chairs[5] = { NULL };
Avatar *Avatar = NULL;
Shelf *Shelves[5] = { NULL };

GLuint tex[4];

GLfloat planes[] = { 0.0, 0.0, 0.3, 0.0 };
GLfloat planet[] = { 0.0, 0.3, 0.0, 0.0 };

int i = 0, current = 0, flag = 0;
bool count = 0;
bool penetrate = 0;
bool selectmode = 0;
bool door1open = 0;
bool door2open = 0;
int countN = 0,  countD = 0, countW = 0;
int countT = 0, countS = 0, countC = 0, countF = 0;
bool POVCAMERA = 0;

struct BoundingBox
{
  VECTOR3D min;
  VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 16;

void fskip(FILE *fp, int num_bytes)
{
	int i;
	for (i = 0; i<num_bytes; i++)
		fgetc(fp);
}

ushort getShort(FILE *fp) //helper function
{ //BMP format uses little-endian integer types
  // get a 2-byte integer stored in little-endian form
	char ic;
	ushort ip;
	ic = fgetc(fp); ip = ic;  //first byte is little one 
	ic = fgetc(fp);  ip |= ((ushort)ic << 8); // or in high order byte
	return ip;
}
//<<<<<<<<<<<<<<<<<<<< getLong >>>>>>>>>>>>>>>>>>>
ulong getLong(FILE *fp) //helper function
{  //BMP format uses little-endian integer types
   // get a 4-byte integer stored in little-endian form
	ulong ip = 0;
	char ic = 0;
	unsigned char uc = ic;
	ic = fgetc(fp); uc = ic; ip = uc;
	ic = fgetc(fp); uc = ic; ip |= ((ulong)uc << 8);
	ic = fgetc(fp); uc = ic; ip |= ((ulong)uc << 16);
	ic = fgetc(fp); uc = ic; ip |= ((ulong)uc << 24);
	return ip;
}

int main(int argc, char **argv)
{
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Scene Modeller");

  initOpenGL(500,500);
  cameraX = 0.0;
  cameraY = 8.0;
  cameraZ = 26.0;
  theta = 40.0;
  theta_head = 90.0;
  phi = 0.0;
  radius = 50.0;
  oldmouseX = 0;
  oldmouseY = 0;

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotionHandler);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(functionKeys);
  glutMainLoop();
  return 0;
}

GLuint readBMPFile(RGBpixmap *pm, char *file)
{
	FILE *fp;
	long index;
	int k, row, col, numPadBytes, nBytesInRow;
	char ch1, ch2;
	ulong fileSize;
	ushort reserved1;    // always 0
	ushort reserved2;     // always 0 
	ulong offBits; // offset to image - unreliable
	ulong headerSize;     // always 40
	ulong numCols; // number of columns in image
	ulong numRows; // number of rows in image
	ushort planes;      // always 1 
	ushort bitsPerPixel;    //8 or 24; allow 24 here
	ulong compression;      // must be 0 for uncompressed 
	ulong imageSize;       // total bytes in image 
	ulong xPels;    // always 0 
	ulong yPels;    // always 0 
	ulong numLUTentries;    // 256 for 8 bit, otherwise 0 
	ulong impColors;       // always 0 
	long count;
	char dum;

	/* open the file */
	if ((fp = fopen(file, "rb")) == NULL)
	{
		printf("Error opening file %s.\n", file);
		exit(1);
	}

	/* check to see if it is a valid bitmap file */
	if (fgetc(fp) != 'B' || fgetc(fp) != 'M')
	{
		fclose(fp);
		printf("%s is not a bitmap file.\n", file);
		exit(1);
	}

	fileSize = getLong(fp);
	reserved1 = getShort(fp);    // always 0
	reserved2 = getShort(fp);     // always 0 
	offBits = getLong(fp); // offset to image - unreliable
	headerSize = getLong(fp);     // always 40
	numCols = getLong(fp); // number of columns in image
	numRows = getLong(fp); // number of rows in image
	planes = getShort(fp);      // always 1 
	bitsPerPixel = getShort(fp);    //8 or 24; allow 24 here
	compression = getLong(fp);      // must be 0 for uncompressed 
	imageSize = getLong(fp);       // total bytes in image 
	xPels = getLong(fp);    // always 0 
	yPels = getLong(fp);    // always 0 
	numLUTentries = getLong(fp);    // 256 for 8 bit, otherwise 0 
	impColors = getLong(fp);       // always 0 

	if (bitsPerPixel != 24)
	{ // error - must be a 24 bit uncompressed image
		printf("Error bitsperpixel not 24\n");
		exit(1);
	}
	//add bytes at end of each row so total # is a multiple of 4
	// round up 3*numCols to next mult. of 4
	nBytesInRow = ((3 * numCols + 3) / 4) * 4;
	numPadBytes = nBytesInRow - 3 * numCols; // need this many
	pm->nRows = numRows; // set class's data members
	pm->nCols = numCols;
	pm->pixel = (RGB *)malloc(3 * numRows * numCols);//make space for array
	if (!pm->pixel) return 2; // out of memory!
	count = 0;
	dum;
	for (row = 0; row < numRows; row++) // read pixel values
	{
		for (col = 0; col < numCols; col++)
		{
			int r, g, b;
			b = fgetc(fp); g = fgetc(fp); r = fgetc(fp); //read bytes
			pm->pixel[count].r = r; //place them in colors
			pm->pixel[count].g = g;
			pm->pixel[count++].b = b;
		}
		fskip(fp, numPadBytes);
	}
	fclose(fp);
}

void setTexture(RGBpixmap *p, GLuint textureID)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p->nCols, p->nRows, 0, GL_RGB,
		GL_UNSIGNED_BYTE, p->pixel);
}

// Setup openGL */
void initOpenGL(int w, int h)
{
	readBMPFile(&pix[0], "wall2.bmp");  
	setTexture(&pix[0], 2000);
	readBMPFile(&pix[1], "floor1.bmp");  
	setTexture(&pix[1], 2002);
	readBMPFile(&pix[2], "floor2.bmp");  
	setTexture(&pix[2], 2001);
	readBMPFile(&pix[3], "floor3.bmp");  
	setTexture(&pix[3], 2003);
	readBMPFile(&pix[4], "door1.bmp"); 
	setTexture(&pix[4], 2004);

	makeTextureMap1();
	makeTextureMap2();
	makeTextureMap3();
	makeTextureMap4();
	makeTextures(tex);
  // Set up viewport, projection, then change to modelview matrix mode - 
  // display function will then set up camera and modeling transforms
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.2,90.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set up and enable lighting
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  
  // Other OpenGL setup
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.6, 0.6, 0.6, 0.0);  
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  // This one is important - renormalize normal vectors 
  glEnable(GL_NORMALIZE);
  
  //Nice perspective.
  glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST);

  // Set up meshes
  VECTOR3D origin1  = VECTOR3D(-16.0f, 0.0f, 0.0f);
  VECTOR3D origin2 = VECTOR3D(-16.0f, 0.0f, -16.0f);
  VECTOR3D origin3 = VECTOR3D(0.0f, 0.0f, -16.0f);
  VECTOR3D dir1v   = VECTOR3D(1.0f, 0.0f, 0.0f);
  VECTOR3D dir2v   = VECTOR3D(0.0f, 0.0f, 1.0f);
  
  floorMesh1 = new QuadMesh(meshSize, 16.0);
  floorMesh2 = new QuadMesh(meshSize, 16.0);
  floorMesh3 = new QuadMesh(meshSize, 16.0);
  floorMesh1->InitMesh(meshSize, origin1, 16.0, 16.0, dir1v, dir2v);
  floorMesh2->InitMesh(meshSize, origin2, 16.0, 16.0, dir1v, dir2v);
  floorMesh3->InitMesh(meshSize, origin3, 16.0, 16.0, dir1v, dir2v);
  
  VECTOR3D ambient = VECTOR3D(0.0f,0.0f,0.0f);
  VECTOR3D specular= VECTOR3D(0.0f,0.0f,0.0f);
  VECTOR3D diffuse1= VECTOR3D(0.9f,0.0f,0.0f);
  VECTOR3D diffuse2 = VECTOR3D(0.0f, 0.9f, 0.0f);
  VECTOR3D diffuse3 = VECTOR3D(0.0f, 0.0f, 0.9f);
  float shininess = 0.0;
  floorMesh1->SetMaterial(ambient, diffuse1, specular, shininess);
  floorMesh2->SetMaterial(ambient, diffuse2, specular, shininess);
  floorMesh3->SetMaterial(ambient, diffuse3, specular, shininess);

  // make walls using CubeMesh
  wallMesh_L1 = createCube();
  wallMesh_L1->tx = -16.0;
  wallMesh_L1->sfz = 16.0;
  wallMesh_L1->sfy = 3.0;

  wallMesh_B1 = createCube();
  wallMesh_B1->sfx = 17.0;
  wallMesh_B1->tz = -17.0;
  wallMesh_B1->sfy = 3.0;

  wallMesh_R1 = createCube();
  wallMesh_R1->sfz = 8.0;
  wallMesh_R1->tz = -8.0;
  wallMesh_R1->tx = 16.0;
  wallMesh_R1->sfy = 3.0;

  wallMesh_R2 = createCube();
  wallMesh_R2->sfz = 8.0;
  wallMesh_R2->tz = 8.0;
  wallMesh_R2->sfy = 3.0;

  wallMesh_F1 = createCube();
  wallMesh_F1->sfx = 8.5;
  wallMesh_F1->tx = 8.5;
  wallMesh_F1->sfy = 3.0;

  wallMesh_F2 = createCube();
  wallMesh_F2->sfx = 9.0;
  wallMesh_F2->tx = -8.0;
  wallMesh_F2->tz = 16.0;
  wallMesh_F2->sfy = 3.0;

  for (i = 0; i < 3; i++)
  {
	  wallMesh_M1[i] = createCube();
	  wallMesh_M2[i] = createCube();
  }
  
  wallMesh_M1[0]->sfx = 4.0;
  wallMesh_M1[0]->tx = -12.0;
  wallMesh_M1[0]->sfy = 2.0;

  wallMesh_M1[1]->sfx = 2.5;
  wallMesh_M1[1]->tx = -2.5;
  wallMesh_M1[1]->sfy = 2.0;

  wallMesh_M1[2]->sfx = 8.0;
  wallMesh_M1[2]->tx = -8.0;
  wallMesh_M1[2]->ty = 4.0;

  wallMesh_M2[0]->sfz = 4.0;
  wallMesh_M2[0]->tz = -12.0;
  wallMesh_M2[0]->sfy = 2.0;

  wallMesh_M2[1]->sfz = 2.5;
  wallMesh_M2[1]->tz = -2.5;
  wallMesh_M2[1]->sfy = 2.0;

  wallMesh_M2[2]->sfz = 8.0;
  wallMesh_M2[2]->tz = -8.0;
  wallMesh_M2[2]->ty = 4.0;

  Door1 = createCube();
  Door1->sfx = 1.5;
  Door1->tx = -6.5;
  Door1->sfz = 0.5;
  Door1->sfy = 2.0;
  Door1->mat_diffuse[0] = 1.0;
  Door1->mat_diffuse[1] = 0.5;
  Door1->mat_diffuse[2] = 0.0;

  Door2 = createCube();
  Door2->sfz = 1.5;
  Door2->tz = -6.5;
  Door2->sfx = 0.5;
  Door2->sfy = 2.0;
  Door2->mat_diffuse[0] = 1.0;
  Door2->mat_diffuse[1] = 0.5;
  Door2->mat_diffuse[2] = 0.0;

  Avatar = createAvatar();
  Avatar->tx = -4.0;
  Avatar->tz = 12.0;
  Avatar->angle = 150;

  // Set up the bounding box of the room
  // Change this if you change your floor/wall dimensions
  BBox.min.Set(-8.0f, 0.0, -8.0);
  BBox.max.Set( 8.0f, 6.0,  8.0);

  Tables[0] = createTable();
  Tables[0]->tx = 8.15;
  Tables[0]->tz = -8.0;

  Tables[1] = createTable();
  Tables[1]->tx = -11.0;
  Tables[1]->tz = 4.0;
  Tables[1]->sfx = 1.5;
  Tables[1]->sfz = 1.5;

  countT = 2;

  Sofas[0] = createSofa();
  Sofas[0]->tx = 8.0;
  Sofas[0]->tz = -12.0;
  Sofas[0]->angle = 180;

  Sofas[1] = createSofa();
  Sofas[1]->tx = 8.0;
  Sofas[1]->tz = -4.0;

  Sofas[2] = createSofa();
  Sofas[2]->tx = -8.0;
  Sofas[2]->tz = -7.0;
  Sofas[2]->angle = 90;

  countS = 3;

  Chairs[0] = createChair();
  Chairs[0]->tx = -9.0;
  Chairs[0]->tz = 7.0;

  Chairs[1] = createChair();
  Chairs[1]->tx = -12.5;
  Chairs[1]->tz = 7.0;

  Chairs[2] = createChair();
  Chairs[2]->tx = -8.5;
  Chairs[2]->tz = -12.0;
  Chairs[2]->angle = 180.0;

  countC = 3;

  Shelves[0] = createShelf();
  Shelves[0]->tx = -12.0;
  Shelves[0]->tz = -8.0;
  Shelves[0]->angle = 270;

  Shelves[1] = createShelf();
  Shelves[1]->tx = -12.0;
  Shelves[1]->tz = 12.0;
  Shelves[1]->angle = 270;

  countF = 2;

}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glLoadIdentity();
  
  // Set up the camera
  if (POVCAMERA == 0)
  {
	  thetar = 3.14*theta / 180;
	  phir = 3.14*phi / 180;
	  if (Avatar->tx > 0) // room 3
	  {
		  cameraX = radius*sin(phir)*sin(thetar) + 8.0;
		  cameraY = radius*cos(thetar);
		  cameraZ = radius*sin(thetar)*cos(phir) - 8.0;
		  gluLookAt(cameraX, cameraY, cameraZ, 8.0, 0.0, -8.0, 0.0, 1.0, 0.0);
	  }
	  else if (Avatar->tx < 0 && Avatar->tz < 0) // room 2
	  {
		  cameraX = radius*sin(phir)*sin(thetar) - 8.0;
		  cameraY = radius*cos(thetar);
		  cameraZ = radius*sin(thetar)*cos(phir) - 8.0;
		  gluLookAt(cameraX, cameraY, cameraZ, -8.0, 0.0, -8.0, 0.0, 1.0, 0.0);
	  }
	  else if (Avatar->tx < 0 && Avatar->tz > 0) // room 1
	  {
		  cameraX = radius*sin(phir)*sin(thetar) - 8.0;
		  cameraY = radius*cos(thetar);
		  cameraZ = radius*sin(thetar)*cos(phir) + 8.0;
		  gluLookAt(cameraX, cameraY, cameraZ, -8.0, 0.0, 8.0, 0.0, 1.0, 0.0);
	  }
	  directionX = sin((Avatar->angle + 90) * 3.14 / 180);
	  directionY = 0;
	  directionZ = cos((Avatar->angle + 90) * 3.14 / 180);
	  direction = sqrt(pow(directionX, 2) + pow(directionY, 2) + pow(directionZ, 2));
  }
  else
  { // Camera attached to avatar
			  thetar = 3.14*theta_head / 180;
			  cameraX = Avatar->tx + 0.7*sin((Avatar->angle + Avatar->angle_head +90) * 3.14 / 180);
			  cameraY = Avatar->ty + Avatar->sfy*3;
			  cameraZ = Avatar->tz + 0.7*cos((Avatar->angle + Avatar->angle_head + 90) * 3.14 / 180);
			  lookATx = Avatar->tx + 25 * sin((Avatar->angle + Avatar->angle_head + 90) * 3.14 / 180);
			  lookATy = Avatar->ty + Avatar->sfy * 3 + 25*cos(thetar);
			  lookATz = Avatar->tz + 25 * cos((Avatar->angle + Avatar->angle_head + 90) * 3.14 / 180);
	  directionX = lookATx - cameraX;
	  directionY = lookATy - cameraY;
	  directionZ = lookATz - cameraZ;
	  direction = sqrt(pow(directionX,2) + pow(directionY,2) + pow(directionZ,2));
	  gluLookAt(cameraX, cameraY, cameraZ, lookATx, lookATy, lookATz, 0.0, 1.0, 0.0);
  }
  
  // Draw all objects
  //...

  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 2000);
  drawCube(wallMesh_L1);
  drawCube(wallMesh_B1);
  drawCube(wallMesh_R1);
  drawCube(wallMesh_R2);
  drawCube(wallMesh_F1);
  drawCube(wallMesh_F2);

  for (i = 0; i < 3; i++)
  {
	  drawCube(wallMesh_M1[i]);
	  drawCube(wallMesh_M2[i]);
  }

  glBindTexture(GL_TEXTURE_2D, 2004);
  drawCube(Door1);
  drawCube(Door2);

  // Draw floor and wall meshes
  glBindTexture(GL_TEXTURE_2D, 2001);
  floorMesh1->DrawMesh(meshSize);
  glBindTexture(GL_TEXTURE_2D, 2002);
  floorMesh2->DrawMesh(meshSize);
  glBindTexture(GL_TEXTURE_2D, 2003);
  floorMesh3->DrawMesh(meshSize);
  glDisable(GL_TEXTURE_2D);

  drawAvatar(Avatar);

  for (i = 0; i < countT; i++) {
	  if (Tables[i]->selected == 0)
	  {
		  glEnable(GL_TEXTURE_2D);
		  glEnable(GL_TEXTURE_GEN_S);
		  glEnable(GL_TEXTURE_GEN_T);
		  glBindTexture(GL_TEXTURE_2D, tex[0]);
		  drawTable(Tables[i]);
		  glDisable(GL_TEXTURE_GEN_S);
		  glDisable(GL_TEXTURE_GEN_T);
		  glDisable(GL_TEXTURE_2D);
	  } 
	  else drawTable(Tables[i]);
  }

  for (i = 0; i < countS; i++) {
	  if (Sofas[i]->selected == 0)
	  {
		  glEnable(GL_TEXTURE_2D);
		  glEnable(GL_TEXTURE_GEN_S);
		  glEnable(GL_TEXTURE_GEN_T);
		  glBindTexture(GL_TEXTURE_2D, tex[1]);
		  drawSofa(Sofas[i]);
		  glDisable(GL_TEXTURE_GEN_S);
		  glDisable(GL_TEXTURE_GEN_T);
		  glDisable(GL_TEXTURE_2D);
	  }
	  else drawSofa(Sofas[i]);
  }

  for (i = 0; i < countC; i++) {
	  if (Chairs[i]->selected == 0)
	  {
		  glEnable(GL_TEXTURE_2D);
		  glEnable(GL_TEXTURE_GEN_S);
		  glEnable(GL_TEXTURE_GEN_T);
		  glBindTexture(GL_TEXTURE_2D, tex[2]);
		  drawChair(Chairs[i]);
		  glDisable(GL_TEXTURE_GEN_S);
		  glDisable(GL_TEXTURE_GEN_T);
		  glDisable(GL_TEXTURE_2D);
	  }
	  else drawChair(Chairs[i]);
  }

  for (i = 0; i < countF; i++) {
	  if (Shelves[i]->selected == 0)
	  {

		  glEnable(GL_TEXTURE_2D);
		  glEnable(GL_TEXTURE_GEN_S);
		  glEnable(GL_TEXTURE_GEN_T);
		  glBindTexture(GL_TEXTURE_2D, tex[3]);
		  drawShelf(Shelves[i]);
		  glDisable(GL_TEXTURE_GEN_S);
		  glDisable(GL_TEXTURE_GEN_T);
		  glDisable(GL_TEXTURE_2D);
	  }
	  else drawShelf(Shelves[i]);;
  }

  glutSwapBuffers();
}


// Called at initialization and whenever user resizes the window */
void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.2,70.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

VECTOR3D pos = VECTOR3D(0,0,0);

// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
  currentButton = button;

  switch(button)
  {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN)
	{
	  ;
	  
	}
	break;
  case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN)
	{
	  ; 
	}
	break;
  default:
	break;
  }
  glutPostRedisplay();
}

// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	// Left mouse button to move the camera around
  if (currentButton == GLUT_LEFT_BUTTON && POVCAMERA == 0)
  {
	  if (xMouse > oldmouseX)
	  {
		  phi = phi + 3.0;
	  }
	  else if (xMouse < oldmouseX)
	  {
		  phi = phi - 3.0;
	  }
	  if (yMouse > oldmouseY)
	  {
		  theta = theta - 3.0;
		  if (theta < 10)
			  theta = 10;
	  }
	  else if (yMouse < oldmouseY)
	  {
		  theta = theta + 3.0;
		  if (theta > 90)
			  theta = 90;
	  }
  }

  // Right mouse button to zoom in or zoom out
  if (currentButton == GLUT_RIGHT_BUTTON && POVCAMERA == 0)
  {
	  if (yMouse > oldmouseY)
	  {
		  radius = radius - 2;
		  if (radius < 1)
			  radius = 1;
	  }
	  if (yMouse < oldmouseY)
	  {
		  radius = radius + 2;
		  if (radius > 50)
			  radius = 50;
	  }
  }

  if (currentButton == GLUT_LEFT_BUTTON && POVCAMERA == 1)
  {
	  if (xMouse > oldmouseX)
	  {
		  Avatar->angle_head = Avatar->angle_head - 1;
		  if (Avatar->angle_head < -90)
			  Avatar->angle_head = -90;
	  }
	  else if (xMouse < oldmouseX)
	  {
		  Avatar->angle_head = Avatar->angle_head + 1;
		  if (Avatar->angle_head > 90)
			  Avatar->angle_head = 90;
	  }
	  if (yMouse > oldmouseY)
	  {
		  theta_head = theta_head + 1;
		  if (theta_head > 120)
			  theta_head = 120;
	  }
	  else if (yMouse < oldmouseY)
	  {
		  theta_head = theta_head - 1;
		  if (theta_head < 10)
			  theta_head = 10;
	  }
  }
  oldmouseX = xMouse;
  oldmouseY = yMouse;
  glutPostRedisplay();
}


VECTOR3D ScreenToWorld(int x, int y)
{
	// you will need this if you use the mouse
	return VECTOR3D(0);
}// ScreenToWorld()

/* Handles input from the keyboard, non-arrow keys */
void keyboard(unsigned char key, int x, int y)
{
  switch (key) 
  {
  case 't':
      currentAction = TRANSLATE;
	  break;
  case 's':
	  currentAction = SCALE;
	  break;
  case 'r':
	  currentAction = ROTATE;
	  break;
  case 'e':
	  currentAction = EXTRUDE;
	  break;
  }
  glutPostRedisplay();
}

void functionKeys(int key, int x, int y)
{
  VECTOR3D min, max;
  VECTOR3D min2, max2;

  if (key == GLUT_KEY_F1)
  {
	  // select mode
	  selectmode = 1;
	  if (Avatar->tx > 0) // room 3
	  {
		  Sofas[2]->selected = 0;
		  Tables[1]->selected = 0;
		  Shelves[0]->selected = 0;
		  Shelves[1]->selected = 0;
		  Chairs[0]->selected = 0;
		  Chairs[1]->selected = 0;
		  Chairs[2]->selected = 0;
		  if (Sofas[0]->selected == 1)
		  {
			  Sofas[0]->selected = 0;
			  Sofas[1]->selected = 1;
		  }
		  else if (Sofas[1]->selected == 1)
		  {
			  Sofas[1]->selected = 0;
			  Tables[0]->selected = 1;
		  }
		  else if (Tables[0]->selected == 1)
		  {
			  Tables[0]->selected = 0;
			  Sofas[0]->selected = 1;
		  }
		  else Sofas[0]->selected = 1;
	  }
	  if (Avatar->tx < 0 && Avatar->tz < 0) //  room 2
	  {
		  Sofas[0]->selected = 0;
		  Sofas[1]->selected = 0;
		  Chairs[0]->selected = 0;
		  Chairs[1]->selected = 0;
		  Tables[0]->selected = 0;
		  Tables[1]->selected = 0;
		  Shelves[1]->selected = 0;
		  if (Sofas[2]->selected == 1)
		  {
			  Sofas[2]->selected = 0;
			  Shelves[0]->selected = 1;
		  }
		  else if (Shelves[0]->selected == 1)
		  {
			  Shelves[0]->selected = 0;
			  Chairs[2]->selected = 1;
		  }
		  else if (Chairs[2]->selected == 1)
		  {
			  Chairs[2]->selected = 0;
			  Sofas[2]->selected = 1;
		  }
		  else Sofas[2]->selected = 1;
	  }
	  if (Avatar->tx < 0 && Avatar->tz > 0) // room1
	  {
		  for (i = 0; i < 3; i++)
		  {
			  Sofas[i]->selected = 0;
		  }
		  Tables[0]->selected = 0;
		  Shelves[0]->selected = 0;
		  Chairs[2]->selected = 0;
		  if (Chairs[0]->selected == 1)
		  {
			  Chairs[0]->selected = 0;
			  Chairs[1]->selected = 1;
		  }
		  else if (Chairs[1]->selected == 1)
		  {
			  Chairs[1]->selected = 0;
			  Shelves[1]->selected = 1;
		  }
		  else if (Shelves[1]->selected == 1)
		  {
			  Shelves[1]->selected = 0;
			  Tables[1]->selected = 1;
		  }
		  else if (Tables[1]->selected == 1)
		  {
			  Tables[1]->selected = 0;
			  Chairs[0]->selected = 1;
		  }
		  else Chairs[0]->selected = 1;
	  }
  }
  else if (key == GLUT_KEY_F5)
  {
	  // door 1 open-close
	  glutIdleFunc(door1animate);
  }
  else if (key == GLUT_KEY_F6)
  {
	  // door 2 open-close
	  glutIdleFunc(door2animate);
  }
  else if (key == GLUT_KEY_F2)
  {
	  // deselect all
	  selectmode = 0;
	  for (i = 0; i < countS; i++)
	  {
		  Sofas[i]->selected = 0;
	  }
	  Tables[0]->selected = 0;
	  Tables[1]->selected = 0;
	  for (i = 0; i < countC; i++)
	  {
		  Chairs[i]->selected = 0;
	  }
	  Shelves[0]->selected = 0;
	  Shelves[1]->selected = 0;
  }
  else if (key == GLUT_KEY_F3)
  {
	  // bring head of the avatar to front position
	  Avatar->angle_head = 0;
	  theta_head = 90;
  }
  else if (key == GLUT_KEY_F4)
  {
	  // Change camera view
	  POVCAMERA = !POVCAMERA;
  }
  else if (key == GLUT_KEY_F7)
  {
	  penetrate = !penetrate;
  }
  else if (key == GLUT_KEY_DOWN)
  {
	  if (selectmode == 0 && Avatar->angle_head == 0 && theta_head == 90.0)
	  {
		  Avatar->tx = Avatar->tx - 0.5*directionX / direction;
		  Avatar->ty = Avatar->ty - 0.5*directionY / direction;
		  Avatar->tz = Avatar->tz - 0.5*directionZ / direction;
		  if (CheckCollisionAvatar())
		  {
			  Avatar->tx = Avatar->tx + 0.5*directionX / direction;
			  Avatar->ty = Avatar->ty + 0.5*directionY / direction;
			  Avatar->tz = Avatar->tz + 0.5*directionZ / direction;
		  }
	  }

	  switch (currentAction)
	  {
	  case TRANSLATE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
					  Tables[i]->tz = Tables[i]->tz + 0.1;
					  TableBBox(Tables[i], &min2, &max2);
					  if (CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1)
						  Tables[i]->tz = Tables[i]->tz - 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
					  Sofas[i]->tz = Sofas[i]->tz + 0.1;
					  SofaBBox(Sofas[i], &min2, &max2);
					  if (CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1)
						  Sofas[i]->tz = Sofas[i]->tz - 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
					  Chairs[i]->tz = Chairs[i]->tz + 0.1;
					  ChairBBox(Chairs[i], &min2, &max2);
					  if (CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)
						  Chairs[i]->tz = Chairs[i]->tz - 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->tz = Shelves[i]->tz + 0.1;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)
					  Shelves[i]->tz = Shelves[i]->tz - 0.1;
			  }
		  }
		  break;
	  case SCALE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->sfz = Tables[i]->sfz - 0.1;
				  TableBBox(Tables[i], &min2, &max2);
				  if ((CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1) || Tables[i]->sfz < 0.1)
					  Tables[i]->sfz = Tables[i]->sfz + 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->sfz = Sofas[i]->sfz - 0.1;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if ((CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1) || Sofas[i]->sfz < 0.1)
					  Sofas[i]->sfz = Sofas[i]->sfz + 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->sfz = Chairs[i]->sfz - 0.1;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if ((CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)|| Chairs[i]->sfz < 0.1)
					  Chairs[i]->sfz = Chairs[i]->sfz + 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->sfz = Shelves[i]->sfz - 0.1;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if ((CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)|| Shelves[i]->sfz < 0.1)
					  Shelves[i]->sfz = Shelves[i]->sfz + 0.1;
			  }
		  }
		  break;
	  case EXTRUDE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected && (Tables[i]->sfy > 0.1))
			  {
				  Tables[i]->sfy = Tables[i]->sfy - 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected && (Sofas[i]->sfy > 0.1))
			  {
				  Sofas[i]->sfy = Sofas[i]->sfy - 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected && (Chairs[i]->sfy > 0.1))
			  {
				  Chairs[i]->sfy = Chairs[i]->sfy - 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected && (Shelves[i]->sfy > 0.1))
			  {
				  Shelves[i]->sfy = Shelves[i]->sfy - 0.1;
			  }
		  }
		  break;
	  }
  }
  else if (key == GLUT_KEY_UP)
  {
	  if (selectmode == 0 && Avatar->angle_head == 0 && theta_head == 90.0)
	  {
		  Avatar->tx = Avatar->tx + 0.5*directionX / direction;
		  Avatar->ty = Avatar->ty + 0.5*directionY / direction;
		  Avatar->tz = Avatar->tz + 0.5*directionZ / direction;
		  if (CheckCollisionAvatar())
		  {
			  Avatar->tx = Avatar->tx - 0.5*directionX / direction;
			  Avatar->ty = Avatar->ty - 0.5*directionY / direction;
			  Avatar->tz = Avatar->tz - 0.5*directionZ / direction;
		  }
	  }
	  switch (currentAction)
	  {
	  case TRANSLATE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->tz = Tables[i]->tz - 0.1;
				  TableBBox(Tables[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1)
					  Tables[i]->tz = Tables[i]->tz + 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->tz = Sofas[i]->tz - 0.1;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1)
					  Sofas[i]->tz = Sofas[i]->tz + 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->tz = Chairs[i]->tz - 0.1;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)
					  Chairs[i]->tz = Chairs[i]->tz + 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->tz = Shelves[i]->tz - 0.1;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)
					  Shelves[i]->tz = Shelves[i]->tz + 0.1;
			  }
		  }
		  break;
	  case SCALE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->sfz = Tables[i]->sfz + 0.1;
				  TableBBox(Tables[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1)
					  Tables[i]->sfz = Tables[i]->sfz - 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->sfz = Sofas[i]->sfz + 0.1;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1)
					  Sofas[i]->sfz = Sofas[i]->sfz - 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->sfz = Chairs[i]->sfz + 0.1;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)
					  Chairs[i]->sfz = Chairs[i]->sfz - 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->sfz = Shelves[i]->sfz + 0.1;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)
					  Shelves[i]->sfz = Shelves[i]->sfz - 0.1;
			  }
		  }
		  break;
	  case EXTRUDE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected && (Tables[i]->sfy*1.4 < 5.9))
			  {
				  Tables[i]->sfy = Tables[i]->sfy + 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected && (Sofas[i]->sfy*3 < 5.9))
			  {
				  Sofas[i]->sfy = Sofas[i]->sfy + 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected && (Chairs[i]->sfy*3.4 < 5.9))
			  {
				  Chairs[i]->sfy = Chairs[i]->sfy + 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected && (Shelves[i]->sfy*3.4 < 5.9))
			  {
				  Shelves[i]->sfy = Shelves[i]->sfy + 0.1;
			  }
		  }
		  break;
	  }
  }
  else if (key == GLUT_KEY_RIGHT)
  {
	  if (selectmode == 0 && Avatar->angle_head == 0 && theta_head == 90.0)
	  {
		  Avatar->angle = Avatar->angle - 2;
	  }

	  switch (currentAction)
	  {
	  case TRANSLATE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->tx = Tables[i]->tx + 0.1;
				  TableBBox(Tables[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1)
					  Tables[i]->tx = Tables[i]->tx - 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->tx = Sofas[i]->tx + 0.1;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1)
					  Sofas[i]->tx = Sofas[i]->tx - 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->tx = Chairs[i]->tx + 0.1;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)
					  Chairs[i]->tx = Chairs[i]->tx - 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->tx = Shelves[i]->tx + 0.1;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)
					  Shelves[i]->tx = Shelves[i]->tx - 0.1;
			  }
		  }
		  break;
	  case SCALE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->sfx = Tables[i]->sfx + 0.1;
				  TableBBox(Tables[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1)
					  Tables[i]->sfx = Tables[i]->sfx - 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->sfx = Sofas[i]->sfx + 0.1;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1)
					  Sofas[i]->sfx = Sofas[i]->sfx - 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->sfx = Chairs[i]->sfx + 0.1;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)
					  Chairs[i]->sfx = Chairs[i]->sfx - 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->sfx = Shelves[i]->sfx + 0.1;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)
					  Shelves[i]->sfx = Shelves[i]->sfx - 0.1;
			  }
		  }
		  break;
	  case ROTATE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->angle = Tables[i]->angle - 2;
				  TableBBox(Tables[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1)
					  Tables[i]->angle = Tables[i]->angle + 2;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->angle = Sofas[i]->angle - 2;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1)
					  Sofas[i]->angle = Sofas[i]->angle + 2;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->angle = Chairs[i]->angle - 2;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)
					  Chairs[i]->angle = Chairs[i]->angle + 2;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->angle = Shelves[i]->angle - 2;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)
					  Shelves[i]->angle = Shelves[i]->angle + 2;
			  }
		  }
		  break;
	  }
  }
  else if (key == GLUT_KEY_LEFT)
  {
	  if (selectmode == 0 && Avatar->angle_head == 0 && theta_head == 90.0)
	  {
		  Avatar->angle = Avatar->angle + 1;
	  }
	  switch (currentAction)
	  {
	  case TRANSLATE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->tx = Tables[i]->tx - 0.1;
				  TableBBox(Tables[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1)
					  Tables[i]->tx = Tables[i]->tx + 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->tx = Sofas[i]->tx - 0.1;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1)
					  Sofas[i]->tx = Sofas[i]->tx + 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->tx = Chairs[i]->tx - 0.1;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)
					  Chairs[i]->tx = Chairs[i]->tx + 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->tx = Shelves[i]->tx - 0.1;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)
					  Shelves[i]->tx = Shelves[i]->tx + 0.1;
			  }
		  }
		  break;
	  case SCALE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->sfx = Tables[i]->sfx - 0.1;
				  TableBBox(Tables[i], &min2, &max2);
				  if ((CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1) || Tables[i]->sfx < 0.1)
					  Tables[i]->sfx = Tables[i]->sfx + 0.1;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->sfx = Sofas[i]->sfx - 0.1;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if ((CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1) || Sofas[i]->sfx < 0.1)
					  Sofas[i]->sfx = Sofas[i]->sfx + 0.1;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->sfx = Chairs[i]->sfx - 0.1;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if ((CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1) || Chairs[i]->sfx < 0.1)
					  Chairs[i]->sfx = Chairs[i]->sfx + 0.1;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->sfx = Shelves[i]->sfx - 0.1;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if ((CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1) || Shelves[i]->sfx < 0.1)
					  Shelves[i]->sfx = Shelves[i]->sfx + 0.1;
			  }
		  }
		  break;
	  case ROTATE:
		  for (i = 0; i < countT; i++)
		  {
			  if (Tables[i]->selected)
			  {
				  Tables[i]->angle = Tables[i]->angle + 2;
				  TableBBox(Tables[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Tables[i]->tx, Tables[i]->tz) == 1)
					  Tables[i]->angle = Tables[i]->angle - 2;
			  }
		  }
		  for (i = 0; i < countS; i++)
		  {
			  if (Sofas[i]->selected)
			  {
				  Sofas[i]->angle = Sofas[i]->angle + 2;
				  SofaBBox(Sofas[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Sofas[i]->tx, Sofas[i]->tz) == 1)
					  Sofas[i]->angle = Sofas[i]->angle - 2;
			  }
		  }
		  for (i = 0; i < countC; i++)
		  {
			  if (Chairs[i]->selected)
			  {
				  Chairs[i]->angle = Chairs[i]->angle + 2;
				  ChairBBox(Chairs[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Chairs[i]->tx, Chairs[i]->tz) == 1)
					  Chairs[i]->angle = Chairs[i]->angle - 2;
			  }
		  }
		  for (i = 0; i < countF; i++)
		  {
			  if (Shelves[i]->selected)
			  {
				  Shelves[i]->angle = Shelves[i]->angle + 2;
				  ShelfBBox(Shelves[i], &min2, &max2);
				  if (CheckCollisionObjects(min2, max2, Shelves[i]->tx, Shelves[i]->tz) == 1)
					  Shelves[i]->angle = Shelves[i]->angle - 2;
			  }
		  }
		  break;
	  }
  }
  glutPostRedisplay();
}

void door1animate()
{
	if (door1open)
	{
		if (Door1->tx < -6.5)
		{
			Door1->tx = Door1->tx + 0.05;
		}
		else
		{
			door1open = 0;
			glutIdleFunc(NULL);
		}
	}
	else {
		if (Door1->tx > -9.5)
		{
			Door1->tx = Door1->tx - 0.05;
		}
		else
		{
			door1open = 1;
			glutIdleFunc(NULL);
		}
	}
	glutPostRedisplay();
}

void door2animate()
{
	if (door2open)
	{
		if (Door2->tz < -6.5)
		{
			Door2->tz = Door2->tz + 0.05;
		}
		else
		{
			door2open = 0;
			glutIdleFunc(NULL);
		}
	}
	else {
		if (Door2->tz > -9.5)
		{
			Door2->tz = Door2->tz - 0.05;
		}
		else
		{
			door2open = 1;
			glutIdleFunc(NULL);
		}
	}
	glutPostRedisplay();
}

int CheckCollisionAvatar()
{
	VECTOR3D min, max;
	int j;
	AvatarBBox(Avatar);
	//printf("Avatar: xmin: %f, xmax=%f, zmin= %f, zmax=%f\n", Avatar->xmin, Avatar->xmax, Avatar->zmin, Avatar->zmax);
	getBBox(wallMesh_B1, &min, &max);
	//printf("1: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (Avatar->zmin < max.z)
		return 1;
	getBBox(wallMesh_L1, &min, &max);
	//printf("2: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (Avatar->xmin < max.x)
		return 1;
	getBBox(wallMesh_F1, &min, &max);
	//printf("3: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (Avatar->zmax > min.z && Avatar->tx > 0)
		return 1;
	getBBox(wallMesh_F2, &min, &max);
	//printf("4: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (Avatar->zmax > min.z && Avatar->tx < 0)
		return 1;
	getBBox(wallMesh_R1, &min, &max);
	//printf("5: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (Avatar->xmax > min.x && Avatar->tz < 0 && Avatar->tx < 0)
		return 1;
	getBBox(wallMesh_R2, &min, &max);
	//printf("6: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (Avatar->xmax > min.x && Avatar->tz > 0 && Avatar->tx < 0)
		return 1;
	getBBox(wallMesh_M1[0], &min, &max);
	//printf("7: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (Avatar->tz > 0 && Avatar->tx < 0 && ((Avatar->tx < -8) || (door1open == 0)))
	{
		if (Avatar->zmin < max.z)
			return 1;
	}
	else if (Avatar->tz < 0 && Avatar->tx < 0 && ((Avatar->tx < -8) || (door1open == 0)))
	{
		if (Avatar->zmax > min.z)
			return 1;
	}
	getBBox(wallMesh_M1[1], &min, &max);
	if (Avatar->tz > 0 && Avatar->tx < 0 && ((Avatar->tx > -5) || (door1open == 0)))
	{
		if (Avatar->zmin < max.z)
			return 1;
	}
	else if (Avatar->tz < 0 && Avatar->tx < 0 && ((Avatar->tx > -5) || (door1open == 0)))
	{
		if (Avatar->zmax > min.z)
			return 1;
	}
	getBBox(wallMesh_M2[0], &min, &max);
	//printf("8: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (Avatar->tz < 0 && ((Avatar->tz < -8) || (door2open == 0)) && Avatar->tx < 0)
	{
		if (Avatar->xmax > min.x)
			return 1;
	}
	else if (Avatar->tz < 0 && ((Avatar->tz < -8) || (door2open == 0)) && Avatar->tx > 0)
	{
		if (Avatar->xmin < max.x)
			return 1;
	}
	getBBox(wallMesh_M2[1], &min, &max);
	if (Avatar->tz < 0 && ((Avatar->tz > -5) || (door2open == 0)) && Avatar->tx < 0)
	{
		if (Avatar->xmax > min.x)
			return 1;
	}
	else if (Avatar->tz < 0 && ((Avatar->tz > -5) || (door2open == 0)) && Avatar->tx > 0)
	{
		if (Avatar->xmin < max.x)
			return 1;
	}
	if (penetrate == 0)
	{
		for (j = 0; j < countC; j++)
		{
			ChairBBox(Chairs[j], &min, &max);
			//printf("9: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
			if (Avatar->xmax > min.x && Avatar->xmin < min.x && Avatar->zmax > min.z && Avatar->zmin < min.z)
				return 1;
			if (max.x > Avatar->xmin && min.x < Avatar->xmin && max.z > Avatar->zmin && min.z < Avatar->zmin)
				return 1;
			if (Avatar->xmax > min.x && Avatar->xmin < min.x && max.z > Avatar->zmin && min.z < Avatar->zmin)
				return 1;
			if (max.x > Avatar->xmin && min.x < Avatar->xmin && Avatar->zmax > min.z && Avatar->zmin < min.z)
				return 1;
		}

		for (j = 0; j < countT; j++)
		{
			TableBBox(Tables[j], &min, &max);
			//printf("10: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
			if (Avatar->xmax > min.x && Avatar->xmin < min.x && Avatar->zmax > min.z && Avatar->zmin < min.z)
				return 1;
			if (max.x > Avatar->xmin && min.x < Avatar->xmin && max.z > Avatar->zmin && min.z < Avatar->zmin)
				return 1;
			if (Avatar->xmax > min.x && Avatar->xmin < min.x && max.z > Avatar->zmin && min.z < Avatar->zmin)
				return 1;
			if (max.x > Avatar->xmin && min.x < Avatar->xmin && Avatar->zmax > min.z && Avatar->zmin < min.z)
				return 1;
		}
		for (j = 0; j < countS; j++)
		{
			SofaBBox(Sofas[j], &min, &max);
			//printf("11: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
			if (Avatar->xmax > min.x && Avatar->xmin < min.x && Avatar->zmax > min.z && Avatar->zmin < min.z)
				return 1;
			if (max.x > Avatar->xmin && min.x < Avatar->xmin && max.z > Avatar->zmin && min.z < Avatar->zmin)
				return 1;
			if (Avatar->xmax > min.x && Avatar->xmin < min.x && max.z > Avatar->zmin && min.z < Avatar->zmin)
				return 1;
			if (max.x > Avatar->xmin && min.x < Avatar->xmin && Avatar->zmax > min.z && Avatar->zmin < min.z)
				return 1;
		}
		for (j = 0; j < countF; j++)
		{
			ShelfBBox(Shelves[j], &min, &max);
			//printf("12: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
			if (Avatar->xmax > min.x && Avatar->xmin < min.x && Avatar->zmax > min.z && Avatar->zmin < min.z)
				return 1;
			if (max.x > Avatar->xmin && min.x < Avatar->xmin && max.z > Avatar->zmin && min.z < Avatar->zmin)
				return 1;
			if (Avatar->xmax > min.x && Avatar->xmin < min.x && max.z > Avatar->zmin && min.z < Avatar->zmin)
				return 1;
			if (max.x > Avatar->xmin && min.x < Avatar->xmin && Avatar->zmax > min.z && Avatar->zmin < min.z)
				return 1;
		}
	}
	return 0;
}

int CheckCollisionObjects(VECTOR3D min2, VECTOR3D max2, GLfloat tx1, GLfloat tz1)
{
	int j;
	//printf("1: min2x=%f, max2x=%f, min2z=%f, max2z=%f\n", min2.x, max2.x, min2.z, max2.z);
	VECTOR3D min, max;

	getBBox(wallMesh_B1, &min, &max);
	//printf("1: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (min2.z < max.z)
		return 1;
	getBBox(wallMesh_L1, &min, &max);
	//printf("2: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (min2.x < max.x)
		return 1;
	getBBox(wallMesh_F1, &min, &max);
	//printf("3: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (max2.z > min.z && tx1 > 0)
		return 1;
	getBBox(wallMesh_F2, &min, &max);
	//printf("4: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (max2.z > min.z && tx1 < 0)
		return 1;
	getBBox(wallMesh_R1, &min, &max);
	//printf("5: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (max2.x > min.x && tz1 < 0 && tx1 < 0)
		return 1;
	getBBox(wallMesh_R2, &min, &max);
	//printf("6: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (max2.x > min.x && tz1 > 0 && tx1 < 0)
		return 1;
	getBBox(wallMesh_M1[0], &min, &max);
	//printf("7: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (tz1 > 0 && tx1 < 0 && ((tx1 < -8) || (door1open == 0)))
	{
		if (min2.z < max.z)
			return 1;
	}
	else if (tz1 < 0 && tx1 < 0 && ((tx1 < -8) || (door1open == 0)))
	{
		if (max2.z > min.z)
			return 1;
	}
	getBBox(wallMesh_M1[1], &min, &max);
	if (tz1 > 0 && tx1 < 0 && ((tx1 > -5) || (door1open == 0)))
	{
		if (min2.z < max.z)
			return 1;
	}
	else if (tz1 < 0 && tx1 < 0 && ((tx1 > -5) || (door1open == 0)))
	{
		if (max2.z > min.z)
			return 1;
	}
	getBBox(wallMesh_M2[0], &min, &max);
	//printf("8: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
	if (tz1 < 0 && ((tz1 < -8) || (door2open == 0)) && tx1 < 0)
	{
		if (max2.x > min.x)
			return 1;
	}
	else if (tz1 < 0 && ((tz1 < -8) || (door2open == 0)) && tx1 > 0)
	{
		if (min2.x < max.x)
			return 1;
	}
	getBBox(wallMesh_M2[1], &min, &max);
	if (tz1 < 0 && ((tz1 > -5) || (door2open == 0)) && tx1 < 0)
	{
		if (max2.x > min.x)
			return 1;
	}
	else if (tz1 < 0 && ((tz1 > -5) || (door2open == 0)) && tx1 > 0)
	{
		if (min2.x < max.x)
			return 1;
	}
	for (j = 0; j < countC; j++)
	{
		ChairBBox(Chairs[j], &min, &max);
		//printf("9: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
		if (max2.x > min.x && min2.x < min.x && max2.z > min.z && min2.z < min.z)
			return 1;
		if (max.x > min2.x && min.x < min2.x && max.z > min2.z && min.z < min2.z)
			return 1;
		if (max2.x > min.x && min2.x < min.x && max.z > min2.z && min.z < min2.z)
			return 1;
		if (max.x > min2.x && min.x < min2.x && max2.z > min.z && min2.z < min.z)
			return 1;
	}

	for (j = 0; j < countT; j++)
	{
		TableBBox(Tables[j], &min, &max);
		//printf("10: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
		if (max2.x > min.x && min2.x < min.x && max2.z > min.z && min2.z < min.z)
			return 1;
		if (max.x > min2.x && min.x < min2.x && max.z > min2.z && min.z < min2.z)
			return 1;
		if (max2.x > min.x && min2.x < min.x && max.z > min2.z && min.z < min2.z)
			return 1;
		if (max.x > min2.x && min.x < min2.x && max2.z > min.z && min2.z < min.z)
			return 1;
	}

	for (j = 0; j < countS; j++)
	{
		SofaBBox(Sofas[j], &min, &max);
		//printf("11: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
		if (max2.x > min.x && min2.x < min.x && max2.z > min.z && min2.z < min.z)
			return 1;
		if (max.x > min2.x && min.x < min2.x && max.z > min2.z && min.z < min2.z)
			return 1;
		if (max2.x > min.x && min2.x < min.x && max.z > min2.z && min.z < min2.z)
			return 1;
		if (max.x > min2.x && min.x < min2.x && max2.z > min.z && min2.z < min.z)
			return 1;
	}

	for (j = 0; j < countF; j++)
	{
		ShelfBBox(Shelves[j], &min, &max);
		//printf("12: minx=%f, maxx%f, minz=%f, maxz=%f\n", min.x, max.x, min.z, max.z);
		if (max2.x > min.x && min2.x < min.x && max2.z > min.z && min2.z < min.z)
			return 1;
		if (max.x > min2.x && min.x < min2.x && max.z > min2.z && min.z < min2.z)
			return 1;
		if (max2.x > min.x && min2.x < min.x && max.z > min2.z && min.z < min2.z)
			return 1;
		if (max.x > min2.x && min.x < min2.x && max2.z > min.z && min2.z < min.z)
			return 1;
	}
	if (penetrate == 0)
	{
		AvatarBBox(Avatar);
		if (Avatar->xmax > min2.x && Avatar->xmin < min2.x && Avatar->zmax > min2.z && Avatar->zmin < min2.z)
			return 1;
		if (max2.x > Avatar->xmin && min2.x < Avatar->xmin && max2.z > Avatar->zmin && min2.z < Avatar->zmin)
			return 1;
		if (Avatar->xmax > min2.x && Avatar->xmin < min2.x && max2.z > Avatar->zmin && min2.z < Avatar->zmin)
			return 1;
		if (max2.x > Avatar->xmin && min2.x < Avatar->xmin && Avatar->zmax > min2.z && Avatar->zmin < min2.z)
			return 1;
	}

	return 0;
}