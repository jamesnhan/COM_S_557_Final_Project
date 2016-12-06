#include "Plane.h"

using std::to_string;
using std::string;
using std::vector;

Plane::Plane(glm::vec3 Position, float Width, float Height)
	: Position(Position), Width(Width), Height(Height)
{
	float halfWidth = Width / 2.0f;
	float halfHeight = Height / 2.0f;

	Vertex topLeft;
	topLeft.Position = glm::vec3(Position.x - halfWidth, Position.y, Position.z + halfHeight);
	topLeft.UV = glm::vec2(0.0f, 1.0f);
	Vertex topRight;
	topRight.Position = glm::vec3(Position.x + halfWidth, Position.y, Position.z + halfHeight);
	topRight.UV = glm::vec2(1.0f, 1.0f);
	Vertex bottomLeft;
	bottomLeft.Position = glm::vec3(Position.x - halfWidth, Position.y, Position.z - halfHeight);
	bottomLeft.UV = glm::vec2(0.0f, 0.0f);
	Vertex bottomRight;
	bottomRight.Position = glm::vec3(Position.x + halfWidth, Position.y, Position.z - halfHeight);
	bottomRight.UV = glm::vec2(1.0f, 0.0f);

	glm::vec3 up = glm::normalize(bottomLeft.Position - topLeft.Position);
	glm::vec3 right = glm::normalize(bottomLeft.Position - bottomRight.Position);
	glm::vec3 normal = glm::cross(up, right);

	topLeft.Normal = normal;
	topRight.Normal = normal;
	bottomLeft.Normal = normal;
	bottomRight.Normal = normal;

	this->Vertices.push_back(topLeft);
	this->Vertices.push_back(topRight);
	this->Vertices.push_back(bottomLeft);
	this->Vertices.push_back(bottomRight);

	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->Vertices.size() & sizeof(Vertex), &this->Vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, UV)));

	glBindVertexArray(0);
}

void Plane::LoadTexture(string Path, string TypeName)
{
	Texture texture;

	GLboolean skip = false;

	for (GLuint j = 0; j < this->TexturesLoaded.size(); ++j)
	{
		if (this->TexturesLoaded[j].Path == aiString(Path))
		{
			texture = this->TexturesLoaded[j];
			skip = true;
			break;
		}
	}

	if (!skip)
	{
		texture.ID = TextureFromFile(Path);
		texture.Type = TypeName;
		texture.Path = Path;
		this->TexturesLoaded.push_back(texture);
	}

	this->Textures.push_back(texture);
}

GLint Plane::TextureFromFile(string Path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width;
	int height;
	unsigned char* image = SOIL_load_image(Path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

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

void Plane::Draw(Shader Shader)
{
	GLuint diffuseNum = 0;
	GLuint specularNum = 0;
	GLuint normalNum = 0;
	GLuint heightNum = 0;

	for (GLuint i = 0; i < this->Textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		string name = this->Textures[i].Type;
		string number;

		if (name == "Diffuse")
		{
			number = to_string(diffuseNum++);
		}
		else if (name == "Specular")
		{
			number = to_string(specularNum++);
		}
		else if (name == "Normal")
		{
			number = to_string(normalNum++);
		}
		else if (name == "Height")
		{
			number = to_string(heightNum++);
		}

		int materialUniformLocation = glGetUniformLocation(Shader.GetProgram(), ("material." + name + "[" + number + "]").c_str());
		glUniform1i(materialUniformLocation, i);
		glBindTexture(GL_TEXTURE_2D, this->Textures[i].ID);
	}

	glUniform1i(glGetUniformLocation(Shader.GetProgram(), "material.NumDiffuse"), diffuseNum);
	glUniform1i(glGetUniformLocation(Shader.GetProgram(), "material.NumSpecular"), specularNum);
	glUniform1i(glGetUniformLocation(Shader.GetProgram(), "material.NumNormal"), normalNum);
	glUniform1i(glGetUniformLocation(Shader.GetProgram(), "material.NumHeight"), heightNum);
	glUniform1f(glGetUniformLocation(Shader.GetProgram(), "material.Shininess"), 0.0f);
	glUniform1f(glGetUniformLocation(Shader.GetProgram(), "material.Opacity"), 1.0f);

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, this->Vertices.size());
	glBindVertexArray(0);

	for (GLuint i = 0; i < this->Textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE0);
}