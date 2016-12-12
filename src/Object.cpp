#include "Object.h"
#include "Vector.h"

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <fstream>
#include <iostream>
using namespace std;

//
//  vector<string> tokenizeString(string input, string delimiters)
//
//      This is a helper function to break a single string into std::vector
//  of strings, based on a given set of delimiter characters.
//
vector<string> tokenizeString(string input, string delimiters)
{
    if(input.size() == 0)
        return vector<string>();
	
    vector<string> retVec = vector<string>();
    size_t oldR = 0, r = 0; 
	
    //strip all delimiter characters from the front and end of the input string.
    string strippedInput;
    int lowerValidIndex = 0, upperValidIndex = input.size() - 1; 
    while((unsigned int)lowerValidIndex < input.size() && delimiters.find_first_of(input.at(lowerValidIndex), 0) != string::npos)
        lowerValidIndex++;
	
    while(upperValidIndex >= 0 && delimiters.find_first_of(input.at(upperValidIndex), 0) != string::npos)
        upperValidIndex--;
	
    //if the lowest valid index is higher than the highest valid index, they're all delimiters! return nothing.
    if((unsigned int)lowerValidIndex >= input.size() || upperValidIndex < 0 || lowerValidIndex > upperValidIndex)
        return vector<string>();
    
    //remove the delimiters from the beginning and end of the string, if any.
    strippedInput = input.substr(lowerValidIndex, upperValidIndex-lowerValidIndex+1);
	
    //search for each instance of a delimiter character, and create a new token spanning
    //from the last valid character up to the delimiter character.
    while((r = strippedInput.find_first_of(delimiters, oldR)) != string::npos)
    {    
        if(oldR != r)           //but watch out for multiple consecutive delimiters!
            retVec.push_back(strippedInput.substr(oldR, r-oldR));
        oldR = r+1; 
    }    
    if(r != 0)
        retVec.push_back(strippedInput.substr(oldR, r-oldR));
	
    return retVec;
}

Object::Object() {
	init();
}

Object::Object( string filename ) : _objFile( filename ) {
	init();
	loadObject();
}

Object::~Object() {
	delete _materials;
	delete _textures;
}

bool Object::loadObjectFile( string filename ) {
	bool result = true;
	_objFile = filename;
	
	result = loadObject();
	
	return result;
}

bool Object::draw() {
	bool result = true;
	
	glPushMatrix();
        glCallList( _objectDisplayList );
    glPopMatrix();
	
	return result;
}

Point* Object::getLocation() { 
	return _location;
}

void Object::init() {
	objHasVertexNormals = false;
    objHasVertexTexCoords = false;
	
	_materials = new map< string, Material* >();
    _textures = new map< string, GLuint* >();
	
	_location = new Point(0, 0, 0);
}

