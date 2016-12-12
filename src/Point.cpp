#include "Point.h"
#include <math.h>

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif


Point::Point() { x = y = z = 0; w = 1; }

Point::Point( int a, int b, int c ) { 
	x = a;
	y = b;
	z = c;
	w = 1;
}

Point::Point( double a, double b, double c ) { 
	x = a;
	y = b;
	z = c;
	w = 1;
}

// OPERATOR OVERLOADS

Point operator*(Point a, float f) {
    return Point(a.getX()*f,a.getY()*f,a.getZ()*f);
}

Point operator/(Point a, float f) {
    return Point(a.getX()/f,a.getY()/f,a.getZ()/f);
}

Point operator/(float f, Point a) {
    return Point(a.getX()/f,a.getY()/f,a.getZ()/f);
}

Point operator*(float f, Point a) {
    return Point(a.getX()*f,a.getY()*f,a.getZ()*f);
}

Point operator+(Point a, Point b) {
    return Point(a.getX()+b.getX(), a.getY()+b.getY(), a.getZ()+b.getZ());
}

Point operator+(Point a, Vector b) {
    return Point(a.getX()+b.getX(), a.getY()+b.getY(), a.getZ()+b.getZ());
}

Vector operator-(Point a, Point b) {
    return Vector(a.getX()-b.getX(), a.getY()-b.getY(), a.getZ()-b.getZ());
}

void glTranslatef( Point p ) {
    glTranslatef( p.getX(), p.getY(), p.getZ() );
}

void gluLookAt( Point eye, Point lookAt, Vector up ) {
    gluLookAt( eye.getX(), eye.getY(), eye.getZ(),
                lookAt.getX(), lookAt.getY(), lookAt.getZ(),
                up.getX(), up.getY(), up.getZ() );
}

Point& Point::operator+=(Vector rhs) {
    this->setX( this->getX() + rhs.getX() );
    this->setY( this->getY() + rhs.getY() );
    this->setZ( this->getZ() + rhs.getZ() );
    return *this;
}


Point& Point::operator-=(Vector rhs) {
    this->setX( this->getX() - rhs.getX() );
    this->setY( this->getY() - rhs.getY() );
    this->setZ( this->getZ() - rhs.getZ() );
    return *this;
}

Point& Point::operator*=(float rhs) {
    this->setX( this->getX() * rhs );
    this->setY( this->getY() * rhs );
    this->setZ( this->getZ() * rhs );
    return *this;
}

Point& Point::operator/=(float rhs) {
    this->setX( this->getX() / rhs );
    this->setY( this->getY() / rhs );
    this->setZ( this->getZ() / rhs );
    return *this;
}

// MEMBER FUNCTIONS

double Point::at(int i) {
    if(i == 0)  return x;
    if(i == 1)  return y;
    if(i == 2)  return z;
    return -1;
}

void Point::glVertex() {
    glVertex3f(x, y, z);
}


