/*
 *  CSCI 441, Computer Graphics, Fall 2016
 *
 *  Project: FP
 *  File: main.cpp
 *
 *  Description:
 *      FINAL PROJEEEEEEEEEEEEECT. Kill all the enemies!
 *
 *
 *  Authors:
 * 		Corbin Olds 
 *		Vanessa Ramos 
 *      Nicholas Zustak
 *
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *
 *  Notes:
 *
 *
 */


// ALLLLLLLL of the includes that we use
#include <GL/glew.h>

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <SOIL/soil.h>

#include "include/Shader_Utils.h";
#include "Point.h"
#include "Vector.h"
#include "Ball.h"
#include "Snow.h"
#include "Snow.cpp"
#include "ParticleSystem.h"
#include "ParticleSystem.cpp"
#include <time.h>
#include <algorithm>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <list>
#include <string>
#include <vector>


using namespace std;

// Snow Variables 

float SnowRadius = 0.1;                     // the base radius of all our spheres (when created, the spheres will
                                                // have an actual radius of SnowRadius +/- rand()
int SnowsTimer = 0;                         // this controls how often to call populate Snows
int SnowsTimerTwo = 0; 
int SnowsTimerThree = 0;
int SnowsTimerFour = 0; 
GLuint snowTexture;
ParticleSystem part;
// Adding trees  
GLuint treeTexHandle;// trees
Point forestCenter;                         // center of our forest
vector< Point > treeSpritePositions;
Vector view = Vector(0.0, 0.0, 0.0);
Vector XZCamera;
Vector XZView;
Vector viewV; 
bool startOver = false; 


//ADDING LIGHTING 
bool light1On = true;
GLuint environmentDL;

//CHANGE 2PM SHADERS
GLuint shaderProgramHandle = 0;
GLuint uniformTimeLoc = 0;
GLuint uniformDressColorLoc = 0;


// GLOBAL VARIABLES ////////////////////////////////////////////////////////////

int windowWidth = 512, windowHeight = 512;

GLint leftMouseButton, rightMouseButton;    // status of the mouse buttons
int mouseX = 0, mouseY = 0;                 // last known X and Y of the mouse

float cameraTheta, cameraPhi, cameraRadius; // camera position in spherical coordinates
Point cameraXYZ;                            // camera position in cartesian coordinates

GLuint brickTexHandle, groundTexHandle;     // a handle to the textures in our OpenGL context
GLuint skyboxHandles[6];                    // all of our skybox handles

vector< Ball* > balls;                      // a collection of the balls in our scene

// some parameters to control the running of our program
float groundSize = 50;                      // the size of our ground plane and therefore bounding box
float ballRadius = 0.5;                     // the base radius of all our spheres (when created, the spheres will
                                                // have an actual radius of ballRadius +/- rand()
int numBalls = 5;                          // the number of balls in our scene

GLUquadric* powerup;
Ball* shot;
int shotStep;

// Car (hero) variables)
float carX, carY, carZ;                     // car position in cartesian coordinates
float powerX, powerZ;
float carHeading = 0;                           // heading in radians
float speed = 0;                            // car speed
float tires = 0.0;                          // tire angle in radians
float freeFallAngle = 0.0;                  // falling angle
float bob;
int pulse = 50;                                  // changes the color of the aura around Crane
int health = 100;
bool bobup;
bool pulseflip = false;
bool moveLegFlip = false;
bool attackMode = false;
bool carfreefall = false;
bool powerupAchieved = false;
bool firing = false;
float moveLeg = 0;
float rotateHand = 0;
int easterEgg = 0;

// GAME STATES
const int BEFORE = 0;
const int KILLTHEM = 1;
const int DEAD = 2;
const int WON = 3;
int gameState = BEFORE;

// END GLOBAL VARIABLES ///////////////////////////////////////////////////////

//
// void computeArcballPosition(float theta, float phi, float radius,
//                              float &x, float &y, float &z);
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. The user passes in the current phi,
//  theta, and radius, as well as variables to hold the resulting X, Y, and Z values.
//  Those variables for X, Y, and Z values get filled with the camera's position in R3.
//
void computeArcballPosition(float theta, float phi, float radius,
                            Point &xyz) {
    xyz.setX( radius * sinf(theta)*sinf(phi) );
    xyz.setY( radius * -cosf(phi) );
    xyz.setZ( radius * -cosf(theta)*sinf(phi) );
}

float getRand() {
	return rand() / (float)RAND_MAX;
}

float getHeightRand() {
	return rand() % 50 + 1;
}


// Added for trees 

bool registerTransparentOpenGLTexture(unsigned char *imageData, unsigned char *imageMask, int texWidth, int texHeight, GLuint &texHandle) {
    // combine the 'mask' array with the image data array into an RGBA array.
    unsigned char *fullData = new unsigned char[texWidth*texHeight*4];
    for(int j = 0; j < texHeight; j++) {
        for(int i = 0; i < texWidth; i++) {
            fullData[(j*texWidth+i)*4+0] = imageData[(j*texWidth+i)*3+0];   // R
            fullData[(j*texWidth+i)*4+1] = imageData[(j*texWidth+i)*3+1];   // G
            fullData[(j*texWidth+i)*4+2] = imageData[(j*texWidth+i)*3+2];   // B
            fullData[(j*texWidth+i)*4+3] = imageMask[(j*texWidth+i)*3+0];   // A
        }
    }
    
    // first off, get a real texture handle.
    glGenTextures(1, &texHandle);
    
    // make sure that future texture functions affect this handle
    glBindTexture(GL_TEXTURE_2D, texHandle);
    
    // set this texture's color to be multiplied by the surface colors --
    //  GL_MODULATE instead of GL_REPLACE allows us to take advantage of OpenGL lighting
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // set the minification and magnification functions to be linear; not perfect
    //  but much better than nearest-texel (GL_NEAREST).
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Set the texture to repeat in S and T -- though it doesn't matter here
    //  because our texture coordinates are always in [0,0] to [1,1].
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // actually transfer the texture to the GPU and use MipMaps!!
    /*    gluBuild2DMipMaps(
     GL_TEXTURE_2D,              //still working with 2D textures, obv.
     GL_RGBA,                    //we're going to provide OpenGL with R, G, B, and A components...
     texWidth,                   //...of this width...
     texHeight,                  //...and this height.
     GL_RGBA,                    //and store it, internally, as RGBA (OpenGL will convert to floats between 0.0 and 1.0f)
     GL_UNSIGNED_BYTE,           //this is the format our data is in, and finally,
     fullData);                  //there's the data!
     */
    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, texWidth, texHeight, GL_RGBA, GL_UNSIGNED_BYTE, fullData );
    
    // clean up our memory on the CPU side since it's already been transfered to the GPU
    delete fullData;
    
    // whoops! i guess this function can't fail. in an ideal world, there would
    // be checks with glGetError() that we could use to determine if this failed.
    return true;
}



//
//  void resizeWindow(int w, int h)
//
//      We will register this function as GLUT's reshape callback.
//   When the window is resized, the OS will tell GLUT and GLUT will tell
//   us by calling this function - GLUT will give us the new window width
//   and height as 'w' and 'h,' respectively - so save those to our global vars.
//
void resizeWindow(int w, int h) {
    float aspectRatio = w / (float)h;
    
    windowWidth = w;
    windowHeight = h;
    
    // update the viewport to fill the window
    glViewport(0, 0, w, h);
    
    // update the projection matrix with the new window properties
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0,aspectRatio,0.1,100000);
}

//
//  void mouseCallback(int button, int state, int thisX, int thisY)
//
//  GLUT callback for mouse clicks. We save the state of the mouse button
//      when this is called so that we can check the status of the mouse
//      buttons inside the motion callback (whether they are up or down).
//
void mouseCallback(int button, int state, int thisX, int thisY) {
    //update the left and right mouse button states, if applicable
    if(button == GLUT_LEFT_BUTTON)
    leftMouseButton = state;
    else if(button == GLUT_RIGHT_BUTTON)
    rightMouseButton = state;
    
    //and update the last seen X and Y coordinates of the mouse
    mouseX = thisX;
    mouseY = thisY;
}


