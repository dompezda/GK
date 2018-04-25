#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <iostream>

typedef GLfloat GLTVector2[2];      // Two component floating point vector
typedef GLfloat GLTVector3[3];      // Three component floating point vector
typedef GLfloat GLTVector4[4];      // Four component floating point vector
typedef GLfloat GLTMatrix[16];      // A column major 4x4 matrix of type GLfloat

									// Adds two vectors together
void gltAddVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult) {
	vResult[0] = vFirst[0] + vSecond[0];
	vResult[1] = vFirst[1] + vSecond[1];
	vResult[2] = vFirst[2] + vSecond[2];
}

// odejmowanie wektorow
void gltSubtractVectors(const GLTVector3 vFirst, const GLTVector3 vSecond, GLTVector3 vResult)
{
	vResult[0] = vFirst[0] - vSecond[0];
	vResult[1] = vFirst[1] - vSecond[1];
	vResult[2] = vFirst[2] - vSecond[2];
}

// skalowanie wektorw przez skalar
void gltScaleVector(GLTVector3 vVector, const GLfloat fScale)
{
	vVector[0] *= fScale; vVector[1] *= fScale; vVector[2] *= fScale;
}

// dlugosc wektora(kolo)
GLfloat gltGetVectorLengthSqrd(const GLTVector3 vVector)
{
	return (vVector[0] * vVector[0]) + (vVector[1] * vVector[1]) + (vVector[2] * vVector[2]);
}

// dlugosc wektora
GLfloat gltGetVectorLength(const GLTVector3 vVector)
{
	return (GLfloat)sqrt(gltGetVectorLengthSqrd(vVector));
}

// tworzenie wektora
void gltNormalizeVector(GLTVector3 vNormal)
{
	GLfloat fLength = 1.0f / gltGetVectorLength(vNormal);
	gltScaleVector(vNormal, fLength);
}

// kopowanie wektora
void gltCopyVector(const GLTVector3 vSource, GLTVector3 vDest)
{
	memcpy(vDest, vSource, sizeof(GLTVector3));
}

// porownywanie 2 wektorow
GLfloat gltVectorDotProduct(const GLTVector3 vU, const GLTVector3 vV)
{
	return vU[0] * vV[0] + vU[1] * vV[1] + vU[2] * vV[2];
}

// krzyzowanie wektorow
void gltVectorCrossProduct(const GLTVector3 vU, const GLTVector3 vV, GLTVector3 vResult)
{
	vResult[0] = vU[1] * vV[2] - vV[1] * vU[2];
	vResult[1] = -vU[0] * vV[2] + vV[0] * vU[2];
	vResult[2] = vU[0] * vV[1] - vV[0] * vU[1];
}



// tworzenie wektora normalnego
void gltGetNormalVector(const GLTVector3 vP1, const GLTVector3 vP2, const GLTVector3 vP3, GLTVector3 vNormal)
{
	GLTVector3 vV1, vV2;

	gltSubtractVectors(vP2, vP1, vV1);
	gltSubtractVectors(vP3, vP1, vV2);

	gltVectorCrossProduct(vV1, vV2, vNormal);
	gltNormalizeVector(vNormal);
}



// przemiana punktu na wektor 4x4
void gltTransformPoint(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vOut)
{
	vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] * vSrcVector[2] + mMatrix[12];
	vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] * vSrcVector[2] + mMatrix[13];
	vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2] + mMatrix[14];
}

// obracanie wektora za pomoca macierzy 4x4
void gltRotateVector(const GLTVector3 vSrcVector, const GLTMatrix mMatrix, GLTVector3 vOut)
{
	vOut[0] = mMatrix[0] * vSrcVector[0] + mMatrix[4] * vSrcVector[1] + mMatrix[8] * vSrcVector[2];
	vOut[1] = mMatrix[1] * vSrcVector[0] + mMatrix[5] * vSrcVector[1] + mMatrix[9] * vSrcVector[2];
	vOut[2] = mMatrix[2] * vSrcVector[0] + mMatrix[6] * vSrcVector[1] + mMatrix[10] * vSrcVector[2];
}


// zdobycie 3 punktow
void gltGetPlaneEquation(GLTVector3 vPoint1, GLTVector3 vPoint2, GLTVector3 vPoint3, GLTVector3 vPlane)
{
	// wektor normalny z 3 punktow 
	gltGetNormalVector(vPoint1, vPoint2, vPoint3, vPlane);

	
	vPlane[3] = -(vPlane[0] * vPoint3[0] + vPlane[1] * vPoint3[1] + vPlane[2] * vPoint3[2]);
}


GLfloat gltDistanceToPlane(GLTVector3 vPoint, GLTVector4 vPlane)
{
	return vPoint[0] * vPlane[0] + vPoint[1] * vPlane[1] + vPoint[2] * vPlane[2] + vPlane[3];
}
// Wielkoci obrotów
const int Sides = 12;

