#pragma once

#include <string>

#include <GL/glew.h>
#include <assimp/types.h>

struct Texture
{
	GLuint ID;
	std::string Type;
	aiString Path;
};
