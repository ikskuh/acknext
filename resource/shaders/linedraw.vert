#version 330
#define BONES_LIMIT 256

layout(location = 0) in vec3 vPosition;
layout(location = 3) in vec3 vColor;

uniform mat4 matView;
uniform mat4 matProj;

out vec3 color;

void main() {

	gl_Position = matProj * matView * vec4(vPosition, 1);
	color = vColor;
}
