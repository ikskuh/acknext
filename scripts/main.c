#include <acknext.h>

#define GLSL(x) ("#version 330\n" #x)

void main()
{
    ENTITY * ent = ent_create("warlock.obj", (VECTOR){ 0, 0, 0 }, NULL);

	camera->position = vector(0, 0, -10);
	camera->widget.position = (POINT){200, 150};
	camera->widget.size = (SIZE){400, 300};

	SHADER * shader = shader_create();

	shader_addSource(shader, VERTEX_SHADER, GLSL(
		uniform mat4 matWorldViewProj;
		void main() {
			gl_Position = matWorldViewProj * vec4(0, 0, 0, 1);
		}
	));
	shader_addSource(shader, FRAGMENT_SHADER, GLSL(
		out vec4 color;
		void main() {
			color = vec4(1,0,0,1);
		}
	));
	shader_link(shader);

	int len = shader_getUniformCount(shader);
	for(int i = 0; i < len; i++) {
		UNIFORM const * uni = shader_getUniform(shader, i);
		engine_log("[%d] %s : %d",
		   uni->location,
		   uni->name,
		   uni->type);
	}
}
