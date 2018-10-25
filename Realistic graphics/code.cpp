#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <GL/glut.h>

static int spin = 0;
static int begin;
static float scale = 1;
static float xtrans = 0.0;
static float ytrans = 0.0;
static float rotate = 0;

														//鼠标控制的变换
void movelight(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    begin = x;
  }
}

void motion(int x, int y)								
{
	spin = (spin + (x - begin)) % 360;
	begin = x;	
	glutPostRedisplay();							//更新鼠标变化引起的视角变换
}

void myinit(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}
/*  Here is where the light position is reset after the modeling
 *  transformation (glRotated) is called.  This places the 
 *  light at a new position in world coordinates.  The cube
 *  represents the position of the light.
 */
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
	  glTranslatef(0.0, 0.0, -5.0);
	  glPushMatrix();
		  {
			  //定义光源
			GLfloat ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
			GLfloat diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
			GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
			GLfloat position[] = {0.0, 0.0, 1.5, 1.0};

			glRotated((GLdouble) spin, 0.0, 1.0, 0.0);
			glLightfv(GL_LIGHT0, GL_POSITION, position);		//light
			glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient); //GL_AMBIENT表示各种光线照射到该材质上，
														//经过很多次反射后最终遗留在环境中的光线强度（颜色）
			glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse); //漫反射后
			glLightfv(GL_LIGHT0, GL_SPECULAR, specular);//镜面反射后

			}
		  glTranslated(0.0, 0.0,3.0);
		  glDisable(GL_LIGHTING);
		  glutSolidSphere(0.1, 40, 32);
		  glEnable(GL_LIGHTING);
	  glPopMatrix();
		  {
			 //定义几何对象材料
			GLfloat mat_ambient[]  = {0.0f, 0.0f, 0.5f, 1.0f};
			GLfloat mat_diffuse[]  = {0.0f, 0.0f, 0.5f, 1.0f};
			GLfloat mat_specular[] = {0.0f, 0.0f, 1.0f, 1.0f};
			GLfloat mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
			GLfloat mat_shininess  = 30.0f;

			glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
			glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
			glMaterialf (GL_FRONT, GL_SHININESS, mat_shininess);
		 }
		  {
			  //定义交互操作
			glScalef(scale,scale,scale);			//scale
			glRotated(rotate,0,1,0);				//rotated
			glTranslatef(xtrans, ytrans, 0.0);		//trans
		  }
		glTranslatef(1.0f, 0.0f, 0.0f);
		glutSolidTeapot(0.5);
		//glutSolidTorus(0.275, 0.85, 20, 20);
		//glutSolidTorus(0.275, 0.85, 20, 20);	
		//glutSolidTetrahedron();
		//glutSolidIcosahedron();
		{										
										//通过关闭深度测试，对比消隐处理的结果
			glPushMatrix();
			glDisable(GL_DEPTH_TEST);
			glTranslatef(-2.0f, 0.0f, 0.0f);
			glutSolidTeapot(0.5);
			glPopMatrix();
			glEnable(GL_DEPTH_TEST);
		}
  glPopMatrix();
  glutSwapBuffers();
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat) w / (GLfloat) h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}
												//定义键盘操作
void KeyBoards(unsigned char key,int x,int y)
{
	switch (key)
	{
		case 'q':								//缩放
			scale = scale*1.2;
			glutPostRedisplay();
			break;
		case 'e':
			scale = scale/1.2;
			glutPostRedisplay();
			break;
		case 'w':								//平移
			ytrans = ytrans + 0.1;
			glutPostRedisplay();
			break;
		case 's':
			ytrans = ytrans - 0.1;
			glutPostRedisplay();
			break;
		case 'd':
			xtrans = xtrans + 0.1;
			glutPostRedisplay();
			break;
		case 'a':
			xtrans = xtrans - 0.1;
			glutPostRedisplay();
			break;
		case 'z':								//旋转
			rotate = rotate - 10;
			glutPostRedisplay();
			break;
		case 'c':
			rotate = rotate + 10;
			glutPostRedisplay();
			break;
	}
}
/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow(argv[0]);
	myinit();
	glutMouseFunc(movelight);		//注册鼠标事件
	glutMotionFunc(motion);
	glutKeyboardFunc(&KeyBoards);  //注册键盘事件
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;           
}
