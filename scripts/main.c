#include <acknext.h>

#include <GL/gl3w.h>

void main()
{
    ENTITY * ent = ent_create("warlock.obj", (VECTOR){ 0, 0, 0 }, NULL);

	camera->position = vector(0, 0, -10);
	camera->widget.position = (POINT){200, 150};
	camera->widget.size = (SIZE){400, 300};

	VERTEX vertices[4];
	vertices[0].position = vector(0, 0, 0);
	vertices[1].position = vector(1, 0, 0);
	vertices[2].position = vector(0, 1, 0);
	vertices[3].position = vector(1, 1, 0);

	vertices[0].color = hexcolor(0xFFFFFFFF);
	vertices[1].color = hexcolor(0xFFFFFFFF);
	vertices[2].color = hexcolor(0xFFFFFFFF);
	vertices[3].color = hexcolor(0xFFFFFFFF);

	vertices[0].texcoord0 = (UVCOORD) { 0.0f, 0.0f };
	vertices[1].texcoord0 = (UVCOORD) { 1.0f, 0.0f };
	vertices[2].texcoord0 = (UVCOORD) { 0.0f, 1.0f };
	vertices[3].texcoord0 = (UVCOORD) { 1.0f, 1.0f };

	INDEX indices[6] = { 0, 1, 2, 1, 2, 3 };

	BUFFER * vbuf = buffer_create(VERTEX_BUFFER);
	buffer_set(vbuf, sizeof(vertices), vertices);

	BUFFER * ibuf = buffer_create(INDEX_BUFFER);
	buffer_set(ibuf, sizeof(indices), indices);

	MATERIAL * mtl = mtl_create();
	mtl->colorTexture = bmap_load("texture.png");

	engine_flags |= CUSTOMDRAW;

	opengl_setIndexBuffer(ibuf);
	opengl_setVertexBuffer(vbuf);
	opengl_setMaterial(mtl);

	while(true)
	{
		wait();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		opengl_draw(GL_TRIANGLES, 0, 6);

		engine_swap();
	}
}
