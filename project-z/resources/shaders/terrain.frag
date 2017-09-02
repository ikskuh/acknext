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

in float distance;

float sum( vec3 v ) { return v.x+v.y+v.z; }

float textureNoTile( in vec2 x, float v )
{
    float k = texture( texAttributes, 0.005*x ).y; // cheap (cache friendly) lookup

    vec2 duvdx = dFdx( x );
    vec2 duvdy = dFdx( x );

    float l = k*8.0;
    float i = floor( l );
    float f = fract( l );

    vec2 offa = sin(vec2(3.0,7.0)*(i+0.0)); // can replace with any other hash
    vec2 offb = sin(vec2(3.0,7.0)*(i+1.0)); // can replace with any other hash

    float cola = textureGrad( texAttributes, x + v*offa, duvdx, duvdy ).x;
    float colb = textureGrad( texAttributes, x + v*offb, duvdx, duvdy ).x;

    return mix( cola, colb, smoothstep(0.2,0.8,f-0.1*(cola-colb)) );
}

void main()
{
	vec3 sun = normalize(vec3(0.3, 0.6, 0.4));
	float h = texture(texEmission, uv0).r;

	// fragment.rgb = 0.5 + 0.5 * normal;
	fragment.rgb = texture(texColor, uv0).rgb;

	float fDetail = 1.2 * textureNoTile(2.0 * position.xz, 0.6);

	fDetail = mix(fDetail, 1.0, clamp(0.025 * distance, 0, 1));

	fragment.rgb *= fDetail;

	fragment.rgb *= dot(normal, sun);

	/*
	float ringA = mod(position.y, 10.0) - 5.0;
	ringA = pow(1.0 - clamp(abs(ringA), 0, 1), 10);

	float ringB = mod(h, 10.0) - 5.0;
	ringB = pow(1.0 - clamp(abs(ringB), 0, 1), 10);

	fragment.rgb = mix(fragment.rgb, vec3(1,0,0), ringA);
	fragment.rgb = mix(fragment.rgb, vec3(0,1,0), ringB);
	*/
	fragment.a = 1.0;
}
