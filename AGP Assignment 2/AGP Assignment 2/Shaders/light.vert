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

void main(void) 
{
	vec4 vertexPosition = modelViewMat * vec4(in_Position,1.0);

	gl_Position = projectionMat * vertexPosition;
}