// This function will be used with the sort algorithm 
bool myfunction(Point i, Point j) {
    //the distance for each sprite is the dot product of our spriteVector
    //with the viewVector.Now sort by distance, greatest to smallest.
    Vector viewVector = Vector(-cameraXYZ.getX(), 0.0, -cameraXYZ.getZ()); 
    Vector spriteVectori = Vector(i.getX() - cameraXYZ.getX(), 0.0, i.getZ() - cameraXYZ.getZ());
    Vector spriteVectorj = Vector(j.getX() - cameraXYZ.getX(), 0.0, j.getZ() - cameraXYZ.getZ());

    if (dot(viewVector, spriteVectori) > dot(viewVector, spriteVectorj))
    {
        return true; 
    }
    else {
        return false; 
    }
}


//
//  void mouseMotion(int x, int y)
//
//  GLUT callback for mouse movement. We update cameraPhi, cameraTheta, and/or
//      cameraRadius based on how much the user has moved the mouse in the
//      X or Y directions (in screen space) and whether they have held down
//      the left or right mouse buttons. If the user hasn't held down any
//      buttons, the function just updates the last seen mouse X and Y coords.
//
void mouseMotion(int x, int y) {
    // mouse is moving; if left button is held down...
    if(leftMouseButton == GLUT_DOWN ) {
        
        // update theta and phi!
        cameraTheta += (x - mouseX)*0.005;
        cameraPhi   += (y - mouseY)*0.005;
        
        // we don't care if theta goes out of bounds; it'll just loop around.
        // phi, though.. it'll flip out if it flips over top. keep it in (0, M_PI)
        if(cameraPhi <= 0)
        cameraPhi = 0+0.001;
        if(cameraPhi >= M_PI)
        cameraPhi = M_PI-0.001;
        
        // update camera (x,y,z) based on (radius,theta,phi)
        computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
                               cameraXYZ);
    } else if( rightMouseButton == GLUT_DOWN ) {
        // for the right mouse button, just determine how much the mouse has moved total.
        // not the best "zoom" behavior -- if X change is positive and Y change is negative,
        // (along the X = Y line), nothing will happen!! but it's ok if you zoom by
        // moving left<-->right or up<-->down, which works for most people i think.
        double totalChangeSq = (x - mouseX) + (y - mouseY);
        cameraRadius += totalChangeSq*0.01;
        
        // limit the camera radius to some reasonable values so the user can't get lost
        if(cameraRadius < 2.0)
        cameraRadius = 2.0;
        if(cameraRadius > 150.0)
        cameraRadius = 150.0;
        
        // update camera (x,y,z) based on (radius,theta,phi)
        computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
                               cameraXYZ);
    }
    
    // and save the current mouseX and mouseY.
    mouseX = x;
    mouseY = y;
}

// drawTrees() //////////////////////////////////////////////////////////////////
//
//  Function to draw a random city using GLUT 3D Primitives
//
////////////////////////////////////////////////////////////////////////////////
void drawCity() {
	// TODO #4: Randomly place buildings of varying heights with random colors
	float color = 0.0;
	float height = 0.0;

	// glPushMatrix();
	glBegin(GL_LINES);
	height = getHeightRand();

	for (int x = -50; x < 50; x++)
	{

		for (int y = -50; y < 50; y++)
		{
			if (getRand() < 0.1 && (x % 6 == 0) && (y % 6 == 0))
			{
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT6);

				// Create light components.
				GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
				GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
				GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
				GLfloat position[] = { 0.0f, 15.0f, 0.0f, 1.0f };

				// Assign created components to GL_LIGHT0.
				glLightfv(GL_LIGHT6, GL_AMBIENT, ambientLight);
				glLightfv(GL_LIGHT6, GL_DIFFUSE, diffuseLight);
				glLightfv(GL_LIGHT6, GL_SPECULAR, specularLight);
				glLightfv(GL_LIGHT6, GL_POSITION, position);

				glEnable(GL_LIGHT7);
				GLfloat positionTwo[] = { 0.0f, 15.0f, 100.0f, 1.0f };
				glLightfv(GL_LIGHT7, GL_AMBIENT, ambientLight);
				glLightfv(GL_LIGHT7, GL_DIFFUSE, diffuseLight);
				glLightfv(GL_LIGHT7, GL_SPECULAR, specularLight);
				glLightfv(GL_LIGHT7, GL_POSITION, positionTwo);

				glEnable(GL_LIGHT5);
				GLfloat positionThree[] = { 100.0f, 15.0f, 0.0f, 1.0f };
				glLightfv(GL_LIGHT5, GL_AMBIENT, ambientLight);
				glLightfv(GL_LIGHT5, GL_DIFFUSE, diffuseLight);
				glLightfv(GL_LIGHT5, GL_SPECULAR, specularLight);
				glLightfv(GL_LIGHT5, GL_POSITION, positionThree);


	
				height = getHeightRand();
				glPushMatrix();
				GLfloat materialColor[] = { getRand(), getRand(), getRand(), 1.0f };
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
				glTranslatef(x, 2.0, y);
				glScalef(1.0, 4.0, 1.0);
				glutSolidCube(1.0);
				glPopMatrix();


				glPushMatrix();
				//glColor3f(0.0, 1.0, 0.0);
				GLfloat materialColorCone[] = {0.0, 1.0,  0.0, 1.0f };
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColorCone);
				glTranslatef(x, 3.0, y);
				glRotated(95, -1.0, 0.0, 0.0);
				glScalef(1.0, 1.0, 3.0);
				glutSolidCone(2.0, 2.0, 50.0, 50.0);
				glPopMatrix();
			}
			//  glColor3f(color*1.0, color*1.0, color*1.0);

		}


	}
	glEnd();
	// glPopMatrix();

}



