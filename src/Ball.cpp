#include "Ball.h"

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <math.h>
#include <stdlib.h>

Ball::Ball() {
    radius = 0.5;
    location = Point(0,0,0);
    direction = Vector(1,0,0);
    _rotation = 0;
    _color = Point(rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5);
}

Ball::Ball( Point l, Vector d, double r ) : radius(r), location(l), direction(d) {
    _rotation = 0;
    _color = Point(rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5);
}

void Ball::draw() {
    float stacks = 16, slices = 16;
    float thetaStep = 6.28f / stacks;
    float phiStep = 6.28f / slices;
    
    direction.normalize();
    Vector rotationAxis = cross( direction, Vector(0,1,0) );
    
    glPushMatrix(); {
        glTranslatef( location );
        glTranslatef( 0, radius, 0 );
        glRotatef( _rotation, rotationAxis );
        glColor3f( _color.getX(), _color.getY(), _color.getZ() );
        
        for( float theta = 0; theta < 6.28; theta += thetaStep ) {
            for( float phi = -3.14; phi < 3.14; phi += phiStep ) {
                Point p1( radius * cos(theta) * sin(phi),
                          radius * cos(phi),
                          radius * sin(theta) * sin(phi) );
                Point p2( radius * cos(theta+thetaStep) * sin(phi),
                          radius * cos(phi),
                          radius * sin(theta+thetaStep) * sin(phi) );
                Point p3( radius * cos(theta) * sin(phi+phiStep),
                          radius * cos(phi+phiStep),
                          radius * sin(theta) * sin(phi+phiStep) );
                Point p4( radius * cos(theta+thetaStep) * sin(phi+phiStep),
                          radius * cos(phi+phiStep),
                          radius * sin(theta+thetaStep) * sin(phi+phiStep) );
            
                Vector n1 = p1 - Point(0,0,0);
                Vector n2 = p2 - Point(0,0,0);
                Vector n3 = p3 - Point(0,0,0);
                Vector n4 = p4 - Point(0,0,0);
            
                glBegin( GL_QUADS ); {
                    glTexCoord2f( theta / 6.28, (phi+3.14f) / 6.28 );
                    n1.glNormal();
                    p1.glVertex();
                
                    glTexCoord2f( (theta+thetaStep) / 6.28, (phi+3.14f) / 6.28 );
                    n2.glNormal();
                    p2.glVertex();
                
                    glTexCoord2f( (theta+thetaStep) / 6.28, ((phi+phiStep)+3.14f) / 6.28 );
                    n4.glNormal();
                    p4.glVertex();
                
                    glTexCoord2f( theta / 6.28, ((phi+phiStep)+3.14f) / 6.28 );
                    n3.glNormal();
                    p3.glVertex();
                } glEnd();
            }
        }
    }; glPopMatrix();
}

void Ball::moveForward(Point target) {
    Vector toTarget = (target - location);
    toTarget.normalize();
    Vector deltaVector = toTarget - direction;
    if(deltaVector.mag() > .1) {
        direction = direction + .02*deltaVector;
        direction.normalize();
    }
    location += direction*0.1;
    _rotation -= ((360.0f / 64.0f));
    if( _rotation < 0 ) {
        _rotation += 360;
    }
}

void Ball::moveBackward() {
    location -= direction*0.1;
    _rotation += ((360.0f / 64.0f));
    if( _rotation > 360 ) {
        _rotation -= 360;
    }
}