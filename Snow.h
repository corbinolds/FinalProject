#ifndef _SNOW_H_
#define _SNOW_H_ 1

#include "Point.h"
#include "Vector.h"

class Snow {
public:

	// CONSTRUCTORS / DESTRUCTORS
    Snow();
	Snow( Point l, Vector d, double r );

	// MISCELLANEOUS
    double radius;
    Point location;
    Vector direction;
	float spriteXWidth;
	float spriteYWidth;
    
    void draw();
    void moveForward();
    void moveBackward();
	void drawTexturedSprite( float spriteXWidth, float spriteYWidth );

	void Fall(); 
    
private:
    double _rotation;
    Point _color; // a cheat because I'm lazy
};

#endif