bool Object::loadObject() {
	bool result = true;
	
	int numFaces = 0;
	float minX = 999999, maxX = -999999, minY = 999999, maxY = -999999, minZ = 999999, maxZ = -999999;
	string line;
	
	_objectDisplayList = glGenLists(1);
    glNewList(_objectDisplayList, GL_COMPILE);
	
    ifstream in( _objFile.c_str() );
    while( getline( in, line ) ) {
		line.erase( line.find_last_not_of( " \n\r\t" ) + 1 );
		
        vector< string > tokens = tokenizeString( line, " \t" );
		if( tokens.size() < 1 ) continue;
		
        //the line should have a single character that lets us know if it's a...
		if( !tokens[0].compare( "#" ) ) {								// comment ignore
		} else if( !tokens[0].compare( "o" ) ) {						// object name ignore
		} else if( !tokens[0].compare( "g" ) ) {						// polygon group name ignore
		} else if( !tokens[0].compare( "mtllib" ) ) {					// material library
			_mtlFile = tokens[1];
			cout << "[.obj]: about to read " << _mtlFile << endl;
			if( loadMaterial() )
				cout << "[.obj]: finished reading " << _mtlFile << endl;
        } else if( !tokens[0].compare( "usemtl" ) ) {                   // use material library
			map< string, Material* >::iterator materialIter = _materials->find( tokens[1] );
			if( materialIter != _materials->end() ) {
				setCurrentMaterial( materialIter->second );
			}
			
			map< string, GLuint* >::iterator textureIter = _textures->find( tokens[1] );
			if( textureIter != _textures->end() ) {
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, (textureIter->second)[0] );
			} else {

			}
		} else if( !tokens[0].compare( "s" ) ) {						// smooth shading
			if( !tokens[1].compare( "off" ) ) {
				glShadeModel( GL_FLAT );
			} else {
				glShadeModel( GL_SMOOTH );
			}
		} else if( !tokens[0].compare( "v" ) ) {						//vertex
			float x = atof( tokens[1].c_str() ),
			y = atof( tokens[2].c_str() ),
			z = atof( tokens[3].c_str() );
			
			if( x < minX ) minX = x;
			if( x > maxX ) maxX = x;
			if( y < minY ) minY = y;
			if( y > maxY ) maxY = y;
			if( z < minZ ) minZ = z;
			if( z > maxZ ) maxZ = z;
			
            vertices.push_back( x );
            vertices.push_back( y );
            vertices.push_back( z );
        } else if( !tokens.at(0).compare( "vn" ) ) {                    //vertex normal
            vertexNormals.push_back( atof( tokens[1].c_str() ) );
            vertexNormals.push_back( atof( tokens[2].c_str() ) );
            vertexNormals.push_back( atof( tokens[3].c_str() ) );
        } else if( !tokens.at(0).compare( "vt" ) ) {                    //vertex tex coord
            vertexTexCoords.push_back(atof(tokens[1].c_str()));
            vertexTexCoords.push_back(atof(tokens[2].c_str()));
        } else if( !tokens.at(0).compare( "f" ) ) {                     //face!
            
			//now, faces can be either quads or triangles (or maybe more?)
            //split the string on spaces to get the number of verts+attrs.
            vector<string> faceTokens = tokenizeString(line, " ");
			
            //some local variables to hold the vertex+attribute indices we read in.
            //we do it this way because we'll have to split quads into triangles ourselves.
            vector<int> v, vn, vt;
			
			bool faceHasVertexTexCoords = false, faceHasVertexNormals = false;
			
            for(long unsigned int i = 1; i < faceTokens.size(); i++) {
                //need to use both the tokens and number of slashes to determine what info is there.
                vector<string> groupTokens = tokenizeString(faceTokens[i], "/");
                int numSlashes = 0;
                for(long unsigned int j = 0; j < faceTokens[i].length(); j++) { if(faceTokens[i][j] == '/') numSlashes++; }
				
                //regardless, we always get a vertex index.
                v.push_back( atoi(groupTokens[0].c_str()) - 1 );
				
                //based on combination of number of tokens and slashes, we can determine what we have.
                if(groupTokens.size() == 2 && numSlashes == 1) {
					int vtI = atoi(groupTokens[1].c_str());
					vtI--;
                    vt.push_back( vtI ); 
					objHasVertexTexCoords = true; 
					faceHasVertexTexCoords = true;
                } else if(groupTokens.size() == 2 && numSlashes == 2) {
					int vnI = atoi(groupTokens[1].c_str());
					vnI--;
                    vn.push_back( vnI ); 
					objHasVertexNormals = true; 
					faceHasVertexNormals = true;
                } else if(groupTokens.size() == 3) {
					int vtI = atoi(groupTokens[1].c_str());
					vtI--;
                    vt.push_back( vtI ); 
					objHasVertexTexCoords = true; 
					faceHasVertexTexCoords = true;
					
					int vnI = atoi(groupTokens[2].c_str());
					vnI--;
                    vn.push_back( vnI ); 
					objHasVertexNormals = true; 
					faceHasVertexNormals = true;					
                } else if(groupTokens.size() != 1) {
                    fprintf(stderr, "Error. Malformed OBJ file, %s.\n", _objFile.c_str());
                    return false;
                }
            }    
			
            //now the local variables have been filled up; push them onto our global 
            //variables appropriately.
			
			glBegin(GL_TRIANGLES); {
				for(long unsigned int i = 1; i < v.size()-1; i++) {
					
					if( faceHasVertexNormals ) {
						glNormal3f( vertexNormals.at( vn[0]*3 ), vertexNormals.at( vn[0]*3+1 ), vertexNormals.at( vn[0]*3+2) );
					} else {
						Point v1 = Point( vertices.at( v[0]*3 ), vertices.at( v[0]*3+1 ), vertices.at( v[0]*3+2 ) );
						Point v2 = Point( vertices.at( v[i]*3 ), vertices.at( v[i]*3+1 ), vertices.at( v[i]*3+2 ) );
						Point v3 = Point( vertices.at( v[i+1]*3 ), vertices.at( v[i+1]*3+1 ), vertices.at( v[i+1]*3+2 ) );
						Vector normal = cross( v2-v1, v3-v1 );
						normal.normalize();
						glNormal3f( normal.getX(), normal.getY(), normal.getZ() );
					}
					if( faceHasVertexTexCoords )
						glTexCoord2f( vertexTexCoords.at( vt[0]*2 ), vertexTexCoords.at( vt[0]*2+1 ) );
					glVertex3f( vertices.at( v[0]*3 ), vertices.at( v[0]*3+1 ), vertices.at( v[0]*3+2 ) );
					
					if( faceHasVertexNormals ) {
						glNormal3f( vertexNormals.at( vn[i]*3 ), vertexNormals.at( vn[i]*3+1 ), vertexNormals.at( vn[i]*3+2 ) );
					} else {
						Point v1 = Point( vertices.at( v[0]*3 ), vertices.at( v[0]*3+1 ), vertices.at( v[0]*3+2 ) );
						Point v2 = Point( vertices.at( v[i]*3 ), vertices.at( v[i]*3+1 ), vertices.at( v[i]*3+2 ) );
						Point v3 = Point( vertices.at( v[i+1]*3 ), vertices.at( v[i+1]*3+1 ), vertices.at( v[i+1]*3+2 ) );
						Vector normal = cross( v3-v2, v1-v2 );
						normal.normalize();
						glNormal3f( normal.getX(), normal.getY(), normal.getZ() );
					}
					if( faceHasVertexTexCoords )
						glTexCoord2f( vertexTexCoords.at( vt[i]*2 ), vertexTexCoords.at( vt[i]*2+1 ) );
					glVertex3f( vertices.at( v[i]*3 ), vertices.at( v[i]*3+1 ), vertices.at( v[i]*3+2 ) );
					
					if( faceHasVertexNormals ) {
						glNormal3f( vertexNormals.at( vn[i+1]*3 ), vertexNormals.at( vn[i+1]*3+1 ), vertexNormals.at( vn[i+1]*3+2) );
					} else {
						Point v1 = Point( vertices.at( v[0]*3 ), vertices.at( v[0]*3+1 ), vertices.at( v[0]*3+2 ) );
						Point v2 = Point( vertices.at( v[i]*3 ), vertices.at( v[i]*3+1 ), vertices.at( v[i]*3+2 ) );
						Point v3 = Point( vertices.at( v[i+1]*3 ), vertices.at( v[i+1]*3+1 ), vertices.at( v[i+1]*3+2 ) );
						Vector normal = cross( v1-v3, v2-v3 );
						normal.normalize();
						glNormal3f( normal.getX(), normal.getY(), normal.getZ() );
					}
					if( faceHasVertexTexCoords )
						glTexCoord2f( vertexTexCoords.at( vt[i+1]*2 ), vertexTexCoords.at( vt[i+1]*2+1 ) );
					glVertex3f( vertices.at( v[i+1]*3 ), vertices.at( v[i+1]*3+1 ), vertices.at( v[i+1]*3+2 ) );				
				} 
			}; glEnd();
			
			numFaces++;
        } else {
			cout << "[.obj]: ignoring line: " << line << endl;
		}
    }
	
	glEndList();
	in.close();
	
    cout << "[.obj]: We read " << vertices.size()/3 << " vertices, " << vertexNormals.size()/3 << " vertex normals, " << vertexTexCoords.size()/2 << " vertex tex coords, and " << numFaces << " faces." << endl;
	cout << "[.obj]: " << (maxX - minX) << " units across in X, " << (maxY - minY) << " units across in Y, " << (maxZ - minZ) << " units across in Z." << endl;
	
	return result;
}

