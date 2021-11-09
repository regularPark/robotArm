#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include "..\include\GL\freeglut.h"
#include <vector>

// ������ ũ��
int Width = 800, Height = 800;

// ��� ������ ���� ������
int ManipulateMode = 0; // 1: ȸ��, 2: �̵�
int StartPt[2];
float Axis[3] = { 1.0, 0.0, 0.0 };
float Angle = 0.0;
float lowerAngle = 0.0;
float upperAngle = 0.0;
float firstFinger = 0.3;
float secondFinger = -0.3;

float RotMat[16] = { 1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1 };
float Zoom = -30.0;
float Pan[3] = { 0.0, 0.0, 0.0 };


// �ݹ� �Լ���
void Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void MouseWheel(int button, int dir, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Render();

// ����� ���� �Լ���
void InitOpenGL();
void GetSphereCoord(int x, int y, float *px, float *py, float *pz);
void RenderFloor();
void RenderRobot();
void drawCube(float sx, float sy, float sz);
void SetupViewVolume();
void SetupViewTransform();
void Sub(double out[3], double a[3], double b[3]);
void Add(double out[3], double a[3], double b[3]);
void Cross(double out[3], double a[3], double b[3]);
void drawLowerarm(float sx, float sy, float sz);
void drawUpperarm(float sx, float sy, float sz);
void timer(int value);
void drawFinger();


void Cross(double out[3], double a[3], double b[3])
{
	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];
}

void Sub(double out[3], double a[3], double b[3])
{
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
	out[2] = a[2] - b[2];
}

void Add(double out[3], double a[3], double b[3])
{
	out[0] = a[0] + b[0];
	out[1] = a[1] + b[1];
	out[2] = a[2] + b[2];
}

int main(int argc, char **argv)
{
	// GLUT �ʱ�ȭ(���� Į�����, RBGA, ���̹��� ���)
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// ������ ����
	glutInitWindowSize(Width, Height);
	glutCreateWindow("3DViewer");

	// OpenGL �ʱ�ȭ
	InitOpenGL();

	// �ݹ��Լ� ���
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(Motion); // ���콺 ��ư ������ ������ ��, �ڵ����� ȣ��Ǵ� �Լ�
	glutMouseWheelFunc(MouseWheel);
	glutDisplayFunc(Render);
	glutTimerFunc(1000 / 30, timer, 1);

	// �޽��� ���� ����
	glutMainLoop();
	return 0;
}

void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 30, timer, 1);
}


void InitOpenGL()
{
	// ���� �׽�Ʈ�� Ȱ��ȭ
	glEnable(GL_DEPTH_TEST);

	// ���� �� Ȱ��ȭ
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);

	GLfloat light0_position[] = { 0.0f, 0.0f, 5.0f, 1.0f };
	GLfloat light0_ambient[] = { 0.2, 0.2, 0.2 };
	GLfloat light0_diffuse[] = { 0.8, 0.8, 0.8 };
	GLfloat light0_specular[] = { 0.7, 0.7, 0.7 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);


}

void Render()
{
	// Clear color buffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up viewing volume
	SetupViewVolume();

	// Set up viewing transformation
	SetupViewTransform();

	glMatrixMode(GL_MODELVIEW);
	RenderFloor();
	RenderRobot();

	// Swap buffers for double buffering.
	glutSwapBuffers();
}

void RenderRobot()
{	
	drawCube(10.0f, 1.0f, 10.0f);
	drawLowerarm(1.0f, 6.0f, 1.0f);
	drawUpperarm(1.0f, 6.0f, 1.0f);
	drawFinger();
}

void Keyboard(unsigned char key, int x, int y)
{
	if (key == '1')
	{
		lowerAngle += 2;		
	}
	else if (key == '2')
	{
		lowerAngle -= 2;
	}
	else if (key == '3')
	{
		upperAngle += 2;
	}
	else if (key == '4')
	{
		upperAngle -= 2;
	}
	else if (key == '5')
	{
		if (firstFinger <= 0.4 && firstFinger >= 0)
		{
			firstFinger -= 0.01;
			secondFinger += 0.01;
		}
		else if (firstFinger <= 0.45 && firstFinger > 0.4)
		{
			firstFinger -= 0.01;
			secondFinger += 0.01;
		}
	}
	else if (key == '6')
	{
		if (firstFinger <= 0.4 && firstFinger >= 0)
		{
			firstFinger += 0.01;
			secondFinger -= 0.01;
		}
		else if (firstFinger >= -0.02 && firstFinger < 0)
		{
			firstFinger += 0.01;
			secondFinger -= 0.01;
		}
	}
	
}

