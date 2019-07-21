#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

#include <bits/stdc++.h>
using namespace std ;
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double wheel_angle ;
struct point
{
    double x,y,z;
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
    point cross(point &otherVector){
        double x = (y*otherVector.z-z*otherVector.y) ;
        double y = (otherVector.x*z-x*otherVector.z) ;
        double z = (x*otherVector.y-otherVector.x*y) ;
        return point(x,y,z) ;
    }
    double dot(point &otherVector){
        return x*otherVector.x+y*otherVector.y+z*otherVector.z ;
    }
};
point wheel_centre ,direction_vector,fixed_vector ;
point normalize(point p){
    double scale = sqrt(p.x*p.x+p.y*p.y+p.z*p.z) ;
    return p*scale ;
}
point Rotation(point rotationVector,point fixed_vector,double rotationAngle){
    point _rotationVector = normalize(rotationVector) ;
    point _fixed_vector = normalize(fixed_vector) ;
    point perpendicular = _fixed_vector.cross(_rotationVector) ;
    return rotationVector*cos(rotationAngle)+perpendicular*sin(rotationAngle) ;
}

point pos,u,l,r,normalizedu,normalizedl,normalizedr;

void drawAxes(){
	if(drawaxes==1){
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
void drawGrid(){
	int i;
	if(drawgrid==1){
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



void draw_wheel(double x, double y, double radius){
	int i;
	int lineAmount = 100;
	point wheel[lineAmount][2] ;
	GLfloat twicePi = 2.0f * M_PI;
    for(i = 0; i <= lineAmount;i++) {
                wheel[i][0] = point(x + (radius * cos(i *  twicePi / lineAmount)),y + (radius* sin(i * twicePi / lineAmount)),wheel_centre.z) ;
                wheel[i][1] = point(x + (radius * cos(i *  twicePi / lineAmount)),y + (radius* sin(i * twicePi / lineAmount)),wheel_centre.z+5) ;
    }
    for(int i=0;i<lineAmount;i++){
         glBegin(GL_QUADS);
        {
            glVertex3f(wheel[i][0].x,wheel[i][0].y,wheel[i][0].z);
			glVertex3f(wheel[i][1].x,wheel[i][1].y,wheel[i][1].z);
			glVertex3f(wheel[i+1][1].x,wheel[i+1][1].y,wheel[i+1][1].z);
			glVertex3f(wheel[i+1][0].x,wheel[i+1][0].y,wheel[i+1][0].z);
        }
        glEnd();
    }
    glBegin(GL_QUADS);
        {
            glVertex3f(wheel[0][0].x,wheel[0][0].y,wheel[0][0].z);
			glVertex3f(wheel[lineAmount/2][1].x,wheel[lineAmount/2][1].y,wheel[lineAmount/2][1].z);
			glVertex3f(wheel[lineAmount/2][0].x,wheel[lineAmount/2][0].y,wheel[lineAmount/2][0].z);
            glVertex3f(wheel[0][1].x,wheel[0][1].y,wheel[0][1].z);
        }
    glEnd();

    glBegin(GL_QUADS);
        {
            glVertex3f(wheel[lineAmount/4][0].x,wheel[lineAmount/4][0].y,wheel[lineAmount/4][0].z);
			glVertex3f(wheel[3*lineAmount/4][1].x,wheel[3*lineAmount/4][1].y,wheel[3*lineAmount/4][1].z);
			glVertex3f(wheel[3*lineAmount/4][0].x,wheel[3*lineAmount/4][0].y,wheel[3*lineAmount/4][0].z);
            glVertex3f(wheel[lineAmount/4][1].x,wheel[lineAmount/4][1].y,wheel[lineAmount/4][1].z);
        }
    glEnd();

}
void keyboardListener(unsigned char key, int x,int y){
	switch(key){
        case 'w':
            wheel_centre.x =  wheel_centre.x+direction_vector.x*2 ;
            break ;
        case 's':
            wheel_centre.x =  wheel_centre.x-direction_vector.x*2 ;
            break ;
        case 'a':
            wheel_angle++ ;
            direction_vector = Rotation(direction_vector,fixed_vector,wheel_angle*(pi/180)) ;
            break ;
        case 'd':
            wheel_angle-- ;
            direction_vector = Rotation(direction_vector,fixed_vector,wheel_angle*(pi/180)) ;
            break ;
		default:
			break;
	}
}



void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			//cameraHeight -= 3.0;
			pos = pos-normalizedu*2.00 ;
            //pos = pos-normalizedl*2.00 ;
			break;
		case GLUT_KEY_UP:		// up arrow key
		//	cameraHeight += 3.0;
            pos = pos+normalizedu*2.00 ;
            //pos = pos+normalizedl*2.00 ;
			break;

		case GLUT_KEY_RIGHT:
			//cameraAngle += 0.03;
			pos = pos+normalizedr*2.00 ;
			break;
		case GLUT_KEY_LEFT:
		    //cameraAngle -= 0.03;
			pos = pos-normalizedr*2.00 ;
			break;
		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
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

	gluLookAt(pos.x,pos.y,pos.z+50, pos.x+l.x,pos.y+l.y,pos.z+l.z+50,u.x,u.y,u.z);
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

    glRotated(wheel_angle,0,0,1) ;
	glTranslated(0,0,25) ;
	glRotated(90,1,0,0) ;
    draw_wheel(wheel_centre.x,wheel_centre.y,25) ;

	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	//drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
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
	//clear the screen
	normalizedl = normalize(l) ;
	normalizedu = normalize(u) ;
	normalizedr = normalize(r) ;
	glClearColor(0,0,0,0);

    wheel_centre.x=0;
	wheel_centre.y=0;
	wheel_centre.z =0;
	wheel_angle= 0 ;

	direction_vector.x=1 ;
	direction_vector.y=0;
	direction_vector.z=0 ;

	fixed_vector.x=0;
	fixed_vector.y=0 ;
	fixed_vector.z=1 ;
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

	glutCreateWindow("Wheel");

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
