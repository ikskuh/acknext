#version 330

vec4 toGamma(vec4 v);

layout(location = 0) out vec4 color;

in vec2 uv;

uniform sampler2D texInput;

uniform float fExposure;

void main()
{
	color = texture(texInput, uv);

	color *= fExposure;

	color = toGamma(color);
}
