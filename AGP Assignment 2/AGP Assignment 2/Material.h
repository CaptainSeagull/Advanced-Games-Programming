#pragma once
#include <glm\glm.hpp>
class Material
{
public:
	glm::vec4 ambient, diffuse, specular;
	float shininess;
	Material();
	Material(glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float shininess);
	~Material();
};

