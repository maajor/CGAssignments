#version 330 core
in vec3 Normal;
in vec3 WorldPos;
in vec2 uv;

out vec4 color;

//uniform vec3 ViewPos;
uniform vec4 tintColor;
uniform sampler2D textureD;

void main()
{
	color = texture(textureD, uv) * tintColor;
}
