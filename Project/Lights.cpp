#include "Lights.h"

using std::string;

Light::Light()
{
	this->Type = "NULL";
	this->Position = glm::vec4();
	this->Color = glm::vec3();
	this->AmbientIntensity = 0.0f;
	this->DiffuseIntensity = 0.0f;
	this->SpecularIntensity = 0.0f;
}

Light::Light(glm::vec3 Position, glm::vec3 Color, float AmbientIntensity, float DiffuseIntensity, float SpecularIntensity)
	: Position(glm::vec4(Position, 1.0f)), Color(Color), AmbientIntensity(AmbientIntensity), DiffuseIntensity(DiffuseIntensity), SpecularIntensity(SpecularIntensity)
{
	this->Type = "NULL";
}

DirectionalLight::DirectionalLight()
	: Light()
{
	this->Type = "Directional";
	this->Position = glm::vec4(glm::vec3(), 0.0f);
}

DirectionalLight::DirectionalLight(glm::vec3 Color, glm::vec3 Direction, float AmbientIntensity, float DiffuseIntensity, float SpecularIntensity)
	: Light(glm::vec3(), Color, AmbientIntensity, DiffuseIntensity, SpecularIntensity), Direction(Direction)
{
	this->Type = "Directional";
	this->Position = glm::vec4(20.0f, 20.0f, 0.0f, 0.0f);
}

PointLight::PointLight()
	: Light()
{
	this->Type = "Point";
	this->Position = glm::vec4(glm::vec3(), 1.0f);
}

PointLight::PointLight(glm::vec3 Position, glm::vec3 Color, float AmbientIntensity, float DiffuseIntensity, float SpecularIntensity, float Constant, float Linear, float Quadratic)
	: Light(Position, Color, AmbientIntensity, DiffuseIntensity, SpecularIntensity), Constant(Constant), Linear(Linear), Quadratic(Quadratic)
{
	this->Type = "Point";
	this->Position = glm::vec4(Position, 1.0f);
}


SpotLight::SpotLight()
	: Light()
{
	this->Type = "Spot";
	this->Position = glm::vec4(glm::vec3(), 2.0f);
}

SpotLight::SpotLight(glm::vec3 Position, glm::vec3 Color, glm::vec3 Direction, float AmbientIntensity, float DiffuseIntensity, float SpecularIntensity, float Constant, float Linear, float Quadratic, float CutOff, float OuterCutOff)
	: Light(Position, Color, AmbientIntensity, DiffuseIntensity, SpecularIntensity), Direction(Direction), Constant(Constant), Linear(Linear), Quadratic(Quadratic), CutOff(CutOff), OuterCutOff(OuterCutOff)
{
	this->Type = "Spot";
	this->Position = glm::vec4(Position, 2.0f);
}
