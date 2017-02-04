#pragma once
#include "rt3d.h"
#include <glm/glm.hpp>
#include "Texture.h"
#include "Shader.h"
#include "snowExplosions.h"
#include <vector>
#include <random>

class snow
{
protected:
	int numParticles;
	GLuint vao[1];
	GLuint vbo[2];
	Texture *texture = nullptr;
	GLfloat *positions;
	GLfloat *velocity;
	glm::vec3 minVelocity;
	glm::vec3 maxVelocity;
	unsigned int previousTime = 0;
	Shader *shader;
	std::vector<snowExplosions*> snowExplosionVector;
	std::default_random_engine numberGenerator;
public:
	snow(const int n);
	~snow();
	void init(Shader *shader);
	int getNumParticles() const { return numParticles; }
	void update();
	void draw(GLfloat pointSize);
	void setTexture(Texture *textureID) { texture = textureID; }
};

