#version 330
#define LIGHT_LIMIT 16

float attenuate(vec3 P, vec3 lightCentre, float lightRadius, float cutoff);
float beckmannDistribution(float x, float roughness);

float orenNayarDiffuse(
        vec3 lightDirection, // lightDir is a unit length vec3 pointing from the surface point toward the light
        vec3 viewDirection,  // viewDir is a unit length vec3 pointing toward the camera
        vec3 surfaceNormal,  // normal is the unit length surface normal at the sample point
        float roughness,     // roughness is a parameter between 0 and 1 measuring the surface roughness. 0 for smooth, 1 for matte
        float albedo);

float cookTorranceSpecular(
        vec3 lightDirection, // lightDir is a unit length vec3 pointing from the surface point toward the light
        vec3 viewDirection,  // eyeDir is a unit length vec3 pointing from the surface point toward the camera
        vec3 surfaceNormal,  // normal is the unit length surface normal at the sample point
        float roughness,     // roughness is the surface roughness parameter, between 0 and 1. 0 means surface is perfectly smooth, 1 means surface is matte
        float fresnel);

struct LightSource
{
	/*0*/ int type;
	/*1*/ float intensity;
	/*2*/ float arc; // actually cos(arc) for convenience
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
uniform vec3 vecViewPos;

vec3 applyLighting(
	vec3 position,
	vec3 normal,
	float roughness, float metallic, float fresnell,
	vec3 cAlbedo)
{
	float albedo = 0.96;
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
			sDiffuse += lights[i].color * mix(cAlbedo.rgb, vec3(0), metallic);
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
			toLight = normalize(lights[i].position - position);
			atten = attenuate(
			            position,
			            lights[i].position,
			            lights[i].intensity,
			            1.0 / 512.0);
			if(dot(lights[i].direction, -toLight) < lights[i].arc) {
				atten = 0;
			}
			break;
		default: discard;
		}

		float ond = orenNayarDiffuse(
		            toLight,
		            toView,
		            normal,
		            roughness,
		            albedo);

		float cts = cookTorranceSpecular(
		            toLight,
		            toView,
		            normal,
		            roughness,
		            fresnell);

		cts *= dot(normal, toLight);

		sDiffuse += atten * lights[i].color * ond * mix(cAlbedo.rgb, vec3(0), metallic);
		sSpecular += atten * lights[i].color * cts * mix(vec3(1), cAlbedo.rgb, metallic);
	}

	return sDiffuse + sSpecular;
}
