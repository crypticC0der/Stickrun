#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
using namespace std;
using namespace std::chrono;

double delta;
enum Color{
	Red=4,
	Blue=2,
	Green=1,
};
enum Shape{
	Square,
	Rectangle,
	RightAngle
};
bool Left_H,Right_H;

class Vector{
	public:
		float x,y;
		Vector(){};
		Vector(float X,float Y){
			x=X;
			y=Y;
		}
		void Print(){
			cout << x << ", " << y <<endl;
		}
		void operator -=(Vector v){
			x-=v.x;
			y-=v.y;
		}
		void operator *=(float f){
			x*=f;
			y*=f;
		}
		void operator /=(float f){
			x/=f;
			y/=f;
		}
		void operator +=(Vector v){
			x+=v.x;
			y+=v.y;
		}
		Vector operator /(float f){
			return Vector(x/f,y/f);
		}
		Vector operator *(float f){
			return Vector(x*f , y*f);
		}
		Vector operator-(Vector v){
			return Vector(x-v.x,y-v.y);
		}
		Vector operator+(Vector v){
			return Vector(x+v.x,y+v.y);
		}
		float Dot(Vector v){
			return x*v.x + y*v.y;
		}


};

class Matrix2{
	public:
		float values[4];
		Matrix2(){};
		Matrix2(float a,float b,float c, float d){
			values[0] = a;
			values[1] = b;
			values[2] = c;
			values[3] = d;
		}
		Vector operator *(Vector v){
			return Vector(values[0]*v.x + values[1]*v.y
					, values[2]*v.x + values[3]*v.y );
		}
		Matrix2 operator *(Matrix2 m){
			return Matrix2(
			values[0] * m.values[0] + values[2] * m.values[1],
			values[1] * m.values[0] + values[3] * m.values[1],
			values[0] * m.values[2] + values[2] * m.values[3],
			values[1] * m.values[2] + values[3] * m.values[3]
					);
		}
};


class Object{
	public:
		Vector pos;
		Shape shape;
		Vector dimensions;
		int c;

		Object(Vector p,Shape s,float ang,Vector d,int color){
			c=color;
			pos = p;
			dimensions=d;
			shape=s;
		}

		void draw(){
			glColor3f(Red & c,Green & c,Blue & c);
			switch (shape){
				case Square:
				case Rectangle: {
					glBegin(GL_QUADS);
					Vector actD = dimensions/(float)200;
					actD.x = (actD.x/2);
					actD/=2;
					Vector actPos = pos/(float)200;
					actPos.x = (actPos.x/2) -1;
					actPos.y -=1;
					glVertex2d(actPos.x+actD.x,actPos.y+actD.y);
					glVertex2d(actPos.x+actD.x,actPos.y-actD.y);
					glVertex2d(actPos.x-actD.x,actPos.y-actD.y);
					glVertex2d(actPos.x-actD.x,actPos.y+actD.y);
					break;
				}
				default: {
					cout << "shape not supported";
				}
			}
			glEnd();
		}
};

class PhysicsObject : public Object {
	public:
		float mass;
		Vector vel;
		PhysicsObject(Vector p,Shape s, float a, Vector d,int c,float m) : Object(p,s,a,d,c){
			mass=m;
		}
		void ApplyAcc(Vector Acc){
			vel+=Acc*delta;
		}
		void ApplyForce(Vector F){
			vel+=F*delta/mass;
		}
		void WallCol(){
			if(pos.x + dimensions.x/2>800 || pos.x -dimensions.x/2 <0){
				vel.x = -vel.x/2;
			}
			if(pos.y -dimensions.y/2 <50){
				vel.y = -vel.y/2;
			}
		}
		void Move(){
			float res(400);
			if(vel.x>0.2){
				res*=-1;
			}else if(vel.x>-0.2){
				res=0;
			}
			vel.x += res*delta;
			if(vel.x<-300){
				vel.x=-300;
			}
			if(vel.x>300){
				vel.x=300;
			}
			pos+=vel*delta;
		}
};
class Player : public PhysicsObject {
	public:
		Player(Vector p,Shape s, float a, Vector d,int c,float m) : PhysicsObject(p,s,a,d,c,m){}
};

void drawString(float x, float y, char *string) {
	glRasterPos2f(x,y);
	for (char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);  // Updates the position
	}
}
Player player(Vector(200,150),Rectangle,0,Vector(50,100), Red,64);

void DrawScene(){
	glColor3f(0,0,0);
	glBegin(GL_LINES);

	glVertex2d(-1,-.5);
	glVertex2d(1,-.5);
	glEnd();
	player.draw();
}

void disInit(){
	glClearColor(1, 1, 1, 1); // Set background color to black and opaque
	cout << "lol" <<endl;
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	DrawScene();
	glFlush();
}

//supports special keys
void keyboardUp(int key, int _x, int _y)
{
	switch (key){
		case GLUT_KEY_DOWN:
			//handle
			break;
		case GLUT_KEY_UP:
			//handle
			break;
		case GLUT_KEY_LEFT:
			Left_H=false;
			break;
		case GLUT_KEY_RIGHT:
			Right_H=false;
			break;
		default:
			cout << "key: " << int(key) <<endl;
			break;
		}
}
void keyboard(int key, int _x, int _y)
{
	switch (key){
		case GLUT_KEY_DOWN:
			//handle
			break;
		case GLUT_KEY_UP:
			//handle
			break;
		case GLUT_KEY_LEFT:
			Left_H=true;
			break;
		case GLUT_KEY_RIGHT:
			Right_H=true;
			break;
		default:
			cout << "key: " << int(key) <<endl;
			break;
		}
}

high_resolution_clock::time_point lastTime;
void run(){
	duration<double> duration = high_resolution_clock::now() - lastTime;
	delta = duration.count();
	lastTime=high_resolution_clock::now();
	usleep(7000); //makes it min of 144fps, it was around 5k fps
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	float acc= 0;
	if(Right_H){
		acc=600;	
	}else if (Left_H){
		acc=-600;
	}
	player.ApplyAcc(Vector(acc,0));
	player.Move();
	DrawScene();
	glFlush();
}
int main(int argc, char** argv) {
	lastTime=high_resolution_clock::now();
	srand (time(NULL));
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(800,400);   // Set the window's initial width & height
    glutCreateWindow("Stickrun"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutSpecialFunc(keyboard);
	glutSpecialUpFunc(keyboardUp);
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}

