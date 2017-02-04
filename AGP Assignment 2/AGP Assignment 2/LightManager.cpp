#include "LightManager.h"
#include "UniformBufferObject.h"
#include <SDL.h>

LightManager::LightManager(UniformBufferObject *lightsUBO) : lightsUBO(lightsUBO)
{
	lights = new std::vector<Light*>();
}

void LightManager::addLight(Light *light)
{
	if (lights->size() < MAX_LIGHTS && lightsUBO != nullptr)
	{
		lights->push_back(light);
		lightsUBO->bind();
		lightsUBO->setUniform("numLights", lights->size());
		UniformBufferObject::unbind();
		light->setUniforms(lightsUBO, lights->size()-1);
		numLights++;
	}
}

void LightManager::addLight(AmbientLight *light)
{
	ambientLight = light;
	lightsUBO->bind();
	lightsUBO->setUniform("numLights", numLights);
	UniformBufferObject::unbind();
	ambientLight->setUniforms(lightsUBO, 0);
}

void LightManager::updateUniforms()
{
	for (unsigned int i = 0; i < lights->size(); i++)
	{
		lights->at(i)->setUniforms(lightsUBO, i);
	}
	ambientLight->setUniforms(lightsUBO, 0);
}

void LightManager::flipSwitches()
{
	for (unsigned int i = 0; i < lights->size(); i++)
	{
			lights->at(i)->turnOnOff();
	}
	ambientLight->turnOnOff();
}

void LightManager::flipSwitches(std::string tag)
{
	for (unsigned int i = 0; i < lights->size(); i++)
	{
		if (lights->at(i)->getTag() == tag)
			lights->at(i)->turnOnOff();
	}
	if (ambientLight->getTag() == tag)
		ambientLight->turnOnOff();
}

void LightManager::update()
{
	for each (Light *light in *lights)
	{
		if (light->tag == "Pointlight")
		{
			/*light->position.y = (sin(SDL_GetTicks()/1000.0f)+1)*60 + 30;
			light->position.x = sin(SDL_GetTicks() / 1000.0f)*60 + 60;
			light->position.z = cos(SDL_GetTicks() / 1000.0f)*60 + 60;*/
		}
	}
}

void LightManager::draw(glm::mat4 viewMat, UniformBufferObject *ubo)
{
	for (unsigned int i = 0; i < lights->size(); i++)
	{
		lights->at(i)->setUniforms(lightsUBO, i);
		lights->at(i)->draw(viewMat, ubo);
	}
}

LightManager::~LightManager()
{
}
