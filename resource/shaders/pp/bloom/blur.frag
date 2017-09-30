#version 330

layout(location = 0) out vec4 color;

in vec2 uv;

uniform sampler2D texInput;

uniform float fExposure;

uniform vec4 vecViewPort;

vec3 cutoff(vec4 cin)
{
	return max(vec3(0), cin.rgb - 1.0);
}

void main()
{
	vec3 sum = vec3(0.0);
	vec2 pixelsize = 0.5 / vec2(vecViewPort.zw);

	for (int x = -4; x <= 4; x++)
	{
		for (int y = -4; y <= 4; y++)
		{
			sum += cutoff(texture(texInput, uv + pixelsize * vec2(x,y)));
		}
	}

	color.rgb = sum / 81.0;
	color.a = 1.0;
}
