#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

#include <bits/stdc++.h>
using namespace std;


double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double max_side = 20.0;
double side = 20.0;

struct point
{
	double x, y, z;

    point(){
    }
    point(double _x,double _y,double _z){
        x=_x;
        y =_y;
        z=_z ;
    }
	point operator + (const point &other)const{
        return point(x+other.x,y+other.y,z+other.z) ;
    }
    point operator - (const point &other)const{
        return point(x-other.x,y-other.y,z-other.z) ;
    }
    point operator * (const point &other)const{
        return point(x*other.x,y*other.y,z*other.z) ;
    }
    point operator * (const double &scale)const{
        return point(x*scale,y*scale,z*scale) ;
    }

}pos,l,r,u;

point normalizedu,normalizedl,normalizedr;

point normalize(point p){
    double scale = sqrt(p.x*p.x+p.y*p.y+p.z*p.z) ;
    return p*scale ;
}

struct point rotate(struct point vec, struct point ref, double sign)
{
	struct point n, ret;
	double angle = 0.05 * sign;

	n.x = ref.y*vec.z - ref.z*vec.y;
	n.y = ref.z*vec.x - ref.x*vec.z;
	n.z = ref.x*vec.y - ref.y*vec.x;

	ret.x = vec.x * cos(angle) + n.x * sin(angle);
	ret.y = vec.y * cos(angle) + n.y * sin(angle);
	ret.z = vec.z * cos(angle) + n.z * sin(angle);

	return ret;
}

