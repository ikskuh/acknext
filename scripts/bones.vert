#version 330
#define BONES_LIMIT 10

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec3 vColor;
layout(location = 4) in vec2 vUV0;
layout(location = 5) in vec2 vUV1;
layout(location = 6) in vec4 vBones;
layout(location = 7) in vec4 vBoneWeight;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

layout(std140) uniform BoneBlock
{
	 mat4 bones[BONES_LIMIT];
};

out vec3 position, color, normal, tangent, cotangent;
out vec2 uv0, uv1;

vec4 applyBoneTransform(vec4 inval)
{
	return vBoneWeight.x * bones[int(vBones.x)] * inval
		+ vBoneWeight.y * bones[int(vBones.y)] * inval
		+ vBoneWeight.z * bones[int(vBones.z)] * inval
		+ vBoneWeight.w * bones[int(vBones.w)] * inval;
}

void main() {

	vec3 mPosition = applyBoneTransform(vec4(vPosition, 1.0)).xyz;
	vec3 mNormal   = applyBoneTransform(vec4(vNormal, 0.0)).xyz;
	vec3 mTangent  = applyBoneTransform(vec4(vTangent, 0.0)).xyz;

	position = (matWorld * vec4(mPosition, 1)).rgb;
	gl_Position = matProj * matView * vec4(position, 1);
	normal = normalize((matWorld * vec4(mNormal, 0.0) ).xyz);
	tangent = normalize(( matWorld * vec4(mTangent, 0.0) ).xyz);
	color = vColor;
	uv0 = vUV0;
	uv1 = vUV1;
	cotangent = cross(tangent, normal);
}
