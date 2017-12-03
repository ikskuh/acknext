#version 330

#define PI 3.141592653589793

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