void drawLowerarm(float sx, float sy, float sz)
{	
	float mat0_ambient[] = { 0.5, 0.5, 0.1 };	// (0.2, 0.2, 0.2) => (0.06, 0.0, 0.0)
	float mat0_diffuse[] = { 0.6, 0.5, 0.0 };	// (0.8, 0.8, 0.8) => (0.48, 0.48, 0.48) * cos(theta)
	float mat0_specular[] = { 0.4, 0.4, 0.3 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) ������ ���ݻ� ������ ��
	float mat0_shininess = 10;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat0_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat0_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat0_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat0_shininess);

	glPushMatrix();
	{			
		glRotatef(lowerAngle, 0.0, 1.0, 0.0);
		glTranslatef(0.0, 0.5 * sy, 0.0);
		glScalef(sx, sy, sz);
		glutSolidCube(0.8);
	}
	glPopMatrix();
}

void drawUpperarm(float sx, float sy, float sz)
{
	float mat0_ambient[] = { 0.3, 0.9, 0.2 };	// (0.2, 0.2, 0.2) => (0.06, 0.0, 0.0)
	float mat0_diffuse[] = { 0.2, 0.8, 0.1 };	// (0.8, 0.8, 0.8) => (0.48, 0.48, 0.48) * cos(theta)
	float mat0_specular[] = { 0.0, 0.0, 0.0 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) ������ ���ݻ� ������ ��
	float mat0_shininess = 0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat0_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat0_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat0_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat0_shininess);

	glTranslatef(0.0, sy, 0.0);
	glScalef(1, 1, 1);
	glutSolidSphere(0.8, 100, 360);
	glPushMatrix();
	{	
		float mat0_ambient[] = { 0.8, 0.5, 0.1 };	// (0.2, 0.2, 0.2) => (0.06, 0.0, 0.0)
		float mat0_diffuse[] = { 0.8, 0.2, 0.4 };	// (0.8, 0.8, 0.8) => (0.48, 0.48, 0.48) * cos(theta)
		float mat0_specular[] = { 0.9, 0.9, 0.9 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) ������ ���ݻ� ������ ��
		float mat0_shininess = 20;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat0_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat0_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat0_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, &mat0_shininess);

		glRotatef(lowerAngle, 0.0, 1.0, 0.0);		
		glRotatef(upperAngle, 0.0, 0.0, 1.0);
		glTranslatef(0.0, 3.0, 0.0);
		glScalef(sx, sy, sz);
		glutSolidCube(0.8);	
	}
	glPopMatrix();	
	

}

void drawFinger(void)
{
	GLfloat light0_position[] = { 0.0f, 0.0f, 5.0f, 1.0f };
	GLfloat light0_ambient[] = { 0.3, 0.2, 0.5 };
	GLfloat light0_diffuse[] = { 0.3, 0.3, 0.3 };
	GLfloat light0_specular[] = { 0.4, 0.4, 0.4 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

	glPushMatrix();
	{	
		float mat0_ambient[] = { 0.2, 0.7, 0.8 };	// (0.2, 0.2, 0.2) => (0.06, 0.0, 0.0)
		float mat0_diffuse[] = { 0.8, 0.8, 0.4 };	// (0.8, 0.8, 0.8) => (0.48, 0.48, 0.48) * cos(theta)
		float mat0_specular[] = { 0.4, 0.4, 0.4 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) ������ ���ݻ� ������ ��
		float mat0_shininess = 0;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat0_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat0_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat0_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, &mat0_shininess);

		glRotatef(lowerAngle, 0.0, 1.0, 0.0);
		glRotatef(upperAngle, 0.0, 0.0, 1.0);
		glTranslatef(firstFinger, 5.9, 0.0);
		glScalef(0.1, 1, 0.8);
		glutSolidCube(1.0);		
	}
	glPopMatrix();

	glPushMatrix();
	{
		glRotatef(lowerAngle, 0.0, 1.0, 0.0);
		glRotatef(upperAngle, 0.0, 0.0, 1.0);
		glTranslatef(secondFinger, 5.9, 0.0);
		glScalef(0.1, 1, 0.8);
		glutSolidCube(1.0);
	}
	glPopMatrix();

	
}

void drawCube(float sx, float sy, float sz)
{
	float mat0_ambient[] = { 0.4, 0.1, 0.6 };	// (0.2, 0.2, 0.2) => (0.06, 0.0, 0.0)
	float mat0_diffuse[] = { 0.1, 0.1, 1.0 };	// (0.8, 0.8, 0.8) => (0.48, 0.48, 0.48) * cos(theta)
	float mat0_specular[] = { 0.0, 0.0, 0.0 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) ������ ���ݻ� ������ ��
	float mat0_shininess = 1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat0_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat0_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat0_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat0_shininess);

	glPushMatrix();
	glTranslatef(0.0, sy * 0.5, 0.0);
	glScalef(sx, sy, sz);
	glutSolidCube(1.0);
	glPopMatrix();
}

void Reshape(int w, int h)
{
	// ����Ʈ ��ȯ
	glViewport(0, 0, w, h);
	Width = w;
	Height = h;
}

void SetupViewVolume()
{
	// ���� ���� ����
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)Width / (double)Height, 1.0, 10000.0);
}

