#version 330 core
in vec3 Normal;
in vec3 WorldPos;
in vec2 Texcoord;

out vec4 color;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_normal2;

uniform int tilingx;
uniform int tilingy;
uniform float _time;

void main()
{
	vec2 til = vec2(Texcoord.x * tilingx + _time, Texcoord.y * tilingy + _time);
	vec4 result = texture(texture_diffuse, til);
	color = vec4(result.xyz, 0.8f);
	//color = vec4(1.0f);
}