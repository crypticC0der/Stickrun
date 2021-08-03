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
	Equalateral,
	Rectangle,
	Isocolies,
	RightAngle
};

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

float* GetTPoints(float x,float y,float t){
	//t is short for theata = angle
	//returns pointer to 8 floats representing angles
	float s{sin(t)}, c{cos(t)}; //saving sin and cos
	float* ret = new float[8];
	ret[0] = c*x-y*s;
	ret[1] = s*x+y*c;
	ret[2] = c*x+y*s;
	ret[3] = s*x-y*c;
	ret[4] = -c*x+y*s;
	ret[5] = -s*x-y*c;
	ret[6] = -c*x-y*s;
	ret[7] = -s*x+y*c;
	return ret;
}

class Object{
	public:
		Vector pos;
		Shape shape;
		float angle;
		Vector dimensions;
		int c;

		Object(Vector p,Shape s,float ang,Vector d,int color){
			c=color;
			pos = p;
			dimensions=d;
			shape=s;
			angle=ang;
		}

		void draw(){
			glColor3f(Red & c,Green & c,Blue & c);
			switch (shape){
				case Square:
				case Rectangle: {
					glBegin(GL_QUADS);
					Vector actD = dimensions/(float)200;
					actD.x = (actD.x/2);
					Vector actPos = pos/(float)200;
					actPos.x = (actPos.x/2) -1;
					actPos.y -=1;
					float* d = GetTPoints(actD.x/2,actD.y/2,angle);
					glVertex2d(actPos.x+d[0],actPos.y+d[1]);
					glVertex2d(actPos.x+d[2],actPos.y+d[3]);
					glVertex2d(actPos.x+d[4],actPos.y+d[5]);
					glVertex2d(actPos.x+d[6],actPos.y+d[7]);
					free(d);
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
		void Move(){
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
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	DrawScene();
	glFlush();
}

//supports special keys
void keyboard(int key, int _x, int _y)
{
	Vector v(0,0);
	switch (key){
		case GLUT_KEY_DOWN:
			v.y=-1;
			break;
		case GLUT_KEY_UP:
			v.y=1;
			break;
		case GLUT_KEY_LEFT:
			v.x=-1;
			break;
		case GLUT_KEY_RIGHT:
			v.x=1;
			break;
		default:
			cout << "key: " << int(key) <<endl;
			break;
		}
	player.ApplyAcc(v*1600);
}

high_resolution_clock::time_point lastTime;
void run(){
	duration<double> duration = high_resolution_clock::now() - lastTime;
	delta = duration.count();
	cout << 1/delta<<endl;
	lastTime=high_resolution_clock::now();
	usleep(7000); //makes it min of 144fps, it was around 5k fps
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	player.Move();
	DrawScene();
	glFlush();
}
int main(int argc, char** argv) {
	srand (time(NULL));
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(800,400);   // Set the window's initial width & height
    glutCreateWindow("Stickrun"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutSpecialFunc(keyboard);
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}

