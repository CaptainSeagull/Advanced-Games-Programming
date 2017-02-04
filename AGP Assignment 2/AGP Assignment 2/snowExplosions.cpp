#include "snowExplosions.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;


snowExplosions::snowExplosions(const int n) : numParticles(n)
{
	if (numParticles <= 0) //Invalid Input
		return;

	//Sets minimum and maximum velocity
	minSpread = glm::vec3(-100.0f, 0.0f, -100.0f);
	maxSpread = glm::vec3(100.0f, 0.0f, 100.0f);
}


snowExplosions::~snowExplosions()
{
	delete[]positions;
	delete[]velocity;
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, vao);
}

void snowExplosions::init(Shader *shader, float x, float y, float z)
{
	this->shader = shader;
	//Initialise arrays
	positions = new GLfloat[numParticles * 3];
	GLfloat *colours = new GLfloat[numParticles * 3];

	//For each co-ordinate of each particle, set position, colour and velocity
	for (int i = 0; i < numParticles * 3; i++)
	{
		if (i % 3 == 0) //X
		{
			positions[i] = x;
			colours[i] = 1.0f;
		}
		else if (i % 3 == 1) //Y
		{
			positions[i] = y;
			colours[i] = 1.0f;
		}
		else //Z
		{
			positions[i] = z;
			colours[i] = 1.0f;
		}
	}

	currentY = y;

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(2, vbo);

	// VBO for position data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * numParticles*sizeof(GLfloat), positions, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// VBO for colour data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * numParticles*sizeof(GLfloat), colours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] colours;
	previousTime = SDL_GetTicks();
}

void snowExplosions::draw(GLfloat pointSize)
{
	shader->bindProgram();
	glPointSize(pointSize); //Defines point size
	texture->bind();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glBindVertexArray(vao[0]); // bind VAO 0 as current object 
	glDepthMask(0); //DEPTH MASK
	glDrawArrays(GL_POINTS, 0, numParticles);
	glDepthMask(1); //DEPTH MASK OFF
	//glDisable(GL_BLEND);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}