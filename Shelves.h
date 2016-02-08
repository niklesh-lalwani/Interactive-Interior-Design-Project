#pragma once
#pragma once
typedef struct Shelf
{
	float tx, ty, tz;			// Translatation Deltas
	float sfx, sfy, sfz;		// Scale Factors
	float angle;				// Angle around y-axis of Shelf coordinate system
	bool selected = 0;
	CubeMesh *side1 = NULL;
	CubeMesh *side2 = NULL;
	CubeMesh *back = NULL;
	CubeMesh *bottom = NULL;
	//CubeMesh *middleV = NULL;
	CubeMesh *middleH = NULL;

	float xmin, xmax, zmin, zmax;

	// Material properties for drawing
	float mat_ambient[4];
	float mat_specular[4];
	float mat_diffuse[4];
	float mat_shininess[1];

	// Material properties if selected
	float highlightMat_ambient[4];
	float highlightMat_specular[4];
	float highlightMat_diffuse[4];
	float highlightMat_shininess[1];

} Shelf;

Shelf *createShelf()
{
	Shelf *newShelf = (Shelf*)calloc(1, sizeof(Shelf));
	newShelf->angle = 0.0;
	newShelf->sfx = newShelf->sfy = newShelf->sfz = 1.0;
	newShelf->tx = 0.0;
	newShelf->ty = 0.0;
	newShelf->tz = 0.0;

	newShelf->mat_ambient[0] = 0.0;
	newShelf->mat_ambient[1] = 0.05;
	newShelf->mat_ambient[2] = 0.0;
	newShelf->mat_ambient[3] = 1.0;
	newShelf->mat_specular[0] = 0.0;
	newShelf->mat_specular[1] = 0.0;
	newShelf->mat_specular[2] = 0.004;
	newShelf->mat_specular[3] = 1.0;
	newShelf->mat_diffuse[0] = 0.5;
	newShelf->mat_diffuse[1] = 0.5;
	newShelf->mat_diffuse[2] = 0.5;
	newShelf->mat_diffuse[3] = 1.0;
	newShelf->mat_shininess[0] = 0;

	newShelf->highlightMat_ambient[0] = 0.0;
	newShelf->highlightMat_ambient[1] = 0.00;
	newShelf->highlightMat_ambient[2] = 0.0;
	newShelf->highlightMat_ambient[3] = 1.0;
	newShelf->highlightMat_specular[0] = 0.0;
	newShelf->highlightMat_specular[1] = 0.0;
	newShelf->highlightMat_specular[2] = 0.0;
	newShelf->highlightMat_specular[3] = 1.0;
	newShelf->highlightMat_diffuse[0] = 1.0;
	newShelf->highlightMat_diffuse[1] = 0.0;
	newShelf->highlightMat_diffuse[2] = 0.0;
	newShelf->highlightMat_diffuse[3] = 1.0;
	newShelf->highlightMat_shininess[0] = 0.0;

	newShelf->back = createCube();
	newShelf->bottom = createCube();
	newShelf->side1 = createCube();
	newShelf->side2 = createCube();
	newShelf->middleH = createCube();
	//newShelf->middleV = createCube();

	return newShelf;
}

void ShelfBBox(Shelf* Shelf1, VECTOR3D *min, VECTOR3D *max)
{
	float temp;

	min->x = Shelf1->tx - max(abs(Shelf1->sfx*1.0*cos(3.14*Shelf1->angle / 180)), abs(Shelf1->sfz*1.0*sin(3.14*Shelf1->angle / 180)));
	max->x = Shelf1->tx + max(abs(Shelf1->sfx*1.0*cos(3.14*Shelf1->angle / 180)), abs(Shelf1->sfz*1.0*sin(3.14*Shelf1->angle / 180)));

	min->z = Shelf1->tz - max(abs(Shelf1->sfx*1.0*sin(3.14*Shelf1->angle / 180)), abs(Shelf1->sfz*1.0*cos(3.14*Shelf1->angle / 180)));
	max->z = Shelf1->tz + max(abs(Shelf1->sfx*1.0*sin(3.14*Shelf1->angle / 180)), abs(Shelf1->sfz*1.0*cos(3.14*Shelf1->angle / 180)));
}

