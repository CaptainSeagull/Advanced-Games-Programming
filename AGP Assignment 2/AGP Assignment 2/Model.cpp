#include "Model.h"
//#include "rt3dObjLoader.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <vector>
#include <iostream>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <unordered_map>

using namespace std;
using namespace glm;

Model::Model() : position(1.0f), scale(1.0f), rotation(1.0f)
{
	
}

void Model::setShader(Shader *shader)
{
	this->shader = shader;
}

void Model::setTexture(Texture *texture)
{
	this->texture = texture;
}
void Model::setMaterial(Material *material)
{
	this->material = material;
}

bool Model::load(std::string file)
{
	return load(file, false, true);
}

bool Model::load(std::string file, bool loadAdjacency)
{
	return load(file, loadAdjacency, true);
}

bool Model::load(std::string file, bool loadAdjacency, bool CCW)
{
	this->CCW = CCW;
	Assimp::Importer *importer = new Assimp::Importer();
	const aiScene *scene = importer->ReadFile(file, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenSmoothNormals);

	if (scene == NULL)
		return false;

	numMeshes = scene->mNumMeshes;
	meshes = new Mesh[numMeshes];

	for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		vector<GLuint> indices;
		const aiMesh* mesh = scene->mMeshes[meshIndex];

		if (loadAdjacency)
		{
			loadAdjacencyData(mesh, indices);
			primitiveType = GL_TRIANGLES_ADJACENCY;
		}
		else
		{
			for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
			{
				aiFace face = mesh->mFaces[faceIndex];

				assert(face.mNumIndices == 3);

				for (int index = 0; index < 3; index++)
					indices.push_back(face.mIndices[index]);
			}
			primitiveType = GL_TRIANGLES;
		}

		meshes[meshIndex].indexCount = indices.size();

		// generate and set up a VAO for the mesh
		glGenVertexArrays(1, &meshes[meshIndex].vao);
		glBindVertexArray(meshes[meshIndex].vao);

		GLuint VBO;

		if (mesh->HasPositions())
		{
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(Shader::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(Shader::POSITION);
		}

		// VBO for normal data
		if (mesh->HasNormals()) {
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(GLfloat), mesh->mNormals, GL_STATIC_DRAW);
			glVertexAttribPointer(Shader::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(Shader::NORMAL);
		}

		// VBO for tex-coord data
		if (mesh->HasTextureCoords(0)) {
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 2 * sizeof(GLfloat), mesh->mTextureCoords[0], GL_STATIC_DRAW);
			glVertexAttribPointer(Shader::TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(Shader::TEXTURE);
		}

		if (mesh->HasFaces()) {
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[meshIndex].indexCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
		}

		// unbind vertex array
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	return true;
}

void Model::loadAdjacencyData(const aiMesh *mesh, std::vector<GLuint> &indices)
{
	glPrimitiveRestartIndex(PRIMITIVE_RESTART);
	//A containter to store all the faces after we have discarded duplicate vertices.
	std::vector<Face> uniqueFaces = std::vector<Face>();

	PositionMap positionMap;
	HalfEdgeStructure halfEdgeMap;

	//Iterate through all the faces in the mesh.
	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
	{
		aiFace face = mesh->mFaces[faceIndex];

		GLuint index1 = face.mIndices[0];
		GLuint index2 = face.mIndices[1];
		GLuint index3 = face.mIndices[2];

		/*Check if the each of the vertices indexed by each vector is present in out position map, 
		if it is we use the first index we found for it, otherwise we add it to the map with our current index.*/
		if (positionMap.find(mesh->mVertices[index1]) == positionMap.end())
		{
			positionMap[mesh->mVertices[index1]] = index1;
		}
		else
		{
			index1 = positionMap[mesh->mVertices[index1]];
		}
		if (positionMap.find(mesh->mVertices[index2]) == positionMap.end())
		{
			positionMap[mesh->mVertices[index2]] = index2;
		}
		else
		{
			index2 = positionMap[mesh->mVertices[index2]];
		}
		if (positionMap.find(mesh->mVertices[index3]) == positionMap.end())
		{
			positionMap[mesh->mVertices[index3]] = index3;
		}
		else
		{
			index3 = positionMap[mesh->mVertices[index3]];
		}

		//Add the face to the HalfEdgeStructure for later use.
		halfEdgeMap.addFace(index1, index2, index3);
		//Add the face to the list of faces without duplicated vertex indices.
		uniqueFaces.push_back(Face(index1, index2, index3));
	}

	//Iterate through all the faces in out list of unique faces. 
	for (unsigned int i = 0; i < uniqueFaces.size(); i++)
	{
		Face face = uniqueFaces[i];

		//We now begin adding the indices to our index list according to the specifications on Triangles Adjacency. 
		//We use our half edge structure to get the indices of the adjecent triangles. 
		//If there is no adjacent triangle then we push the primitive restart index, to specify that the mesh has ended for non-closed meshes. 

		//Index 1 - First triangle index
		indices.push_back(face.a);

		//Index 2 - First adjacent triangle index
		HalfEdge *halfEdge = halfEdgeMap.getHalfEdge(face.a, face.b);
		if (halfEdge->opposite != nullptr)
			indices.push_back(halfEdge->opposite->next->second);
		else
			indices.push_back(PRIMITIVE_RESTART);

		//Index 3 - Second triangle index
		indices.push_back(face.b);

		//Index 4 - Second adjacent triangle index
		halfEdge = halfEdgeMap.getHalfEdge(face.b, face.c);
		if (halfEdge->opposite != nullptr)
			indices.push_back(halfEdge->opposite->next->second);
		else
			indices.push_back(PRIMITIVE_RESTART);

		//Index 5 - Last triangle index
		indices.push_back(face.c);

		//Index 6 - Last adjacent triangle index.
		halfEdge = halfEdgeMap.getHalfEdge(face.c, face.a);
		if (halfEdge->opposite != nullptr)
			indices.push_back(halfEdge->opposite->next->second);
		else
			indices.push_back(PRIMITIVE_RESTART);
	}
}

void Model::draw(glm::mat4 viewMat, UniformBufferObject *ubo)
{
	if (shader != nullptr)
	{
		shader->bindProgram();
		if (material != nullptr)
			shader->setMaterial(material);
		if (texture != nullptr)
		{
			texture->bind();
			shader->setUniform("textured", true);
		}
		else
			shader->setUniform("textured", false);
		if (ubo != nullptr)
			ubo->bind();
		
		mat4 modelMat(1.0f);
		modelMat = translate(modelMat, position);
		modelMat = glm::scale(modelMat, scale);
		modelMat = glm::rotate(modelMat, rotation.x, vec3(1.0f, 0.0f, 0.0f));
		modelMat = glm::rotate(modelMat, rotation.y, vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::rotate(modelMat, rotation.z, vec3(0.0f, 0.0f, 1.0f));

		mat4 modelViewMat = viewMat * modelMat;
		mat4 mvpMat = ubo->getProjectionMat() * viewMat * modelMat;
		mat3 normalMat = transpose(inverse(mat3(modelMat)));

		ubo->setUniform("modelMat", modelMat);
		ubo->setUniform("modelViewMat", modelViewMat);
		ubo->setUniform("normalMat", normalMat);
		ubo->setUniform("mvpMat", mvpMat);

		if (CCW)
			glFrontFace(GL_CCW);
		else
			glFrontFace(GL_CW);

		drawRawData();
		Texture::unbind();
		UniformBufferObject::unbind();
	}
}

void Model::drawRawData()
{
	for (unsigned int i = 0; i < numMeshes; i++)
	{
		glBindVertexArray(meshes[i].vao);
		glDrawElements(primitiveType, meshes[i].indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}


Model::~Model()
{
}
