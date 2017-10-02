#version 400

layout(vertices = 4) out;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

uniform int iSubdivision;

uniform vec3 vecViewPos;

uniform float fArc; // tan(0.5 * fov)

uniform float fTerrainScale;
uniform vec3 vecTesselationParameters;

float distToPivot(vec3 pos)
{
	// ignore height for better sampling
	return length(pos.xz - vecViewPos.xz);
}

vec4 project(vec3 position)
{
    vec4 result = matProj * matView * matWorld * vec4(position, 1);
    result /= result.w;
    return result;
}

float calcTessLevel(int a, int b)
{
	float mindist = min(
		distToPivot(gl_in[a].gl_Position.xyz),
		distToPivot(gl_in[b].gl_Position.xyz));

	float f = vecTesselationParameters.x * fTerrainScale / (mindist * fArc);

	return max(iSubdivision * f, 2);
}

bool offscreen(vec4 pos)
{
	return false;
	if(pos.z < 0)
		return true;
	// take a small threshold so some clipping artifacts are prevented
	return abs(pos.x) > 1.2;
}

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	if(gl_InvocationID == 0)
	{
		vec4 p[4];
		for(int i = 0; i < 4; i++) {
			p[i] = project(gl_in[i].gl_Position.xyz);
		}

		if(offscreen(p[0]) && offscreen(p[1]) && offscreen(p[2]) && offscreen(p[3]))
		{
			gl_TessLevelOuter[1] = 0;
			gl_TessLevelOuter[2] = 0;
			gl_TessLevelOuter[3] = 0;
			gl_TessLevelOuter[0] = 0;
			gl_TessLevelInner[0] = 0;
			gl_TessLevelInner[1] = 0;
		}
		else
		{
			gl_TessLevelOuter[1] = calcTessLevel(0,1);
			gl_TessLevelOuter[2] = calcTessLevel(1,2);
			gl_TessLevelOuter[3] = calcTessLevel(2,3);
			gl_TessLevelOuter[0] = calcTessLevel(3,0);

			float maxLevel = 1;
			for(int i = 0; i < 4; i++) {
				maxLevel = max(maxLevel, gl_TessLevelOuter[i]);
			}

			gl_TessLevelInner[0] = maxLevel;
			gl_TessLevelInner[1] = maxLevel;
		}
	}
}
