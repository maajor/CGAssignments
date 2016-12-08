#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormalMetal;
uniform sampler2D gAlbedoRough;

#define PI 3.1415926

struct PointLight {
	vec3 position;
	vec3 lightColor;

	//float Linear;
	//float Quadratic;
};

struct DirLight{
	vec3 lightColor;
	vec3 direction;
	float intensity;
};

const int NR_LIGHTS = 32;
uniform PointLight pointLight[NR_LIGHTS];
uniform DirLight dirlight;

uniform vec3 ViewPos;

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir, float metallic, float roughness);
vec3 CalcPointColor(PointLight light, vec3 worldPos, vec3 normal, vec3 viewDir, float metallic, float roughness);

void main()
{
	// Retrieve data from gbuffer
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormalMetal, TexCoords).rgb;
	float Metallic = texture(gNormalMetal, TexCoords).a;
	vec3 Diffuse = texture(gAlbedoRough, TexCoords).rgb;
	float Roughness = texture(gAlbedoRough, TexCoords).a;

	//decode normal
	Normal = (Normal - 0.5) * 2;

	// Then calculate lighting as usual
	vec3 lighting = Diffuse * 0.1; // hard-coded ambient component
	vec3 viewDir = normalize(ViewPos - FragPos);

	lighting += CalcDirColor(dirlight, Normal, viewDir, Metallic, Roughness);

	
	for (int i = 0; i < NR_LIGHTS; ++i)
	{
		lighting += CalcPointColor(pointLight[i], FragPos, Normal, viewDir, Metallic, Roughness);
	}
	
	FragColor = vec4(lighting, 1.0);
	//FragColor = vec4(Roughness, Roughness, Roughness, 1.0);
}

float D_GGX(in float roughness, in float NdH)
{
	float m = roughness * roughness;
	float m2 = m * m;
	float d = (NdH * m2 - NdH) * NdH + 1.0;
	return m2 / (PI * d * d);
}

float G_schlick(in float roughness, in float NdV, in float NdL)
{
	float k = (roughness + 1)* (roughness + 1) * 0.125;
	float V = NdV * (1.0 - k) + k;
	float L = NdL * (1.0 - k) + k;
	return 0.25 / (V * L);
}

vec3 fresnel_factor(vec3 f0, float HdV)
{
	return mix(f0, vec3(1.0), pow(1 - HdV, 5.0));
}

vec3 cooktorrance_specular(float NdL, float NdV, float NdH, vec3 specfresnel, float roughness)
{

	float D = D_GGX(roughness, NdH);

	float G = G_schlick(roughness, NdV, NdL);

	//float rim = mix(1.0 - roughness * 0.9, 1.0, NdV);

	//return (1.0 / rim) * specular * G * D;
	return  specfresnel * G * D;
}

vec3 CalcPointColor(PointLight light, vec3 worldPos, vec3 normal, vec3 viewDir, float metallic, float roughness){
	vec3 L = light.position - worldPos;
	vec3 V = viewDir;
	vec3 H = normalize(L + V);
	vec3 N = normal;

	float A = 20.0 / dot(light.position - worldPos, light.position - worldPos);
	//float A = 10;

	vec3 base = texture(gAlbedoRough, TexCoords).rgb;

	vec3 specular = mix(vec3(0.04), base, metallic);

	float NdL = max(0.0, dot(normal, L));
	float NdV = max(0.001, dot(normal, viewDir));
	float NdH = max(0.001, dot(normal, H));
	float HdV = max(0.001, dot(H, viewDir));
	float LdV = max(0.001, dot(L, viewDir));

	vec3 specfresnel = fresnel_factor(specular, HdV);
	vec3 specref = cooktorrance_specular(NdL, NdV, NdH, specfresnel, roughness);

	specref *= vec3(NdL);
	//vec3 diffref = (vec3(1.0) - specfresnel) * (1.0 / PI) * NdL;
	vec3 diffref = (1.0 / PI) * NdL * vec3(1.0);

	vec3 light_color = vec3(1.0) * light.lightColor * A;
	vec3 reflected_light = specref * light_color;
	vec3 diffuse_light = diffref * light_color;

	vec3 result = diffuse_light * mix(base, vec3(0.0), metallic) + reflected_light;
	return result;
}

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir, float metallic, float roughness){
	vec3 L = light.direction;
	vec3 V = viewDir;
	vec3 H = normalize(L + V);
	vec3 N = normal;

	vec3 base = texture(gAlbedoRough, TexCoords).rgb;

	vec3 specular = mix(vec3(0.04), base, metallic);

	float NdL = max(0.0, dot(normal, light.direction));
	float NdV = max(0.001, dot(normal, viewDir));
	float NdH = max(0.001, dot(normal, H));
	float HdV = max(0.001, dot(H, viewDir));
	float LdV = max(0.001, dot(light.direction, viewDir));

	vec3 specfresnel = fresnel_factor(specular, HdV);
	vec3 specref = cooktorrance_specular(NdL, NdV, NdH, specfresnel, roughness);

	specref *= vec3(NdL);
	vec3 diffref = (vec3(1.0) - specfresnel) * (1.0 / PI) * NdL;

	vec3 light_color = vec3(1.0) * light.lightColor;
	vec3 reflected_light = specref * light_color;
	vec3 diffuse_light = diffref * light_color;

	vec3 result = diffuse_light * mix(base, vec3(0.0), metallic) + reflected_light;
	return result;
}
