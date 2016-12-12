/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 120

varying vec4 theColor;
uniform float time;
uniform bool dressColor;
varying vec4 sampleVertex;

void main() {
    /*****************************************/
    /********* Vertex Calculations  **********/
    /*****************************************/

	//if (gl_Vertex.x > 0 && gl_Vertex.y > 0 && gl_Vertex.z > 0) {
		sampleVertex = vec4(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z + (.4 * ((sin(time) + 1)/2)) - .2, 1.0);
	//} 
	//else {
	//	sampleVertex = gl_Vertex;
	//}

	gl_Position = gl_ModelViewProjectionMatrix * sampleVertex;
    
	if (dressColor) {
		theColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	else {
		theColor = gl_Vertex;
	}
}
