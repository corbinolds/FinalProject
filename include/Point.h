#ifndef _POINT_H_
#define _POINT_H_ 1

#include "PointBase.h"
#include "Vector.h"

class Point : public PointBase {
public:

	// CONSTRUCTORS / DESTRUCTORS
    Point();

	Point( int a, int b, int c );
    Point( double a, double b, double c );

	// MISCELLANEOUS
    double at(int i);

    void glVertex();

	Point& operator+=(Vector rhs);
	Point& operator-=(Vector rhs);
	Point& operator*=(float rhs);
	Point& operator/=(float rhs);

};

// RELATED OPERATORS

Point operator*(Point a, float f);
Point operator/(Point a, float f);
Point operator/(float f, Point p);
Point operator*(float f, Point a);
Vector operator-(Point a, Point b);
Point operator+(Point a, Point b); // cause I need this
Point operator+(Point a, Vector b);
Point operator+(Vector a, Point b);

void glTranslatef( Point p );
void gluLookAt( Point eye, Point lookAt, Vector up );

#endif
