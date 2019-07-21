#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <bits/stdc++.h>

#define PI acos(-1)

using namespace std;

double eyeX,eyeY,eyeZ;
double lookX,lookY,lookZ;
double upX,upY,upZ;
double fovY,aspectRatio,near,far;

string command;

double p1x,p1y,p1z,p2x,p2y,p2z,p3x,p3y,p3z;
double tx,ty,tz;
double sx,sy,sz;
double angle,ax,ay,az;
double rx,ry,rz,ux,uy,uz,lx,ly,lz;
double l;
double **V,**R,**T;
double fovX,t,r;
double **P;

stack<pair<double**,bool> >mystack;

double** null(){
    double** M = new double*[4];

    for(int i=0;i<4;i++)
        M[i] = new double[4];

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            M[i][j] = 0;

    return M;
}

double** identity(){
    double** MI = null();

    for(int i=0;i<4;i++)
        MI[i][i] = 1.0;

    return MI;
}

double** multiplication(double** M1,double** M2){
    double** mul = null();
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            for(int k=0;k<4;k++)
                mul[i][j] += M1[i][k]*M2[k][j];
    return mul;
}

void translate(double tx, double ty, double tz)
{
    double** M = identity();

    M[0][3] = tx;
    M[1][3] = ty;
    M[2][3] = tz;

    double** prev = mystack.top().first;
    double** res = multiplication(prev,M);
    mystack.push(make_pair(res, false));
}

void scale(double sx, double sy, double sz)
{
    double** M = identity();

    M[0][0] = sx;
    M[1][1] = sy;
    M[2][2] = sz;

    double** prev = mystack.top().first;
    double** res = multiplication(prev,M);
    mystack.push(make_pair(res, false));
}

double** triangle(double p1x, double p1y, double p1z,double p2x, double p2y, double p2z,double p3x, double p3y, double p3z)
{
        double** M = null();

        M[0][0] = p1x;
        M[0][1] = p2x;
        M[0][2] = p3x;
        M[0][3] = 1.0;

        M[1][0] = p1y;
        M[1][1] = p2y;
        M[1][2] = p3y;
        M[1][3] = 1.0;

        M[2][0] = p1z;
        M[2][1] = p2z;
        M[2][2] = p3z;
        M[2][3] = 1.0;

        M[3][0] = M[3][1] = M[3][2] = M[3][3] = 1.0;

        double** res = multiplication(mystack.top().first, M);
        return res;
}

void rotate(double angle, double ax, double ay, double az)
{
    double cos_theta = cos(PI * angle / 180.0);
    double sin_theta = sin(PI * angle / 180.0);

    double l = sqrt(ax * ax + ay * ay + az * az);
    double ux = ax / l;
    double uy = ay / l;
    double uz = az / l;

    double** M = null();

    M[0][0] = cos_theta + ux * ux * (1 - cos_theta);
    M[0][1] = ux * uy * (1 - cos_theta) - uz * sin_theta;
    M[0][2] = ux * uz * (1 - cos_theta) + uy * sin_theta;

    M[1][0] = ux * uy * (1 - cos_theta) + uz * sin_theta;
    M[1][1] = cos_theta + uy * uy * (1 - cos_theta);
    M[1][2] = uy * uz * (1 - cos_theta) - ux * sin_theta;

    M[2][0] = ux * uz * (1 - cos_theta) - uy * sin_theta;
    M[2][1] = uy * uz * (1 - cos_theta) + ux * sin_theta;
    M[2][2] = cos_theta + uz * uz * (1 - cos_theta);

    M[3][3] = 1.0;

    double** prev = mystack.top().first;
    double** res = multiplication(prev,M);
    mystack.push(make_pair(res, false));
}

void push()
{
    double** M = mystack.top().first;
    mystack.push(make_pair(M, true));
}

void pop()
{
    while(mystack.top().second == false){
        mystack.pop();
    }
    mystack.pop();
}

ofstream stage1;
ofstream stage2;
ofstream stage3;

int main()
{
    freopen("scene.txt", "r", stdin);
    mystack.push(make_pair(identity(), false));

    stage1.open("stage1.txt");
    stage2.open("stage2.txt");
    stage3.open("stage3.txt");

    cin >> eyeX >> eyeY >> eyeZ;
    cin >> lookX >> lookY >> lookZ;
    cin >> upX >> upY >> upZ;
    cin >> fovY >> aspectRatio >> near >> far;

    lx = lookX - eyeX;
    ly = lookY - eyeY;
    lz = lookZ - eyeZ;
    l = sqrt(lx * lx + ly * ly + lz * lz);
    lx /= l;
    ly /= l;
    lz /= l;

    rx = ly * upZ - lz * upY;
    ry = lz * upX - lx * upZ;
    rz = lx * upY - ly * upX;
    l = sqrt(rx * rx + ry * ry + rz * rz);
    rx /= l;
    ry /= l;
    rz /= l;

    ux = ry * lz - rz * ly;
    uy = rz * lx - rx * lz;
    uz = rx * ly - ry * lx;
    l = sqrt(ux * ux + uy * uy + uz * uz);
    ux /= l;
    uy /= l;
    uz /= l;

    T = identity();
    T[0][3] = -eyeX;
    T[1][3] = -eyeY;
    T[2][3] = -eyeZ;

    R = null();
    R[0][0] = rx;
    R[0][1] = ry;
    R[0][2] = rz;
    R[1][0] = ux;
    R[1][1] = uy;
    R[1][2] = uz;
    R[2][0] = -lx;
    R[2][1] = -ly;
    R[2][2] = -lz;
    R[3][3] = 1.0;
    V = multiplication(R, T);

    fovX = fovY * aspectRatio;
    t = near * tan(fovY /2.0 * PI / 180.0);
    r = near * tan(fovX /2.0 * PI / 180.0);

    P = null();
    P[0][0] = near / r;
    P[1][1] = near / t;
    P[2][2] = -(far + near) / (far - near);
    P[2][3] = -(2 * far * near) / (far - near);
    P[3][2] = -1.0;

    while (1)
    {
        cin >> command;

        if (command == "triangle")
        {
            cin >> p1x >> p1y >> p1z;
            cin >> p2x >> p2y >> p2z;
            cin >> p3x >> p3y >> p3z;

            double **triangle1 = triangle(p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                        stage1 << fixed << setprecision(7) << triangle1[j][i] << " ";
                }
                stage1<< endl;
            }
            stage1<< endl;

            double **triangle2 = multiplication(V, triangle1);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                        stage2<< fixed << setprecision(7) << triangle2[j][i] << " ";
                }
                stage2<< endl;
            }
            stage2<< endl;

            double **triangle3 = multiplication(P, triangle2);
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                        stage3<< fixed << setprecision(7) << triangle3[j][i]/triangle3[3][i] << " ";
                }
                stage3<< endl;
            }
            stage3<< endl;
        }
        else if (command == "translate")
        {
            cin >> tx >> ty >> tz;
            translate(tx, ty, tz);
        }
        else if (command == "scale")
        {
            cin >> sx >> sy >> sz;
            scale(sx, sy, sz);
        }
        else if (command == "rotate")
        {
            cin >> angle >> ax >> ay >> az;
            rotate(angle, ax, ay, az);
        }
        else if (command == "push")
        {
            push();
        }
        else if (command == "pop")
        {
            pop();
        }
        else if (command == "end")
        {
            break;
        }
        else
        {
            cout << "PLEASE GIVE CORRECT COMMAND" << endl;
        }
    }

    return 0;
}

