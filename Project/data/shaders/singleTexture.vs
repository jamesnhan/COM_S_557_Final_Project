#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inTangent;

struct VS_OUT
{
	vec3 Camera;
	vec3 FragmentPosition;
	vec3 Normal;
	vec2 UV;
	mat3 TBN;
};

out VS_OUT passFragmentInfo;

uniform vec3 camera;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * model * vec4(inPosition, 1.0f);
	mat3 normalMatrix = transpose(inverse(mat3(model)));

	passFragmentInfo.Camera = camera;
	passFragmentInfo.FragmentPosition = (model * vec4(inPosition, 1.0)).xyz;
	passFragmentInfo.Normal = normalMatrix * inNormal;
	passFragmentInfo.UV = inUV;

	vec3 T = normalize(normalMatrix * inTangent);
	vec3 N = normalize(normalMatrix * inNormal);
	vec3 B = normalize(normalMatrix * cross(N, T));

	passFragmentInfo.TBN = transpose(mat3(T, B, N));
}