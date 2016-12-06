#pragma once

#include <GL/GLEW.h>

#define GLM_FORCE_INLINE
#include <glm/glm.hpp>

#include "Shader.h"

class CoordinateSystem
{
public:
	CoordinateSystem(GLfloat Length);
	void Draw(Shader Shader) const;

private:
	float Length;
	glm::mat4 ModelMatrix;
	GLuint VAO;
	GLuint VBO;
	int VertexCount;
	GLfloat* BufferData;
};