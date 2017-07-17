#include <acknext.h>

#include <GL/gl3w.h>

#define GLSL(x) ("#version 330\n" #x)

void main()
{
    ENTITY * ent = ent_create("warlock.obj", (VECTOR){ 0, 0, 0 }, NULL);

	camera->position = vector(0, 0, -10);
	camera->widget.position = (POINT){200, 150};
	camera->widget.size = (SIZE){400, 300};

	SHADER * shader = shader_create();

	shader_addSource(shader, VERTEX_SHADER, GLSL(
		layout(location = 0) in vec3 vPosition;
		layout(location = 3) in vec3 vColor;
		out vec3 color;
		void main() {
			gl_Position = vec4(vPosition, 1);
			color = vColor;
		}
	));
	shader_addSource(shader, FRAGMENT_SHADER, GLSL(
		in vec3 color;
		out vec4 fragment;
		void main() {
			fragment = vec4(color,1);
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

	VERTEX vertices[3];
	vertices[0].position = vector(0, 0, 0);
	vertices[1].position = vector(1, 0, 0);
	vertices[2].position = vector(0, 1, 0);

	vertices[0].color = hexcolor(0xFF00FFFF);
	vertices[1].color = hexcolor(0xFFFFFF00);
	vertices[2].color = hexcolor(0xFFFF00FF);

	INDEX indices[3] = { 0, 1, 2 };

	BUFFER * vbuf = buffer_create(VERTEX_BUFFER);
	buffer_set(vbuf, sizeof(vertices), vertices);

	BUFFER * ibuf = buffer_create(INDEX_BUFFER);
	buffer_set(ibuf, sizeof(indices), indices);

	GLuint pgm = shader_getObject(shader);

	engine_flags |= CUSTOMDRAW;

	glUseProgram(pgm);

	while(true)
	{
		wait();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		opengl_setIndexBuffer(ibuf);
		opengl_setVertexBuffer(vbuf);

		opengl_draw(GL_TRIANGLES, 0, 3);

		engine_swap();
	}
}
