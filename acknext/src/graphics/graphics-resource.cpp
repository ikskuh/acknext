#include <engine.hpp>

char const * srcVertexShader = R"GLSL(#version 330
	layout(location = 0) in vec3 vPosition;
	layout(location = 1) in vec3 vNormal;
	layout(location = 2) in vec3 vTangent;
	layout(location = 3) in vec3 vColor;
	layout(location = 4) in vec2 vUV0;
	layout(location = 5) in vec2 vUV1;
	layout(location = 6) in vec4 vBoneWeight;

	uniform mat4 matWorld;
	uniform mat4 matView;
	uniform mat4 matProj;

	out vec3 position, color, normal, tangent;
	out vec2 uv0, uv1;

	void main() {
		position = (matWorld * vec4(vPosition, 1)).rgb;
		gl_Position = matProj * matView * vec4(position, 1);
		normal = normalize(( matWorld * vec4(vNormal, 0.0) ).xyz);
		tangent = normalize(( matWorld * vec4(vTangent, 0.0) ).xyz);
		color = vColor;
		uv0 = vUV0;
		uv1 = vUV1;
	}
)GLSL";

char const * srcFragmentShader = R"GLSL(#version 330
	in vec3 position;
	in vec3 color;
	in vec2 uv0;
	in vec3 normal;

	uniform sampler2D texColor;
	uniform sampler2D texEmission;
	uniform sampler2D texAttributes;

	uniform vec3 vecEmission;
	uniform vec3 vecColor;

	uniform float fGamma;

	uniform vec3 vecViewPos;
	uniform vec3 vecViewDir;

	out vec4 fragment;

	vec3 degamma(vec3 v) { return pow(v, vec3(fGamma)); }
	vec3 withgamma(vec3 v) { return pow(v, vec3(1.0 / fGamma)); }
	vec4 degamma(vec4 v) { return pow(v, vec4(fGamma)); }
	vec4 withgamma(vec4 v) { return pow(v, vec4(1.0 / fGamma)); }

	float3 reflect( float3 i, float3 n )
	{
		return i - 2.0 * n * dot(n,i);
	}

	float Attenuate(vec3 P, vec3 lightCentre, float lightRadius, float cutoff)
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

	void main() {
		vec3 realNormal = normal;
        if(!gl_FrontFacing) {
	        realNormal = -normal;
        }



        fragment.rgb = vec3(length(position - vecViewPos) / 64);
		fragment.a = 1.0;
	}
)GLSL";
