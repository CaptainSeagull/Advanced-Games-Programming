#version 330
const int maxLights = 102;

layout (shared) uniform LightBlock
{
	int numLights;
	vec4 position[maxLights];
	vec4 direction[maxLights];
	vec4 ambient;
	vec4 diffuse[maxLights];
	vec4 specular[maxLights];
	vec3 attenuation[maxLights];
	float cosCutoffAngle[maxLights];
	float secondaryCutoffAngle[maxLights];
};

struct materialStruct
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform materialStruct material;

layout (location = 0) out vec4 colour;

void main(void) 
{   
	colour = ambient * material.ambient;
}