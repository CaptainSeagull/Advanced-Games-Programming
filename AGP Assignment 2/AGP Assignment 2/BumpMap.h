#pragma once

#include <vector>
#include <GL/glew.h>
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include "glm\glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>

class BumpMap
{
private:
	GLuint meshIndexCount;
	GLuint meshObjects[2];
	GLuint textures[4];
	GLuint reflectionMapProgram;

public:
	BumpMap();
	~BumpMap();

	void render(GLuint textures[], GLuint reflectionMapProgram, std::stack<glm::mat4> mvStack, float &theta, glm::vec3 eye, glm::vec3 up, glm::vec3 at, GLuint meshObjects[], GLuint meshIndexCount);

	void calculateTangents(std::vector<GLfloat> &tangents, std::vector<GLfloat> &verts, std::vector<GLfloat> &normals, std::vector<GLfloat> &tex_coords, std::vector<GLuint> &indices);

	GLuint loadBitmap(char *fname);
};