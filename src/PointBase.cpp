#include "PointBase.h"
#include <stdlib.h>

double PointBase::getX() { return x; }

void PointBase::setX( double x ) { this->x = x; }

double PointBase::getY() { return y; }

void PointBase::setY( double y ) { this->y = y; }

double PointBase::getZ() { return z; }

void PointBase::setZ( double z ) { this->z = z; }

double PointBase::getW() { return w; }

double* PointBase::asVector() {
	double* _vec = (double*)malloc( 4 * sizeof(double) );
	_vec[0] = x;
	_vec[1] = y;
	_vec[2] = z;
	_vec[3] = w;
	return _vec;
}
