#version 410 core

#define MIN_LIGHT 0.000001
#define MAX_TEXTURES 4
#define MAX_LIGHTS 32

struct Material
{
	int NumDiffuse;
	sampler2D[MAX_TEXTURES] Diffuse;
	int NumSpecular;
	sampler2D[MAX_TEXTURES] Specular;
	int NumNormal;
	sampler2D[MAX_TEXTURES] Normal;
	int NumHeight;
	sampler2D[MAX_TEXTURES] Height;
	float Shininess;
	float Opacity;
};

struct VS_OUT
{
	vec3 Camera;
	vec3 FragmentPosition;
	vec3 Normal;
	vec2 UV;
	mat3 TBN;
};

struct Light
{
	vec4 Position;
	vec3 Color;
	vec3 Direction;
	float Ambient;
	float Diffuse;
	float Specular;
	float Constant;
	float Linear;
	float Quadratic;
	float CutOff;
	float OuterCutOff;
};

in VS_OUT passFragmentInfo;

out vec4 color;

uniform Material material;
uniform bool doGammaCorrection;
uniform bool alternateDimension;
uniform int numDirectionalLights;
uniform Light[MAX_LIGHTS] DirectionalLights;
uniform int numPointLights;
uniform Light[MAX_LIGHTS] PointLights;
uniform int numSpotLights;
uniform Light[MAX_LIGHTS] SpotLights;

vec4 CalculateLighting(Light light, vec3 surfacePosition, vec3 normal)
{
	vec3 color = vec3(1.0, 1.0, 1.0);

	for (int i = 0; i < material.NumDiffuse; ++i)
	{
		color *= texture(material.Diffuse[i], passFragmentInfo.UV).rgb;
	}

	for (int i = 0; i < material.NumSpecular; ++i)
	{
		color *= texture(material.Specular[i], passFragmentInfo.UV).rgb;
	}

	float distance = length(vec3(light.Position) - surfacePosition);
	float attenuation = 0.0;

	vec3 ambient = light.Color * light.Ambient * color;
	vec3 lightDirection = normalize(passFragmentInfo.TBN * vec3(light.Position) - passFragmentInfo.TBN * passFragmentInfo.FragmentPosition);

	float diffuseAngle = max(dot(lightDirection, normal), 0.0);
	vec3 diffuse = light.Color * light.Diffuse * diffuseAngle * color;

	vec3 viewDirection = normalize(passFragmentInfo.TBN * passFragmentInfo.Camera - passFragmentInfo.TBN * passFragmentInfo.FragmentPosition);
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float spec = pow(max(dot(normal, halfDirection), 0.0), material.Shininess);
	vec3 specular = light.Color * light.Specular * spec * color;

	float opacity = material.Opacity;

	// DirectionalLight, no attenuation
	if (light.Position.w == 0.0)
	{
		attenuation = 1.0;
		lightDirection = normalize(passFragmentInfo.TBN * light.Position.xyz - passFragmentInfo.TBN * passFragmentInfo.FragmentPosition);
		diffuseAngle = max(dot(lightDirection, normal), 0.0);
		diffuse = light.Color * light.Diffuse * diffuseAngle * color;
		halfDirection = normalize(lightDirection + viewDirection);
		spec = pow(max(dot(normal, halfDirection), 0.0), material.Shininess);
		specular = light.Color * light.Specular * spec * color;
	}
	// PointLight, with attenuation
	else if (light.Position.w == 1.0)
	{
		attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);
	}
	// SpotLight, with attenuation and cutoff
	else if (light.Position.w == 2.0)
	{
		attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * distance * distance);

		vec3 coneDirectionNorm = normalize(vec3(light.Direction));
		vec3 surfaceToLight = normalize(vec3(light.Position) - surfacePosition);
		vec3 rayDirection = -surfaceToLight.xyz;

		float theta = dot(rayDirection, coneDirectionNorm); 
		float epsilon = light.CutOff - light.OuterCutOff;
		float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

		ambient *= intensity;
		diffuse *= intensity;
		specular *= intensity;

		if (alternateDimension)
		{
			opacity *= intensity;
		}
	}

	return vec4((ambient + diffuse + specular) * attenuation, opacity);
}

void main(void)
{
	vec3 position = passFragmentInfo.FragmentPosition;
	vec3 normal = texture(material.Normal[0], passFragmentInfo.UV).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	color = vec4(0.0, 0.0, 0.0, 0.0);

	for (int i = 0; i < numDirectionalLights; ++i)
	{
		color += CalculateLighting(DirectionalLights[i], position, normal);
	}

	for (int i = 0; i < numPointLights; ++i)
	{
		color += CalculateLighting(PointLights[i], position, normal);
	}
	
	for (int i = 0; i < numSpotLights; ++i)
	{
		color += CalculateLighting(SpotLights[i], position, normal);
	}

	if (doGammaCorrection)
	{
		vec4 gamma = vec4(1.0/2.2);
		color = pow(color, gamma);
	}
}