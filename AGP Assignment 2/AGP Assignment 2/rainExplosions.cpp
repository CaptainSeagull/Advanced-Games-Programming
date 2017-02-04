#include "rainExplosions.h"
#include <random>
#include<chrono>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

rainExplosions::rainExplosions(const int n) : numParticles(n)
{
	if (numParticles <= 0) //Invalid Input
		return;

	//Sets minimum and maximum velocity
	minVelocity = glm::vec3(-300.0f, 0.0f, -300.0f);
	maxVelocity = glm::vec3(300.0f, 500.0, 300.0f);
}


rainExplosions::~rainExplosions()
{
	delete[]positions;
	delete[]velocity;
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, vao);
}

void rainExplosions::init(Shader *shader, float x, float y, float z)
{
	this->shader = shader;
	//Initialise arrays
	positions = new GLfloat[numParticles * 3];
	GLfloat *colours = new GLfloat[numParticles * 3];
	velocity = new GLfloat[numParticles * 3];
	acceleration = new GLfloat[3];
	acceleration[0] = 0.0f;
	acceleration[1] = -1000.0f;
	acceleration[2] = 0.0f; 
	//lifetime = 400;

	//Create random number generator and set values to randomise between for X, Y + Z
	unsigned seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine numberGenerator(seed);
	std::uniform_real_distribution <float> velocityXDistribution(minVelocity.x, maxVelocity.x);
	std::uniform_real_distribution <float> velocityYDistribution(minVelocity.y, maxVelocity.y);
	std::uniform_real_distribution <float> velocityZDistribution(minVelocity.z, maxVelocity.z);

	//For each co-ordinate of each particle, set position, colour and velocity
	for (int i = 0; i < numParticles * 3; i++)
	{
		if (i % 3 == 0) //X
		{

			positions[i] = x;
			velocity[i] = velocityXDistribution(numberGenerator);
			colours[i] = 1.0f;
		}
		else if (i % 3 == 1) //Y
		{
			positions[i] = y;
			//velocity[i] = velocityYDistribution(numberGenerator);
			velocity[i] = maxVelocity.y;
			colours[i] = 1.0f;
		}
		else //Z
		{
			positions[i] = z;
			velocity[i] = velocityZDistribution(numberGenerator);
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

void rainExplosions::draw(GLfloat pointSize)
{
	shader->bindProgram();
	glPointSize(pointSize); //Defines point size
	//shader->setUniform("pointSize", pointSize);
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

void rainExplosions::update()
{
	float deltaTime = (SDL_GetTicks() - previousTime) / 1000.0f;
		
	for (int i = 0; i < numParticles * 3; i++) //For each co-ordinate of each particle
	{
		if (i % 3 == 0) //X
		{
			velocity[i] += acceleration[0] * deltaTime;
		}
		else if (i % 3 == 1) //Y
		{
			velocity[i] += acceleration[1] * deltaTime;
		}
		else //Z 
		{
			velocity[i] += acceleration[2] * deltaTime;
		}

		positions[i] += velocity[i] * deltaTime; //Update positions
	}

	currentY += velocity[1] * deltaTime;

	glBindVertexArray(vao[0]); // bind VAO 0 as current object 

	// VBO for position data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * numParticles*sizeof(GLfloat), positions, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0); // bind VAO 0 as current object 

	previousTime = SDL_GetTicks(); //Get new "previousTime" for next pass of update
}