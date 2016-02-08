#pragma once

GLfloat textureMap1[64][64][3];
GLfloat textureMap2[64][64][3];
GLfloat textureMap3[64][64][3];
GLfloat textureMap4[64][64][3];
//GLuint tex[2];

void assignColor(GLfloat col[3], GLfloat r, GLfloat g, GLfloat b) {
	col[0] = r;
	col[1] = g;
	col[2] = b;
}

int isEven(int x) {
	if (x % 2 == 0)
		return 1;
	else
		return 0;
}

void makeTextureMap1()
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j< 64; j++) {
			if (isEven(i / 8 + j / 8))
				assignColor(textureMap1[i][j], 1.0, 0.0, 0.0);
			else
				assignColor(textureMap1[i][j], 0.0, 0.0, 0.0);
		}
}

void makeTextureMap2()
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j< 64; j++) {
			if ((i % 4 == 0) && (j % 4 == 0))
				assignColor(textureMap2[i][j], 0.0, 0.0, 0.0);
			else
				assignColor(textureMap2[i][j], 1.0, 1.0, 1.0);
		}
}

void makeTextureMap3()
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j< 64; j++) {
			if ((i + j) % 16 == 0)
				assignColor(textureMap3[i][j], 0.50, 0.20, 0.00);
			else
				assignColor(textureMap3[i][j], 0.0, 0.5, 0.5);
		}
}

void makeTextureMap4()
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j< 64; j++) {
			if ((i + j) % 16 == 0)
				assignColor(textureMap4[i][j], 0.0, 0.0, 0.0);
			else
				assignColor(textureMap4[i][j], 0.82, 0.41, 0.12);
		}
}

void makeTextures(GLuint tex[4])
{
	GLfloat planes[] = { 0.0, 0.0, 0.3, 0.0 };
	GLfloat planet[] = { 0.0, 0.3, 0.0, 0.0 };

	glGenTextures(4, tex);

	/************************************ */
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planes);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planet);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap1);

	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planes);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planet);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap2);

	glBindTexture(GL_TEXTURE_2D, tex[2]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planes);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planet);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap3);

	glBindTexture(GL_TEXTURE_2D, tex[3]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planes);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planet);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap4);
}


