#pragma once
#include <glm\glm.hpp>

class FreeCamera
{
private:
	glm::vec3 eye = glm::vec3(0.0f, 1.0f, 5.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 viewMat = glm::mat4(1.0f);
	float angle = 0;
public:
	FreeCamera();
	glm::mat4 getViewMat(){ return viewMat; }
	glm::vec3 getPosition() { return eye; }
	void moveForward(float distance);
	void moveRight(float distance);
	void moveUp(float distance);
	void rotateY(float angle);
	void setPosition(glm::vec3 position);
	void update();
	~FreeCamera();
};

