/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 120

varying vec4 theColor;
varying vec4 something;
uniform float time;

void main() {

    /*****************************************/
    /******* Final Color Calculations ********/
    /*****************************************/
    
	gl_FragColor = theColor;
}