#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoRough;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic1;

void main()
{
	// Store the fragment position vector in the first gbuffer texture
	gPosition = FragPos;
	// Also store the per-fragment normals into the gbuffer

	//calculate new normal based on normal map
	vec3 normalDir = normalize(Normal);
	vec3 tangentDir = normalize(Tangent);
	vec3 bitangentDir = cross(normalDir, tangentDir);
	vec3 unpackedNormal = texture(texture_normal1, TexCoords).rgb;
	unpackedNormal.r = (unpackedNormal.r - 0.5) * 2;
	unpackedNormal.g = (unpackedNormal.g - 0.5) * 2;
	unpackedNormal.b = sqrt(1 - unpackedNormal.r * unpackedNormal.r - unpackedNormal.g * unpackedNormal.g);
	unpackedNormal = normalize(unpackedNormal);

	gNormal.rgb = unpackedNormal.r * bitangentDir + unpackedNormal.g * tangentDir + unpackedNormal.b * normalDir;
	//encode normal 
	gNormal.rgb = (gNormal.rgb + 1) / 2;
	gNormal.a = texture(texture_metallic1, TexCoords).r;
	// And the diffuse per-fragment color
	gAlbedoRough.rgb = texture(texture_diffuse1, TexCoords).rgb;
	// Store specular intensity in gAlbedoSpec's alpha component
	gAlbedoRough.a = texture(texture_roughness1, TexCoords).r;
	//color = texture(texture_specular1, TexCoords);
}
