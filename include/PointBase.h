#ifndef _POINTBASE_H_
#define _POINTBASE_H_ 1

class PointBase {
public:
	// GETTERS / SETTERS
	double getX();
	void setX( double x );
	double getY();
	void setY( double y );
	double getZ();
	void setZ( double z );
	
	double getW();

	double* asVector();
	
protected:
	// MEMBER VARIABLES
	double x,y,z,w;
};

#endif
