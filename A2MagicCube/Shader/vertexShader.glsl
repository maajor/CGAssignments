#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in vec3 color;
out vec3 WorldPos;
out vec3 Normal;
out vec2 Texcoord;
out vec3 VertexColor;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	WorldPos = vec3(model * vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * normal;
	Texcoord = texcoord;
	VertexColor = color;
}