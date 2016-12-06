#include "Model.h"

using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using Assimp::Importer;

Model::Model()
{
	this->GammaCorrection = false;
	this->NumberOfVertices = 0;
}

Model::Model(string Path, bool Gamma)
	: GammaCorrection(Gamma)
{
	this->NumberOfVertices = 0;
	this->LoadModel(Path);
}

void Model::Draw(Shader Shader)
{
	for (GLuint i = 0; i < this->Meshes.size(); ++i)
	{
		this->Meshes[i].Draw(Shader);
	}
}

void Model::LoadModel(string Path)
{
	Importer importer;
	const aiScene* scene = importer.ReadFile(Path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cerr << "Error: Assimp scene importing failed!\n" << importer.GetErrorString() << endl;
		return;
	}

	this->Directory = Path.substr(0, Path.find_last_of('/'));
	this->ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* Node, const aiScene* Scene)
{
	for (GLuint i = 0; i < Node->mNumMeshes; ++i)
	{
		aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
		Mesh processedMesh = this->ProcessMesh(mesh, Scene);
		this->Meshes.push_back(processedMesh);
		this->NumberOfVertices += processedMesh.Vertices.size();
	}

	for (GLuint i = 0; i < Node->mNumChildren; ++i)
	{
		this->ProcessNode(Node->mChildren[i], Scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* CurrentMesh, const aiScene* Scene)
{
	float shininess = 0;
	float opacity = 0;
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	for (GLuint i = 0; i < CurrentMesh->mNumVertices; ++i)
	{
		Vertex vertex;

		vertex.Position = glm::vec3(CurrentMesh->mVertices[i].x, CurrentMesh->mVertices[i].y, CurrentMesh->mVertices[i].z);
		vertex.Normal = glm::vec3(CurrentMesh->mNormals[i].x, CurrentMesh->mNormals[i].y, CurrentMesh->mNormals[i].z);
		this->Vertices.push_back(vertex.Position);

		if (CurrentMesh->mTextureCoords[0])
		{
			vertex.UV = glm::vec2(CurrentMesh->mTextureCoords[0][i].x, CurrentMesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.UV = glm::vec2(0.0f, 0.0f);
		}

		if (CurrentMesh->mTangents)
		{
			vertex.Tangent = glm::vec3(CurrentMesh->mTangents[i].x, CurrentMesh->mTangents[i].y, CurrentMesh->mTangents[i].z);
		}
		else
		{
			vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		if (CurrentMesh->mBitangents)
		{
			vertex.Bitangent = glm::vec3(CurrentMesh->mBitangents[i].x, CurrentMesh->mBitangents[i].y, CurrentMesh->mBitangents[i].z);
		}
		else
		{
			vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}
	
	for (GLuint i = 0; i < CurrentMesh->mNumFaces; ++i)
	{
		aiFace face = CurrentMesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (CurrentMesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = Scene->mMaterials[CurrentMesh->mMaterialIndex];
		
		material->Get(AI_MATKEY_SHININESS, shininess);
		material->Get(AI_MATKEY_OPACITY, opacity);

		vector<Texture> diffuseMaps = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE, "Diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = this->LoadMaterialTextures(material, aiTextureType_SPECULAR, "Specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		vector<Texture> normalMaps = this->LoadMaterialTextures(material, aiTextureType_HEIGHT, "Normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		vector<Texture> heightMaps = this->LoadMaterialTextures(material, aiTextureType_AMBIENT, "Height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures, shininess, opacity);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* Mat, aiTextureType Type, std::string TypeName)
{
	vector<Texture> textures;

	for (GLuint i = 0; i < Mat->GetTextureCount(Type); ++i)
	{
		aiString str;
		Mat->GetTexture(Type, i, &str);
		GLboolean skip = false;

		for (GLuint j = 0; j < this->TexturesLoaded.size(); ++j)
		{
			if (this->TexturesLoaded[j].Path == str)
			{
				textures.push_back(this->TexturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.ID = TextureFromFile(str.C_Str(), this->Directory);
			texture.Type = TypeName;
			texture.Path = str;
			textures.push_back(texture);
			this->TexturesLoaded.push_back(texture);
		}
	}

	return textures;
}

GLint Model::TextureFromFile(string Path, string Directory)
{
	string filename = Directory + '/' + Path;

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width;
	int height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}

void Model::Dispose()
{
	for (Mesh mesh : this->Meshes)
	{
		mesh.Dispose();
	}
}

void Model::SetMatrix(glm::mat4 Matrix)
{
	this->Matrix = Matrix;
}

glm::mat4 Model::GetMatrix() const
{
	return this->Matrix;
}

int Model::Size() const
{
	return this->NumberOfVertices;
}

vector<glm::vec3> Model::GetVertices() const
{
	return this->Vertices;
}

void Model::UpdateVertices(float* Vertices)
{
	vector<Vertex> newVertices = this->Meshes[0].Vertices;

	for (int i = 0; i < newVertices.size(); ++i)
	{
		glm::vec3 newPosition = glm::vec3(Vertices[i * 3 + 0], Vertices[i * 3 + 1], Vertices[i * 3 + 2]);
		newVertices[i].Position = newPosition;
		this->Vertices[i] = newPosition;
	}

	this->Meshes[0].Vertices = newVertices;
}