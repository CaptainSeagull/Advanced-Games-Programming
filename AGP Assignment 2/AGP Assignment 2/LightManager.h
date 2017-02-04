#pragma once
#include <map>
#include <vector>
#include "Pointlight.h"
#include "Spotlight.h"
#include "AmbientLight.h"
#include <string>

class LightManager
{
private:
	std::vector<Light*> *lights;
	Light *ambientLight;
	UniformBufferObject *lightsUBO;
	const unsigned int MAX_LIGHTS = 102;
	unsigned int numLights = 0;
public:
	LightManager(UniformBufferObject *lightsUBO);
	void addLight(Light *light);
	void addLight(AmbientLight *light);
	void updateUniforms();
	void draw(glm::mat4 viewMat, UniformBufferObject *ubo);
	void update();
	void flipSwitches();
	void flipSwitches(std::string tags);
	int lightCount() { return numLights; }
	~LightManager();
};