//
//  void initScene()
//
//  A basic scene initialization function; should be called once after the
//      OpenGL context has been created. Doesn't need to be called further.
//      Just sets up a few function calls so that our main() is cleaner.
//
void initScene() {
    // tell OpenGL to use a depth test
    glEnable(GL_DEPTH_TEST);
    
    // tell OpenGL not to use the material system; just use whatever we pass with glColor()
    //glEnable( GL_COLOR_MATERIAL );
   // glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    
    // tells OpenGL to not blend colors across triangles
    glShadeModel( GL_SMOOTH );

    //Added for tress 
        // tells OpenGL to not blend colors across triangles
    glShadeModel( GL_FLAT );
    
    // fill up the treeSpritePositions with some meaningful information...
    srand( time(NULL) );
    forestCenter = Point(0, 0, 0);
    int numSprites = 10;
    float rangeX = groundSize;
    float rangeZ = groundSize;
    for(int i = 0; i < numSprites; i++) {
        // TODO #1: Populate our sprite locations
        double x = -rangeX + (double) (rand()/(float)RAND_MAX * rangeX);
        double z = -rangeZ + (double) (rand()/(float)RAND_MAX * rangeZ);
        treeSpritePositions.push_back(Point(x, 0.0, z));
        printf("\nNumber of sprites in vector: %d", treeSpritePositions.size());
    }
	drawCity();
    
    // and enable alpha blending once and for all.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}




//
//  void drawTexturedSprite()
//
//  Draws a textured quad in the XY plane
//
void drawTexturedSprite( float spriteXWidth, float spriteYWidth ) {
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

// bubbleSort cause can't be f***'d to implement something better
void bubbleSortPositions(vector<Point> &positions) {
    bool swap = true;
    while(swap) {
        swap = false;
        for(int i = 0; i < positions.size() - 1; i++) {
            Point first = positions[i];
            Point second = positions[i+1];
            Vector spriteVector1 = Vector(first.getX() - XZCamera.getX(), 0.0, first.getZ() - XZCamera.getZ());
            Vector spriteVector2 = Vector(second.getX() - XZCamera.getX(), 0.0, second.getZ() - XZCamera.getZ());
            double distance1 = dot(spriteVector1, XZView);
            double distance2 = dot(spriteVector2, XZView);
            // we want the bigger distance to be first in the vector
            if(distance2 < distance1) {
                Point temp = Point(first.getX(), first.getY(), first.getZ());
                positions[i] = positions[i+1];
                positions[i+1] = temp;
                swap = true;
            }
        }
    }
}


//
//  void populateBalls()
//
//  Randomly create numBalls balls.  Each ball will be randomly placed on our ground
//      plane.  It will be pointed in a random direction and will have a random
//      radius.  It IS possible that two spheres are created in the same place.
//      We'll just live with this result and restart the program if it happens.
//
void populateBalls() {
    srand( time(NULL) );
    float rangeX = groundSize*0.25;
    float rangeZ = groundSize*2;
    
    // for each ball
    for(int i = 0; i < numBalls; i++) {
        // create a new ball

		glEnable(GL_LIGHT0);
		float diffuseCol[4] = { .8,.008,.008,.010 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseCol);
		float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientCol);
		float lPosition[4] = { groundSize*0.25 + rand() / (float)RAND_MAX * rangeX, 0,  (rangeZ * (i / (float)numBalls)) - rangeZ / 2.0f, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, lPosition);

        balls.push_back(  new Ball( Point( groundSize*0.25 + rand()/(float)RAND_MAX * rangeX,                         // on one half of the map
                                           0.0f,                                                        // Y
                                           (rangeZ * (i/(float)numBalls)) - rangeZ/2.0f),               // Z
                                    Vector( rand()/(float)RAND_MAX, 0.0, rand()/(float)RAND_MAX ),  // with a random direction
                                    ballRadius ) );         
		// and a constant radius
    }
}

//
//  void drawSkyboxPanel( ...params... )
//
//  Draws a single textured quad.  This became a bit more complex than intended, but it
//      allows for a generic definition.  We specify the two in-plane dimensions (dim1
//      and dim2) to determine our four corners.  These corners are centered around
//      point and are size distance away.  The quad is oriented with the provided
//      normal.  The provided texture is applied to the quad and can be flipped
//      along either axis if needed.
//
void drawSkyboxPanel( GLuint texHandle, Point point, Vector dim1, Vector dim2, Vector normal, int flippingFactor, int flippingFactor2, int size ) {
    // we're cheating by making our quads intersect at the edges to try and remove the joint line we sometimes see
    Point p1 = point + dim1*-(size+1) + dim2*-(size+1);  // LL corner
    Point p2 = point + dim1*(size+1) + dim2*-(size+1);   // LR corner
    Point p3 = point + dim1*(size+1) + dim2*(size+1);    // UR corner
    Point p4 = point + dim1*-(size+1) + dim2*(size+1);   // UL corner
    
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texHandle );
    glPushMatrix(); {
   //     glColor4f(1,1,1,1);
        glBegin( GL_QUADS ); {
            glTexCoord2f(0, 0);
            normal.glNormal();
            p1.glVertex();
            
            glTexCoord2f(1*flippingFactor, 0);
            normal.glNormal();
            p2.glVertex();
            
            glTexCoord2f(1*flippingFactor, 1*flippingFactor2);
            normal.glNormal();
            p3.glVertex();
            
            glTexCoord2f(0, 1*flippingFactor2);
            normal.glNormal();
            p4.glVertex();
        }; glEnd();
    }; glPopMatrix();
    glDisable( GL_TEXTURE_2D );
}

//
//  void drawSkybox( int size )
//
//  Draws the six planes of our sky box.  Texturing each and having them oriented the correct way.
//
void drawSkybox( int size ) {
    //                 texHandle             Point                  dim1            dim2            Normal         flip   size          panel
    drawSkyboxPanel( skyboxHandles[0], Point(   -1,     0, -size), Vector(1,0,0), Vector(0,1,0), Vector( 0, 0, 1), -1,  1, size );     // back
    drawSkyboxPanel( skyboxHandles[1], Point(-size,     0,    -1), Vector(0,0,1), Vector(0,1,0), Vector( 1, 0, 0),  1,  1, size );     // left
    drawSkyboxPanel( skyboxHandles[2], Point(    1,     0,  size), Vector(1,0,0), Vector(0,1,0), Vector( 0, 0,-1),  1,  1, size );     // front
    drawSkyboxPanel( skyboxHandles[3], Point( size,     0,     1), Vector(0,0,1), Vector(0,1,0), Vector(-1, 0, 0), -1,  1, size );     // right
    drawSkyboxPanel( skyboxHandles[4], Point(    0, -size,     0), Vector(1,0,0), Vector(0,0,1), Vector( 0, 1, 0), -1, -1, size );     // bottom
    drawSkyboxPanel( skyboxHandles[5], Point(    0,  size,     0), Vector(1,0,0), Vector(0,0,1), Vector( 0,-1, 0), -1,  1, size );     // top
    //                 texHandle             Point                  dim1            dim2            Normal         flip   size          panel
}

void drawWheel() {
    glutSolidTorus(2, 4, 6, 6);
}

//CHANGE 2PM
void setUpShaders() {
	string firstFile = "shaders/customShader.v.glsl";
	string secondFile = "shaders/customShader.f.glsl";

	shaderProgramHandle = createShaderProgram((char *)firstFile.c_str(), (char *)secondFile.c_str());

	uniformTimeLoc = glGetUniformLocation(shaderProgramHandle, "time");
	uniformDressColorLoc = glGetUniformLocation(shaderProgramHandle, "dressColor");
}
//END OF CHANGE 2PM

// drawCar() //////////////////////////////////////////////////////////////////
//
//  Function to draw a car using GLUT 3D Primitives
//
////////////////////////////////////////////////////////////////////////////////
void drawCar() {
    
    // drawWheels
    // make 'em gray
    
    glPushMatrix();
    glScalef(.2, .2, .2);
    {
   // glColor3f(2.0/255, 2.0/255, 2.0/255);
	GLfloat materialColorCar[] = { 2.0 / 255,2.0 / 255, 2.0 / 255, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColorCar);
    glPushMatrix();
    glTranslatef(-10, 5, 10);
    glRotatef(tires, 0, 0, 1.0);
    drawWheel();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(10, 5, 10);
    glRotatef(tires, 0, 0, 1.0);
    drawWheel();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-10, 5, -10);
    glRotatef(tires, 0, 0, 1.0);
    drawWheel();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(10, 5, -10);
    glRotatef(tires, 0, 0, 1.0);
    drawWheel();
    glPopMatrix();

    // draw the aura around the car

    glPushMatrix();
    glTranslatef(0, 5, 0);
    if(pulse >= 100) {
        pulseflip = true;
    }
    if(pulse <= 0) {
        pulseflip = false;
    }
    if(pulseflip) {
        pulse--;
    }
    else{
        pulse++;
    }
    if(attackMode) {
        glLineWidth(3.0f);  
       // glColor3f(float(120+pulse)/255.0, (120+pulse)/255.0, 1.0);
    }
    else {
       // glColor3f((255.0 - float(health)*2.55)/255.0, (float(health)*2.55)/255.0, 1.0); 
    }
    glDisable( GL_LIGHTING );
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 30; i++)
    {
        float theta = 2.0 * M_PI * float(i) / float(30);

        float x = 18 * cosf(theta);
        float z = 18 * sinf(theta);

        glVertex3f(x, 0.0, z);

    }
   glEnable( GL_LIGHTING );


    glEnd();
    glPopMatrix();
    
    // draw car body
        
    glPushMatrix();
   // glColor3f(0, 110.0/255, 1.0);
    glTranslatef(0, 5, 0);
    glScalef(20, 5, 20);
    glutSolidCube(1.0);
    glPopMatrix();
        
    // draw engine sphere thing
    glPushMatrix();
   // glColor3f(0, 200/255, 1.0);
    glTranslatef(8.0, 5.0 + bob, 0);
    glutSolidSphere(3.5, 10.0, 10.0);
    glPopMatrix();
        
    }
    glPopMatrix();
    
}

