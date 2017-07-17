#include <acknext.h>

#define GLSL(x) #x

void main()
{
    ENTITY * ent = ent_create("warlock.obj", (VECTOR){ 0, 0, 0 }, NULL);

	camera->position = vector(0, 0, -10);
	camera->widget.position = (POINT){200, 150};
	camera->widget.size = (SIZE){400, 300};

	SHADER * shader = shader_create();

	shader_addSource(shader, VERTEX_SHADER, GLSL(
		int main() {
			gl_Position = vec4(0, 0, 0, 1);
		}
	));
	shader_addSource(shader, FRAGMENT_SHADER, GLSL(
		out vec4 color;
		int main() {
			color = vec4(1,0,0,1);
		}
	));
	shader_link(shader);
}
