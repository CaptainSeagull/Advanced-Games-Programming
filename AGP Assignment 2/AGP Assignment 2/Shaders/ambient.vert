#version 330

layout (std140) uniform MatrixBlock
{
	mat4 modelMat;
	mat4 viewMat;
	mat4 modelViewMat;
	mat4 projectionMat;
	mat4 mvpMat;
	mat3 normalMat;
	vec3 eyePos;
};

layout (location = 0) in  vec3 in_Position;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec2 in_TexCoord;

void main(void) 
{
	gl_Position = mvpMat * vec4(in_Position, 1.0f);
}
