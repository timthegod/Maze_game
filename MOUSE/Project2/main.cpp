#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>    // std::min
#include <dos.h>
#include <conio.h>
#include <iostream>
#include <climits>
#include <time.h>



#define n 4
#define N 16

GLfloat v[4][2] = { { -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, -8.0 },{ -8.0, -8.0 } };
// initial color intdex 0:road, 1:mouse, 2:wall, 3:entrance, 4:exit
float colorArray[5][3] = { { 0.7098, 0.7647, 0.9647 },{ 0, 0.1254, 0.9411 },{ 0.3137, 0.3255, 0.4078 },{ 1.0, 00, 0 },{ 0, 0, 1.0 } };
float *colorMap[N][N] = { {} };

int Maze[N][N] = { {} };

typedef struct {
	int x;
	int y;
} Point;

Point pt(int x, int y) {
	Point p = { x, y };
	return p;
}
/*開始拜訪迷宮*/
int visit(int maze[][N], Point start, Point end)
{
	if (!maze[start.x][start.y])
	{
		maze[start.x][start.y] = 1;
		colorMap[start.x][start.y] = colorArray[1];
		if (!maze[end.x][end.y] &&
			!(visit(maze, pt(start.x, start.y + 1), end) ||
				visit(maze, pt(start.x + 1, start.y), end) ||
				visit(maze, pt(start.x, start.y - 1), end) ||
				visit(maze, pt(start.x - 1, start.y), end)))
		{
			maze[start.x][start.y] = 0;
		}
	}
	return maze[end.x][end.y];
}
void print(int maze[][N]) {
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) switch (maze[i][j]) {
		case 0: printf("  "); break;
		case 1: printf("◇"); break;
		case 2: printf("█");
		}
		printf("\n");
	}
}

void line(GLfloat *a, GLfloat *b)
{
	glVertex2fv(a);
	glVertex2fv(b);
}

void findRowCol(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int *Row, int *Col)
{
	*Row = (int)std::max(*Row, (int)(a[1]));
	*Row = (int)std::max(*Row, (int)(b[1]));
	*Row = (int)std::max(*Row, (int)(c[1]));        // find this point  =>|-----|
	*Row = (int)std::max(*Row, (int)(d[1]));        //                    |     |
	*Col = (int)std::min(*Col, (int)(a[0]));        //                    |-----|
	*Col = (int)std::min(*Col, (int)(b[0]));
	*Col = (int)std::min(*Col, (int)(c[0]));
	*Col = (int)std::min(*Col, (int)(d[0]));

	*Row = -(*Row) + 8;
	*Col += 8;
}

void square(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d)
{
	int row = INT_MIN, col = INT_MAX;
	findRowCol(a, b, c, d, &row, &col);
	printf("%d %d\n", row, col);
	glColor3f(colorMap[row][col][0], colorMap[row][col][1], colorMap[row][col][2]);
	glVertex2fv(a);
	glVertex2fv(b);
	glVertex2fv(c);
	glVertex2fv(d);
}

void divide_square(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int m)
{
	GLfloat v0[2], v1[2], v2[2], v3[2], CENTER[2];
	int j;
	if (m > 0)
	{                                                           //  a---v0---b
		for (j = 0; j<2; j++) v0[j] = (a[j] + b[j]) / 2;        //  |        |
		for (j = 0; j<2; j++) v1[j] = (b[j] + c[j]) / 2;        // v3 CENTER v1
		for (j = 0; j<2; j++) v2[j] = (c[j] + d[j]) / 2;        //  |        |
		for (j = 0; j<2; j++) v3[j] = (d[j] + a[j]) / 2;        //  d---v2---c
		CENTER[0] = (v3[0] + v1[0]) / 2;
		CENTER[1] = (v0[1] + v2[1]) / 2;

		divide_square(a, v0, CENTER, v3, m - 1);
		divide_square(v0, b, v1, CENTER, m - 1);
		divide_square(CENTER, v1, c, v2, m - 1);
		divide_square(v3, CENTER, v2, d, m - 1);
	}
	else
	{
		square(a, b, c, d);
	}
}

