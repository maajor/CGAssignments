#version 330 core
in vec3 Normal;
in vec3 WorldPos;
in vec2 Texcoord;
in vec4 gl_FragCoord;

out vec4 color;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_normal2;
uniform sampler2D texture_flowmap;
uniform sampler2D texture_noise;
uniform sampler2D texture_refl;
uniform sampler2D texture_refr;
uniform samplerCube skybox;

uniform int tilingx;
uniform int tilingy;
uniform float _time;
uniform vec3 ViewPos;

struct DirLight{
	vec3 lightColor;
	vec3 direction;
	float intensity;
};

uniform DirLight dirlight;

void main()
{
	vec2 flowTil = vec2(5.0f, 5.0f);
	vec2 texTil = vec2(tilingx, tilingy);
	vec2 flowspeed = (texture(texture_flowmap, Texcoord * flowTil).rg * 2.0f - 1.0f);
	float cycleOffset = texture(texture_noise, Texcoord* flowTil).r;
	
	float prePhase = cycleOffset * 0.5f + _time;
	float phase0 = prePhase - floor(prePhase);
	float phase1 = prePhase + 0.5f - floor(prePhase + 0.5f);
	float lerpfactor = (abs(.5f - phase0) / .5f);
	
	vec4 normalT0 = texture(texture_normal, Texcoord* texTil + flowspeed * 0.2f * phase0);
	vec4 normalT1 = texture(texture_normal2, Texcoord* texTil + flowspeed * 0.2f * phase1);
	vec4 normalMix = mix(normalT0, normalT1, lerpfactor);
	vec3 normalT;
	normalT.xz = normalMix.xy * 2.0f - 1.0f;
	normalT.y = sqrt(1 - normalT.x * normalT.x - normalT.z * normalT.z);

	vec3 viewDir = normalize(WorldPos - ViewPos);
	vec3 WorldNormal = normalize(Normal);
	vec3 refDir = reflect(viewDir, normalT);
	vec4 skyCol = texture(skybox, refDir);

	vec3 vRefrBump = normalT.xyz * vec3(0.075, 1.0, 0.075);
	vec3 vReflBump = normalT.xyz * vec3(0.02, 1.0, 0.02);

	vec2 texelSize = 1.0 / vec2(textureSize(texture_refl, 0));
	vec4 refrColor = texture(texture_refr, gl_FragCoord.xy * texelSize + vRefrBump.xz);
	vec4 reflColor = texture(texture_refl, gl_FragCoord.xy * texelSize + vReflBump.xz);
	vec4 refrB = texture(texture_refr, gl_FragCoord.xy * texelSize);

	vec4 refraction = refrB * refrColor.w + refrColor * (1 - refrColor.w);

	//dirlight highlight
	vec3 halfDir = normalize(viewDir + dirlight.direction);
	vec3 specular = pow(max(dot(normalT, halfDir), 0), 32) * dirlight.lightColor;

	//float lerpFactor = pow((1 - dot(-viewDir, WorldNormal)), 4);
	//vec3 mixColor = mix(texresult.xyz, skyCol.xyz, lerpFactor);
	//color = vec4(skyCol.xyz * reflColor.xyz * refrColor.xyz + specular, 0.6f);

	//color = fresnel*reflColor + refraction + vec4(skyCol.xyz + specular * refraction.xyz, 0.6f);
	
	color = vec4((skyCol.xyz + specular) * reflColor.xyz + refraction.xyz, 0.6f);
	//color = vec4(1.0f);
}