void drawCones (){

    glPushMatrix();
  //  glColor3f( 1.0,  1.34, 0.0);
    glTranslatef(2.5, 1.0, 1.0);
    glRotated(-270, 0.0, 1.0, 0.0);
    glScalef(0.25, 0.25, 0.1);
    glutSolidCone(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();

    glPushMatrix();
   // glColor3f( 1.0,  1.34,  0.0);
    glTranslatef(2.5, 1.0, -1.0);
    glRotated(-270, 0.0, 1.0, 0.0);
    glScalef(0.25, 0.25, 0.1);
    glutSolidCone(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();

    glPushMatrix();
 //   glColor3f( 1.0,  0.0,  0.0);
    glTranslatef(-0.5, 3.0, 0.0);
    glScalef(3.0, 1.5, 3.5);
    glutSolidCube(1.0);
    glPopMatrix();

}

void drawHead() {

    glPushMatrix();
   // glColor3f( 1, 0.894118, 0.768627);
    glTranslatef(0, 7.0, 0.0);
    glutSolidSphere(1.0, 50, 50);
    glPopMatrix();

}



void drawDress() {

	//CHANGE 2PM SHADER
	GLfloat materialColor[] = { 0.541176, 0.168627,  0.886275, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
	glPushMatrix(); {
		if (health <= 50 && !attackMode) {
			glUseProgram(shaderProgramHandle);

			bool isRed = true;
			glUniform1f(uniformDressColorLoc, isRed);
		}
		if (attackMode) {
			glUseProgram(shaderProgramHandle);


			float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
			glUniform1f(uniformTimeLoc, currentTime);

			bool isRed = false;
			glUniform1f(uniformDressColorLoc, isRed);
		}

		glTranslatef(0, 3.0, 0.0);
		glRotated(-90, 1.0, 0.0, 0.0);
		glScalef(1.0, 1.0, 2.0);
		glutSolidCone(2.0, 2.0, 50.0, 50.0);

		glUseProgram(0);

	}; glPopMatrix();
	//END CHANGE 2PM
}



void drawHands()

{

	glPushMatrix(); {
		if (health <= 50 && !attackMode) {
			glUseProgram(shaderProgramHandle);

			bool isRed = true;
			glUniform1f(uniformDressColorLoc, isRed);
		}
		else if (attackMode) {
			glUseProgram(shaderProgramHandle);


			float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
			glUniform1f(uniformTimeLoc, currentTime);

			bool isRed = false;
			glUniform1f(uniformDressColorLoc, isRed);
		}
		else {
			GLfloat materialColor[] = { 0.541176, 0.168627,  0.886275, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
			//glColor3f(0.541176, 0.168627, 0.886275);
		}
		glPushMatrix();
		glTranslatef(-.80, 5.0, -0.2);
		glRotated(90 + rotateHand, 1.0, 0.0, 0.0);
		glRotated(45, 0.0, 0.0, 1.0);
		glScalef(0.5, 2.0, 0.5);
		glutSolidCube(1.0);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(.80, 5.0, -0.2);
		glRotated(90 + rotateHand, 1.0, 0.0, 0.0);
		glRotated(-45, 0.0, 0.0, 1.0);
		glScalef(0.5, 2.0, 0.5);
		glutSolidCube(1.0);
		glPopMatrix();

		glUseProgram(0);

	} glPopMatrix();

   /* glPushMatrix();
    glColor3f( 0.541176, 0.168627, 0.886275);
    glTranslatef(-.80, 5.0, -0.2);
    glRotated(90 + rotateHand, 1.0, 0.0, 0.0);
    glRotated(45, 0.0, 0.0, 1.0);
    glScalef(0.5, 2.0, 0.5);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.541176, 0.168627, 0.886275);
    glTranslatef(.80, 5.0, -0.2);
    glRotated(90 + rotateHand, 1.0, 0.0, 0.0);
    glRotated(-45, 0.0, 0.0, 1.0);
    glScalef(0.5, 2.0, 0.5);
    glutSolidCube(1.0);
    glPopMatrix();*/
}



void drawEars() {

    glPushMatrix();
   // glColor3f( 1, 0.894118, 0.768627);
    glTranslatef(.5, 7.75, 0.0);
    glRotated(270, 1.0, 0.0, 0.0);
    glScalef(.25, .25, .40);
    glutSolidCone(1.0, 2.0, 50.0, 50.0);
    glPopMatrix();


    glPushMatrix();
  //  glColor3f( 1, 0.894118, 0.768627);
    glTranslatef(-0.5, 7.75, 0.0);
    glRotated(270, 1.0, 0.0, 0.0);
    glScalef(.25, .25, .40);
    glutSolidCone(1.0, 2.0, 50.0, 50.0);
    glPopMatrix();

}



void drawEyes() {

    glPushMatrix();
  //  glColor3f(0,  0.0,  0);
    glTranslatef(-0.45, 7.0, 1.0);
    glScalef(0.025, 0.05, 0.025);
    glutSolidTorus(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();



    glPushMatrix();
   // glColor3f(0,  0.0,  0);
    glTranslatef(0, 7.0, 1.0);
    glScalef(0.025, 0.05, 0.025);
    glutSolidTorus(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();

}



void drawLegs() {

    glPushMatrix();
  //  glColor3f(0.866667, 0.627451, 0.866667);
    glTranslatef(-.75, 2.0, 0.0 + moveLeg);
    glScalef(0.5, 2.0, 0.5);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
 //   glColor3f(0.866667, 0.627451, 0.866667);
    glTranslatef(0.75, 2.0, 0.0 - moveLeg);
    glScalef(0.5, 2.0, 0.5);
    glutSolidCube(1.0);
    glPopMatrix();

    if (moveLeg >=1.5)
    {
        moveLegFlip = true;
    }
    if (moveLeg <= 0) {
        moveLegFlip = false;
    }

}

// This function writes the names of the characters

void drawName(char scrtext[]) {

    glPushMatrix();
	GLfloat materialColor[] = { 1.0,1.0, 1.0, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
    glTranslatef(0, 5, 0);
    glRotatef(90, 0.0,1.0,0.0);
    glScalef(.005,.005,.1);
    for (int c=0; scrtext[c] != 0; ++c)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, scrtext[c]);
    glPopMatrix();

}


void drawHuman()

{
    drawHead();
    drawDress();
	glPushMatrix();
	glTranslatef(0, .25, 0);
	drawLegs();
	glPopMatrix();
    drawHands();
    drawEars();
    drawEyes();
    glPushMatrix();
    glTranslatef(0.0, 4, 0.0);
    drawName("Artemis");
    glPopMatrix();
}


void drawArtemis()

{
    glPushMatrix();
  //  glColor3f( 1.0,  0.0,  0.0);
  //  glColor3f(getRand(), getRand(), getRand() + 0.5);
    glTranslatef(0.0, 1.0, 0.0);
    glScalef(5.0, 2.0, 4.0);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
	GLfloat materialColor[] = { 0.833,  0.0,  0.34, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
    glScalef(0.3, 0.3, 0.3);
    glutSolidTorus(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();

    glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
    glTranslatef(-3.0, 1.0, 2.0);
    glScalef(0.3, 0.3, 0.3);
    glutSolidTorus(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();

    glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
    glTranslatef(3.0, 1.0, 2.0);
    glScalef(0.3, 0.3, 0.3);
    glutSolidTorus(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();

    glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
    glTranslatef(3.0, 1.0, -2.0);
    glScalef(0.3, 0.3, 0.3);
    glutSolidTorus(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();

    glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
    glTranslatef(-3.0, 1.0, -2.0);
    glScalef(0.3, 0.3, 0.3);
    glutSolidTorus(2.0, 2.0, 50.0, 50.0);
    glPopMatrix();

    drawCones();

    glPushMatrix();
    drawName("Artemis");
    glPopMatrix();

}

void newGame() 
{
	balls.clear(); 
	populateBalls(); 
	gameState = BEFORE; 
	pulseflip = false;
	moveLegFlip = false;
	attackMode = false;
	carfreefall = false;
	powerupAchieved = false;
	firing = false;
	health = 100; 
	glEnable(GL_LIGHT0);
	freeFallAngle = 0; 
	carY = 0; 
	carX = 0; 
	carZ = 0; 
	powerup = gluNewQuadric();
    gluQuadricNormals(powerup, GLU_SMOOTH);
    powerX = -(float)groundSize + 2*(rand()/(float)RAND_MAX * groundSize);
    powerZ = -(float)groundSize + 2*(rand()/(float)RAND_MAX * groundSize);
}

void shoot() {
    shot = new Ball(
            Point(carX, carY, carZ),
            Vector( cos(carHeading*(M_PI/180))*150, 0.0, -sin(carHeading*(M_PI/180))*150),  // shoot in the direction of artemis
            ballRadius );
    // printf("Shot xdir: %f ydir: %f", cos(carHeading*(M_PI/180))*50, -sin(carHeading*(M_PI/180))*50);
    firing = true;
    shotStep = 0;
}


// generateEnvironmentDL() /////////////////////////////////////////////////////
//
//  This function creates a display list with the code to draw a simple 
//      environment for the user to navigate through.
//
//  And yes, it uses a global variable for the display list.
//  I know, I know! Kids: don't try this at home. There's something to be said
//      for object-oriented programming after all.
//
////////////////////////////////////////////////////////////////////////////////
void generateEnvironmentDL() {
    int gridX = 100;
    int gridY = 100;
    float spacing = 0.5f;

    environmentDL = glGenLists(1);
    glNewList(environmentDL, GL_COMPILE);

	glEnable(GL_LIGHT1);
	float diffuseCol[4] = { 0.0,0.12,0.12,.30 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseCol);
	float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientCol);
	float lPosition[4] = {0, 0, 10, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, lPosition);

	glEnable(GL_LIGHT2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseCol);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientCol);
	float lPosition2[4] = { 20, 0, 0, 1.0 };
	glLightfv(GL_LIGHT2, GL_POSITION, lPosition2);

	glEnable(GL_LIGHT3);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuseCol);
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambientCol);
	float lPosition3[4] = { 10, 0, 0, 1.0};
	glLightfv(GL_LIGHT3, GL_POSITION, lPosition3);

	glEnable(GL_LIGHT4);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, diffuseCol);
	glLightfv(GL_LIGHT4, GL_AMBIENT, ambientCol);
	float lPosition4[4] = { -10, 0, 0, 1.0 };
	glLightfv(GL_LIGHT4, GL_POSITION, lPosition4);

	glEnable(GL_LIGHT5);
	float diffuseColLast[4] = { 0.10,0.22,0.002,.0 };
	glLightfv(GL_LIGHT5, GL_DIFFUSE, diffuseColLast);
	glLightfv(GL_LIGHT5, GL_AMBIENT, ambientCol);
	float lPosition5[5] = {0, 15, 0, 1.0 };
	glLightfv(GL_LIGHT5, GL_POSITION, lPosition5);

	drawCity(); 

    glEndList();
}

//
//  void renderScene()
//
//  GLUT callback for scene rendering. Sets up the modelview matrix, renders
//      a teapot to the back buffer, and switches the back buffer with the
//      front buffer (what the user sees).
//
void renderScene(void) {
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    
    // set our camera position
    Point carPt = Point(carX, carY, carZ);
    gluLookAt( cameraXYZ + carPt, carPt, Vector(0,1,0) );    // Hey! I rewrote the gluLookAt() to take gluLookAt( Point, Point, Vector )
    
	//float lPosition[4] = { 0.0, 10.0, 0.0, 1.0 };
	//glLightfv(GL_LIGHT0, GL_POSITION, lPosition);

    //Added for trees 
    view = Vector(-cameraXYZ.getX(), -cameraXYZ.getY(), -cameraXYZ.getZ())*-1.0;

    // clear the render buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// Lighting
    glCallList( environmentDL );
    
    // draw our ground
    glPushMatrix(); {
		GLfloat materialColor[] = { 1.0,  1.0,  1.0, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
        float squareSize = groundSize + ballRadius;
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, groundTexHandle );
        glBegin( GL_QUADS ); {
            glNormal3f( 0, 1, 0 ); glTexCoord2f( -squareSize, -squareSize ); glVertex3f( -squareSize, 0, -squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f( -squareSize,  squareSize ); glVertex3f( -squareSize, 0,  squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f(  squareSize,  squareSize ); glVertex3f(  squareSize, 0,  squareSize );
            glNormal3f( 0, 1, 0 ); glTexCoord2f(  squareSize, -squareSize ); glVertex3f(  squareSize, 0, -squareSize );
        }; glEnd();
        // CHANGE: 1:16pm
        // attempting to draw "side panels"
        glBegin( GL_QUADS ); {
            glNormal3f( -1, 0, 0 ); glTexCoord2f( -squareSize, -squareSize ); glVertex3f( -squareSize, 0, -squareSize );
            glNormal3f( -1, 0, 0 ); glTexCoord2f( -squareSize,  squareSize ); glVertex3f( -squareSize, 0,  squareSize );
            glNormal3f( -1, 0, 0 ); glTexCoord2f(  squareSize,  squareSize ); glVertex3f( -squareSize, -400,  squareSize );
            glNormal3f( -1, 0, 0 ); glTexCoord2f(  squareSize, -squareSize ); glVertex3f( -squareSize, -400, -squareSize );
        }; glEnd();
        glBegin( GL_QUADS ); {
            glNormal3f(  1, 0, 0 ); glTexCoord2f( -squareSize, -squareSize ); glVertex3f( squareSize, 0, -squareSize );
            glNormal3f(  1, 0, 0 ); glTexCoord2f( -squareSize,  squareSize ); glVertex3f( squareSize, 0,  squareSize );
            glNormal3f(  1, 0, 0 ); glTexCoord2f(  squareSize,  squareSize ); glVertex3f( squareSize, -400,  squareSize );
            glNormal3f(  1, 0, 0 ); glTexCoord2f(  squareSize, -squareSize ); glVertex3f( squareSize, -400, -squareSize );
        }; glEnd();
        glBegin( GL_QUADS ); {
            glNormal3f(  0, 0, 1 ); glTexCoord2f( -squareSize, -squareSize ); glVertex3f( squareSize, 0,  squareSize );
            glNormal3f(  0, 0, 1 ); glTexCoord2f( -squareSize,  squareSize ); glVertex3f( -squareSize, 0,  squareSize );
            glNormal3f(  0, 0, 1 ); glTexCoord2f(  squareSize,  squareSize ); glVertex3f( -squareSize, -400,  squareSize );
            glNormal3f(  0, 0, 1 ); glTexCoord2f(  squareSize, -squareSize ); glVertex3f( squareSize, -400, squareSize );
        }; glEnd();
        glBegin( GL_QUADS ); {
            glNormal3f(  0, 0, -1 ); glTexCoord2f( -squareSize, -squareSize ); glVertex3f( squareSize, 0,  -squareSize );
            glNormal3f(  0, 0, -1 ); glTexCoord2f( -squareSize,  squareSize ); glVertex3f( -squareSize, 0,  -squareSize );
            glNormal3f(  0, 0, -1 ); glTexCoord2f(  squareSize,  squareSize ); glVertex3f( -squareSize, -400,  -squareSize );
            glNormal3f(  0, 0, -1 ); glTexCoord2f(  squareSize, -squareSize ); glVertex3f( squareSize, -400, -squareSize );
        }; glEnd();
        glDisable(GL_TEXTURE_2D);
    }; glPopMatrix();
    
    // draw our skybox
    drawSkybox( 200 );

    // draw Crane
    glPushMatrix();
    glTranslatef(carX, carY - 1.2, carZ);
    glRotatef(carHeading - 270, 0.0, 1.0, 0.0);
    if(carfreefall) {
        glRotatef(-freeFallAngle, 1.0, 0.0, 0.0);
    }
	if(carY < -50)
	{
		printf("THIS IS CALLED");
		newGame(); 
	}
	GLfloat materialForEnemies[] = { 1,	0.829,	0.829 , 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialForEnemies);
    drawHuman();
    glPopMatrix();



	
    
    // enable textures.
    glEnable( GL_TEXTURE_2D );
   // glColor4f(1,1,1,1);
	GLfloat materialColor[] = { 1.0,  1.0,  1.0, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
    glBindTexture(GL_TEXTURE_2D, brickTexHandle);   // make all of our spheres bricky

    //Trees 

    // enable textures
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, treeTexHandle );  // from here on out we draw trees so only bind this once to save context switching in OpenGL
    //glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
    
    // TODO #4: Sort trees by distance to camera
    XZCamera = Vector(cameraXYZ.getX()+forestCenter.getX(), 0.0, cameraXYZ.getZ()+forestCenter.getZ());
    XZView = Vector(view.getX(), 0.0, view.getY());
    
    // TODO #4: Sort trees by distance to camera
    sort(treeSpritePositions.begin(), treeSpritePositions.end(), myfunction);

    // TODO #3: Compute rotation of tree to face camera

    // Calculate view vector 
    viewV = Vector(cameraXYZ.getX() + forestCenter.getX(), 0.0, cameraXYZ.getZ()+ forestCenter.getZ()); 
    // Normalize view vector
    viewV.normalize(); 
    // Tree Normal 
    Vector treeNormalV = Vector(0.0, 0.0, 1.0); 
    // Normalize Tree Normal 
    treeNormalV.normalize(); 
    // dot product them
    double dotP = dot(treeNormalV, viewV);
    // Take the cross product 
    Vector crossP = cross(treeNormalV, viewV); 
    // Take the inverse cos 
    double angleOfRotation = acos(dotP) * 180 / M_PI;
    
    for( unsigned int i = 0; i < treeSpritePositions.size(); i++ ) {
        glPushMatrix(); {
            // TODO #2: Draw our sprite!
            glTranslatef(treeSpritePositions[i].getX(), 0, treeSpritePositions[i].getZ()); 
            glRotatef(angleOfRotation,crossP.getX(),crossP.getY(),crossP.getZ());
            drawTexturedSprite(20, 20); 
        }; glPopMatrix();
    }

    glDisable( GL_TEXTURE_2D );



    // draw every sphere
    for( unsigned int i = 0; i < balls.size(); i++ ) {

		glPushMatrix(); {
			GLfloat materialColor[] = { 0.541176, 0.168627,  0.886275, 1.0f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
			balls[i]->draw();			
		}; glPopMatrix();

        if(firing) {
            shot->drawShot();
        }
    }
	
	glEnable(GL_TEXTURE_2D);

    // DRAW POWERUP
    if(!powerupAchieved) {
		glEnable(GL_LIGHTING);
		
		//test a light
		glEnable(GL_LIGHT0);
		float diffuseCol[4] = { .5,.008,.008,.30 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseCol);
		float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientCol);
		float lPosition[4] = { powerX, 1.0 + bob, powerZ, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, lPosition);

        glPushMatrix();
      //  glColor3f(1.0, (204.0/255.0), 0.0);
		GLfloat materialColorPower[] = { 1.0, (204.0 / 255.0),  0.0, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColorPower);
        glTranslatef(powerX, 1.0 + bob, powerZ);
        glRotatef(90.0, 1.0, 0.0, 0.0);
        gluCylinder(powerup, 1.0, 1.0, 3.0, 16, 3);
       // glColor3f(1.0, 1.0, 1.0);
        glPopMatrix();

		//glDisable(GL_LIGHTING);
    } 
	glDisable(GL_TEXTURE_2D);


     // DRAW GAME STATE
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, windowWidth, windowHeight, 0.0, -1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_DEPTH_BUFFER_BIT);
    
   // glColor3f( 1.0, 1.0, 1.0 );
    glRasterPos2i(10, 20);
    
    // PUT GAME STATE HERE /////////////////
    switch(gameState) {
        case BEFORE:    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'P' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'O' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'W' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'E' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'R' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'U' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'P' );
                        break;
        case KILLTHEM:  glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'T' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'T' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'C' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'K' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '!' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '!' );
                        break;
        case DEAD:      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'Y' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'O' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'U' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'D' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'I' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'E' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'D' );
                        break;
        case WON:       glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'Y' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'O' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'U' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'W' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'I' );
                        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'N' );
                        break;
    }
    
    
    // Making sure we can render 3d again
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

        // enable texture for snow
    //glEnable( GL_TEXTURE_2D );
  //  glColor3f(1.0,1.0,1.0);
	GLfloat materialColorPowerParticles[] = { 1.0, 1.0,  0.0, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColorPowerParticles);
    glBindTexture(GL_TEXTURE_2D, snowTexture);   // make all of our spheres bricky
    part.draw();

    
    //glDisable( GL_TEXTURE_2D );
    
    //push the back buffer to the screen
    glutSwapBuffers();
}

//
//  void normalKeysDown(unsigned char key, int x, int y)
//
//      GLUT keyboard callback for when a regular key is pressed.
//
void normalKeysDown(unsigned char key, int x, int y) {

    switch( key ) {
        case 'q':
        case 'Q':
        case 27:
        exit(0);
        break;
    }
    if(key == 'w' && !carfreefall) {
        speed = .5;
		easterEgg = 0; 
    }
    
    if(key == 's' && !carfreefall) {
        speed = -.5;
		easterEgg = 0; 
    }
    
    if(key == 'd' && !carfreefall) {
        carHeading -= 22.5;
		easterEgg += 1; 
    }
    
    if(key == 'a' && !carfreefall) {
        carHeading += 22.5;
		easterEgg += 1;
    }
    if(key == 32 && attackMode && !firing) {
        shoot();
    }
	if (key == '1') {
		if (light1On) {
			glDisable(GL_LIGHT0);
			light1On = !light1On;
		}
		else {
			glEnable(GL_LIGHT0);
			light1On = !light1On;
		}
	}

	if (easterEgg > 100)
	{
		powerupAchieved = true; 
		attackMode = true; 
	}
}

//
// void myTimer(int value)
//
//  We have to take an integer as input per GLUT's timer function prototype;
//  but we don't use that value so just ignore it. We'll register this function
//  once at the start of the program, and then every time it gets called it
//  will perpetually re-register itself and tell GLUT that the screen needs
//  be redrawn. yes, I said "needs be."
//
void myTimer(int value){

    for (int i = 0; i < part.Snows.size(); i++)
    {
        part.Snows[i]->Fall(); 
    }

        if (SnowsTimer >= 90) {
        part.populateSnows(groundSize); 
        SnowsTimer = 0;
        while (part.Snows.size() > 20)
        {
            part.Snows.erase(part.Snows.begin());
        }

    }

    if (SnowsTimerTwo >= 120) {
        part.populateSnows(groundSize);
        SnowsTimerTwo = 0;
    }
    
    SnowsTimer++;
    SnowsTimerTwo++;

    if (firing) {
        shotStep++;
        if(shotStep >= 90) {
            firing = false;
            delete shot;
        }
    }


    // Car things
    if(fabs(speed) > .01) { 
        if(!moveLegFlip) {
        moveLeg += 0.05; //Artemis
        }
        else {
            moveLeg -= 0.05;
        }
    }
    rotateHand +=2;

    if (bob >= 8.0) {
        bobup = false;
    }
    if (bob <= 2.0) {
        bobup = true;
    }
    if (bobup) {
        bob += 0.20;
    }
    else {
        bob -= 0.20;
    }
    if(health <= 0) {
        carfreefall = true;
    }
    if(carfreefall) {
        carY -= .5;
        freeFallAngle += 12.0;
        if(fmod(freeFallAngle, 360.0) <= .1) {
            freeFallAngle = 0.0;
        }
        gameState = DEAD;
    }
    carX += cos(carHeading*(M_PI/180))*speed;
    carZ += -sin(carHeading*(M_PI/180))*speed;
    
    if(fabs(speed) > 0.1) {
        if (speed > .1) {
        tires -= speed*10.0;
        speed -= 0.01;
        }
        if (speed < -.1) {
            tires += speed*10.0;
            speed += 0.01;
        }
    }
    else {
        speed = 0;
    }
    if (carX >= groundSize + 4.0) {
        carfreefall = true;
        //carX = groundSize;
    }
    if (carX <= -groundSize - 4.0) {
        carfreefall = true;
        //carX = -groundSize;
    }
    if (carZ >= groundSize + 4.0) {
        carfreefall = true;
        //carZ = groundSize;
    }
    if (carZ <= -groundSize - 4.0) {
        carfreefall = true;
        //carZ = -groundSize;
    }

    if(powerupAchieved) {
        gameState = KILLTHEM;
		glDisable(GL_LIGHT0);
    }
    if(balls.size() == 0) {
        gameState = WON;
    }
    
    for(unsigned int i = 0; i < balls.size(); i++) {
        balls[i]->moveForward(Point(carX, carY, carZ));
        if(firing) {
            // simple way to get it to move faster
            shot->moveForward(Point(shot->location.getX() + shot->direction.getX(),
                                    shot->location.getY() + shot->direction.getY(),
                                    shot->location.getZ() + shot->direction.getZ()
                                    )
                                );
            shot->moveForward(Point(shot->location.getX() + shot->direction.getX(),
                                    shot->location.getY() + shot->direction.getY(),
                                    shot->location.getZ() + shot->direction.getZ()
                                    )
                                );
        }
    }
    
    
    // TODO #2
    //   check every ball if it "collides" with the edges of our ground plane
    
    for(unsigned int i = 0; i < balls.size(); i++) {
        if( balls[i]->location.getX() > groundSize ) {
            Vector norm = Vector(-1, 0, 0);
            Vector outVec = balls[i]->direction - 2*dot(balls[i]->direction, norm)*norm;
            balls[i]->direction = outVec;
        }
        if( balls[i]->location.getX() < -groundSize ) {
            Vector norm = Vector(1, 0, 0);
            Vector outVec = balls[i]->direction - 2*dot(balls[i]->direction, norm)*norm;
            balls[i]->direction = outVec;
        }
        if( balls[i]->location.getZ() > groundSize ) {
            Vector norm = Vector(0, 0, -1);
            Vector outVec = balls[i]->direction - 2*dot(balls[i]->direction, norm)*norm;
            balls[i]->direction = outVec;
        }
        if( balls[i]->location.getZ() < -groundSize ) {
            Vector norm = Vector(0, 0, 1);
            Vector outVec = balls[i]->direction - 2*dot(balls[i]->direction, norm)*norm;
            balls[i]->direction = outVec;
        }
        double dist = sqrt((balls[i]->location.getX() - carX) *
                                (balls[i]->location.getX() - carX) +
                                 (balls[i]->location.getY() - carY) *
                                  (balls[i]->location.getY() - carY) +
                                   (balls[i]->location.getZ() - carZ) *
                                    (balls[i]->location.getZ() - carZ)
                               );
        // note, hard coded 3.6 is the radius of our car
        if(balls[i]->radius + 3.6 > dist) {
            // erase the 6th element
            delete balls[i];
            balls.erase (balls.begin()+i);
            if (!attackMode) {
                health -= 50;
            }
        }
        if(firing) {
            double distShot = sqrt((balls[i]->location.getX() - shot->location.getX()) *
                                (balls[i]->location.getX() - shot->location.getX()) +
                                 (balls[i]->location.getY() - shot->location.getY()) *
                                  (balls[i]->location.getY() - shot->location.getY()) +
                                   (balls[i]->location.getZ() - shot->location.getZ()) *
                                    (balls[i]->location.getZ() - shot->location.getZ())
                               );
            //printf("\nDistance: %f, max: %f", distShot, balls[i]->radius + shot->radius);
            if(distShot < 2.5) {
                // DIIIIIRRECT HIT
                printf("\nHit!\n");
                delete balls[i];
                balls.erase (balls.begin()+i);
            }
        }
    }
    
    
    // TODO #3
    //   check for interball collisions
    //   warning this may not be perfect...balls can get caught and
    //     continually bounce back-and-forth in place off of
    //     each other but with fewer balls we would need just the
    //     right conditions of multiple balls colliding all at
    //     once.
    
    for(unsigned int i = 0; i < balls.size(); i++) {
        for(unsigned int j = i + 1; j < balls.size(); j++) {
            // May as well factor in y height distance as well, in case needed
            // Cmon professor implement a distance function for two points
            // You know I have anxiety about editing class files
            double dist = sqrt((balls[i]->location.getX() - (balls[j]->location.getX())) *
                                (balls[i]->location.getX() - (balls[j]->location.getX())) +
                                 (balls[i]->location.getY() - (balls[j]->location.getY())) *
                                  (balls[i]->location.getY() - (balls[j]->location.getY())) +
                                   (balls[i]->location.getZ() - (balls[j]->location.getZ())) *
                                    (balls[i]->location.getZ() - (balls[j]->location.getZ()))
                               );
            Vector N1 = -1.0*Vector(balls[j]->location.getX() - (balls[i]->location.getX()),
                                    balls[j]->location.getY() - (balls[i]->location.getY()),
                                    balls[j]->location.getZ() - (balls[i]->location.getZ())
                                    );
            Vector N2 = -1.0*N1;
            if(balls[i]->radius + balls[j]->radius > dist) {
                balls[i]->moveBackward();
                balls[j]->moveBackward();
                balls[i]->direction = balls[i]->direction - 2*dot(balls[i]->direction, N1)*N1;
                balls[j]->direction = balls[j]->direction - 2*dot(balls[j]->direction, N2)*N2;
                balls[i]->moveForward(Point(carX, carY, carZ));
                balls[j]->moveForward(Point(carX, carY, carZ));
            }
        }
    }

    // Check if crane hit the powerup!
    double dist = sqrt(        (powerX - carX) *
                                (powerX - carX) +
                                 (0.0) *
                                  (0.0) +
                                   (powerZ - carZ) *
                                    (powerZ - carZ)
                               );
    if (dist < (1.0 + 3.6)) {
        powerupAchieved = true;
        attackMode = true;
    }
    
    glutPostRedisplay();
    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);
}

//
//  registerOpenGLRGBTexture( ...params... )
//
//  Takes in the RGB data of a texture of size width * height.  Registers
//      the texture with OpenGL setting the handle value to textureHandle.
//      Also sets the texture parameters for Min, Mag, S, T, and application
//      mode.
//
bool registerOpenGLRGBTexture(unsigned char *textureData,
                           unsigned int texWidth, unsigned int texHeight,
                           GLuint &textureHandle) {
    // make sure we are receiving data
    if(textureData == 0) {
        fprintf(stderr,"Cannot register texture; no data specified.");
        return false;
    }
    
    glGenTextures(1, &textureHandle);                                   // generate a handle
    glBindTexture(GL_TEXTURE_2D, textureHandle);                        // bind the handle
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        // set our mode to modulate with lighting
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // lerp on min filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // lerp on mag filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // repeat along S dimension
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // repeat along T dimesion
    
    GLenum errCode;
    const GLubyte *errString;
    
    // check for error
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errString = gluErrorString(errCode);
        fprintf(stderr,"Cannot register texture; OpenGL error: %s.", errString);
        return false;
    }
    
    // send the texture to the GPU
    glTexImage2D( GL_TEXTURE_2D,    // texture type (a 2D texture)
                              0,    // level of detail
                         GL_RGB,    // internal storage data format
                       texWidth,    // width of texture
                      texHeight,    // height of texture
                              0,    // border = 0
                         GL_RGB,    // display format, must match the internal format
               GL_UNSIGNED_BYTE,    // data type in storage
                    textureData);   // the actual data
    
    return true;
}


