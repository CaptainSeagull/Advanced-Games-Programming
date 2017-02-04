#include "UniformBufferObject.h"
#include <glm\gtc\type_ptr.hpp>
#include <iostream>

UniformBufferObject::UniformBufferObject(const GLchar *blockName, const GLchar **uniformNames, const GLuint numUniforms, 
	Shader *shader, GLint bindingPoint, GLenum mode) : projectionMat(glm::mat4(1.0f))
{
	GLuint *uniformIndices = new GLuint[numUniforms];
	glGetUniformIndices(shader->getShaderProgram(), numUniforms, uniformNames, uniformIndices);

	for (unsigned int i = 0; i < numUniforms; i++)
	{
		if (uniformIndices[i] == GL_INVALID_INDEX)
			std::cout << uniformNames[i] << " not found in uniform block: " << blockName << " and shader: " << shader->getName() << std::endl;
	}

	GLint *uniformOffsets = new GLint[numUniforms];
	GLint *arrayStrides = new GLint[numUniforms];
	GLint *matrixStrides = new GLint[numUniforms];

	glGetActiveUniformsiv(shader->getShaderProgram(), numUniforms, uniformIndices, GL_UNIFORM_OFFSET, uniformOffsets);
	glGetActiveUniformsiv(shader->getShaderProgram(), numUniforms, uniformIndices, GL_UNIFORM_ARRAY_STRIDE, arrayStrides);
	glGetActiveUniformsiv(shader->getShaderProgram(), numUniforms, uniformIndices, GL_UNIFORM_MATRIX_STRIDE, matrixStrides);


	GLuint blockIndex = glGetUniformBlockIndex(shader->getShaderProgram(), blockName);
	if (blockIndex == GL_INVALID_INDEX)
		std::cout << "Could not get index for uniform block " << blockName << " in " << shader->getName() << std::endl;

	GLint blockSize, maxSize;
	glGetActiveUniformBlockiv(shader->getShaderProgram(), blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxSize);

	if (blockSize < maxSize)
	{
		generateBuffer(blockSize, mode, bindingPoint);

		for (unsigned int i = 0; i < numUniforms; i++)
		{
			uniformData[uniformNames[i]].index = uniformIndices[i];
			uniformData[uniformNames[i]].offset = uniformOffsets[i];
			uniformData[uniformNames[i]].arrayStride = arrayStrides[i];
			uniformData[uniformNames[i]].matrixStride = matrixStrides[i];
		}
	}
	else
		std::cout << "Uniform Block " << blockName << " in " << shader->getName() << " exceeds maximum block size of " << maxSize << " with " << blockSize << std::endl;

	delete[] uniformIndices;
	delete[] uniformOffsets;
	delete[] arrayStrides;
	delete[] matrixStrides;
}

void UniformBufferObject::generateBuffer(GLuint size, GLenum mode, GLint bindingPoint)
{
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
	glBufferData(GL_UNIFORM_BUFFER, size, 0, mode);
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, bufferID);
}

void UniformBufferObject::bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
}

void UniformBufferObject::setUniform(std::string name, glm::mat4 val)
{
	setUniform(name, val, 0);
}

void UniformBufferObject::setUniform(std::string name, glm::mat3 val)
{
	setUniform(name, val, 0);
}

void UniformBufferObject::setUniform(std::string name, glm::vec4 val)
{
	setUniform(name, val, 0);
}

void UniformBufferObject::setUniform(std::string name, glm::vec3 val)
{
	setUniform(name, val, 0);
}

void UniformBufferObject::setUniform(std::string name, float val)
{
	setUniform(name, val, 0);
}

void UniformBufferObject::setUniform(std::string name, int val)
{
	setUniform(name, val, 0);
}

void UniformBufferObject::setUniform(std::string name, unsigned int val)
{
	setUniform(name, val, 0);
}

void UniformBufferObject::setUniform(std::string name, glm::mat4 val, unsigned int index)
{
	bind();
	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, 
			uniformData[name].offset + uniformData[name].matrixStride*i + uniformData[name].arrayStride*index, 
			16, 
			glm::value_ptr(val[i]));
	}
	unbind();

	if (name == "projectionMat")
		projectionMat = val;
}

void UniformBufferObject::setUniform(std::string name, glm::mat3 val, unsigned int index)
{
	bind();
	for (int i = 0; i < 3; i++)
	{
		glBufferSubData(GL_UNIFORM_BUFFER,
			uniformData[name].offset + uniformData[name].matrixStride*i + uniformData[name].arrayStride*index,
			12,
			glm::value_ptr(val[i]));
	}
	unbind();
}

void UniformBufferObject::setUniform(std::string name, glm::vec4 val, unsigned int index)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, 
		uniformData[name].offset + uniformData[name].arrayStride*index, 
		16,
		glm::value_ptr(val));
	unbind();
}

void UniformBufferObject::setUniform(std::string name, glm::vec3 val, unsigned int index)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER,
		uniformData[name].offset + uniformData[name].arrayStride*index,
		12,
		glm::value_ptr(val));
	unbind();
}

void UniformBufferObject::setUniform(std::string name, float val, unsigned int index)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER,
		uniformData[name].offset + uniformData[name].arrayStride*index,
		4,
		&val);
	unbind();
}

void UniformBufferObject::setUniform(std::string name, int val, unsigned int index)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER,
		uniformData[name].offset + uniformData[name].arrayStride*index,
		4,
		&val);
	unbind();
}

void UniformBufferObject::setUniform(std::string name, unsigned int val, unsigned int index)
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER,
		uniformData[name].offset + uniformData[name].arrayStride*index,
		4,
		&val);
	unbind();
}



UniformBufferObject::~UniformBufferObject()
{
}
