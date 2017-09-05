#version 400

layout(vertices = 4) out;

uniform int iSubdivision;

uniform vec3 vecViewPos;

float distToPivot(vec3 pos)
{
	// ignore height for better sampling
	return length(pos.xz - vecViewPos.xz);
}

float calcTessLevel(int a, int b)
{
	float mindist = min(
		distToPivot(gl_in[a].gl_Position.xyz),
		distToPivot(gl_in[b].gl_Position.xyz));

	float f = clamp((mindist - 50) / 16000, 0, 1);
	f = pow(1 - f, 32);

	return max(iSubdivision * f, 2);
}

void main()
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

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