bool Object::loadMaterial() {
	bool result = true;
    return result;
	string line;
	
    ifstream in( _mtlFile.c_str() );
	if( !in.is_open() ) {
		cerr << "[.mtl]: [ERROR] could not open material file: " << _mtlFile << endl;
		return false;
	}
    while( getline( in, line ) ) {
		line.erase( line.find_last_not_of( " \n\r\t" ) + 1 );
		
        vector< string > tokens = tokenizeString( line, " /\t" );
		if( tokens.size() < 1 ) continue;
		
		Material *currMaterial= NULL;
		string materialName;
		
		//the line should have a single character that lets us know if it's a...
		if( !tokens[0].compare( "#" ) ) {							// comment
		} else if( !tokens[0].compare( "newmtl" ) ) {						//new material
			currMaterial = new Material();
			materialName = tokens[1];
			_materials->insert( pair<string, Material*>( materialName, currMaterial ) );
		} else if( !tokens[0].compare( "Ka" ) ) {					// ambient component
			GLfloat ambient[4];
			ambient[0] = atof( tokens[1].c_str() );
			ambient[1] = atof( tokens[2].c_str() );
			ambient[2] = atof( tokens[3].c_str() );
			ambient[3] = 1;
			currMaterial->setAmbient( ambient );
		} else if( !tokens[0].compare( "Kd" ) ) {					// diffuse component
			GLfloat diffuse[4];
			diffuse[0] = atof( tokens[1].c_str() );
			diffuse[1] = atof( tokens[2].c_str() );
			diffuse[2] = atof( tokens[3].c_str() );
			diffuse[3] = 1;			
			currMaterial->setDiffuse( diffuse );
		} else if( !tokens[0].compare( "Ks" ) ) {					// specular component
			GLfloat specular[4];
			specular[0] = atof( tokens[1].c_str() );
			specular[1] = atof( tokens[2].c_str() );
			specular[2] = atof( tokens[3].c_str() );
			specular[3] = 1;						
			currMaterial->setSpecular( specular );
		} else if( !tokens[0].compare( "Ke" ) ) {					// emissive component
			GLfloat emissive[4];
			emissive[0] = atof( tokens[1].c_str() );
			emissive[1] = atof( tokens[2].c_str() );
			emissive[2] = atof( tokens[3].c_str() );
			currMaterial->setEmissive( emissive );
		} else if( !tokens[0].compare( "Ns" ) ) {					// shininess component
			currMaterial->setShininess( atof( tokens[1].c_str() ) );
		} else if( !tokens[0].compare( "Tr" ) || !tokens[0].compare( "d" ) ) {					// transparency component - Tr or d can be used depending on the format
			currMaterial->getAmbient()[3] = atof( tokens[1].c_str() );
			currMaterial->getDiffuse()[3] = atof( tokens[1].c_str() );
			currMaterial->getSpecular()[3] = atof( tokens[1].c_str() );
		} else if( !tokens[0].compare( "illum" ) ) {				// illumination type component
			currMaterial->setIllumination( atoi( tokens[1].c_str() ) );
		} else {
			cout << "[.mtl]: ignoring line: " << line << endl;
		}
	}
	
	in.close();
	
	return result;
}