// readPPM() ///////////////////////////////////////////////////////////////////
//
//  This function reads an ASCII PPM, returning true if the function succeeds and
//      false if it fails. If it succeeds, the variables imageWidth and
//      imageHeight will hold the width and height of the read image, respectively.
//
//  It's not terribly robust.
//
//  Returns the image as an unsigned character array containing
//      imageWidth*imageHeight*3 entries (for that many bytes of storage).
//
//  NOTE: this function expects imageData to be UNALLOCATED, and will allocate
//      memory itself. If the function fails (returns false), imageData
//      will be set to NULL and any allocated memory will be automatically deallocated.
//
////////////////////////////////////////////////////////////////////////////////
bool readPPM(string filename, int &imageWidth, int &imageHeight, unsigned char* &imageData)
{
    FILE *fp = fopen(filename.c_str(), "r");
    int temp, maxValue;
    fscanf(fp, "P%d", &temp);
    if(temp != 3)
    {
        fprintf(stderr, "Error: PPM file is not of correct format! (Must be P3, is P%d.)\n", temp);
        fclose(fp);
        return false;
    }
    
    //got the file header right...
    fscanf(fp, "%d", &imageWidth);
    fscanf(fp, "%d", &imageHeight);
    fscanf(fp, "%d", &maxValue);
    
    //now that we know how big it is, allocate the buffer...
    imageData = new unsigned char[imageWidth*imageHeight*3];
    if(!imageData)
    {
        fprintf(stderr, "Error: couldn't allocate image memory. Dimensions: %d x %d.\n", imageWidth, imageHeight);
        fclose(fp);
        return false;
    }
    
    //and read the data in.
    for(int j = 0; j < imageHeight; j++)
    {
        for(int i = 0; i < imageWidth; i++)
        {
            int r, g, b;
            fscanf(fp, "%d", &r);
            fscanf(fp, "%d", &g);
            fscanf(fp, "%d", &b);
            
            imageData[(j*imageWidth+i)*3+0] = r;
            imageData[(j*imageWidth+i)*3+1] = g;
            imageData[(j*imageWidth+i)*3+2] = b;
        }
    }
    
    fclose(fp);
    return true;
}

