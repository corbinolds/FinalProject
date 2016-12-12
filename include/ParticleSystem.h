#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_ 1

#include "Point.h"
#include "Vector.h"
#include "Snow.h"

class ParticleSystem {
	public:
		// CONSTRUCTORS / DESTRUCTORS
		ParticleSystem();
		ParticleSystem(int num); 

		// MISCELLANEOUS
		vector< Snow* > Snows;                      // a collection of the Snows in our scene


		void populateSnows(); 
		void draw(); 
		void update(); 
	private:
		int numSnows;;                          // the number of Snows in our scene
    
};

#endif
