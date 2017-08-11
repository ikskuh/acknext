#version 330

#define PI 3.141592653589793

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

uniform float fGamma;
uniform vec2 vecTime;

uniform vec3 vecViewPos;
uniform vec3 vecViewDir;

/*
uniform int iLightType; // 0=ambient, 1=point, 2=directional, 3=spot,
uniform vec3 vecLightPos;
uniform vec3 vecLightDir;
uniform vec3 vecLightColor;
uniform float fLightIntensity;
uniform float fLightArc;
*/

out vec4 fragment;

vec3 degamma(vec3 v) { return pow(v, vec3(fGamma)); }
vec3 withgamma(vec3 v) { return pow(v, vec3(1.0 / fGamma)); }
vec4 degamma(vec4 v) { return pow(v, vec4(fGamma)); }
vec4 withgamma(vec4 v) { return pow(v, vec4(1.0 / fGamma)); }

// https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/
float attenuate(vec3 P, vec3 lightCentre, float lightRadius, float cutoff)
{
	// calculate normalized light vector and distance to sphere light surface
	float r = lightRadius;
	vec3 L = lightCentre - P;
	float distance = length(L);
	float d = max(distance - r, 0);
	L /= distance;

	// calculate basic attenuation
	float denom = d/r + 1;
	float attenuation = 1 / (denom*denom);

	// scale and bias attenuation such that:
	//   attenuation == 0 at extent of max influence
	//   attenuation == 1 when d == 0
	attenuation = (attenuation - cutoff) / (1 - cutoff);
	attenuation = max(attenuation, 0);

	return attenuation;
}

// https://github.com/stackgl/glsl-diffuse-oren-nayar
float orenNayarDiffuse(
	vec3 lightDirection, // lightDir is a unit length vec3 pointing from the surface point toward the light
	vec3 viewDirection,  // viewDir is a unit length vec3 pointing toward the camera
	vec3 surfaceNormal,  // normal is the unit length surface normal at the sample point
	float roughness,     // roughness is a parameter between 0 and 1 measuring the surface roughness. 0 for smooth, 1 for matte
	float albedo)        // albedo measures the intensity of the diffuse reflection. Values >0.96 do not conserve energy
{
	float LdotV = dot(lightDirection, viewDirection);
	float NdotL = dot(lightDirection, surfaceNormal);
	float NdotV = dot(surfaceNormal, viewDirection);

	float s = LdotV - NdotL * NdotV;
	float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

	float sigma2 = roughness * roughness;
	float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
	float B = 0.45 * sigma2 / (sigma2 + 0.09);

	return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}

// https://github.com/stackgl/glsl-specular-beckmann
float beckmannDistribution(
	float x,
	float roughness) // roughness is a parameter between 0 and 1 measuring the surface roughness. Small values are shinier, larger values are rougher.
{
	float NdotH = max(x, 0.0001);
	float cos2Alpha = NdotH * NdotH;
	float tan2Alpha = (cos2Alpha - 1.0) / cos2Alpha;
	float roughness2 = roughness * roughness;
	float denom = PI * roughness2 * cos2Alpha * cos2Alpha;
	return exp(tan2Alpha / roughness2) / denom;
}

// https://github.com/stackgl/glsl-specular-cook-torrance
float cookTorranceSpecular(
	vec3 lightDirection, // lightDir is a unit length vec3 pointing from the surface point toward the light
	vec3 viewDirection,  // eyeDir is a unit length vec3 pointing from the surface point toward the camera
	vec3 surfaceNormal,  // normal is the unit length surface normal at the sample point
	float roughness,     // roughness is the surface roughness parameter, between 0 and 1. 0 means surface is perfectly smooth, 1 means surface is matte
	float fresnel)       // fresnel the Fresnel exponent. 0 = no Fresnel, higher values create a rim effect around objects
{
	float VdotN = max(dot(viewDirection, surfaceNormal), 0.0);
	float LdotN = max(dot(lightDirection, surfaceNormal), 0.0);

	//Half angle vector
	vec3 H = normalize(lightDirection + viewDirection);

	//Geometric term
	float NdotH = max(dot(surfaceNormal, H), 0.0);
	float VdotH = max(dot(viewDirection, H), 0.000001);
	float x = 2.0 * NdotH / VdotH;
	float G = min(1.0, min(x * VdotN, x * LdotN));

	//Distribution term
	float D = beckmannDistribution(NdotH, roughness);

	//Fresnel term
	float F = pow(1.0 - VdotN, fresnel);

	//Multiply terms and done
	return  G * F * D / max(PI * VdotN * LdotN, 0.000001);
}

/*
uniform int iLightType; // 0=ambient, 1=point, 2=directional, 3=spot,
uniform vec3 vecLightPos;
uniform vec3 vecLightDir;
uniform vec3 vecLightColor;
uniform float fLightIntensity;
uniform float fLightArc;
*/
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

	vec4 cDiffuse = vecColor * vec4(degamma(color),1) * degamma(texture(texColor, uv0));
	vec4 cEmissive = vecEmission * degamma(texture(texEmission, uv0));
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
	if(cDiffuse.a <= 0.5) discard;

	if(iDebugMode == 1) {
		cDiffuse = vec4(1);
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
	float albedo = 0.96;
	float occlusion = cAttribute.a;

	vec3 toView = normalize(vecViewPos - position);

	vec3 sDiffuse = vec3(0);
	vec3 sSpecular = vec3(0);

	int limit = min(LIGHT_LIMIT, iLightCount);
	for(int i = 0; i < limit; i++)
	{
		vec3 toLight = lights[i].direction;
		float atten = 1.0;

		switch(lights[i].type)
		{
			case 0: // ambient
				sDiffuse += lights[i].color * mix(cDiffuse.rgb, vec3(0), metallic);
				continue;
			case 1: // point
				atten = attenuate(
					position,
					lights[i].position,
					lights[i].intensity,
					1.0 / 512.0);
				toLight = normalize(lights[i].position - position);
				break;
			case 2: // directional
				atten = 1.0;
				toLight = -lights[i].direction;
				break;
			case 3: // spot
				atten = attenuate(
					position,
					lights[i].position,
					lights[i].intensity,
					1.0 / 512.0);
				toLight = normalize(lights[i].position - position);
				break;
			break;
			default: discard;
		}

		float ond = orenNayarDiffuse(
			toLight,
			toView,
			realNormal,
			roughness,
			albedo);

		float cts = cookTorranceSpecular(
			toLight,
			toView,
			realNormal,
			roughness,
			fresnell);

		// cts = clamp(dot(toLight, realNormal), 0, 1);

		sDiffuse += atten * lights[i].color * ond * mix(cDiffuse.rgb, vec3(0), metallic);
		sSpecular += atten * lights[i].color * cts * mix(vec3(1), cDiffuse.rgb, metallic);
	}
	fragment.rgb = withgamma(
		occlusion * sDiffuse +
		occlusion * sSpecular +
		cEmissive.rgb);
	fragment.a = 1.0;
}