//
//  registerSOILTexture( filename, handle )
//
//  Loads a texture using SOIL and registers it with OpenGL
//
void registerSOILTexture( string filename, GLuint &handle ) {
    // load the texture image and return a handle to the texture
    handle = SOIL_load_OGL_texture( filename.c_str(),
                                            SOIL_LOAD_AUTO,
                                            SOIL_CREATE_NEW_ID,
                                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y| SOIL_FLAG_COMPRESS_TO_DXT );
    glBindTexture(GL_TEXTURE_2D, handle);                               // bind the handle
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);        // set our mode to modulate with lighting
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );   // lerp MIPMAP and lerp on texel on min filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // lerp on mag filter
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // repeat along S dimension
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // repeat along T dimesion
}

//
//  registerTextures()
//
//  Register all of our textures with OpenGL.  This includes the brick for our spheres,
//      our ground plane, and our skybox.  Provide some output to the user so they know
//      what is causing the loading delay.
//
void registerTextures() {
    ////// LOAD IN OUR TEXTURES //////////////////////////////////////////////////////////////
    unsigned char *brickTexData;
    int brickTexWidth, brickTexHeight;
    readPPM("textures/brick.ppm", brickTexWidth, brickTexHeight, brickTexData);
    registerOpenGLRGBTexture(brickTexData, brickTexWidth, brickTexHeight, brickTexHandle);
    printf( "[INFO]: brick texture read in and registered\n" );
    
    registerSOILTexture( "textures/snow_tile.jpg", groundTexHandle );
    printf( "[INFO]: ground texture read in and registered\n" );
    
    printf( "[INFO]: registering skybox..." );
    fflush( stdout );
    registerSOILTexture( "textures/skybox/back.png",   skyboxHandles[0] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/right.png",  skyboxHandles[1] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/front.png",  skyboxHandles[2] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/left.png",   skyboxHandles[3] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/bottom.png", skyboxHandles[4] );   printf( "." ); fflush( stdout );
    registerSOILTexture( "textures/skybox/top.png",    skyboxHandles[5] );   printf( "." ); fflush( stdout );
    printf( "skybox textures read in and registered!\n" );
    //////////////////////////////////////////////////////////////////////////////////////////

    ////// LOAD IN OUR TRANSPARENT TEXTURES //////////////////////////////////////////////////
    // some relevant variables for the texture files that we won't need later...so don't make them global
    int rgbWidth, rgbHeight, alphaWidth, alphaHeight, channels;
    unsigned char *treeTexData, *treeMaskData;

    treeTexData = SOIL_load_image( "textures/pinetreeDiff.jpg",
                                  &rgbWidth, &rgbHeight, &channels,
                                  SOIL_LOAD_AUTO );
    printf( "[INFO]: read in image treeDiff.jpg of size %d x %d with %d colors\n", rgbWidth, rgbHeight, channels );
    treeMaskData = SOIL_load_image( "textures/pinetreeOpa.jpg",
                                   &alphaWidth, &alphaHeight, &channels,
                                   SOIL_LOAD_AUTO );
    printf( "[INFO]: read in image treeOpa.jpg of size %d x %d with %d colors\n", alphaWidth, alphaHeight, channels );
    registerTransparentOpenGLTexture( treeTexData, treeMaskData, rgbWidth, rgbHeight, treeTexHandle );
    printf( "[INFO]: transparent texture read in and registered\n" );
    //////////////////////////////////////////////////////////////////////////////////////////


      // Load Snow Texture 
    snowTexture =
        SOIL_load_OGL_texture(
            "textures/snow_image.jpg",
            SOIL_LOAD_AUTO,
            SOIL_CREATE_NEW_ID,
            SOIL_FLAG_MIPMAPS
            | SOIL_FLAG_COMPRESS_TO_DXT
        );

}

