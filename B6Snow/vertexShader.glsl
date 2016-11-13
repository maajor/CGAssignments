#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
out vec3 WorldPos;
out vec3 Normal;
out vec2 uv;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 ViewPos;

void main()
{
	//gl_Position = projection * view * model * vec4(position, 1.0f);
	//WorldPos = vec3(model * vec4(position, 1.0f));
	//Normal = mat3(transpose(inverse(model))) * normal;
	vec3 center = vec3(0, 0, 0);
	vec3 viewer = vec3(inverse(model) * vec4(ViewPos, 1));
	vec3 viewDir = normalize(viewer - center);
	vec3 right = cross(vec3(0, 1, 0), viewDir);
	vec3 up = normalize(cross(viewDir, right));
	vec3 offset = position.xyz - center;
	vec3 localPos = center + right * offset.x + up * offset.z + viewDir * offset.y;
	gl_Position = projection * view * model * vec4(localPos.xyz, 1.0f);

	uv = texcoord;
}