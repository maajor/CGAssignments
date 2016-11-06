#version 330 core
in vec3 Normal;
in vec3 WorldPos;

out vec4 color;

uniform vec3 Albeido;
uniform vec3 ViewPos;
uniform vec3 AmbientColor;

struct DirLight{
	vec3 lightColor;
	vec3 direction;
	float intensity;
};

struct SpotLight{
	vec3 position;
	vec3 direction;
	vec3 lightColor;
	float innerCutOff;
	float outerCutOff;
	float k0;
	float k1;
	float k2;
};

uniform DirLight dirlight;
#define NR_SPOT_LIGHTS 1
uniform SpotLight spotLight[NR_SPOT_LIGHTS];

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotColor(SpotLight spotLight, vec3 worldPos, vec3 normal, vec3 viewDir);

void main()
{
	vec3 ViewDir = normalize(vec3(ViewPos - WorldPos));
	vec3 norm = normalize(Normal);
	vec3 result = AmbientColor;
	result += CalcDirColor(dirlight, norm, ViewDir);
	for (int i = 0; i < NR_SPOT_LIGHTS; i++){
	result += CalcSpotColor(spotLight[i], WorldPos, norm, ViewDir);
	}
	color = vec4(result, 1.0f);
}

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir){
	vec3 albeido = Albeido;
	vec3 diffuse = (0.5f * dot(light.direction, normal) + 0.5f) * light.lightColor * albeido;
	vec3 halfDir = normalize(viewDir + light.direction);
	vec3 specular = pow(max(dot(normal, halfDir), 0), 32) * light.lightColor;
	return (diffuse + specular);
}

vec3 CalcSpotColor(SpotLight light, vec3 worldPos, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(light.position - worldPos);

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerCutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 albeido = Albeido;
	vec3 diffuse = max(dot(lightDir, normal), 0.0f) * light.lightColor * albeido;
	vec3 halfDir = normalize(viewDir + lightDir);
	vec3 specular = pow(max(dot(normal, halfDir), 0.0f), 32.0f) * light.lightColor;

	float distance = length(light.position - worldPos);
	float attenuation = 1.0f / (light.k0 + light.k1 * distance + light.k2 * (distance * distance));

	return (diffuse + specular) * intensity * attenuation;
}
