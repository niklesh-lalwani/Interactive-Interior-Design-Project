#pragma once

typedef struct Table
{
	float tx, ty, tz;			// Translatation Deltas
	float sfx, sfy, sfz;		// Scale Factors
	float angle;				// Angle around y-axis of Table coordinate system
	bool selected = 0;

	CubeMesh *table_top = NULL;

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

} Table;

Table *createTable()
{
	Table *newTable = (Table*)calloc(1, sizeof(Table));
	newTable->angle = 0.0;
	newTable->sfx = newTable->sfy = newTable->sfz = 1.0;
	newTable->tx = 0.0;
	newTable->ty = 0.0;
	newTable->tz = 0.0;

	newTable->mat_ambient[0] = 0.0;
	newTable->mat_ambient[1] = 0.05;
	newTable->mat_ambient[2] = 0.0;
	newTable->mat_ambient[3] = 1.0;
	newTable->mat_specular[0] = 0.0;
	newTable->mat_specular[1] = 0.0;
	newTable->mat_specular[2] = 0.004;
	newTable->mat_specular[3] = 1.0;
	newTable->mat_diffuse[0] = 0.5;
	newTable->mat_diffuse[1] = 0.5;
	newTable->mat_diffuse[2] = 0.5;
	newTable->mat_diffuse[3] = 1.0;
	newTable->mat_shininess[0] = 0;

	newTable->highlightMat_ambient[0] = 0.0;
	newTable->highlightMat_ambient[1] = 0.00;
	newTable->highlightMat_ambient[2] = 0.0;
	newTable->highlightMat_ambient[3] = 1.0;
	newTable->highlightMat_specular[0] = 0.0;
	newTable->highlightMat_specular[1] = 0.0;
	newTable->highlightMat_specular[2] = 0.0;
	newTable->highlightMat_specular[3] = 1.0;
	newTable->highlightMat_diffuse[0] = 1.0;
	newTable->highlightMat_diffuse[1] = 0.0;
	newTable->highlightMat_diffuse[2] = 0.0;
	newTable->highlightMat_diffuse[3] = 1.0;
	newTable->highlightMat_shininess[0] = 0.0;

	newTable->table_top = createCube();
	newTable->table_top->sfx = 2.0;
	newTable->table_top->sfy = 0.2;

	return newTable;
}

void TableBBox(Table* Table1, VECTOR3D *min, VECTOR3D *max)
{
	float temp;
	min->x = Table1->tx - max(abs(Table1->sfx*2.0*cos(3.14*Table1->angle / 180)), abs(Table1->sfz*1.0*sin(3.14*Table1->angle / 180)));
	max->x = Table1->tx + max(abs(Table1->sfx*2.0*cos(3.14*Table1->angle / 180)), abs(Table1->sfz*1.0*sin(3.14*Table1->angle / 180)));

	min->z = Table1->tz - max(abs(Table1->sfx*2.0*sin(3.14*Table1->angle / 180)), abs(Table1->sfz*1.0*cos(3.14*Table1->angle / 180)));
	max->z = Table1->tz + max(abs(Table1->sfx*2.0*sin(3.14*Table1->angle / 180)), abs(Table1->sfz*1.0*cos(3.14*Table1->angle / 180)));

}

void drawTable(Table* Table1)
{
	if (Table1->selected)
	{
		// Setup the material and lights used for the Table1
		glMaterialfv(GL_FRONT, GL_AMBIENT, Table1->highlightMat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Table1->highlightMat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Table1->highlightMat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, Table1->highlightMat_shininess);
	}
	else
	{
		// Setup the material and lights used for the Table1
		glMaterialfv(GL_FRONT, GL_AMBIENT, Table1->mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, Table1->mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Table1->mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, Table1->mat_shininess);
	}

	GLUquadricObj* legs[4];
	int i;

	for (i = 0; i < 4; i++)
	{
		legs[i] = gluNewQuadric();
	}

	//TableBBox(Table1);

	glPushMatrix();
	// transform code
	glTranslatef(Table1->tx, Table1->ty, Table1->tz);
	glRotatef(Table1->angle, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, Table1->sfy, 0.0f);
	glScalef(Table1->sfx, Table1->sfy, Table1->sfz);

	drawCube(Table1->table_top);
	glTranslatef(0.0f, -1.0, 0.0f);
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
}