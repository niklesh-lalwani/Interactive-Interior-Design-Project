#pragma once
#pragma once
typedef struct Chair
{
	float tx, ty, tz;			// Translatation Deltas
	float sfx, sfy, sfz;		// Scale Factors
	float angle;				// Angle around y-axis of Chair coordinate system
	bool selected = 0;
	CubeMesh *seat = NULL;
	CubeMesh *back = NULL;

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

} Chair;

Chair *createChair()
{
	Chair *newChair = (Chair*)calloc(1, sizeof(Chair));
	newChair->angle = 0.0;
	newChair->sfx = newChair->sfy = newChair->sfz = 1.0;
	newChair->tx = -4.0;
	newChair->ty = 0.0;
	newChair->tz = -4.0;

	newChair->mat_ambient[0] = 0.0;
	newChair->mat_ambient[1] = 0.05;
	newChair->mat_ambient[2] = 0.0;
	newChair->mat_ambient[3] = 1.0;
	newChair->mat_specular[0] = 0.0;
	newChair->mat_specular[1] = 0.0;
	newChair->mat_specular[2] = 0.004;
	newChair->mat_specular[3] = 1.0;
	newChair->mat_diffuse[0] = 0.5;
	newChair->mat_diffuse[1] = 0.5;
	newChair->mat_diffuse[2] = 0.5;
	newChair->mat_diffuse[3] = 1.0;
	newChair->mat_shininess[0] = 0;

	newChair->highlightMat_ambient[0] = 0.0;
	newChair->highlightMat_ambient[1] = 0.00;
	newChair->highlightMat_ambient[2] = 0.0;
	newChair->highlightMat_ambient[3] = 1.0;
	newChair->highlightMat_specular[0] = 0.0;
	newChair->highlightMat_specular[1] = 0.0;
	newChair->highlightMat_specular[2] = 0.0;
	newChair->highlightMat_specular[3] = 1.0;
	newChair->highlightMat_diffuse[0] = 1.0;
	newChair->highlightMat_diffuse[1] = 0.0;
	newChair->highlightMat_diffuse[2] = 0.0;
	newChair->highlightMat_diffuse[3] = 1.0;
	newChair->highlightMat_shininess[0] = 0.0;
	
	newChair->seat = createCube();
	newChair->back = createCube();

	return newChair;
}

void ChairBBox(Chair* Chair1, VECTOR3D *min, VECTOR3D *max)
{
	float temp;

	min->x = Chair1->tx - max(abs(Chair1->sfx*1.0*cos(3.14*Chair1->angle / 180)), abs(Chair1->sfz*1.0*sin(3.14*Chair1->angle / 180)));
	max->x = Chair1->tx + max(abs(Chair1->sfx*1.0*cos(3.14*Chair1->angle / 180)), abs(Chair1->sfz*1.0*sin(3.14*Chair1->angle / 180)));

	min->z = Chair1->tz - max(abs(Chair1->sfx*1.0*sin(3.14*Chair1->angle / 180)), abs(Chair1->sfz*1.0*cos(3.14*Chair1->angle / 180)));
	max->z = Chair1->tz + max(abs(Chair1->sfx*1.0*sin(3.14*Chair1->angle / 180)), abs(Chair1->sfz*1.0*cos(3.14*Chair1->angle / 180)));
}

void drawChair(Chair* Chair1)
{

	if (Chair1->selected)
	{
		// Setup the material and lights used for the Chair1
		glMaterialfv(GL_FRONT, GL_AMBIENT, Chair1->highlightMat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Chair1->highlightMat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Chair1->highlightMat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, Chair1->highlightMat_shininess);
	}
	else
	{
		// Setup the material and lights used for the Chair1
		glMaterialfv(GL_FRONT, GL_AMBIENT, Chair1->mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Chair1->mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Chair1->mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, Chair1->mat_shininess);
	}

	//makeTextures(tex);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, tex);

	GLUquadricObj* legs[4];
	int i;
	
	for (i = 0; i < 4; i++)
	{
		legs[i] = gluNewQuadric();
	}

	glPushMatrix();
	// transform code
	glTranslatef(Chair1->tx, Chair1->ty, Chair1->tz);
	glRotatef(Chair1->angle, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, Chair1->sfy, 0.0);
	glScalef(Chair1->sfx, Chair1->sfy, Chair1->sfz);

	glTranslatef(0.0, -1.0, 0.0);
	glPushMatrix();
	glTranslatef(0.0, 1.0, 0.0);
	glScalef(1.0, 0.1, 1.0);
	drawCube(Chair1->seat);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 2.3, 1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glScalef(1.0, 0.1, 1.2);
	drawCube(Chair1->back);
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
}