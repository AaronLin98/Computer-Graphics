
/*
 *  clip.cpp
 *  This program clips the line with the given window.
 */
#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>
using namespace std;

class Point{
private:
	float x;
	float y;
public:
	Point(){
		this->x = 0;
		this->y = 0;
	}
	Point(float x,float y){
		this->x = x;
		this->y = y;
	}
	float getx(){
		return x;
	}
	float gety(){
		return y;
	}
};

#define	drawOneLine(x1,y1,x2,y2) glBegin(GL_LINES); \
	glVertex2f ((x1),(y1)); glVertex2f ((x2),(y2)); glEnd();


void myinit(void)
{
    glShadeModel (GL_FLAT);
    glClearColor (0.0, 0.0, 0.0, 0.0);
}


void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h) 
	gluOrtho2D (0.0, 1.0, 0.0, 1.0*(GLfloat)h/(GLfloat)w);
    else 
	gluOrtho2D (0.0, 1.0*(GLfloat)w/(GLfloat)h, 0.0, 1.0);
	//gluOrtho2D (0.0, 400.0, 0.0, 150.0);
    glMatrixMode(GL_MODELVIEW);
}

//-------------------------------
// your task!!!
//-------------------------------

void myclip(vector<Point> line,vector<Point> poly,vector<Point> &desline)
// line clipping algorithm 
{
	float r;
	float u1 = 0,u2 = 1;
	bool flag1 = false;
	bool flag2 = false;
	float p1x, p1y;
	float p2x, p2y;
	float pk[4],qk[4];
	//��ʼ��pk,��qk
	pk[1] = line[1].getx() - line[0].getx();
	pk[0] = line[0].getx() - line[1].getx();
	pk[3] = line[1].gety() - line[0].gety();
	pk[2] = line[0].gety() - line[1].gety();
	qk[0] = line[0].getx() - poly[0].getx();
	qk[1] = poly[1].getx() - line[0].getx();
	qk[2] = line[0].gety() - poly[0].gety();
	qk[3] = poly[3].gety() - line[0].gety();
	//��4���߽����ж�
	for(int i=0;i<4;i++){
		//����u1��u2ֵ
		if(pk[i] != 0){
			r = (float)qk[i]/(float)pk[i];
			if(pk[i] < 0){
				u1 = max(u1,r,0);
			}
			if(pk[i] > 0){
				u2 = min(u2,r,1);
			}
		}
		//��ʱ�߶��ڴ����⣬����
		else if(pk[i] == 0 && qk[i]<0){
			return;
		}
		//��ʱ�߶��ڴ��ڵ�ĳһ�ߵ��ڲ࣬����ѭ��
		else if(pk[i] == 0 && qk[i]>=0){
			flag2 = true;
		}
	}
	//�߶��봰���뽻�ӣ�����
	if(u1 > u2){
		return;
	}
	//����߶��봰��ĳһ��ƽ��
	if(flag2){
		//��y��ƽ��
		if(pk[0] == 0){
			float a,b;
			a = min((float)qk[2]/(float)pk[2],(float)qk[3]/(float)pk[3]);
			b = max((float)qk[2]/(float)pk[2],(float)qk[3]/(float)pk[3]);
			u1 = max(0,a);
			u2 = min(1,b);
		}
		//��x��ƽ��
		else if(pk[2] ==0 ){
			float a,b;
			a = min((float)qk[0]/(float)pk[0],(float)qk[1]/(float)pk[1]);
			b = max((float)qk[0]/(float)pk[0],(float)qk[1]/(float)pk[1]);
			u1 = max(0,a);
			u2 = min(1,b);
		}
	}
	//�����и��
	p1x = line[0].getx() + u1*(line[1].getx() - line[0].getx());
	p1y = line[0].gety() + u1*(line[1].gety() - line[0].gety());
	p2x = line[0].getx() + u2*(line[1].getx() - line[0].getx());
	p2y = line[0].gety() + u2*(line[1].gety() - line[0].gety());
	Point s(p1x,p1y);
	Point d(p2x,p2y);
	desline.push_back(s);
	desline.push_back(d);
}

void initial(vector<Point> &line,vector<Point> &poly){
	Point lp1(0.1f, 0.5f);
	Point lp2(0.9f,0.9f);
	//��ʼ���߶�
	line.push_back(lp1);
	line.push_back(lp2);
	Point pp1(0.2f, 0.3f);
	Point pp2(0.8f, 0.3f);
	Point pp3(0.8f, 0.7f);
	Point pp4(0.2f, 0.7f);
	//��ʼ�������
	poly.push_back(pp1);
	poly.push_back(pp2);
	poly.push_back(pp3);
	poly.push_back(pp4);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
// The window
// ------------------------------------
//  you can change the window definition on yourself.
// ------------------------------------
    glColor4f (0.0, 1.0, 1.0, 0.75);
	glBegin(GL_POLYGON);
	    glVertex2f( 0.2f, 0.3f); // Bottom Left
		glVertex2f( 0.8f, 0.3f); // Bottom Left
		glVertex2f( 0.8f, 0.7f); // Bottom Right
		glVertex2f( 0.2f, 0.7f); // Bottom Right
	glEnd();
// ------------------------------------
//	please define your own line segment and draw 
//	it here with different color and line width
// ------------------------------------
	//�Լ�������һ���߶�
	glColor3f (1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex2f( 0.1f, 0.5f);
		glVertex2f( 0.9f, 0.9f);
	glEnd();
 	//-------------------------------
	//do the clipping in myclip() funtion 
	//-------------------------------
	//��line����¼�߶εĶ˵�
	vector<Point> line;
	//��poly����¼����εĶ˵�
	vector<Point> poly;
	//��desline����¼�и��ߵĶ˵�
	vector<Point> desline;
	//��ʼ��line,poly
	initial(line,poly);
	//ʵ���и�
	myclip(line,poly,desline);
// ------------------------------------
//	please draw clipped line here with another 
//  color and line width
// ------------------------------------   
	//����߶����и����
	if(desline.size()>0){
		//�����и��߶�
		glColor3f (1.0, 1.0, 0.0);
		glBegin(GL_LINES);
		glVertex2f( desline[0].getx(),desline[0].gety());
		glVertex2f( desline[1].getx(),desline[1].gety());
		glEnd();
	}
    glFlush();
}


/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA);
   //define size and the relative positon of the applicaiton window on the display
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
 	//init the defined window with "argv[1]" as topic showed on the top the window
   glutCreateWindow (argv[0]);
	// opengl setup
   myinit ();
	//define callbacks
   glutDisplayFunc(display); 
   glutReshapeFunc(myReshape);
   //enter the loop for display
   glutMainLoop();

	return 1;
}
