#version 330
#define LIGHT_LIMIT 16
#pragma acknext include "Hallo, Welt!"

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

uniform int iLightCount;

void main()
{
	float h = texture(texEmission, uv0).r;

	fragment.rgb = texture(texColor, uv0).rgb;

	float ringA = mod(position.y, 10.0) - 5.0;
	ringA = pow(1.0 - clamp(abs(ringA), 0, 1), 10);

	float ringB = mod(h, 10.0) - 5.0;
	ringB = pow(1.0 - clamp(abs(ringB), 0, 1), 10);

	fragment.rgb = mix(fragment.rgb, vec3(1,0,0), ringA);
	fragment.rgb = mix(fragment.rgb, vec3(0,1,0), ringB);

	fragment.a = 1.0;
}
