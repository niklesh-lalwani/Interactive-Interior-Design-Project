#pragma once
#pragma once
typedef struct Avatar
{
	float tx, ty, tz;			// Translatation Deltas
	float sfx, sfy, sfz;		// Scale Factors
	float angle;				// Angle around y-axis of Avatar coordinate system
	float angle_head;
	bool selected;

	float xmin, xmax, zmin, zmax;

	// Material properties for drawing
	float mat_ambient[4];
	float mat_specular[4];
	float mat_diffuse[4];
	float mat_shininess[1];

	// Material properties if selected

	float nose_Mat_diffuse[4];

} Avatar;

Avatar *createAvatar()
{
	Avatar *newAvatar = (Avatar*)calloc(1, sizeof(Avatar));
	newAvatar->angle = 0.0;
	newAvatar->angle_head = 0.0;
	newAvatar->sfx = newAvatar->sfy = newAvatar->sfz = 1.0;
	newAvatar->tx = 0.0;
	newAvatar->ty = 0.0;
	newAvatar->tz = 0.0;

	newAvatar->mat_ambient[0] = 0.0;
	newAvatar->mat_ambient[1] = 0.05;
	newAvatar->mat_ambient[2] = 0.0;
	newAvatar->mat_ambient[3] = 1.0;
	newAvatar->mat_specular[0] = 0.0;
	newAvatar->mat_specular[1] = 0.0;
	newAvatar->mat_specular[2] = 0.004;
	newAvatar->mat_specular[3] = 1.0;
	newAvatar->mat_diffuse[0] = 1.0;
	newAvatar->mat_diffuse[1] = 1.0;
	newAvatar->mat_diffuse[2] = 0.0;
	newAvatar->mat_diffuse[3] = 1.0;
	newAvatar->mat_shininess[0] = 0;
	newAvatar->nose_Mat_diffuse[0] = 1.0;
	newAvatar->nose_Mat_diffuse[1] = 0.0;
	newAvatar->nose_Mat_diffuse[2] = 0.0;
	newAvatar->nose_Mat_diffuse[3] = 1.0;


	return newAvatar;
}

void AvatarBBox(Avatar* Avatar1)
{
	float temp;
	Avatar1->xmin = Avatar1->tx - 0.7;
	Avatar1->xmax = Avatar1->tx + 0.7;
	Avatar1->zmin = Avatar1->tz - 0.7;
	Avatar1->zmax = Avatar1->tz + 0.7;
}

void drawAvatar(Avatar* Avatar1)
{
		// Setup the material and lights used for the Avatar1
	glMaterialfv(GL_FRONT, GL_AMBIENT, Avatar1->mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Avatar1->mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Avatar1->mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, Avatar1->mat_shininess);

	GLUquadricObj* body;

	body = gluNewQuadric();

	AvatarBBox(Avatar1);

	glPushMatrix();
	// transform code
	glTranslatef(Avatar1->tx, Avatar1->ty, Avatar1->tz);
	glRotatef(Avatar1->angle, 0.0, 1.0, 0.0);
	glTranslatef(0.0f, Avatar1->sfy, 0.0);
	glScalef(Avatar1->sfx, Avatar1->sfy, Avatar1->sfz);

	// Draw Avatar
	// Draw cylinder
	glPushMatrix();
	glTranslatef(0.0f, 1.5f, 0.0f);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	gluCylinder(body, 0.3, 0.3, 2.5, 20, 20);
	glPopMatrix();

	glPushMatrix();

	glRotatef(Avatar1->angle_head, 0.0, 1.0, 0.0);
	//Draw Head
	glPushMatrix();
	glTranslatef(0.0f, 1.5f, 0.0f);
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix();

	// Draw eye/nose- to identify the front side
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Avatar1->nose_Mat_diffuse);
	glTranslatef(0.5, 1.5, 0.0);
	glutSolidSphere(0.2, 10, 10);
	glPopMatrix();

	glPopMatrix();

	// Draw point on body- to identify the front side
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Avatar1->nose_Mat_diffuse);
	glTranslatef(0.3, 0.5, 0.0);
	glutSolidSphere(0.2, 10, 10);
	glPopMatrix();

	glPopMatrix();
}