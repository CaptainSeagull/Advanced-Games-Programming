#pragma once
#include "Light.h"
class Spotlight :
	public Light
{
private:
	glm::vec4 ambient, diffuse, specular, direction;
	float coneAngle, secondaryConeAngle;
	void setUniforms(UniformBufferObject *ubo, unsigned int index);
public:
	Spotlight();
	Spotlight(glm::vec4 position, glm::vec4 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float coneAngle, float secondaryConeAngle);
	void setDirection(glm::vec4 direction){ this->direction = direction; }
	~Spotlight();
};

