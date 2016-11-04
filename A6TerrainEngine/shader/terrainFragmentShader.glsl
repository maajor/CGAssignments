#version 330 core
in vec3 Normal;
in vec3 WorldPos;
in vec2 Texcoord;
in vec3 VertexColor;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_detail1;

uniform vec4 clipping;

uniform vec3 ViewPos;
uniform vec3 AmbientColor;

struct DirLight{
	vec3 lightColor;
	vec3 direction;
	float intensity;
};

struct PointLight{
	vec3 position;
	vec3 lightColor;

	float k0;
	float k1;
	float k2;
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
#define NR_POINT_LIGHTS 4
uniform PointLight pointLight[NR_POINT_LIGHTS];
#define NR_SPOT_LIGHTS 1
uniform SpotLight spotLight[NR_SPOT_LIGHTS];

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointColor(PointLight light, vec3 worldPos, vec3 normal, vec3 viewDir);
vec3 CalcSpotColor(SpotLight spotLight, vec3 worldPos, vec3 normal, vec3 viewDir);

void main()
{
	float clipPos = dot(WorldPos, clipping.xyz) + clipping.w;
	if (clipPos < 0.0) {
		discard;
	}
	vec3 ViewDir = normalize(vec3(ViewPos - WorldPos));
	vec3 norm = normalize(Normal);
	vec3 result = AmbientColor;
	result += CalcDirColor(dirlight, norm, ViewDir);
	/*
	for (int i = 0; i < NR_POINT_LIGHTS; i++){
	result += CalcPointColor(pointLight[i], WorldPos, norm, ViewDir);
	}
	for (int i = 0; i < NR_SPOT_LIGHTS; i++){
	result += CalcSpotColor(spotLight[i], WorldPos, norm, ViewDir);
	}*/
	color = vec4(result, 1.0f);
	//color = vec4(Texcoord, 0.0f, 1.0f);

}

vec3 CalcDirColor(DirLight light, vec3 normal, vec3 viewDir){
	vec3 albeido = VertexColor * vec3(texture(texture_diffuse1, Texcoord));
	vec2 detailTil = vec2(5.0f, 5.0f);
	vec3 detail = vec3(texture(texture_detail1, Texcoord * detailTil));
	//vec3 albeido = VertexColor;
	vec3 diffuse = (0.5f * dot(light.direction, normal) + 0.5f) * light.lightColor * albeido;
	vec3 halfDir = normalize(viewDir + light.direction);
	vec3 specular = pow(max(dot(normal, halfDir), 0), 32) * light.lightColor * albeido * vec3(texture(texture_specular1, Texcoord));
	return (diffuse * detail + specular);
}

vec3 CalcPointColor(PointLight light, vec3 worldPos, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(light.position - worldPos);

	//vec3 albeido = VertexColor * vec3(texture(material.diffuseMap, Texcoord));
	vec3 albeido = VertexColor;
	vec3 diffuse = max(dot(lightDir, normal), 0.0f) * light.lightColor * albeido;
	vec3 halfDir = normalize(viewDir + lightDir);
	vec3 specular = pow(max(dot(normal, halfDir), 0.0f), 32.0f) * light.lightColor * albeido * vec3(texture(texture_specular1, Texcoord));

	float distance = length(light.position - worldPos);
	float attenuation = 1.0f / (light.k0 + light.k1 * distance + light.k2 * distance * distance);
	return (diffuse + specular) * attenuation;

}

vec3 CalcSpotColor(SpotLight light, vec3 worldPos, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(light.position - worldPos);

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerCutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	//vec3 albeido = VertexColor * vec3(texture(material.diffuseMap, Texcoord));
	vec3 albeido = VertexColor;
	vec3 diffuse = max(dot(lightDir, normal), 0.0f) * light.lightColor * albeido;
	vec3 halfDir = normalize(viewDir + lightDir);
	vec3 specular = pow(max(dot(normal, halfDir), 0.0f), 32.0f) * light.lightColor * albeido * vec3(texture(texture_specular1, Texcoord));

	float distance = length(light.position - worldPos);
	float attenuation = 1.0f / (light.k0 + light.k1 * distance + light.k2 * (distance * distance));

	return (diffuse + specular) * intensity * attenuation;
}

