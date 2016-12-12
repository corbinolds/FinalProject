#ifndef _SHADER_UTILS_H_
#define _SHADER_UTILS_H_

#include <GL/glew.h>

#include <stdio.h>

#include <fstream>
#include <string>
using namespace std;

// readTextFromFile() //////////////////////////////////////////////////////////////
//
//  Reads in a text file as a single string. Used to aid in shader loading.
//
////////////////////////////////////////////////////////////////////////////////
void readTextFromFile(string filename, char* &output){
    string buf = string("");
    string line;
    
    ifstream in(filename.c_str());
    while( getline(in, line) ) {
        buf += line + "\n";
    }
    output = new char[buf.length()+1];
    strncpy(output, buf.c_str(), buf.length());
    output[buf.length()] = '\0';
    
    in.close();
}

// printLog() //////////////////////////////////////////////////////////////////
//
//  Check for errors from compiling or linking a vertex/fragment/shader program
//	Prints to terminal
//
////////////////////////////////////////////////////////////////////////////////
void printLog( GLuint handle ) {
    int infologLength = 0;
    int maxLength;
    bool isShader;
    
    /* check if the handle is to a vertex/fragment shader */
    if( glIsShader( handle ) ) {
        glGetShaderiv(  handle, GL_INFO_LOG_LENGTH, &maxLength );

        isShader = true;
    }
    /* check if the handle is to a shader program */
    else {
        glGetProgramiv( handle, GL_INFO_LOG_LENGTH, &maxLength );

        isShader = false;
    }
    
    /* create a buffer of designated length */
    char infoLog[maxLength];
    
    if( isShader ) {
        /* get the info log for the vertex/fragment shader */
        glGetShaderInfoLog(  handle, maxLength, &infologLength, infoLog );
    } else {
        /* get the info log for the shader program */
        glGetProgramInfoLog( handle, maxLength, &infologLength, infoLog );
    }
    
    /* if the length of the log is greater than 0 */
    if( infologLength > 0 ) {
        /* print info to terminal */
        printf( "[INFO]: %s Handle %d: %s\n", (isShader ? "Shader" : "Program"), handle, infoLog );
    }
}

// compileShader() ///////////////////////////////////////////////////////////////
//
//  Compile a given shader program
//
////////////////////////////////////////////////////////////////////////////////
GLuint compileShader( string filename, GLenum shaderType ) {
	GLuint shaderHandle = 0;
	char *shaderString;
    
    /* create a handle to our shader */
    // TODO #1: create the shader handle
	shaderHandle = glCreateShader(shaderType);

	/* read in each text file and store the contents in a string */
	readTextFromFile( filename.c_str(), shaderString );

    /* send the contents of each program to the GPU */
    // TODO #2: send the shader code to the GPU
	glShaderSource(shaderHandle, 1, (const char**)&shaderString, NULL);
    
    /* we are good programmers so free up the memory used by each buffer */
    delete [] shaderString;
    
    /* compile each shader on the GPU */
    // TODO #3: compile the shader on the GPU glCompileShader( shaderHandle );
	glCompileShader(shaderHandle);

    /* check the shader log */
    printLog( shaderHandle );
    
    /* return the handle of our shader */
    return shaderHandle;
}

// createShaderProgram() ///////////////////////////////////////////////////////
//
//  Compile and Register our Vertex and Fragment Shaders
//
////////////////////////////////////////////////////////////////////////////////
GLuint createShaderProgram( char *vertexShaderFilename, char *fragmentShaderFilename ) {
	GLuint shaderProgramHandle;

    /* compile each one of our shaders */
    GLuint vertexShaderHandle   = compileShader( vertexShaderFilename,   GL_VERTEX_SHADER   );
    GLuint fragmentShaderHandle = compileShader( fragmentShaderFilename, GL_FRAGMENT_SHADER );

    /* get a handle to a shader program */
    // TODO #4: create the program handle
	shaderProgramHandle = glCreateProgram();
    
    /* attach the vertex and fragment shaders to the shader program */
    // TODO #5: attach our shaders to the program
	glAttachShader(shaderProgramHandle, vertexShaderHandle);
	glAttachShader(shaderProgramHandle, fragmentShaderHandle);
    
    /* link all the programs together on the GPU */
    // TODO #6: link our shader program together on the GPU
	glLinkProgram(shaderProgramHandle);

    /* check the program log */
    printLog( shaderProgramHandle );
    
    /* return handle */
    return shaderProgramHandle;
}

#endif
