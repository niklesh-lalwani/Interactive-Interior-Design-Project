#pragma once
typedef struct Sofa
{
	float tx, ty, tz;			// Translatation Deltas
	float sfx, sfy, sfz;		// Scale Factors
	float angle;				// Angle around y-axis of Sofa coordinate system
	bool selected = 0;
	CubeMesh *seat = NULL;
	CubeMesh *back = NULL;
	CubeMesh *side1 = NULL;
	CubeMesh *side2 = NULL;

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

} Sofa;

Sofa *createSofa()
{
	Sofa *newSofa = (Sofa*)calloc(1, sizeof(Sofa));
	newSofa->angle = 0.0;
	newSofa->sfx = newSofa->sfy = newSofa->sfz = 1.0;
	newSofa->tx = 0.0;
	newSofa->ty = 0.0;
	newSofa->tz = 0.0;

	newSofa->mat_ambient[0] = 0.0;
	newSofa->mat_ambient[1] = 0.05;
	newSofa->mat_ambient[2] = 0.0;
	newSofa->mat_ambient[3] = 1.0;
	newSofa->mat_specular[0] = 0.0;
	newSofa->mat_specular[1] = 0.0;
	newSofa->mat_specular[2] = 0.004;
	newSofa->mat_specular[3] = 1.0;
	newSofa->mat_diffuse[0] = 0.5;
	newSofa->mat_diffuse[1] = 0.5;
	newSofa->mat_diffuse[2] = 0.5;
	newSofa->mat_diffuse[3] = 1.0;
	newSofa->mat_shininess[0] = 0;

	newSofa->highlightMat_ambient[0] = 0.0;
	newSofa->highlightMat_ambient[1] = 0.00;
	newSofa->highlightMat_ambient[2] = 0.0;
	newSofa->highlightMat_ambient[3] = 1.0;
	newSofa->highlightMat_specular[0] = 0.0;
	newSofa->highlightMat_specular[1] = 0.0;
	newSofa->highlightMat_specular[2] = 0.0;
	newSofa->highlightMat_specular[3] = 1.0;
	newSofa->highlightMat_diffuse[0] = 1.0;
	newSofa->highlightMat_diffuse[1] = 0.0;
	newSofa->highlightMat_diffuse[2] = 0.0;
	newSofa->highlightMat_diffuse[3] = 1.0;
	newSofa->highlightMat_shininess[0] = 0.0;

	newSofa->seat = createCube();
	newSofa->back = createCube();
	newSofa->side1 = createCube();
	newSofa->side2 = createCube();

	return newSofa;
}

void SofaBBox(Sofa* Sofa1, VECTOR3D *min, VECTOR3D *max)
{
	float temp;

	min->x = Sofa1->tx - max(abs(Sofa1->sfx*2.0*cos(3.14*Sofa1->angle / 180)), abs(Sofa1->sfz*1.2*sin(3.14*Sofa1->angle / 180)));
	max->x = Sofa1->tx + max(abs(Sofa1->sfx*2.0*cos(3.14*Sofa1->angle / 180)), abs(Sofa1->sfz*1.2*sin(3.14*Sofa1->angle / 180)));

	min->z = Sofa1->tz - max(abs(Sofa1->sfx*2.0*sin(3.14*Sofa1->angle / 180)), abs(Sofa1->sfz*1.2*cos(3.14*Sofa1->angle / 180)));
	max->z = Sofa1->tz + max(abs(Sofa1->sfx*2.0*sin(3.14*Sofa1->angle / 180)), abs(Sofa1->sfz*1.2*cos(3.14*Sofa1->angle / 180)));

}

void drawSofa(Sofa* Sofa1)
{
	if (Sofa1->selected)
	{
		// Setup the material and lights used for the Sofa1
		glMaterialfv(GL_FRONT, GL_AMBIENT, Sofa1->highlightMat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Sofa1->highlightMat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Sofa1->highlightMat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, Sofa1->highlightMat_shininess);
	}
	else
	{
		// Setup the material and lights used for the Sofa1
		glMaterialfv(GL_FRONT, GL_AMBIENT, Sofa1->mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Sofa1->mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Sofa1->mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, Sofa1->mat_shininess);
	}

	//makeTextures(tex);
	//glBindTexture(GL_TEXTURE_2D, tex);

	GLUquadricObj* legs[4];
	int i;

	//SofaBBox(Sofa1);
	for (i = 0; i < 4; i++)
	{
		legs[i] = gluNewQuadric();
	}

	glPushMatrix();
	// transform code
	glTranslatef(Sofa1->tx, Sofa1->ty, Sofa1->tz);
	glRotatef(Sofa1->angle, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, Sofa1->sfy, 0.0);
	glScalef(Sofa1->sfx, Sofa1->sfy, Sofa1->sfz);

	glTranslatef(0.0, -1.0, 0.0);
	glPushMatrix();
	glTranslatef(0.0, 1.0, 0.0);
	glScalef(2.0, 0.2, 1.2);
	drawCube(Sofa1->seat);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 2.3, 1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(2.0, 0.1, 1.0);
	drawCube(Sofa1->back);
	glPopMatrix();
	// Draw side 1
	glPushMatrix();
	glTranslatef(-1.8, 1.8, -1.2);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(0.2, 1.2, 0.5);
	drawCube(Sofa1->side1);
	glPopMatrix();
	// Draw side 2
	glPushMatrix();
	glTranslatef(1.8, 1.8, -1.2);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(0.2, 1.2, 0.5);
	drawCube(Sofa1->side2);
	glPopMatrix();

	// Draw leg1
	glPushMatrix();
	glTranslatef(1.8f, 1.0f, 0.8f);
	glRotatef(90.0f, 1.0, 0.0, 0.0); // rotate by 90 degrees
	gluCylinder(legs[0], 0.1f, 0.1f, 1.0f, 30, 3);
	glPopMatrix();

	// Draw leg2
	glPushMatrix();
	glTranslatef(-1.8f, 1.0f, 0.8f);
	glRotatef(90.0f, 1.0, 0.0, 0.0); // rotate by 90 degrees
	gluCylinder(legs[1], 0.1f, 0.1f, 1.0f, 30, 3);
	glPopMatrix();

	// Draw leg3
	glPushMatrix();
	glTranslatef(1.8f, 1.0f, -0.8f);
	glRotatef(90.0f, 1.0, 0.0, 0.0); // rotate by 90 degrees
	gluCylinder(legs[2], 0.1f, 0.1f, 1.0f, 30, 3);
	glPopMatrix();

	// Draw leg4
	glPushMatrix();
	glTranslatef(-1.8f, 1.0f, -0.8f);
	glRotatef(90.0f, 1.0, 0.0, 0.0); // rotate by 90 degrees
	gluCylinder(legs[3], 0.1f, 0.1f, 1.0f, 30, 3);
	glPopMatrix();

	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
}