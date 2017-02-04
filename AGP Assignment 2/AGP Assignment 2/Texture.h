#pragma once
#include <string>
#include <GL\glew.h>

class Texture
{
private:
	GLuint id;
public:
	Texture();
	void load(std::string file);
	void bind();
	void bind(GLenum bindingPoint);
	static void unbind();
	~Texture();
};

