#pragma once
#include <string>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <map>
#include "Material.h"

class Shader
{
private:
	std::map<std::string, GLuint> uniformLocations;
	std::string name;
	GLuint vertexID = -1, fragmentID = -1, geometryID = -1, programID = -1;
	char* loadFile(std::string file, int &fileSize);
	void printError(const GLint id);
	GLuint createShader(std::string file, GLenum type);
public:

	enum ShaderAttributes
	{
		POSITION,
		COLOUR,
		NORMAL,
		TEXTURE,
		INDEX
	};
	enum ShaderTextures
	{
		CUBEMAP,
		TEXTUREMAP
	};

	Shader(std::string vertex, std::string fragment, std::string name);
	Shader(std::string vertex, std::string fragment, std::string geometry, std::string name);
	void bindProgram();
	void setMaterial(Material *material);
	void setUniform(std::string name, glm::mat4 mat);
	void setUniform(std::string name, glm::vec3 vec3);
	void setUniform(std::string name, glm::vec4 vec4);
	void setUniform(std::string name, GLfloat floatingPoint);
	void setUniform(std::string name, GLint integer);
	GLuint getUniformLocation(std::string uniform);
	GLuint getShaderProgram() { return programID; }
	std::string getName(){ return name; }
	bool compile(GLuint shaderID);
	~Shader();
};

