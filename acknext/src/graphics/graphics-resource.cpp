#include <engine.hpp>

char const * srcVertexShader = R"GLSL(#version 330
	layout(location = 0) in vec3 vPosition;
	layout(location = 3) in vec3 vColor;
	layout(location = 4) in vec2 vUV0;
	layout(location = 5) in vec2 vUV1;

	uniform vec3 vecColor;
	uniform mat4 matWorld;
	uniform mat4 matView;
	uniform mat4 matProj;

	out vec3 color;
	out vec2 uv0, uv1;

	void main() {
		gl_Position = matProj * matView * matWorld * vec4(vPosition, 1);
		color = vColor * vecColor;
		uv0 = vUV0;
		uv1 = vUV1;
	}
)GLSL";

char const * srcFragmentShader = R"GLSL(#version 330
	in vec3 color;
	in vec2 uv0;

	uniform sampler2D texColor;

	out vec4 fragment;

	void main() {
		fragment = vec4(color,1) * texture(texColor, uv0);
	}
)GLSL";
