#version 330 core
in vec3 Normal;
in vec3 WorldPos;
in vec2 Texcoord;
in vec3 Tangent;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic1;

uniform vec3 ViewPos;
uniform vec3 AmbientColor;

#define PI 3.1415926

struct DirLight{
	vec3 lightColor;
	vec3 direction;
	float intensity;
};

struct PointLight{
	vec3 position;
	vec3 lightColor;

	//float k0;
	//float k1;
	//float k2;
};

uniform DirLight dirlight;
#define NR_POINT_LIGHTS 32
uniform PointLight pointLight[NR_POINT_LIGHTS];

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir, float metallic, float roughness);
vec3 CalcPointColor(PointLight light, vec3 worldPos, vec3 normal, vec3 viewDir, float metallic, float roughness);

void main()
{
	vec3 ViewDir = normalize(vec3(ViewPos - WorldPos));

	//calculate normal
	vec3 normalDir = normalize(Normal);
	vec3 tangentDir = normalize(Tangent);
	vec3 bitangentDir = cross(normalDir, tangentDir);
	vec3 unpackedNormal = texture(texture_normal1, Texcoord).rgb;
	unpackedNormal.r = (unpackedNormal.r - 0.5) * 2;
	unpackedNormal.g = (unpackedNormal.g - 0.5) * 2;
	unpackedNormal.b = sqrt(1 - unpackedNormal.r * unpackedNormal.r - unpackedNormal.g * unpackedNormal.g);
	unpackedNormal = normalize(unpackedNormal);

	vec3 norm = unpackedNormal.r * bitangentDir + unpackedNormal.g * tangentDir + unpackedNormal.b * normalDir;

	float Metallic = texture(texture_metallic1, Texcoord).r;
	vec3 Diffuse = texture(texture_diffuse1, Texcoord).rgb;
	float Roughness = texture(texture_roughness1, Texcoord).r;


	vec3 lighting = Diffuse * 0.1; // hard-coded ambient component

	lighting += CalcDirColor(dirlight, norm, ViewDir, Metallic, Roughness);


	for (int i = 0; i < NR_POINT_LIGHTS; ++i)
	{
		lighting += CalcPointColor(pointLight[i], WorldPos, norm, ViewDir, Metallic, Roughness);
	}
	color = vec4(lighting, 1.0);

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

	vec3 base = texture(texture_diffuse1, Texcoord).rgb;

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

	vec3 base = texture(texture_diffuse1, Texcoord).rgb;

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