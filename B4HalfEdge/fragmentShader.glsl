#version 330 core
in vec3 Normal;
in vec3 WorldPos;

out vec4 color;

uniform vec3 shadeColor;

void main()
{
	color = vec4(shadeColor, 1.0f);
}
