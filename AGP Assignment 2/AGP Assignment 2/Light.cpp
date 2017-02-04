#include "Light.h"
#include "Model.h"
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;
using namespace std;

Light::Light() : Light(vec4(0.0f))
{

}

Light::Light(glm::vec4 position) : position(position)
{
}

void Light::draw(glm::mat4 viewMat, UniformBufferObject *ubo)
{
	if (mesh != nullptr && shader != nullptr && ubo != nullptr && on)
	{
		ubo->bind();
		shader->bindProgram();
		shader->setUniform("textured", false);
		shader->setUniform("colour", vec4(1.0f, 1.0f, 1.0f, 1.0f));

		mat4 modelMat(1.0f);
		modelMat = glm::translate(modelMat, vec3(position));
		modelMat = scale(modelMat, vec3(100.0f, 100.0f, 100.0f));

		mat4 modelViewMat = viewMat * modelMat;
		mat3 normalMat = transpose(inverse(mat3(modelViewMat)));

		ubo->setUniform("modelMat", modelMat);
		ubo->setUniform("modelViewMat", modelViewMat);
		ubo->setUniform("normalMat", normalMat);

		mesh->drawRawData();
		UniformBufferObject::unbind();
	}
}

Light::~Light()
{
}
