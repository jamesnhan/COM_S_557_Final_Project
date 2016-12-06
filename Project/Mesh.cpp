#include "Mesh.h"

using std::vector;
using std::string;
using std::to_string;

Mesh::Mesh(vector<Vertex> Verticies, vector<GLuint> Indices, vector<Texture> Textures, float Shininess, float Opacity)
{
	this->Vertices = Verticies;
	this->Indices = Indices;
	this->Textures = Textures;
	this->Shininess = Shininess;
	this->Opacity = Opacity;

	this->SetupMesh();
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->Vertices.size() * sizeof(Vertex), &this->Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &this->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->Indices.size() * sizeof(GLuint), &this->Indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, UV)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, Tangent)));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, Bitangent)));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader Shader)
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
	glUniform1f(glGetUniformLocation(Shader.GetProgram(), "material.Shininess"), this->Shininess);
	glUniform1f(glGetUniformLocation(Shader.GetProgram(), "material.Opacity"), this->Opacity);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->Vertices.size() * sizeof(Vertex), &this->Vertices[0], GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	for (GLuint i = 0; i < this->Textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::Dispose()
{
	glDeleteVertexArrays(1, &this->VAO);
}