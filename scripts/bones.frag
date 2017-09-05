#version 330

in vec3 position;
in vec3 tangent;
in vec3 cotangent;
in vec3 color;
in vec2 uv0;
in vec3 normal;

out vec4 fragment;

void main()
{
	fragment.rgb = color;
	fragment.a = 1.0;
}
