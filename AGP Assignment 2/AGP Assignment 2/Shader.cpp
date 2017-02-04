#include "Shader.h"
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(std::string vertex, std::string fragment, std::string name) : Shader(vertex, fragment, "", name)
{
}

Shader::Shader(std::string vertex, std::string fragment, std::string geometry, std::string name)
{
	uniformLocations = std::map<std::string, GLuint>();
	this->name = name;

	if (vertex.size() > 0)
		vertexID = createShader(vertex, GL_VERTEX_SHADER);
	if (fragment.size() > 0)
		fragmentID = createShader(fragment, GL_FRAGMENT_SHADER);
	if (geometry.size() > 0)
		geometryID = createShader(geometry, GL_GEOMETRY_SHADER);
	
	programID = glCreateProgram();

	if (vertexID != -1)
		glAttachShader(programID, vertexID);
	if (fragmentID != -1)
		glAttachShader(programID, fragmentID);
	if (geometryID != -1)
		glAttachShader(programID, geometryID);

	glLinkProgram(programID);
	glUseProgram(programID);

	GLuint matricesIndex = glGetUniformBlockIndex(programID, "MatrixBlock");
	if (matricesIndex == GL_INVALID_INDEX)
		std::cout << "Could not get index for uniform block MatrixBlock in " << name << std::endl;
	glUniformBlockBinding(programID, matricesIndex, 0);

	GLuint lightsIndex = glGetUniformBlockIndex(programID, "LightBlock");
	if (lightsIndex == GL_INVALID_INDEX)
		std::cout << "Could not get index for uniform block LightBlock in " << name << std::endl;
	glUniformBlockBinding(programID, lightsIndex, 1);
}

GLuint Shader::createShader(std::string file, GLenum type)
{
	GLuint id = glCreateShader(type);

	const char* source;
	int length = 0;

	source = loadFile(file, length);

	if (source == NULL)
	{
		std::cout << "Unable to load shader from file " << file << std::endl;
		return -1;
	}

	glShaderSource(id, 1, &source, &length);

	delete []source;

	return (compile(id)) ? id : -1;
}


bool Shader::compile(GLuint shaderID)
{
	GLint compiled;
	bool successful = true;

	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cout << name << " shader not compiled" << std::endl;
		printError(shaderID);
		successful = false;
	}

	return successful;
}

void Shader::bindProgram()
{
	glUseProgram(programID);
}

void Shader::setMaterial(Material *material)
{
	glUseProgram(programID);

	glUniform4fv(getUniformLocation("material.ambient"), 1, glm::value_ptr(material->ambient));
	glUniform4fv(getUniformLocation("material.diffuse"), 1, glm::value_ptr(material->diffuse));
	glUniform4fv(getUniformLocation("material.specular"), 1, glm::value_ptr(material->specular));
	glUniform1f(getUniformLocation("material.shininess"), material->shininess);
}

void Shader::setUniform(std::string name, glm::mat4 matrix4)
{
	glUseProgram(programID);
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix4));
}

void Shader::setUniform(std::string name, glm::vec3 vector3)
{
	glUseProgram(programID);
	glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vector3));
}

void Shader::setUniform(std::string name, glm::vec4 vector4)
{
	glUseProgram(programID);
	glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(vector4));
}

void Shader::setUniform(std::string name, GLint integer)
{
	glUseProgram(programID);
	glUniform1i(getUniformLocation(name), integer);
}

void Shader::setUniform(std::string name, GLfloat floatingPoint)
{
	glUseProgram(programID);
	glUniform1f(getUniformLocation(name), floatingPoint);
}

GLuint Shader::getUniformLocation(std::string uniform)
{
	std::map<std::string, GLuint>::iterator iter = uniformLocations.find(uniform);
	if (iter == uniformLocations.end())
	{
		GLuint location = glGetUniformLocation(programID, uniform.c_str());
		uniformLocations[uniform] = location;
		if (location == -1)
			std::cout << "Uniform " << uniform << " not found in shader program " << name << std::endl;
		return location;
	}
	else
		return iter->second;
}

char* Shader::loadFile(std::string file, int &fileSize)
{
	int size;
	char * memblock;

	// file read based on example in cplusplus.com tutorial
	// ios::ate opens file at the end
	std::ifstream stream(file, std::ios::in | std::ios::binary | std::ios::ate);
	if (stream.is_open()) 
	{
		size = (int)stream.tellg(); // get location of file pointer i.e. file size
		memblock = new char[size];
		stream.seekg(0, std::ios::beg);
		stream.read(memblock, size);
		stream.close();
		std::cout << "Shader " << file << " loaded..." << std::endl;
	}
	else {
		std::cout << "Unable to open shader " << file << std::endl;
		// should ideally set a flag or use exception handling
		// so that calling function can decide what to do now
		return nullptr;
	}

	fileSize = size;
	return memblock;
}
void Shader::printError(const GLint id)
{
	int maxLength = 0;
	int logLength = 0;
	GLchar *logMessage;

	// Find out how long the error message is
	if (!glIsShader(id))
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);
	else
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

	if (maxLength > 0) { // If message has some contents
		logMessage = new GLchar[maxLength];
		if (!glIsShader(id))
			glGetProgramInfoLog(id, maxLength, &logLength, logMessage);
		else
			glGetShaderInfoLog(id, maxLength, &logLength, logMessage);
		std::cout << "Shader Info Log:" << std::endl << logMessage << std::endl;
		delete[] logMessage;
	}
	// should additionally check for OpenGL errors here
}


Shader::~Shader()
{
}
