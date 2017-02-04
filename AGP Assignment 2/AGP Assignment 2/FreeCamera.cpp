#include "FreeCamera.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <iostream>

using namespace glm;

FreeCamera::FreeCamera()
{
}

void FreeCamera::moveRight(float distance)
{
	eye = glm::vec3(eye.x + distance*cos(radians(angle)), eye.y, eye.z + distance*sin(radians(angle)));
}

void FreeCamera::moveForward(float distance)
{
	eye = glm::vec3(eye.x + distance*sin(radians(angle)), eye.y, eye.z - distance*cos(radians(angle)));
}

void FreeCamera::moveUp(float distance)
{
	eye.y += distance;
}

void FreeCamera::rotateY(float angle)
{
	this->angle += angle;
	if (this->angle >= 360)
		this->angle -= 360;
	else if (this->angle <= -360)
		this->angle += 360;
}

void FreeCamera::update()
{
	vec3 at = glm::vec3(eye.x + 1.0f*sin(radians(angle)), eye.y, eye.z - 1.0f*cos(radians(angle)));

	viewMat = lookAt(eye, at, up);
}

void FreeCamera::setPosition(glm::vec3 newPosition)
{
	eye = newPosition;
}

FreeCamera::~FreeCamera()
{
}
