#include "Snow.h"

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <math.h>
#include <stdlib.h>

Snow::Snow() {
    radius = 0.5;
    location = Point(0,0,0);
    direction = Vector(1,0,0);
    _rotation = 0;
    _color = Point(rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5);
	spriteXWidth = 0.1;
	spriteYWidth = 0.1;
}

Snow::Snow( Point l, Vector d, double r ) : radius(r), location(l), direction(d) {
    _rotation = 0;
    _color = Point(rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5, rand() * 100 % 50 / 100.0 + 0.5);
}

void Snow::draw() {
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
void Snow::moveForward() {
    location += direction*0.1;
    _rotation -= ((360.0f / 64.0f));
    if( _rotation < 0 ) {
        _rotation += 360;
    }
}

void Snow::Fall() {
	location += Vector(0,1,0)*-0.1; 
}

void Snow::drawTexturedSprite( float spriteXWidth, float spriteYWidth ) {
    glBegin( GL_QUADS ); {
        glNormal3f( 0, 0, 1 );
        glTexCoord2f( 0, 0.01 );
        glVertex3f( -(spriteXWidth/2.0f), spriteYWidth, 0 );
        
        glNormal3f( 0, 0, 1 );
        glTexCoord2f( 0, 1 );
        glVertex3f( -(spriteXWidth/2.0f),         0.0f, 0 );
        
        glNormal3f( 0, 0, 1 );
        glTexCoord2f( 1, 1 );
        glVertex3f( (spriteXWidth/2.0f),         0.0f, 0 );
        
        glNormal3f( 0, 0, 1 );
        glTexCoord2f( 1, 0.01 );
        glVertex3f( (spriteXWidth/2.0f), spriteYWidth, 0 );
    }; glEnd();
}


void Snow::moveBackward() {
    location -= direction*9.8;
    _rotation += ((360.0f / 64.0f));
    if( _rotation > 360 ) {
        _rotation -= 360;
    }
}
