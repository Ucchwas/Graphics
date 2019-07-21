#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))
#include<bits/stdc++.h>
using namespace std ;
#include <glut.h>

#define pi (2*acos(0.0))
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double outerRadius ,innerRadius;
double angle;

struct Point
{
    double x,y,z;
    Point(){

    }
    Point(double _x,double _y,double _z){
        x=_x;
        y =_y;
        z=_z ;
    }
    Point operator * (const Point &other)const{
        return Point(x*other.x,y*other.y,z*other.z) ;
    }
    Point operator * (const double &scale)const{
        return Point(x*scale,y*scale,z*scale) ;
    }
    Point operator + (const Point &other)const{
        return Point(x+other.x,y+other.y,z+other.z) ;
    }
    Point operator - (const Point &other)const{
        return Point(x-other.x,y-other.y,z-other.z) ;
    }
    Point cross(Point &otherVector){

        double x = (y*otherVector.z-z*otherVector.y) ;
        double y = (otherVector.x*z-x*otherVector.z) ;
        double z = (x*otherVector.y-otherVector.x*y) ;
        return Point(x,y,z) ;
    }
};
Point normalize(Point p){
    double scale = sqrt(p.x*p.x+p.y*p.y+p.z*p.z) ;
    return p*scale ;
}
Point Rotation(Point rotationVector,Point fixedVector,double rotationAngle){
    fixedVector = normalize(fixedVector) ; // unit vector
    Point perpendicular = fixedVector.cross(rotationVector) ;
    return rotationVector*cos(rotationAngle)+perpendicular*sin(rotationAngle) ;
}

Point pos,u,l,r,normalizedu,normalizedl,normalizedr;

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct Point Points[100];
    glColor3f(0.7,0.7,0.7);
    //generate Points
    for(i=0;i<=segments;i++)
    {
        Points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        Points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated Points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(Points[i].x,Points[i].y,0);
			glVertex3f(Points[i+1].x,Points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct Point Points[100];
    //generate Points
    for(i=0;i<=segments;i++)
    {
        Points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        Points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated Points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(Points[i].x,Points[i].y,0);
			glVertex3f(Points[i+1].x,Points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct Point Points[100][100];
	int i,j;
	double h,r;
	//generate Points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			Points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			Points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			Points[i][j].z=h;
		}
	}
	//draw quads using generated Points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(Points[i][j].x,Points[i][j].y,Points[i][j].z);
				glVertex3f(Points[i][j+1].x,Points[i][j+1].y,Points[i][j+1].z);
				glVertex3f(Points[i+1][j+1].x,Points[i+1][j+1].y,Points[i+1][j+1].z);
				glVertex3f(Points[i+1][j].x,Points[i+1][j].y,Points[i+1][j].z);
                //lower hemisphere
                glVertex3f(Points[i][j].x,Points[i][j].y,-Points[i][j].z);
				glVertex3f(Points[i][j+1].x,Points[i][j+1].y,-Points[i][j+1].z);
				glVertex3f(Points[i+1][j+1].x,Points[i+1][j+1].y,-Points[i+1][j+1].z);
				glVertex3f(Points[i+1][j].x,Points[i+1][j].y,-Points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1': // look up
		    innerRadius++ ;
			break;
        case '2':
            innerRadius-- ;
            break ;
        case '3':
            outerRadius++ ;
            break ;
        case '4':
            outerRadius-- ;
            break ;
		default:
			break;
	}
}

void drawTorus(int stacks, int slices,double outeradius,double inneradius){
/*
https://www.opengl.org/discussion_boards/showthread.php/138775-Torus
*/

    for (int i = 0; i < stacks; i++) {
            glBegin(GL_QUAD_STRIP);
            for (int j = 0; j <= slices; j++) {
                for (int k = 1; k >= 0; k--) {
                        double s = (i + k) % stacks + 0.9;
                        double t = j % slices;
                        double x = (1+.1*cos(s*2*M_PI/stacks))*cos(t*2*M_PI/(double)slices);
                        double y = (1+.1*cos(s*2*M_PI/stacks))*sin(t*2*M_PI/(double)slices);
                        double z = .1 * sin(s * 2*M_PI / stacks);
                        if((i+k)%10<5)  glColor3f(1,1,1);
                        else  glColor3f(0,0,0);
                       // cout<<x<<" "<<y<<" "<<z<<endl ;
                        if(k==0) glVertex3f(x*outeradius, y*outeradius, z*outeradius);
                        else glVertex3f(x*inneradius, y*inneradius, z*inneradius);
                }
            }
        glEnd();
    }
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			//cameraHeight -= 3.0;
            pos = pos-normalizedl*2.00 ;
			break;
		case GLUT_KEY_UP:		// up arrow key
		//	cameraHeight += 3.0;
            pos = pos+normalizedl*2.00 ;
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			pos = pos+normalizedr*2.00 ;
			break;
		case GLUT_KEY_LEFT:
			pos = pos-normalizedr*2.00 ;
			break;
		case GLUT_KEY_PAGE_UP:
			pos = pos+normalizedu*2.00 ;
			break;
		case GLUT_KEY_PAGE_DOWN:
			pos = pos-normalizedu*2.00 ;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
		    outerRadius++ ;
			break;
		case GLUT_KEY_END:
		    outerRadius-- ;
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
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



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	gluLookAt(pos.x-30,pos.y-30,pos.z+50, pos.x+l.x-30,pos.y+l.y-30,pos.z+l.z+50,u.x,u.y,u.z);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
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

   // drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	drawTorus(100,100,outerRadius,innerRadius) ;
	//drawTorus(100,100,innerRadius) ;
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	pos.x=pos.y=100;
	pos.z=0 ;
    //////////////////////////// up vector ////////////////////
    u.x=u.y=0 ;
    u.z=1 ;
    /////////////////////////// right vector ////////////////////
    r.x=-1.00/sqrt(2) ;
    r.y=1.00/sqrt(2) ;
    r.z =0 ;
    ///////////////////////////// left vector////////////////////
    l.x=r.x ;
    l.y=r.x ;
    l.z=0 ;
	//clear the screen
	normalizedl = normalize(l) ;
	normalizedu = normalize(u) ;
	normalizedr = normalize(r) ;
	glClearColor(0,0,0,0);
    outerRadius = 30 ;
    innerRadius=25 ;
	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Fully Controllable Camera");

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
