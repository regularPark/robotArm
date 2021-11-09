#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include "..\include\GL\freeglut.h"
#include <vector>

// 윈도우 크기
int Width = 800, Height = 800;

// 장면 조작을 위한 변수들
int ManipulateMode = 0; // 1: 회전, 2: 이동
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


// 콜백 함수들
void Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void MouseWheel(int button, int dir, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Render();

// 사용자 정의 함수들
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
	// GLUT 초기화(더블 칼라버퍼, RBGA, 깊이버퍼 사용)
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// 윈도우 생성
	glutInitWindowSize(Width, Height);
	glutCreateWindow("3DViewer");

	// OpenGL 초기화
	InitOpenGL();

	// 콜백함수 등록
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(Motion); // 마우스 버튼 누리고 움직일 때, 자동으로 호출되는 함수
	glutMouseWheelFunc(MouseWheel);
	glutDisplayFunc(Render);
	glutTimerFunc(1000 / 30, timer, 1);

	// 메시지 루프 진입
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
	// 깊이 테스트를 활성화
	glEnable(GL_DEPTH_TEST);

	// 조명 모델 활성화
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
	float mat0_specular[] = { 0.4, 0.4, 0.3 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) 시점과 전반사 사이의 각
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
	float mat0_specular[] = { 0.0, 0.0, 0.0 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) 시점과 전반사 사이의 각
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
		float mat0_specular[] = { 0.9, 0.9, 0.9 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) 시점과 전반사 사이의 각
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
		float mat0_specular[] = { 0.4, 0.4, 0.4 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) 시점과 전반사 사이의 각
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
	float mat0_specular[] = { 0.0, 0.0, 0.0 };	// (0.7, 0.7, 0.7) => (0.63, 0.63, 0.63) * cos(phi) 시점과 전반사 사이의 각
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
	// 뷰포트 변환
	glViewport(0, 0, w, h);
	Width = w;
	Height = h;
}

void SetupViewVolume()
{
	// 관측 공간 지정
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)Width / (double)Height, 1.0, 10000.0);
}

void SetupViewTransform()
{
	// 모델 뷰 행렬을 단위 행렬로 초기화, M = I
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 줌 인/아웃을 위한 변환, M = I * T_zoom
	glTranslatef(0.0, 0.0, Zoom);

	// 새로운 회전을 적용, M = I * T_zoom * R_new
	glRotatef(Angle, Axis[0], Axis[1], Axis[2]);

	// 기존 회전을 적용, M = I * T_zoom * R_new * R_old	//   R_n .... * R3 * R2 * R1
	glMultMatrixf(RotMat);

	// 회전 행렬 추출, R_old = R_new * R_old
	glGetFloatv(GL_MODELVIEW_MATRIX, RotMat);
	RotMat[12] = RotMat[13] = RotMat[14] = 0.0;

	// 이동 변환을 적용, M = I * T_zoom * R_new * R_old * T_pan
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
			ManipulateMode = 1;	// 회전

		if (button == GLUT_RIGHT_BUTTON)
			ManipulateMode = 2;	// 이동
	}
	if (state == GLUT_UP)
	{
		ManipulateMode = 0;	// 리셋
		StartPt[0] = StartPt[1] = 0;
		Angle = 0.0;
	}
}

void Motion(int x, int y)
{
	// 회전축과 회전 각도 계산
	if (ManipulateMode == 1)
	{
		// 단위 구 위의 좌표 계산
		float px, py, pz, qx, qy, qz;
		GetSphereCoord(StartPt[0], StartPt[1], &px, &py, &pz);
		GetSphereCoord(x, y, &qx, &qy, &qz);

		// 회전 축과 각도 계산
		Axis[0] = py * qz - pz * qy;
		Axis[1] = pz * qx - px * qz;
		Axis[2] = px * qy - py * qx;
		Angle = 0.0;
		float len = Axis[0] * Axis[0] + Axis[1] * Axis[1] + Axis[2] * Axis[2];
		if (len > 0.0001) // 일정 변위 이상만 처리
			Angle = acos(px * qx + py * qy + pz * qz) * 180.0f / 3.141592f;
	}

	// 이동 변위 계산
	if (ManipulateMode == 2)
	{
		float dx = (float)(x - StartPt[0]) * 0.01f;
		float dy = (float)(StartPt[1] - y) * 0.01f;
		// 회전 행렬 및 역행렬
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
*	\brief 마우스 스크롤을 처리하는 콜백 함수
*
*	\param button[in]	마우스 버튼 정보(GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON)
*	\param dir[in]		스크롤의 방향
*	\param x[in]		좌측 상단을 (0, 0) 기준으로 마우스 포인터의 가로 위치
*	\param y[in]		좌측 상단을 (0, 0) 기준으로 마우스 포인터의 세로 위치
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

	float r = (*px) * (*px) + (*py) * (*py); // 원점으로부터의 거리 계산
	if (r >= 1.0f)
	{
		*px = *px / sqrt(r);
		*py = *py / sqrt(r);
		*pz = 0.0f;
	}
	else
		*pz = sqrt(1.0f - r);  // 일반적인 경우
}
