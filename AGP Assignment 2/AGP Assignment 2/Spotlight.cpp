#include "Spotlight.h"
#include "Shader.h"
#include "UniformBufferObject.h"
using namespace glm;

Spotlight::Spotlight() : Spotlight(vec4(0.0f), vec4(0.0f), vec4(0.0f), vec4(0.0f), vec4(0.0f), 0.0f, 0.0f)
{
}

Spotlight::Spotlight(glm::vec4 position, glm::vec4 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float coneAngle, float secondaryConeAngle) :
Light(position), direction(direction), ambient(ambient), diffuse(diffuse), specular(specular), coneAngle(coneAngle), secondaryConeAngle(secondaryConeAngle)
{
}

void Spotlight::setUniforms(UniformBufferObject *ubo, unsigned int index)
{
	if (ubo != nullptr)
	{
		ubo->bind();
		if (on)
		{
			ubo->setUniform("diffuse", diffuse, index);
			ubo->setUniform("specular", specular, index);
			ubo->setUniform("attenuation", vec3(0.0f, 0.007f, 0.00035f), index);
			ubo->setUniform("position", position, index);
			ubo->setUniform("direction", direction, index);
			ubo->setUniform("cosCutoffAngle", glm::cos(glm::radians(coneAngle)), index);
			ubo->setUniform("secondaryCutoffAngle", glm::cos(glm::radians(secondaryConeAngle)), index);
		}
		else
		{
			ubo->setUniform("diffuse", vec4(0.0f, 0.0f, 0.0f, 1.0f), index);
			ubo->setUniform("specular", vec4(0.0f, 0.0f, 0.0f, 1.0f), index);
			ubo->setUniform("attenuation", vec3(0.0f, 0.0f, 0.0f), index);
			ubo->setUniform("position", vec4(0.0f, 0.0f, 0.0f, 1.0f), index);
			ubo->setUniform("direction", vec4(0.0f, 0.0f, 0.0f, 1.0f), index);
			ubo->setUniform("cosCutoffAngle", 0, index);
			ubo->setUniform("secondaryCutoffAngle", 0, index);
		}
		UniformBufferObject::unbind();
	}
}

Spotlight::~Spotlight()
{
}
