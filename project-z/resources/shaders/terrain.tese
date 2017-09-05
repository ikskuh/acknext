#version 400

layout(quads,equal_spacing,ccw) in;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

out vec3 position;
out vec2 uv0;

out float distance;

uniform sampler2D texHeightmap;

vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2, in vec4 v3)
{
	vec4 a = mix(v0, v1, gl_TessCoord.x);
	vec4 b = mix(v3, v2, gl_TessCoord.x);
	return mix(a, b, gl_TessCoord.y);
}

uniform vec3 vecViewPos;

uniform ivec2 vecTerrainSize;
uniform ivec2 vecTileSize;
uniform float fTerrainScale;

void main()
{
	vec3 pos = interpolate(
		gl_in[0].gl_Position,
		gl_in[1].gl_Position,
		gl_in[2].gl_Position,
		gl_in[3].gl_Position).xyz;

	vec2 uv = (pos.xz + 0.5) / vec2(fTerrainScale * vecTerrainSize.x, fTerrainScale * vecTerrainSize.y);

	pos.y = texture(texHeightmap, uv).r;
	uv.y = 1.0 - uv.y; // normal textures are "upside down"

	position = (matWorld * vec4(pos, 1)).rgb;

	distance = length(position - vecViewPos);

	gl_Position = matProj * matView * vec4(position, 1);

	uv0 = uv;
}
