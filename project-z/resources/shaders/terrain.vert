#version 330

uniform ivec2 vecTerrainSize;
uniform ivec2 vecTileSize;
uniform float fTerrainScale;

void main()
{
	gl_Position.x = fTerrainScale * vecTileSize.x * float(gl_VertexID % vecTerrainSize.x);
	gl_Position.y = 0;
	gl_Position.z = fTerrainScale * vecTileSize.y * float(gl_VertexID / vecTerrainSize.x);
	gl_Position.w = 1.0;
}