// Wielkoci obrotów
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
// Zmiana przestrzeni widocznej i okna.
// Wywolywana w momencie zmiany rozmiaru okna

void ChangeSize(int w, int h)
{
	GLfloat fAspect;

	// Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)

		h = 1;

	// Zrównanie wielkoœci widoku i okna
	glViewport(0, 0, w, h);
	fAspect = (GLfloat)w / (GLfloat)h;

	// Ustalenie uk³adu wspó³rzêdnych
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Utworzenie rzutowania perspektywicznego
	gluPerspective(35.0f, fAspect, 1.0, 40.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Ta funkcja wykonuje wszystkie konieczne inicjalizacje kontekstu renderowania.
// Tutaj, konfiguruje i inicjalizuje oœwietlenie sceny
void SetupRC()
{
	GLuint texture;
	int x, y;
	char pixels[256 * 256];
	GLint iWidth = 256;
	GLint iHeight = 256;
	GLint iComponents = GL_RGB;
	GLenum eFormat = {
		GL_RGBA
	};

	// Wartoci i wspólrzdne swiatla
	GLfloat whiteLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };
	GLfloat sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
	GLfloat lightPos[] = { -10.f, 5.0f, 5.0f, 1.0f };
	glEnable(GL_DEPTH_TEST); // Usuwanie ukrytych powierzchni
	glEnable(GL_CULL_FACE); // Nie bedziemy prowadzic obliczen wnetrza samolotu
	glFrontFace(GL_CCW); // Wieloktay z nawinieciem przeciwnym do ruchu wskazówek zegara
						 // Wlaczenie owietlenia
	glEnable(GL_LIGHTING);

	// Konfiguracja i wlaczenie swiatla numer 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
	// W³czenie ledzenia kolorów
	glEnable(GL_COLOR_MATERIAL);

	// Wlaciwoci owietlenia otoczenia i rozproszenia
	//  wartosci podawane funkcji glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Czarne tlo
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// ladowanie tekstury
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Generowanie tekstury
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	for (y = 0; y < 256; y++)
	{
		for (x = 0; x < 256; x++)
			pixels[y * 256 + x] = rand() % 256;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
}


// Reakcje na klawisze strzalek
void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)

		xRot -= 5.0f;

	if (key == GLUT_KEY_DOWN)

		xRot += 5.0f;

	if (key == GLUT_KEY_LEFT)

		yRot -= 5.0f;

	if (key == GLUT_KEY_RIGHT)

		yRot += 5.0f;

	xRot = (GLfloat)((const int)xRot % 360);

	yRot = (GLfloat)((const int)yRot % 360);

	// Odswiezenie zawartoci okna

	glutPostRedisplay();
}


void GenerateVerticles(GLTVector3 * vertices)
{
	for (auto i = 0; i < Sides; ++i)
	{
		vertices[i][0] = sinf(i / static_cast<float>(Sides) * 2 * M_PI) / 2;
		vertices[i][1] = 0;
		vertices[i][2] = cosf(i / static_cast<float>(Sides) * 2 * M_PI) / 2;
	}
	vertices[Sides][0] = 0.f;
	vertices[Sides][1] = 1.0f;
	vertices[Sides][2] = 0.f;
}

// Wywolywana w celu przerysowania sceny
void RenderScene(void)

{
	GLTVector3 vNormal;
	GLTVector3 vCorners[Sides + 1];
	GenerateVerticles(vCorners);
	//GLTVector3 vCorners[5] = { { 0.0f, .80f, 0.0f }, // Góra 0
	//{ -0.5f, 0.0f, -.50f }, // Lewy ty³ 1
	//{ 0.5f, 0.0f, -0.50f }, // Prawy ty³ 2
	//{ 0.5f, 0.0f, 0.5f }, // Prawy przód 3
	//{ -0.5f, 0.0f, 0.5f } }; // Lewy przód 4

	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Zapisanie stanu macierzy i wykonanie obrotów
	glPushMatrix();

	// Cofniecie obiektów
	glTranslatef(0.0f, -0.25f, -4.0f);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	// Rysowanie piramidy
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	// Podstawa piramidy
	glNormal3f(0.0f, -1.0f, 0.0f);
	for (int i = 0; i < Sides; ++i)
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i]);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(vCorners[(i + 1) % Sides]);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(vCorners[(i + Sides / 2) % Sides]);
	}


	for (int i = 0; i < Sides; ++i)
	{
		gltGetNormalVector(vCorners[Sides], vCorners[(i + 1) % Sides], vCorners[i], vNormal);
		glNormal3fv(vNormal);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3fv(vCorners[Sides]);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(vCorners[(i + 1) % Sides]);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(vCorners[i]);
	}
	glEnd();

	// Odtworzenie stanu macierzy
	glPopMatrix();
	// Zamiana buforów
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Textured Pyramid");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();
	return 0;
}