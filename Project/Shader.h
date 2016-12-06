#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include "Lights.h"

#define MAX_LIGHTS 256

class Shader
{
public:
	Shader(std::string VertexPath, std::string FragmentPath);
	GLuint GetProgram() const;
	void Use() const;
	void AddLight(Light* Light);
	void RemoveLight(Light* Light);
	void ClearLights();

private:
	GLuint Program;
	std::vector<Light*> Lights;
};