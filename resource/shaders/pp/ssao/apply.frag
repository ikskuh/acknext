#version 330

layout(location = 0) out vec3 color;

in vec2 uv;

uniform vec4 vecViewPort;

uniform mat4 matView;
uniform mat4 matProj;

uniform sampler2D texInput;
uniform sampler2D texNormal;
uniform sampler2D texPosition;
uniform sampler2D texNoise;

const int kernelSize = 32;
const float kernelScale = 0.05;

void main()
{
	vec2 noiseuv = vecViewPort.zw / 256.0;
	vec2 pixelscale = 1.0 / vecViewPort.zw;

	vec3 pos    = texture(texPosition, uv).rgb;
	vec3 normal = texture(texNormal, uv).rgb;

	vec4 tform = matProj * matView * vec4(pos, 1);
	tform.xyz /= tform.w;

	float samples = 0.0;
	for(int i = 0; i < kernelSize; i++)
	{
		vec3 rand = normalize(2.0 * texture(texNoise, noiseuv * pos.xz + vec2(0.3, 0.6) * uv * i).rgb - 1.0);

		if(dot(rand, normal) < 0)
			rand = -rand;

		vec3 testpos = pos + kernelScale * rand;

		vec4 ss0 = matProj * matView * vec4(testpos, 1);
		ss0.xyz /= ss0.w;

		vec3 refpos = texture(texPosition, 0.5 + 0.5 * ss0.xy).rgb;

		vec4 ss1 = matProj * matView * vec4(refpos, 1);
		ss1.xyz /= ss1.w;

		if(ss1.z < ss0.z && distance(testpos, refpos) < 10 * kernelScale)
			samples += 1.0;
	}
	samples *= (1.0 / float(kernelSize));

	color = vec3(1.0 - samples);
}
