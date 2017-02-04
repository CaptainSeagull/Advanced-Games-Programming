#pragma once
#include <GL\glew.h>
#include <map>
#include "Shader.h"

class UniformBufferObject
{
private:
	struct uniform
	{
		GLuint index;
		GLint offset;
		GLint arrayStride;
		GLint matrixStride;
	};

	std::map<std::string, uniform> uniformData;
	GLuint bufferID;

	void generateBuffer(GLuint size, GLenum mode, GLint bindingPoint);
	GLint getUniformSize(GLenum type);
	glm::mat4 projectionMat;
public:
	UniformBufferObject(const GLchar *blockName, const GLchar **uniformNames, const GLuint numUniforms, 
		Shader *shader, GLint bindingPoint, GLenum mode);
	void bind();
	void setUniform(std::string name, glm::mat4 val);
	void setUniform(std::string name, glm::mat3 val);
	void setUniform(std::string name, glm::vec4 val);
	void setUniform(std::string name, glm::vec3 val);
	void setUniform(std::string name, float val);
	void setUniform(std::string name, int val);
	void setUniform(std::string name, unsigned int val);
	void setUniform(std::string name, glm::mat4 val, unsigned int index);
	void setUniform(std::string name, glm::mat3 val, unsigned int index);
	void setUniform(std::string name, glm::vec4 val, unsigned int index);
	void setUniform(std::string name, glm::vec3 val, unsigned int index);
	void setUniform(std::string name, float val, unsigned int index);
	void setUniform(std::string name, int val, unsigned int index);
	void setUniform(std::string name, unsigned int val, unsigned int index);
	glm::mat4 getProjectionMat(){ return projectionMat; }
	static void unbind(){ glBindBuffer(GL_UNIFORM_BUFFER, 0); }
	~UniformBufferObject();
};

