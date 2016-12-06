#pragma once

#include <vector>
#include <string>

#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"

#define MAX_MATERIALS 10

class Mesh
{
public:
	std::vector<Vertex> Vertices;
	std::vector<GLuint> Indices;
	std::vector<Texture> Textures;

	Mesh(std::vector<Vertex> Vertices, std::vector<GLuint> Indices, std::vector<Texture> Textures, float Shininess, float Opacity);
	void Draw(Shader Shader);
	void Dispose();

private:
	GLuint VAO, VBO, EBO;
	float Shininess;
	float Opacity;

	void SetupMesh();
};