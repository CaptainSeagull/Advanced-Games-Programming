#pragma once
#include <glm\glm.hpp>
#include <map>
#include <string>
#include <iostream>

class UniformBufferObject;
class Shader;
class Model;

class Light
{
	friend class LightManager;
protected:
	glm::vec4 position;
	virtual void setUniforms(UniformBufferObject *ubo, unsigned int index) = 0;
	Model *mesh;
	Shader *shader;
	std::string tag;
	bool on = true;
public:
	Light();
	Light(glm::vec4 position);
	void setPosition(glm::vec4 position) { this->position = position; }
	void move(glm::vec3 distance) { position += glm::vec4(distance, 0.0f); }
	void setMesh(Model *mesh, Shader *shader) { this->mesh = mesh; this->shader = shader; }
	void draw(glm::mat4 viewMat, UniformBufferObject *ubo);
	void setTag(std::string tag){ this->tag = tag; }
	void turnOnOff(){ on = !on; }
	std::string getTag(){ return tag; }
	~Light();
};

