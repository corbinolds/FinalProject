#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <math.h>
#include "ParticleSystem.h"
#include "Particle.h"
#include "Snow.h"
#include <stdlib.h>
#include "Vector.h"
#include <vector>


                 // a collection of the Snows in our scene
                          // the number of Snows in our scene

ParticleSystem::ParticleSystem() {   
	numSnows = 50;
	Snows.clear(); 
}

ParticleSystem::ParticleSystem(int num) {   
	numSnows = num;
}

void ParticleSystem::populateSnows(float num) {
    //srand( time(NULL) );
	float groundSize = num;                      // the size of our ground plane and therefore bounding box
    float rangeX = groundSize*2;
    float rangeZ = groundSize*2;
	float SnowRadius = 0.1;                     // the base radius of all our spheres (when created, the spheres will

    
    // for each Snow
    for(int i = 0; i < numSnows; i++) {
        // create a new Snow
        Snows.push_back(  new Snow( Point( rand()/(float)RAND_MAX * rangeX - rangeX/2.0f,           // at a random location X
                                           10.0f,                                                        // Y
                                           (rangeZ * (i/(float)numSnows)) - rangeZ/2.0f),               // Z
                                    Vector( rand()/(float)RAND_MAX, 10.0, rand()/(float)RAND_MAX ),  // with a random direction
                                    SnowRadius  ));            // and a random radius
    }
}
    // draw every sphere
void ParticleSystem::draw() {
    for(int i = 0; i < Snows.size(); i++ ) {
        Snows[i]->draw();
    }
}

void ParticleSystem::update() {
	
}