void divide_squareLine(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int m)
{
	GLfloat v0[2], v1[2], v2[2], v3[2], center[2];
	int j;
	if (m > 0)
	{
		for (j = 0; j < 2; j++) v0[j] = (a[j] + b[j]) / 2;
		for (j = 0; j < 2; j++) v1[j] = (b[j] + c[j]) / 2;
		for (j = 0; j < 2; j++) v2[j] = (c[j] + d[j]) / 2;
		for (j = 0; j < 2; j++) v3[j] = (d[j] + a[j]) / 2;
		center[0] = (v3[0] + v1[0]) / 2;
		center[1] = (v0[1] + v2[1]) / 2;
		divide_squareLine(a, v0, center, v3, m - 1);
		divide_squareLine(v0, b, v1, center, m - 1);
		divide_squareLine(center, v1, c, v2, m - 1);
		divide_squareLine(v3, center, v2, d, m - 1);
	}
	else
	{
		line(a, b);
		line(b, c);
		line(c, d);
		line(d, a);
	}
}

void drawText(const char *text, int length, int x, int y)
{
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	gluOrtho2D(-10.0, 10.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	for (int i = 0; i < length; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);
	divide_square(v[0], v[1], v[2], v[3], n);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 0.0);
	divide_squareLine(v[0], v[1], v[2], v[3], n);
	glEnd();

	std::string text;
	text = "403262277 CSIE YANG TING-YU";
	glColor3f(0, 0, 0);
	drawText(text.data(), text.size(), -10, 9);
	//glFlush();
	glutSwapBuffers();

	glFlush();
}

void colorMap_mazeinit()
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			colorMap[i][j] = colorArray[0];
			Maze[i][j] = 0;
		}
	}
	for (int i = 0; i < N; i++)
	{
		colorMap[0][i] = colorArray[2];
		Maze[0][i] = 2;
	}
	for (int i = 0; i < N; i++)
	{
		colorMap[i][0] = colorArray[2];
		Maze[i][0] = 2;
	}
	for (int i = 0; i < N; i++)
	{
		colorMap[N - 1][i] = colorArray[2];
		Maze[N - 1][i] = 2;
	}
	for (int i = 0; i < N; i++)
	{
		colorMap[i][N - 1] = colorArray[2];
		Maze[i][N - 1] = 2;
	}
	colorMap[1][1] = colorArray[3];
	colorMap[N - 2][N - 2] = colorArray[4];
}

void myinit()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-10.0, 10.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(1.0, 1.0, 1.0, 1.0);

}

void Keyboard(unsigned char key, int x, int y)
{
	//printf("你所按按鍵的碼是%x\t此時視窗內的滑鼠座標是(%d,%d)\n", key, x, y);
	//printf("你所按按鍵的碼是%x\t此時視窗內的滑鼠座標是(%f,%f)\n", key, (float)(x - 300) / 30, -(float)(y - 300) / 30);
	int row = (ceil(-(float)(y - 300) / 30)*(-1)) + 8, col = floor((float)(x - 300) / 30) + 8;
	//printf("你所按按鍵的碼是%x\t此時視窗內的滑鼠座標(row, col)是(%d,%d)\n", key, row, col);
	if (!((row < 0 || col < 0 || row >(N - 1) || col >(N - 1)) || (row == 0 && col == 0) || (row == N - 1 && col == N - 1)))
		switch (key)
		{
		case 'w':
			colorMap[row][col] = colorArray[2];
			Maze[row][col] = 2;
			break;
		case 'r':
			colorMap[row][col] = colorArray[0];
			break;
		case 'q':
			visit(Maze, pt(1, 1), pt(N - 2, N - 2));
			glutDisplayFunc(display);
			break;
		case 'c':
			colorMap_mazeinit();
			break;
		default:
			break;
		}
	glutPostRedisplay();
}





int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	colorMap_mazeinit();
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("2D mouse");
	glutKeyboardFunc(Keyboard);
	glutDisplayFunc(display);
	myinit();
	glutMainLoop();

}