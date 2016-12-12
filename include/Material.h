#ifndef _OPENGL_MATERIAL_H_
#define _OPENGL_MATERIAL_H_

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

class Material {
public:
	Material();
	
	GLfloat* getAmbient();
	void setAmbient( GLfloat amb[4] );
	
	GLfloat* getDiffuse();
	void setDiffuse( GLfloat diff[4] );
	
	GLfloat* getSpecular();
	void setSpecular( GLfloat spec[4] );
	
	GLfloat* getEmissive();
	void setEmissive( GLfloat emis[4] );
	
	GLfloat getShininess();
	void setShininess( GLfloat shin );
	
	GLint getIllumination();
	void setIllumination( GLint illum );
	
private:
	GLfloat _ambient[4];
	GLfloat _diffuse[4];
	GLfloat _specular[4];
	GLfloat _emissive[4];
	GLfloat _shininess;
	GLint _illumination;
};

void setCurrentMaterial( Material *material );

#endif
