#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Material.h"

class Light
{
public:
	bool Enabled = true;
	std::string Type;
	glm::vec4 Position;
	glm::vec3 Color;
	float AmbientIntensity;
	float DiffuseIntensity;
	float SpecularIntensity;

	Light();
	Light(glm::vec3 Position, glm::vec3 Color, float AmbientIntensity, float DiffuseIntensity, float SpecularIntensity);
};

class DirectionalLight : public Light
{
public:
	glm::vec3 Direction;

	DirectionalLight();
	DirectionalLight(glm::vec3 Color, glm::vec3 Direction, float AmbientIntensity, float DiffuseIntensity, float SpecularIntensity);
};

class PointLight : public Light
{
public:
	float Constant;
	float Linear;
	float Quadratic;

	PointLight();
	PointLight(glm::vec3 Position, glm::vec3 Color, float AmbientIntensity, float DiffuseIntensity, float SpecularIntensity, float Constant, float Linear, float Quadratic);
};

class SpotLight : public Light
{
public:
	glm::vec3 Direction;
	float Constant;
	float Linear;
	float Quadratic;
	float CutOff;
	float OuterCutOff;

	SpotLight();
	SpotLight(glm::vec3 Position, glm::vec3 Color, glm::vec3 Direction, float AmbientIntensity, float DiffuseIntensity, float SpecularIntensity, float Constant, float Linear, float Quadratic, float CutOff, float OuterCutOff);
};