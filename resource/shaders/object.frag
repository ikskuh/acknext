#version 330
#define LIGHT_LIMIT 16

in vec3 position;
in vec3 tangent;
in vec3 cotangent;
in vec3 color;
in vec2 uv0;
in vec3 normal;

layout(location = 0) out vec3 frag_Color;
layout(location = 1) out vec3 frag_Position;
layout(location = 2) out vec3 frag_Normal;
layout(location = 3) out vec3 frag_Attrib; // (roughness, metallic, ???)

uniform int iDebugMode;

uniform sampler2D texAlbedo;
uniform sampler2D texEmission;
uniform sampler2D texAttributes;
uniform sampler2D texNormalMap;

uniform vec4 vecEmission;
uniform vec4 vecAlbedo;
uniform vec3 vecAttributes;

uniform vec3 vecViewPos;
uniform vec3 vecViewDir;

uniform bool useNormalMapping = false;

vec3 applyLighting(
	vec3 position,
	vec3 normal,
	float roughness, float metallic, float fresnell,
	vec3 cAlbedo);

vec3 applyFog(vec3 position, vec3 surface);

uniform int iLightCount;

void main() {

	vec4 cAlbedo = vecAlbedo * vec4(color,1) * texture(texAlbedo, uv0);
	vec4 cEmissive = vecEmission * texture(texEmission, uv0);

	// cAttribute = [ roughness, metallic, fresnell ]
	vec4 cAttribute = vec4(vecAttributes, 1) * texture(texAttributes, uv0);

	// Alpha testing
	if(cAlbedo.a <= 0.5) {
		discard;
	}

	vec3 realNormal;
	if(useNormalMapping) {
		vec4 cNormalMap = texture(texNormalMap, uv0);

		mat3 mApplyNormal;
		vec3 polyNormal;
		if(gl_FrontFacing) {
			polyNormal = normal;
			mApplyNormal = mat3(tangent,cotangent,normal);
		} else {
			polyNormal = -normal;
			mApplyNormal = mat3(-tangent,-cotangent,-normal);
		}

		cNormalMap.rg = 2.0 * cNormalMap.rg - vec2(1.0);
		cNormalMap.rgb = normalize(cNormalMap.rgb);

		if(!gl_FrontFacing) {
			cNormalMap.rg = -cNormalMap.rg;
		}

		realNormal = mApplyNormal * cNormalMap.rgb;
	} else {
		if(gl_FrontFacing) {
			realNormal = normal;
		} else {
			realNormal = -normal;
		}
	}

	if(iDebugMode == 1) {
		cAlbedo = vec4(1);
	} else if(iDebugMode == 2) {
		frag_Color = 0.5 + 0.5 * realNormal;
		return;
	} else if(iDebugMode == 3) {
		frag_Color = vec3(float(iLightCount) / (LIGHT_LIMIT - 1));
		return;
	}

	float roughness = cAttribute.r; // 0[smooth]     → 1[matte]
	float metallic = cAttribute.g;  // 0[dielectric] → 1[metal]
	float fresnell = cAttribute.b;  // 0[none]       → ∞[rim]
	float occlusion = cAttribute.a;

	vec3 lightedSurface;
	if(length(realNormal) > 0.1) {
		lightedSurface = applyLighting(
			position, realNormal,
			roughness, metallic, fresnell,
			cAlbedo.rgb);
	} else {
		lightedSurface = cAlbedo.rgb;
	}

	vec3 surface = occlusion * lightedSurface + cEmissive.rgb;

	frag_Color = applyFog(position, surface);
	frag_Position = position;
	frag_Normal = realNormal;
	frag_Attrib = vec3(roughness, metallic, 0.0);
}
