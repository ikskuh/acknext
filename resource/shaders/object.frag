#version 330
#define LIGHT_LIMIT 16

in vec3 position;
in vec3 tangent;
in vec3 cotangent;
in vec3 color;
in vec2 uv0;
in vec3 normal;

uniform int iDebugMode;

uniform sampler2D texColor;
uniform sampler2D texEmission;
uniform sampler2D texAttributes;
uniform sampler2D texNormalMap;

uniform vec4 vecEmission;
uniform vec4 vecColor;
uniform vec3 vecAttributes;

uniform vec3 vecViewPos;
uniform vec3 vecViewDir;

out vec4 fragment;

vec3 toLinear(vec3 v);
vec4 toLinear(vec4 v);

vec3 toGamma(vec3 v);
vec4 toGamma(vec4 v);

vec3 applyLighting(
	vec3 position,
	vec3 normal,
	float roughness, float metallic, float fresnell,
	vec3 cAlbedo);

struct LightSource
{
	/*0*/ int type;
	/*1*/ float intensity;
	/*2*/ float arc;
	/*4*/ vec3 position;
	/*8*/ vec3 direction;
	/*C*/ vec3 color;
};

// std140 is necessary for alignment!
layout(std140) uniform LightBlock
{
	LightSource lights[LIGHT_LIMIT];
};

uniform int iLightCount;

void main() {

	mat3 mApplyNormal;
	vec3 polyNormal;
	if(gl_FrontFacing) {
		polyNormal = normal;
		mApplyNormal = mat3(tangent,cotangent,normal);
	} else {
		polyNormal = -normal;
		mApplyNormal = mat3(-tangent,-cotangent,-normal);
	}

	vec4 cAlbedo = vecColor * vec4(toLinear(color),1) * toLinear(texture(texColor, uv0));
	vec4 cEmissive = vecEmission * toLinear(texture(texEmission, uv0));
	vec4 cAttribute = vec4(vecAttributes, 1) * texture(texAttributes, uv0);
	vec4 cNormalMap = texture(texNormalMap, uv0);
	// cAttribute = [ roughness, metallic, fresnell ]

	cNormalMap.rg = 2.0 * cNormalMap.rg - vec2(1.0);
	cNormalMap.rgb = normalize(cNormalMap.rgb);


	if(!gl_FrontFacing) {
		cNormalMap.rg = -cNormalMap.rg;
	}

	vec3 realNormal = mApplyNormal * cNormalMap.rgb;

	// Alpha testing
	if(cAlbedo.a <= 0.5) {
		discard;
	}

	if(iDebugMode == 1) {
		cAlbedo = vec4(1);
	} else if(iDebugMode == 2) {
		fragment.rgb = 0.5 + 0.5 * realNormal;
		fragment.a = 1.0;
		return;
	} else if(iDebugMode == 3) {
		fragment.rgb = vec3(float(iLightCount) / (LIGHT_LIMIT - 1));
		fragment.a = 1.0;
		return;
	}

	float roughness = cAttribute.r; // 0[smooth]     → 1[matte]
	float metallic = cAttribute.g;  // 0[dielectric] → 1[metal]
	float fresnell = cAttribute.b;  // 0[none]       → ∞[rim]
	float occlusion = cAttribute.a;

	vec3 lightedSurface = applyLighting(
		position, realNormal,
		roughness, metallic, fresnell,
		cAlbedo.rgb);

	fragment.rgb = toGamma(
		occlusion * lightedSurface +
		cEmissive.rgb);
	fragment.a = 1.0;
}
