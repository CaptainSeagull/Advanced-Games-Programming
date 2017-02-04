#pragma once
#include "rt3d.h"
#include <glm/glm.hpp>
#include "Texture.h"
#include "Shader.h"

class snowExplosions
{
protected:
	int numParticles;
	GLuint vao[1];
	GLuint vbo[2];
	Texture *texture = nullptr;
	GLfloat *positions;
	GLfloat *velocity;
	GLfloat *acceleration;
	glm::vec3 minSpread;
	glm::vec3 maxSpread;
	unsigned int previousTime = 0;
	GLfloat currentY;
	Shader *shader;
public:
	snowExplosions(const int n);
	~snowExplosions();
	void init(Shader *shader, float x, float y, float z);
	int getNumParticles() const { return numParticles; }
	void draw(GLfloat pointSize);
	void setTexture(Texture *textureID) { texture = textureID; }
	GLfloat getCurrentY() { return currentY; }
};
