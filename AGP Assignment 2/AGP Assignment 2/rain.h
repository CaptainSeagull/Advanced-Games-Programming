#pragma once
#include "rt3d.h"
#include <glm/glm.hpp>
#include "Texture.h"
#include "Shader.h"
#include "rainExplosions.h"
#include <vector>

class rain
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
	std::vector<rainExplosions*> rainExplosionVector;

public:
	rain(const int n);
	~rain();
	void init(Shader *shader);
	int getNumParticles() const { return numParticles; }
	void update();
	void draw(GLfloat pointSize);
	void setTexture(Texture *textureID) { texture = textureID; }
};