void drawAxes()
{
	if (drawaxes == 1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES); {
			glVertex3f(100, 0, 0);
			glVertex3f(-100, 0, 0);

			glVertex3f(0, -100, 0);
			glVertex3f(0, 100, 0);

			glVertex3f(0, 0, 100);
			glVertex3f(0, 0, -100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if (drawgrid == 1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES); {
			for (i = -8; i <= 8; i++) {

				if (i == 0)
					continue;	//SKIP the MAIN axes

                //lines parallel to Y-axis
				glVertex3f(i * 10, -90, 0);
				glVertex3f(i * 10, 90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i * 10, 0);
				glVertex3f(90, i * 10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
	//glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS); {
		glVertex3f(a, a, 0);
		glVertex3f(a, -a, 0);
		glVertex3f(-a, -a, 0);
		glVertex3f(-a, a, 0);
	}glEnd();
}

void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawsixSquares()
{
	glColor3f(1, 1, 1);
	for (int i = 0; i<25; i++)
	{
		glPushMatrix();
	}

	glTranslated(0, 0, max_side);
	drawSquare(side);

	glPopMatrix();
	glTranslated(0, 0, -max_side);
	drawSquare(side);

	glPopMatrix();
	glRotated(90, 1, 0, 0);
	glTranslated(0, 0, max_side);
	drawSquare(side);

	glPopMatrix();
	glRotated(-90, 1, 0, 0);
	glTranslated(0, 0, max_side);
	drawSquare(side);

	glPopMatrix();
	glRotated(90, 0, 1, 0);
	glTranslated(0, 0, max_side);
	drawSquare(side);

	glPopMatrix();
	glRotated(-90, 0, 1, 0);
	glTranslated(0, 0, max_side);
	drawSquare(side);
}


void drawOneEighthSphere(double radius, int slices, int stacks)
{
	struct point points[100][100];
	int i, j;
	double h, r;
	//generate points
	for (i = 0; i <= stacks; i++)
	{
		h = radius * sin(((double)i / (double)stacks)*(pi / 2));
		r = radius * cos(((double)i / (double)stacks)*(pi / 2));
		for (j = 0; j <= slices; j++)
		{
			points[i][j].x = r * cos(((double)j / (double)slices) * pi / 2);
			points[i][j].y = r * sin(((double)j / (double)slices) * pi / 2);
			points[i][j].z = h;
		}
	}
	//draw quads using generated points
	for (i = 0; i<stacks; i++)
	{
		glColor3f(1, 0, 0);
		for (j = 0; j<slices; j++)
		{
			glBegin(GL_QUADS); {
				//upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}

void draweightSpheres()
{
	double s = side;
	double r = max_side - side;

	double slices = 50;
	double stacks = 50;

	glPushMatrix();
	{
		glTranslatef(s, s, s);
		glRotated(0, 0, 0, 1);
		drawOneEighthSphere(r, slices, stacks);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-s, s, s);
		glRotated(90, 0, 0, 1);
		drawOneEighthSphere(r, slices, stacks);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-s, -s, s);
		glRotated(180, 0, 0, 1);
		drawOneEighthSphere(r, slices, stacks);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(s, -s, s);
		glRotated(-90, 0, 0, 1);
		drawOneEighthSphere(r, slices, stacks);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(s, s, -s);
		glRotated(180, 1, 1, 0);
		drawOneEighthSphere(r, slices, stacks);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-s, s, -s);
		glRotated(90, 0, 0, 1);
		glRotated(180, 1, 1, 0);
		drawOneEighthSphere(r, slices, stacks);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-s, -s, -s);
		glRotated(180, 0, 0, 1);
		glRotated(180, 1, 1, 0);
		drawOneEighthSphere(r, slices, stacks);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(s, -s, -s);
		glRotated(-90, 0, 0, 1);
		glRotated(180, 1, 1, 0);
		drawOneEighthSphere(r, slices, stacks);
	}
	glPopMatrix();
}

void drawOneFourthCylinder(double radius, double height, int slices, int stacks)
{
	struct point points[100][100];
	int i, j;
	double h, r;
	//generate points
	for (i = 0; i <= stacks; i++)
	{
		h = height * sin(((double)i / (double)stacks)*(pi / 2));
		//r=radius*cos(((double)i/(double)stacks)*(pi/2));
		//h = height;
		r = radius;
		for (j = 0; j <= slices; j++)
		{
			points[i][j].x = r * cos(((double)j / (double)slices)*.5*pi);
			points[i][j].y = r * sin(((double)j / (double)slices)*.5*pi);
			points[i][j].z = h;
		}
	}
	//draw quads using generated points
	for (i = 0; i<stacks; i++)
	{
		glColor3f(0, 1, 0);
		for (j = 0; j<slices; j++)
		{
			glBegin(GL_QUADS); {
				//upper hemisphere
				glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
				glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
				glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
				glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
				//lower hemisphere
				glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
				glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
				glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
				glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
			}glEnd();
		}
	}

}

void drawtwelveCylinders()
{
	double s = side;
	double r = max_side - side;

	glPushMatrix();
	{
		glTranslatef(s, s, -s - r);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-s, s, -s - r);
		glRotated(90, 0, 0, 1);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-s, -s, -s - r);
		glRotated(180, 0, 0, 1);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(s, -s, -s - r);
		glRotated(-90, 0, 0, 1);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();


	glPushMatrix();
	{
		glTranslatef(0, s, -r);
		glRotated(-90, 0, 1, 0);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(0, s, -s - s - r);
		glRotated(90, 0, 1, 0);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();


	glPushMatrix();
	{
		glTranslatef(0, -s, -s - s - r);
		glRotated(-90, 1, 0, 0);
		glRotated(90, 0, 1, 0);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();


	glPushMatrix();
	{
		glTranslatef(0, -s, s - s - r);
		glRotated(90, 1, 0, 0);
		glRotated(-90, 0, 1, 0);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();



	glPushMatrix();
	{
		glTranslatef(s, 0, s - s - r);
		glRotated(90, 1, 0, 0);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(s, 0, -s - s - r);
		glRotated(-90, 1, 0, 0);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();


	glPushMatrix();
	{
		glTranslatef(-s, 0, s - s - r);
		glRotated(-90, 0, 1, 0);
		glRotated(90, 1, 0, 0);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(-s, 0, -s - s - r);
		glRotated(90, 0, 1, 0);
		glRotated(-90, 1, 0, 0);
		drawOneFourthCylinder(r, s, 50, 50);
	}
	glPopMatrix();
}

void keyboardListener(unsigned char key, int x, int y)
{

	switch (key)
	{
	case '1':
		l = rotate(l, u, 1);
		r = rotate(r, u, 1);
		break;
	case '2':
		l = rotate(l, u, -1);
		r = rotate(r, u, -1);
		break;
	case '3':
		u = rotate(u, r, -1);
		l = rotate(l, r, -1);
		break;
	case '4':
		u = rotate(u, r, 1);
		l = rotate(l, r, 1);
		break;
	case '5':
		u = rotate(u, l, -1);
		r = rotate(r, l, -1);
		break;
	case '6':
		u = rotate(u, l, 1);
		r = rotate(r, l, 1);
		break;
	default:
		break;
	}
}


void specialKeyListener(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_DOWN:
        pos = pos-normalizedl*2.00;
		break;
	case GLUT_KEY_UP:
        pos = pos+normalizedl*2.00;
		break;
	case GLUT_KEY_RIGHT:
        pos = pos+normalizedr*2.00;
		break;
	case GLUT_KEY_LEFT:
        pos = pos-normalizedr*2.00;
		break;
	case GLUT_KEY_PAGE_UP:
        pos = pos+normalizedu*2.00;
		break;
	case GLUT_KEY_PAGE_DOWN:
        pos = pos-normalizedu*2.00;
		break;
	case GLUT_KEY_INSERT:
		break;
	case GLUT_KEY_HOME:
		side -= 0.2;
		side = max(side, 0.0);
		break;
	case GLUT_KEY_END:
		side += 0.2;
		side = min(side, max_side);
		break;
	default:
		break;
	}
}


void mouseListener(int button, int state, int x, int y) {	//x, y is the x-y of the screen (2D)
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {		// 2 times?? in ONE click? -- solution is checking DOWN or UP
			drawaxes = 1 - drawaxes;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		//........
		break;

	case GLUT_MIDDLE_BUTTON:
		//........
		break;

	default:
		break;
	}
}



void display() {

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();
	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	gluLookAt(pos.x,pos.y,pos.z, pos.x+l.x,pos.y+l.y,pos.z+l.z,u.x,u.y,u.z);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

	//glColor3f(1,0,0);
	//drawSquare(10);

	//drawSS();

	//drawCircle(30,24);

	//drawCone(20,50,24);

	draweightSpheres();
	drawsixSquares();
	drawtwelveCylinders();

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate() {
	angle += 0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init() {
	//codes for initialization

	max_side = 20;
	side = 20;

	pos.x=100;
	pos.y=100;
	pos.z=0 ;
    u.x=0;
    u.y=0 ;
    u.z=1 ;
    r.x=-1.00/sqrt(2) ;
    r.y=1.00/sqrt(2) ;
    r.z =0 ;
    l.x=r.x ;
    l.y=r.x ;
    l.z=0 ;

	drawgrid = 0;
	drawaxes = 1;
	cameraHeight = 150.0;
	cameraAngle = 1.0;
	angle = 0;

    normalizedl = normalize(l) ;
	normalizedu = normalize(u) ;
	normalizedr = normalize(r) ;
	//clear the screen
	glClearColor(0, 0, 0, 0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(50, 1, 1, 1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Sphere_to_cube");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
