/*
 *   Vertex Shader
 *   For the power up!
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 120

varying vec4 theColor;
varying vec4 something;
uniform float time;

void main() {
    /*****************************************/
    /********* Vertex Calculations  **********/
    /*****************************************/
    
    
    // TODO #8: Perform the Vertex Transformation from Object Space to Clip Space
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	something = vec4(sin(time), cos(time), 0.5, 1.0);
    
	theColor = something;

}