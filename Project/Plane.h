#pragma once

#include <string>
#include <vector>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SOIL.h"
#include "Vertex.h"
#include "Texture.h"
#include "Mesh.h"
#include "Lights.h"
#include "Shader.h"

#define MAX_LIGHTS 10

class Plane
{
public:
	Plane(glm::vec3 Position, float Width, float Height);
	void LoadTexture(std::string Path, std::string TypeName);
	void Draw(Shader Shader);

private:
	glm::vec3 Position;
	float Width;
	float Height;
	std::vector<Vertex> Vertices;
	std::vector<Texture> Textures;
	std::vector<Texture> TexturesLoaded;
	GLuint VAO, VBO;

	static GLint TextureFromFile(std::string Path);
};
