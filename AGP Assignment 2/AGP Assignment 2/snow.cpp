#include "snow.h"
#include <iostream>
#include<chrono>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

static GLfloat acceleration[] = { 0.0f, -0.0f, 0.0f };

static const glm::vec3 position(0.0f, 450.0f, 0.0f);
static const glm::vec3 size(2500.0f, 1800.f, 2500.0f);

snow::snow(const int n) : numParticles(n)
{
	if (numParticles <= 0) //Invalid Input
		return;

	//Set minimum and maximum velocities
	minVelocity = glm::vec3(0.0f, 100.0f, 0.0f);
	maxVelocity = glm::vec3(0.0f, 200.0f, 0.0f);
}

snow::~snow()
{
	delete[]positions;
	delete[]velocity;
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, vao);
}

void snow::init(Shader *shader)
{
	this->shader = shader;
	//Initialise arrays
	positions = new GLfloat[numParticles * 3];
	GLfloat *colours = new GLfloat[numParticles * 3];
	velocity = new GLfloat[numParticles * 3];

	//Create random number generator and set values to randomise between for X, Y + Z
	unsigned seed = (unsigned int)std::chrono::system_clock::now().time_since_epoch().count();
	numberGenerator = std::default_random_engine(seed);
	std::uniform_real_distribution <float> velocityXDistribution(minVelocity.x, maxVelocity.x);
	std::uniform_real_distribution <float> velocityYDistribution(minVelocity.y, maxVelocity.y);
	std::uniform_real_distribution <float> velocityZDistribution(minVelocity.z, maxVelocity.z);
	std::uniform_real_distribution <float> positionXDistribution(position.x - size.x / 2.0f, position.x + size.x / 2.0f);
	std::uniform_real_distribution <float> positionYDistribution(position.y - size.y / 2.0f, position.y + size.y / 2.0f);
	std::uniform_real_distribution <float> positionZDistribution(position.z - size.z / 2.0f, position.z + size.z / 2.0f);

	//For each co-ordinate of each particle, set position, colour and velocity
	for (int i = 0; i < numParticles * 3; i++)
	{
		if (i % 3 == 0) //X
		{
			positions[i] = positionXDistribution(numberGenerator);
			velocity[i] = velocityXDistribution(numberGenerator);
			colours[i] = 1.0f;
		}
		else if (i % 3 == 1) //Y
		{
			positions[i] = positionYDistribution(numberGenerator);
			velocity[i] = -velocityYDistribution(numberGenerator);
			colours[i] = 1.0f;
		}
		else //Z
		{
			positions[i] = positionZDistribution(numberGenerator);
			velocity[i] = velocityZDistribution(numberGenerator);
			colours[i] = 1.0f;
		}
	}

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

void snow::draw(GLfloat pointSize)
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

	for (auto rainExplosion : snowExplosionVector)
		rainExplosion->draw(pointSize);
}

void snow::update()
{
	float deltaTime = (SDL_GetTicks() - previousTime) / 1000.0f;

	for (int i = 0; i < numParticles * 3; i++) //For each co-ordinate of each particle
	{
		if (i % 3 == 0) //X
			velocity[i] += acceleration[0] * deltaTime;
		else if (i % 3 == 1) //Y
		{
			velocity[i] += acceleration[1] * deltaTime;
			if (positions[i] <= position.y - size.y / 2.0f) //If particle is off screen, respawn above screen
			{
				snowExplosionVector.push_back(new snowExplosions(1));
				snowExplosionVector.back()->setTexture(texture);
				snowExplosionVector.back()->init(shader, positions[i - 1], positions[i], positions[i + 1]);

				std::uniform_real_distribution <float> positionXDistribution(position.x - size.x / 2.0f, position.x + size.x / 2.0f);
				std::uniform_real_distribution <float> positionYDistribution(position.y - size.y / 2.0f, position.y + size.y / 2.0f);
				std::uniform_real_distribution <float> positionZDistribution(position.z - size.z / 2.0f, position.z + size.z / 2.0f);
				positions[i] = maxVelocity.y;
				positions[i - 1] = positionXDistribution(numberGenerator);
				positions[i + 1] = positionZDistribution(numberGenerator);
			}
		}
		else //Z 
			velocity[i] += acceleration[2] * deltaTime;

		positions[i] += velocity[i] * deltaTime; //Update positions

		if (snowExplosionVector.size() >= 500)
		{
			snowExplosions* snow = snowExplosionVector[0];
			delete snow;
			snowExplosionVector.erase(snowExplosionVector.begin());
		}
	}

	glBindVertexArray(vao[0]); // bind VAO 0 as current object 

	// VBO for position data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * numParticles*sizeof(GLfloat), positions, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // bind VAO 0 as current object 

	previousTime = SDL_GetTicks(); //Get new "previousTime" for next pass of update
}

