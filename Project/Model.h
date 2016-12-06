#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "SOIL.h"

class Model
{
public:
	Model();
	Model(std::string Path, bool Gamma = false);
	void Draw(Shader Shader);
	void Dispose();
	void SetMatrix(glm::mat4 Matrix);
	glm::mat4 GetMatrix() const;
	int Size() const;
	std::vector<glm::vec3> GetVertices() const;
	void UpdateVertices(float* Vertices);

private:
	std::vector<Texture> TexturesLoaded;
	std::vector<Mesh> Meshes;
	std::string Directory;
	bool GammaCorrection;
	glm::mat4 Matrix;
	int NumberOfVertices;
	std::vector<glm::vec3> Vertices;
	
	void LoadModel(std::string Path);
	void ProcessNode(aiNode* Node, const aiScene* Scene);
	Mesh ProcessMesh(aiMesh* Mesh, const aiScene* Scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* Mat, aiTextureType Type, std::string TypeName);
	static GLint TextureFromFile(std::string Path, std::string Directory);
};
