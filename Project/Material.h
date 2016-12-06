#pragma once

#include <glm/glm.hpp>

struct Material
{
	// Material Properties
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	glm::vec3 Emissive;
	float Shininess;
	float Transparency;
};