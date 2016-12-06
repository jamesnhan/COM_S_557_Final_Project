#version 330 core

layout (location = 0) in vec3 inPosition;

out vec3 uv;

uniform mat4 view;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * view * vec4(inPosition, 1.0f);
	uv = inPosition;
}