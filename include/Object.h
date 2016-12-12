#ifndef _OPENGL_OBJECT_H_
#define _OPENGL_OBJECT_H_

#include "Material.h"
#include "Point.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

class Object {
public:
	Object();
	Object( string filename );
	~Object();
	
	bool loadObjectFile( string filename );
	
	bool draw();

	Point* getLocation();
	
private:
	string _objFile;
	string _mtlFile;
	GLuint _objectDisplayList;
	
	bool objHasVertexTexCoords;
	bool objHasVertexNormals;
	
	void init();
	
	bool loadObject();
	bool loadMaterial();
	
	vector< GLfloat > vertices;
	vector< GLfloat > vertexNormals;
	vector< GLfloat > vertexTexCoords;
	
	map< string, Material* >* _materials;
	map< string, GLuint* >* _textures;

	Point* _location;
};

#endif
