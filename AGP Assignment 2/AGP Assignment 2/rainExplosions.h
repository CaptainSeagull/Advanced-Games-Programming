#pragma once
#include "rt3d.h"
#include <glm/glm.hpp>
#include "Texture.h"
#include "Shader.h"

class rainExplosions
{
protected:
	int numParticles;
	GLuint vao[1];
	GLuint vbo[2];
	Texture *texture = nullptr;
	GLfloat *positions;
	GLfloat *velocity;
	GLfloat *acceleration;
	glm::vec3 minVelocity;
	glm::vec3 maxVelocity;
	unsigned int previousTime = 0;
	GLfloat currentY;
	Shader *shader;
public:
	rainExplosions(const int n);
	~rainExplosions();
	void init(Shader *shader, float x, float y, float z);
	int getNumParticles() const { return numParticles; }
	void update();
	void draw(GLfloat pointSize);
	void setTexture(Texture *textureID) { texture = textureID; }
	GLfloat getCurrentY() { return currentY; }
};

