#version 330

layout(location = 0) out vec4 color;

in vec2 uv;

uniform sampler2D texInput;

uniform float fExposure;

uniform vec4 vecViewPort;

uniform float fCutoff;

uniform vec2 vecBlurScale;

vec3 cutoff(vec4 cin)
{
	return max(vec3(0), cin.rgb - fCutoff);
}

void main()
{
	vec3 sum = vec3(0.0);
	vec2 pixelsize = vecBlurScale / vec2(vecViewPort.zw);

	const float[] kernel = float[13](
		0.068786, 0.072672, 0.076014, 0.078719, 0.08071, 0.081929, 0.082339, 0.081929, 0.08071, 0.078719, 0.076014, 0.072672, 0.068786
	);

	for (int x = -6; x <= 6; x++)
	{
		for (int y = -6; y <= 6; y++)
		{
			sum += kernel[x+6] * kernel[y+6] * cutoff(texture(texInput, uv + pixelsize * vec2(x,y)));
		}
	}

	color.rgb = sum;
	color.a = 1.0;
}
