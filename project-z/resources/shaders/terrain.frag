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

uniform sampler2D      texDetail; // required for noise
uniform usampler2D     texTerrainMaterial;
uniform sampler2DArray texTerrainMaterials;

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

in float distance;

float sum( vec3 v ) { return v.x+v.y+v.z; }

vec3 textureNoTile(in uint ind, in vec2 x, float v )
{
    float k = texture( texDetail, 0.005*x ).y; // cheap (cache friendly) lookup

    vec2 duvdx = dFdx( x );
    vec2 duvdy = dFdx( x );

    float l = k*8.0;
    float i = floor( l );
    float f = fract( l );

    vec2 offa = sin(vec2(3.0,7.0)*(i+0.0)); // can replace with any other hash
    vec2 offb = sin(vec2(3.0,7.0)*(i+1.0)); // can replace with any other hash

    vec3 cola = toLinear(textureGrad( texTerrainMaterials, vec3(x + v*offa, float(ind)), duvdx, duvdy ).rgb);
    vec3 colb = toLinear(textureGrad( texTerrainMaterials, vec3(x + v*offb, float(ind)), duvdx, duvdy ).rgb);

    return mix( cola, colb, smoothstep(0.2,0.8,f-0.1*(cola-colb)) );
}

float map(float v, float a0, float a1, float b0, float b1)
{
	return b0 + (b1-b0) * (v - b0) / (b1 - b0);
}

void main()
{
	vec2 dx = vec2(1.0 / 2048.0, 0.0);
	vec2 dy = vec2(0.0, 1.0 / 2048.0);
	vec2 xy = fract(2048.0f * uv0);

	uint type00 = texture(texTerrainMaterial, uv0).r;
	uint type01 = texture(texTerrainMaterial, uv0+dy).r;
	uint type10 = texture(texTerrainMaterial, uv0+dx).r;
	uint type11 = texture(texTerrainMaterial, uv0+dx+dy).r;

	vec3 c00 = textureNoTile(type00, 1024.0 * uv0, 0.6);
	vec3 c01 = textureNoTile(type01, 1024.0 * uv0, 0.6);
	vec3 c10 = textureNoTile(type10, 1024.0 * uv0, 0.6);
	vec3 c11 = textureNoTile(type11, 1024.0 * uv0, 0.6);

	vec3 albedo = mix(
		mix(c00, c01, xy.y),
		mix(c10, c11, xy.y),
		xy.x);

	fragment.rgb = toGamma(applyLighting(position, normal, 0.9, 0.0, 150.0, albedo));
	// fragment.rgb = toGamma(albedo);
	fragment.a = 1.0;
}
