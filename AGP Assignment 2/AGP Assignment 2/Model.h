#pragma once
#include "Shader.h"
#include "Texture.h"
#include "UniformBufferObject.h"
#include "HalfEdgeStructure.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <vector>

typedef std::map<aiVector3D, GLuint> PositionMap;

class Model
{
protected:
	struct Mesh
	{
		GLuint vao, indexCount;
	};
	struct Face
	{
		GLuint a, b, c;
		Face(GLuint a, GLuint b, GLuint c) : a(a), b(b), c(c){}
	};

	Shader *shader = nullptr;
	Material *material = nullptr;
	Texture *texture = nullptr;
	glm::vec3 position, scale, rotation;
	Mesh *meshes;
	unsigned int numMeshes = 0;
	void loadAdjacencyData(const aiMesh *mesh, std::vector<GLuint> &indices);
	GLenum primitiveType;
	const GLint PRIMITIVE_RESTART = -1;
	bool CCW = true;
public:
	Model();
	void setShader(Shader *shader);
	void setMaterial(Material *material);
	void setTexture(Texture *texture);
	bool load(std::string file);
	bool load(std::string file, bool loadAdjacency);
	bool load(std::string file, bool loadAdjacency, bool CCW);
	virtual void draw(glm::mat4 viewMat, UniformBufferObject *ubo);
	virtual void drawRawData();
	void setPosition(glm::vec3 position) { this->position = position; }
	void setScale(glm::vec3 scale) { this->scale = scale; }
	void setRotate(glm::vec3 rotation) { this->rotation = rotation; }
	void rotate(glm::vec3 rotation){ this->rotation += rotation; }
	~Model();
};

