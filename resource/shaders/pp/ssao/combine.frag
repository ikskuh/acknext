#version 330

layout(location = 0) out vec4 color;

in vec2 uv;

uniform sampler2D texInput;
uniform sampler2D texOcclusion;

void main()
{
	color = texture(texInput, uv) * max(vec4(0.2), texture(texOcclusion, uv).rrrr);
}
