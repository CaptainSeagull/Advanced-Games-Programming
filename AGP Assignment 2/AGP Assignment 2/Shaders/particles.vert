// Vertex Shader – file "particles.vert"
#version 330

const float MAX_PARTICLE_SIZE = 30;
const float MAX_DISTANCE = 1000;

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
out vec3 ex_Color;
out float height;

layout(shared) uniform MatrixBlock
{
	 mat4 modelMat;
	 mat4 viewMat;
	 mat4 modelviewMat;
	 mat4 projectionMat;
	 mat3 normalMat;
	 vec3 eyePos;
};

out float distanceToEye;
uniform float pointSize;

void main(void)
{
	ex_Color = in_Color; 

	for(int i=0; i<3; i++)
	{
		vec4 originalPos = vec4(in_Position, 1.0); 
		vec4 worldPosition = originalPos;
		gl_Position = projectionMat * viewMat * (originalPos);
		height = gl_Position.y;
	}

	//distanceToEye = length(eyePos - worldPosition.xyz);
	//gl_PointSize = MAX_PARTICLE_SIZE;

	//if(distanceToEye >= MAX_DISTANCE)
	//	gl_PointSize = 0;
}
