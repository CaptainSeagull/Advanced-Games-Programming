#pragma once
#include "Light.h"
class Pointlight :
	public Light
{
private:
	glm::vec4 ambient, diffuse, specular;
	float cutoff;
	void setUniforms(UniformBufferObject *ubo, unsigned int index);
public:
	Pointlight();
	Pointlight(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular);
	~Pointlight();
};