void SetupViewTransform()
{
	// �� �� ����� ���� ��ķ� �ʱ�ȭ, M = I
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// �� ��/�ƿ��� ���� ��ȯ, M = I * T_zoom
	glTranslatef(0.0, 0.0, Zoom);

	// ���ο� ȸ���� ����, M = I * T_zoom * R_new
	glRotatef(Angle, Axis[0], Axis[1], Axis[2]);

	// ���� ȸ���� ����, M = I * T_zoom * R_new * R_old	//   R_n .... * R3 * R2 * R1
	glMultMatrixf(RotMat);

	// ȸ�� ��� ����, R_old = R_new * R_old
	glGetFloatv(GL_MODELVIEW_MATRIX, RotMat);
	RotMat[12] = RotMat[13] = RotMat[14] = 0.0;

	// �̵� ��ȯ�� ����, M = I * T_zoom * R_new * R_old * T_pan
	glTranslatef(Pan[0], Pan[1], Pan[2]);
}

void RenderFloor()
{
	glDisable(GL_LIGHTING);
	glColor3f(0.7f, 0.7f, 0.7f);
	for (int x = -10; x <= 10; x++)
	{
		if (x == 0)
			continue;
		glBegin(GL_LINES);
		glVertex3f((float)x, 0.0, -10.0f);
		glVertex3f((float)x, 0.0, 10.0f);
		glVertex3f(-10.0f, 0.0, (float)x);
		glVertex3f(10.0f, 0.0, (float)x);
		glEnd();
	}

	glLineWidth(2.0f);
	glColor3f(0.3f, 0.3f, 0.3f);
	glBegin(GL_LINES);
	glVertex3f(-10.0f, 0.0f, 0.0f);
	glVertex3f(10.0f, 0.0, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, -10.0f);
	glVertex3f(0.0f, 0.0, 10.0f);
	glEnd();
	glLineWidth(1.0f);
	glEnable(GL_LIGHTING);
}

void Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		StartPt[0] = x;
		StartPt[1] = y;
		if (button == GLUT_LEFT_BUTTON)
			ManipulateMode = 1;	// ȸ��

		if (button == GLUT_RIGHT_BUTTON)
			ManipulateMode = 2;	// �̵�
	}
	if (state == GLUT_UP)
	{
		ManipulateMode = 0;	// ����
		StartPt[0] = StartPt[1] = 0;
		Angle = 0.0;
	}
}

void Motion(int x, int y)
{
	// ȸ����� ȸ�� ���� ���
	if (ManipulateMode == 1)
	{
		// ���� �� ���� ��ǥ ���
		float px, py, pz, qx, qy, qz;
		GetSphereCoord(StartPt[0], StartPt[1], &px, &py, &pz);
		GetSphereCoord(x, y, &qx, &qy, &qz);

		// ȸ�� ��� ���� ���
		Axis[0] = py * qz - pz * qy;
		Axis[1] = pz * qx - px * qz;
		Axis[2] = px * qy - py * qx;
		Angle = 0.0;
		float len = Axis[0] * Axis[0] + Axis[1] * Axis[1] + Axis[2] * Axis[2];
		if (len > 0.0001) // ���� ���� �̻� ó��
			Angle = acos(px * qx + py * qy + pz * qz) * 180.0f / 3.141592f;
	}

	// �̵� ���� ���
	if (ManipulateMode == 2)
	{
		float dx = (float)(x - StartPt[0]) * 0.01f;
		float dy = (float)(StartPt[1] - y) * 0.01f;
		// ȸ�� ��� �� �����
		// R = 0 4 8   invR = 0 1 2
		//     1 5 9          4 5 6    
		//     2 6 10         8 9 10
		// invR * (dx, dy, 0)
		Pan[0] += RotMat[0] * dx + RotMat[1] * dy;
		Pan[1] += RotMat[4] * dx + RotMat[5] * dy;
		Pan[2] += RotMat[8] * dx + RotMat[9] * dy;
	}

	StartPt[0] = x;	// Update startpt as current position
	StartPt[1] = y;
	glutPostRedisplay();
}

/*!
*	\brief ���콺 ��ũ���� ó���ϴ� �ݹ� �Լ�
*
*	\param button[in]	���콺 ��ư ����(GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON)
*	\param dir[in]		��ũ���� ����
*	\param x[in]		���� ����� (0, 0) �������� ���콺 �������� ���� ��ġ
*	\param y[in]		���� ����� (0, 0) �������� ���콺 �������� ���� ��ġ
*/
void MouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
		Zoom += 1.0;
	else
		Zoom -= 1.0;
	glutPostRedisplay();
}

void GetSphereCoord(int x, int y, float *px, float *py, float *pz)
{
	*px = (2.0f * x - Width) / Width;
	*py = (-2.0f * y + Height) / Height;

	float r = (*px) * (*px) + (*py) * (*py); // �������κ����� �Ÿ� ���
	if (r >= 1.0f)
	{
		*px = *px / sqrt(r);
		*py = *py / sqrt(r);
		*pz = 0.0f;
	}
	else
		*pz = sqrt(1.0f - r);  // �Ϲ����� ���
}
