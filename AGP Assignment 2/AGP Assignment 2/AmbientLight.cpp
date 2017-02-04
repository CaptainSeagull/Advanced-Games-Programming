#include "AmbientLight.h"
#include "UniformBufferObject.h"

AmbientLight::AmbientLight() : AmbientLight(glm::vec4(0.0f))
{
}

AmbientLight::AmbientLight(glm::vec4 colour) : colour(colour)
{
}

void AmbientLight::setUniforms(UniformBufferObject *ubo, unsigned int index)
{
	if (ubo != nullptr)
	{
		ubo->bind();
		if (on)
		{
			ubo->setUniform("ambient", colour);
		}
		else
		{
			ubo->setUniform("ambient", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		}
		UniformBufferObject::unbind();
	}
}

AmbientLight::~AmbientLight()
{
}
