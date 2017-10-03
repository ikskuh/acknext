#version 330

uniform ivec2 vecTerrainSize;
uniform ivec2 vecTileSize;
uniform float fTerrainScale;

out vec2 vertUV;

void main()
{
	gl_Position.x = fTerrainScale * vecTileSize.x * float(gl_VertexID % vecTerrainSize.x);
	gl_Position.y = 0;
	gl_Position.z = fTerrainScale * vecTileSize.y * float(gl_VertexID / vecTerrainSize.x);

	gl_Position.x -= 0.5 * fTerrainScale * vecTerrainSize.x;
	gl_Position.z -= 0.5 * fTerrainScale * vecTerrainSize.y;

	gl_Position.w = 1.0;
}