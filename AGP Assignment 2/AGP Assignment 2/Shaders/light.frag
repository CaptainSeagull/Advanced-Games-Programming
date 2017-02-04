#version 330

precision highp float;

uniform vec4 colour;

layout (location = 0) out vec4 outColour;
 
void main(void) 
{   
	outColour = colour;
}