#include "Material.h"

Material::Material() {
	this->_illumination = 2;
	
	_ambient[0] = 0.0;	_ambient[1] = 0.0;	_ambient[2] = 0.0;	_ambient[3] = 1.0;
	_diffuse[0] = 0.0;	_diffuse[1] = 0.0;	_diffuse[2] = 0.0;	_diffuse[3] = 1.0;
	_specular[0] = 0.0;	_specular[1] = 0.0;	_specular[2] = 0.0;	_specular[3] = 1.0;
	_emissive[0] = 0.0; _emissive[1] = 0.0;	_emissive[2] = 0.0;	_emissive[3] = 1.0;
	
	_shininess = 0;
}

GLfloat* Material::getAmbient() { return _ambient; }
void Material::setAmbient( GLfloat* amb ) { 
	this->_ambient[0] = amb[0]; 
	this->_ambient[1] = amb[1];
	this->_ambient[2] = amb[2];
	this->_ambient[3] = amb[3];
}

GLfloat* Material::getDiffuse() { return _diffuse; }
void Material::setDiffuse( GLfloat diff[4] ) { 
	this->_diffuse[0] = diff[0]; 
	this->_diffuse[1] = diff[1]; 
	this->_diffuse[2] = diff[2]; 
	this->_diffuse[3] = diff[3]; 
}

GLfloat* Material::getSpecular() { return _specular; }
void Material::setSpecular( GLfloat spec[4] ) { 
	this->_specular[0] = spec[0]; 
	this->_specular[1] = spec[1]; 
	this->_specular[2] = spec[2]; 
	this->_specular[3] = spec[3];
}

GLfloat* Material::getEmissive() { return _emissive; }
void Material::setEmissive( GLfloat emis[4] ) { 
	this->_emissive[0] = emis[0]; 
	this->_emissive[1] = emis[1]; 
	this->_emissive[2] = emis[2]; 
	this->_emissive[3] = emis[3];
}

GLfloat Material::getShininess() { return _shininess; }
void Material::setShininess( GLfloat shin ) { this->_shininess = shin; }

GLint Material::getIllumination() { return _illumination; }
void Material::setIllumination( GLint illum ) { this->_illumination = illum; }

void setCurrentMaterial( Material *material ) {
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, material->getAmbient() );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, material->getDiffuse() );
	if( material->getIllumination() == 2 )
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, material->getSpecular() );
	else {
		GLfloat flat[4] = {0.0, 0.0, 0.0, 1.0};
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, flat );
	}
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, material->getEmissive() );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, material->getShininess() );
}
