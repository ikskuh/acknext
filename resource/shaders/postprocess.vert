#version 330
#define BONES_LIMIT 256

layout(location = 0) in vec3 vPosition;
layout(location = 4) in vec2 vUV0;

out vec2 uv;

void main()
{
	gl_Position = vec4(vPosition.xy, 0, 1);
	uv = vUV0;
}
