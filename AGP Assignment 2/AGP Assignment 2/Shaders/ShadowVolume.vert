#version 330

layout (shared) uniform MatrixBlock
{
	mat4 modelMat;
	mat4 viewMat;
	mat4 modelViewMat;
	mat4 projectionMat;
	mat4 mvpMat;
	mat3 normalMat;
	vec3 eyePos;
};

layout (location = 0) in vec3 in_Position;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec2 in_TexCoord;

out Vectors
{
	vec3 vertexWorldPosition;
	vec3 vertexWorldNormal;
} vs_out;

void main ()
{
	vs_out.vertexWorldPosition = (modelMat * vec4(in_Position, 1.0)).xyz;
	vs_out.vertexWorldNormal = (normalMat * in_Normal);
}