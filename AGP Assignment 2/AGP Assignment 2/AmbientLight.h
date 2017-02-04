#pragma once
#include "Light.h"
class AmbientLight :
	public Light
{
private:
	glm::vec4 colour;
	void setUniforms(UniformBufferObject *ubo, unsigned int index);
public:
	AmbientLight();
	AmbientLight(glm::vec4 colour);
	~AmbientLight();
};

