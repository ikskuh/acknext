#version 330

#define PI 3.141592653589793

in vec3 position;
in vec3 tangent;
in vec3 cotangent;
in vec3 color;
in vec2 uv0;
in vec3 normal;

uniform sampler2D texColor;
uniform sampler2D texEmission;
uniform sampler2D texAttributes;
uniform sampler2D texNormalMap;

uniform vec3 vecEmission;
uniform vec3 vecColor;
uniform vec3 vecAttributes;

uniform float fGamma;
uniform vec2 vecTime;

uniform vec3 vecViewPos;
uniform vec3 vecViewDir;

out vec4 fragment;

vec3 degamma(vec3 v) { return pow(v, vec3(fGamma)); }
vec3 withgamma(vec3 v) { return pow(v, vec3(1.0 / fGamma)); }
vec4 degamma(vec4 v) { return pow(v, vec4(fGamma)); }
vec4 withgamma(vec4 v) { return pow(v, vec4(1.0 / fGamma)); }

vec3 reflect( vec3 i, vec3 n )
{
	return i - 2.0 * n * dot(n,i);
}

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

	vec4 cDiffuse = vec4(vecColor, 1) * vec4(degamma(color),1) * degamma(texture(texColor, uv0));
	vec4 cEmissive = vec4(vecEmission, 1) * degamma(texture(texEmission, uv0));
	vec4 cAttribute = vec4(vecAttributes, 1) * texture(texAttributes, uv0);
	vec4 cNormalMap = texture(texNormalMap, uv0);
	// cAttribute = [ roughness, metallic, fresnell ]

	cNormalMap.rgb = normalize(2.0 * cNormalMap.rgb - 1.0);

	if(gl_FrontFacing == false) {
		cNormalMap.rg = -cNormalMap.rg;
	}

	vec3 realNormal = mApplyNormal * cNormalMap.rgb;

	// Alpha testing
	if(cDiffuse.a <= 0.5) discard;

	vec3 lightPosition = vec3(0, 24, -148);
	vec3 lightColor = vec3(1,1,1);
	vec3 toView = normalize(vecViewPos - position);
	vec3 toLight = normalize(lightPosition - position);

//	fragment = vec4(
//		0.5 + 0.5 * mix(
//			realNormal,
//	        polyNormal,
//			0.5 + 0.5 * sin(3 * vecTime.x)),
//		1.0);
//	return;

	float roughness = cAttribute.r; // 0[smooth]     → 1[matte]
	float metallic = cAttribute.g;  // 0[dielectric] → 1[metal]
	float fresnell = cAttribute.b;  // 0[none]       → ∞[rim]
	float albedo = 0.8;

	float atten = attenuate(
		position,
		lightPosition,
		64,
		1.0 / 512.0);

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

	cts *= max(dot(realNormal, toLight), 0); // ??

	vec3 sDiffuse = atten * lightColor * ond * mix(cDiffuse.rgb, vec3(0), metallic);
	vec3 sSpecular = atten * lightColor * cts * mix(vec3(1), cDiffuse.rgb, metallic);

	fragment.rgb = withgamma(sDiffuse + sSpecular + cEmissive.rgb);
	fragment.a = 1.0;
}
