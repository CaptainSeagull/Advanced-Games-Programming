#include "Material.h"
using namespace glm;

Material::Material() : Material(vec4(0.0f), vec4(0.0f), vec4(0.0f), 0.0f)
{
}

Material::Material(glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float shininess)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
}

Material::~Material()
{
}