void drawShelf(Shelf* Shelf1)
{
	if (Shelf1->selected)
	{
		// Setup the material and lights used for the Shelf1
		glMaterialfv(GL_FRONT, GL_AMBIENT, Shelf1->highlightMat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Shelf1->highlightMat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Shelf1->highlightMat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, Shelf1->highlightMat_shininess);
	}
	else
	{
		// Setup the material and lights used for the Shelf1
		glMaterialfv(GL_FRONT, GL_AMBIENT, Shelf1->mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Shelf1->mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Shelf1->mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, Shelf1->mat_shininess);
	}

	//makeTextures(tex);
	//glBindTexture(GL_TEXTURE_2D, tex);

	GLUquadricObj* legs[4];
	int i;

	//ShelfBBox(Shelf1);
	for (i = 0; i < 4; i++)
	{
		legs[i] = gluNewQuadric();
	}

	glPushMatrix();
	// transform code
	glTranslatef(Shelf1->tx, Shelf1->ty, Shelf1->tz);
	glRotatef(Shelf1->angle, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, Shelf1->sfy, 0.0);
	glScalef(Shelf1->sfx, Shelf1->sfy, Shelf1->sfz);
	glTranslatef(0.0, -1.0, 0.0);

	glPushMatrix();
	glTranslatef(0.0, 1.0, 0.0);
	glScalef(1.0, 0.05, 1.0);
	drawCube(Shelf1->bottom);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 2.0, 1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(1.0, 0.05, 1.0);
	drawCube(Shelf1->back);
	glPopMatrix();
	// Draw side 1
	glPushMatrix();
	glTranslatef(-1.0, 2.0, -1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(0.05, 1.0, 1.0);
	drawCube(Shelf1->side1);
	glPopMatrix();
	// Draw side 2
	glPushMatrix();
	glTranslatef(1.0, 2.0, -1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(0.05, 1.0, 1.0);
	drawCube(Shelf1->side2);
	glPopMatrix();

/*	glPushMatrix();
	glTranslatef(0.0, 2.0, -1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(0.05, 1.0, 1.0);
	drawCube(Shelf1->middleV);
	glPopMatrix();*/

	glPushMatrix();
	glTranslatef(0.0, 2.0, 0.0);
	glScalef(1.0, 0.05, 1.0);
	drawCube(Shelf1->middleH);
	glPopMatrix();

	// Draw leg1
	glPushMatrix();
	glTranslatef(0.8f, 1.0f, 0.8f);
	glRotatef(90.0f, 1.0, 0.0, 0.0); // rotate by 90 degrees
	gluCylinder(legs[0], 0.1f, 0.1f, 1.0f, 30, 3);
	glPopMatrix();

	// Draw leg2
	glPushMatrix();
	glTranslatef(-0.8f, 1.0f, 0.8f);
	glRotatef(90.0f, 1.0, 0.0, 0.0); // rotate by 90 degrees
	gluCylinder(legs[1], 0.1f, 0.1f, 1.0f, 30, 3);
	glPopMatrix();

	// Draw leg3
	glPushMatrix();
	glTranslatef(0.8f, 1.0f, -0.8f);
	glRotatef(90.0f, 1.0, 0.0, 0.0); // rotate by 90 degrees
	gluCylinder(legs[2], 0.1f, 0.1f, 1.0f, 30, 3);
	glPopMatrix();

	// Draw leg4
	glPushMatrix();
	glTranslatef(-0.8f, 1.0f, -0.8f);
	glRotatef(90.0f, 1.0, 0.0, 0.0); // rotate by 90 degrees
	gluCylinder(legs[3], 0.1f, 0.1f, 1.0f, 30, 3);
	glPopMatrix();

	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}