#version 330

layout(location = 0) out vec4 color;

in vec2 uv;

uniform sampler2D texInput;
uniform sampler2D texBloom;

void main()
{
	color = texture(texInput, uv) + texture(texBloom, uv);
	color.a = 1.0;
}
