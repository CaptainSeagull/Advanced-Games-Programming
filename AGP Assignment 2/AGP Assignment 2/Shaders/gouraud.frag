// Phong fragment shader phong-tex.frag matched with phong-tex.vert
#version 330

// Some drivers require the following
precision highp float;

uniform sampler2D textureUnit0;
uniform bool textured = true;

in VS_OUT
{
	vec4 colour;
	vec2 texCoord;
} fs_in;

layout (location = 0) out vec4 colour;
 
void main(void) 
{
	vec4 texColour = texture(textureUnit0, fs_in.texCoord);
	vec4 outColour = fs_in.colour;

	if(textured)
		colour = fs_in.colour * texColour;
	else
		colour = fs_in.colour;
}