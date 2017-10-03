#version 330
#define LIGHT_LIMIT 16

in vec3 position;
in vec2 uv0;

uniform int iDebugMode;

uniform sampler2D      texNoise;
uniform usampler2D     texTerrainMaterial;
uniform sampler2DArray texTerrainMaterials;

uniform vec4 vecEmission;
uniform vec4 vecColor;
uniform vec3 vecAttributes;

uniform vec3 vecViewPos;
uniform vec3 vecViewDir;


layout(location = 0) out vec3 frag_Color;
layout(location = 1) out vec3 frag_Position;
layout(location = 2) out vec3 frag_Normal;

vec3 toLinear(vec3 v);
vec4 toLinear(vec4 v);

vec3 toGamma(vec3 v);
vec4 toGamma(vec4 v);

vec3 applyLighting(
	vec3 position,
	vec3 normal,
	float roughness, float metallic, float fresnell,
	vec3 cAlbedo);

vec3 applyFog(vec3 position, vec3 surface);

uniform int iLightCount;

in float distance;

float sum( vec3 v ) { return v.x+v.y+v.z; }

vec3 textureNoTile(in uint ind, in vec2 x, float v )
{
    float k = texture( texNoise, 0.005 * x ).y; // cheap (cache friendly) lookup

    vec2 duvdx = dFdx( x );
    vec2 duvdy = dFdx( x );

    float l = k*8.0;
    float i = floor( l );
    float f = fract( l );

    vec2 offa = sin(vec2(3.0,7.0)*(i+0.0)); // can replace with any other hash
    vec2 offb = sin(vec2(2.0,8.0)*(i+1.5)); // can replace with any other hash

    vec3 cola = toLinear(textureGrad( texTerrainMaterials, vec3(x + v*offa, float(ind)), duvdx, duvdy ).rgb);
    vec3 colb = toLinear(textureGrad( texTerrainMaterials, vec3(x + v*offb, float(ind)), duvdx, duvdy ).rgb);

    return mix( cola, colb, smoothstep(0.2,0.8,f-0.1*(cola-colb)) );
}

float map(float v, float a0, float a1, float b0, float b1)
{
	return b0 + (b1-b0) * (v - b0) / (b1 - b0);
}

uniform ivec2 vecTerrainSize;
uniform sampler2D texNormalMap;

void main()
{
	vec3 normal = normalize(2.0 * texture(texNormalMap, uv0).xzy - 1.0);

	vec2 dx = vec2(1.0 / float(vecTerrainSize.x), 0.0);
	vec2 dy = vec2(0.0, 1.0 / float(vecTerrainSize.y));
	vec2 xy = pow(fract(vec2(vecTerrainSize.xy) * uv0), vec2(2.0));

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

	vec3 surface = applyLighting(position, normal, 0.9, 0.0, 150.0, albedo);

	frag_Color = applyFog(position, surface);
	frag_Position = position;
	frag_Normal = normal;
}
