#include "Pointlight.h"
#include "Shader.h"
#include "UniformBufferObject.h"

using namespace glm;

Pointlight::Pointlight() : Pointlight(vec4(0.0f), vec4(0.0f), vec4(0.0f), vec4(0.0f))
{
}

Pointlight::Pointlight(vec4 position, vec4 ambient, vec4 diffuse, vec4 specular) :
Light(position), ambient(ambient), diffuse(diffuse), specular(specular)
{
}

void Pointlight::setUniforms(UniformBufferObject *ubo, unsigned int index)
{
	if (ubo != nullptr)
	{
		ubo->bind();
		if (on)
		{
			ubo->setUniform("diffuse", diffuse, index);
			ubo->setUniform("specular", specular, index);
			//ubo->setUniform("attenuation", vec3(0.0f, 0.007f, 0.00035f), index);
			ubo->setUniform("attenuation", vec3(1.0f, 0.0f, 0.0f), index);
			ubo->setUniform("position", position, index);
			ubo->setUniform("direction", vec4(0.0f, 0.0f, 0.0f, 0.0f), index);
			ubo->setUniform("cosCutoffAngle", 0, index);
			ubo->setUniform("secondaryCutoffAngle", 0, index);
			UniformBufferObject::unbind();
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

Pointlight::~Pointlight()
{
}