// main() //////////////////////////////////////////////////////////////////////
//
//  Program entry point. Does not process command line arguments.
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
    //create a double-buffered GLUT window at (50,50) with predefined windowsize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(windowWidth,windowHeight);
    glutCreateWindow("Final Project: First Person Shooter");

	GLenum glewResult = glewInit();
	/* check for an error */
	if (glewResult != GLEW_OK) {
		printf("[ERROR]: Error initalizing GLEW\n");
		return 0;
	}
    
    printf( "[INFO]: GLUT initialized and OpenGL Context created\n" );
    
    fprintf(stdout, "[INFO]: /--------------------------------------------------------\\\n");
    fprintf(stdout, "[INFO]: | OpenGL Information                                     |\n");
    fprintf(stdout, "[INFO]: |--------------------------------------------------------|\n");
    fprintf(stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION));
    fprintf(stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER));
    fprintf(stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR));
    fprintf(stdout, "[INFO]: \\--------------------------------------------------------/\n\n");
    
    //give the camera a 'pretty' starting point!
    cameraRadius = 25.0f;
    cameraTheta = 2.80;
    cameraPhi = 2.0;
    computeArcballPosition(cameraTheta, cameraPhi, cameraRadius,
                           cameraXYZ);
    
    // register callback functions...
    glutKeyboardFunc(normalKeysDown);
    glutDisplayFunc(renderScene);
    glutReshapeFunc(resizeWindow);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotion);
    
    // do some basic OpenGL setup
    initScene();
    printf( "[INFO]: OpenGL Scene Initialized\n" );
    registerTextures();
    printf( "[INFO]: All textures registered\n" );
    // create all of our balls TESTING 
	
	// CHANGE 2PM
	setUpShaders();
	printf("[INFO]: Shader compilation complete\n");

    populateBalls();

    //Create all of our snows 

    part.populateSnows(groundSize);
	
	generateEnvironmentDL();            //Adding lighting

    printf( "[INFO]: Spheres have been randomly generated\n" );

    powerup = gluNewQuadric();
    gluQuadricNormals(powerup, GLU_SMOOTH);
    powerX = -(float)groundSize + 2*(rand()/(float)RAND_MAX * groundSize);
    powerZ = -(float)groundSize + 2*(rand()/(float)RAND_MAX * groundSize);

    printf("\nPowerup located at x: %f z: %f \n", powerX, powerZ);
    
    // start our timer
    glutTimerFunc((unsigned int)(1000.0 / 60.0), myTimer, 0);
    
    printf( "[INFO]: And here...we...go...\n\n" );
    
    //and enter the GLUT loop, never to exit.
    glutMainLoop();

    gluDeleteQuadric(powerup);
    
    return(0);
}
