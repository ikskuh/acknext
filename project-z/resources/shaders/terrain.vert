#version 330

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

out vec3 position, color, normal, tangent, cotangent;
out vec2 uv0, uv1;

uniform sampler2D texEmission;

uniform ivec2 vecTerrainSize;

void main() {

	vec3 pos;
	pos.x = float(gl_VertexID % vecTerrainSize.x);
	pos.z = float(gl_VertexID / vecTerrainSize.x);

	vec2 uv = (pos.xz + 0.5) / vec2(vecTerrainSize.x, vecTerrainSize.y);
	uv.y = 1.0 - uv.y;

	pos.y = texture(texEmission, uv).r;

	position = (matWorld * vec4(pos, 1)).rgb;

	gl_Position = matProj * matView * vec4(position, 1);

	normal = vec3(0,1,0); // read from normal map :)
	tangent = vec3(1,0,0);
	color = vec3(1,1,1);
	uv0 = uv;
	uv1 = uv;
	cotangent = cross(tangent, normal);
}
