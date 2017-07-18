#include <acknext.h>

#include <GL/gl3w.h>

void main()
{
    ENTITY * ent = ent_create("warlock.obj", (VECTOR){ 0, 0, 0 }, NULL);

	camera->position = vector(0, 0, -10);
	camera->widget.position = (POINT){200, 150};
	camera->widget.size = (SIZE){400, 300};

	VERTEX vertices[8];
	vertices[0].position = vector(0, 0, 0);
	vertices[1].position = vector(1, 0, 0);
	vertices[2].position = vector(0, 1, 0);

	vertices[0].color = hexcolor(0xFFFFFFFF);
	vertices[1].color = hexcolor(0xFFFFFFFF);
	vertices[2].color = hexcolor(0xFFFFFFFF);

	vertices[0].texcoord0 = (UVCOORD) { 0.0f, 0.0f };
	vertices[1].texcoord0 = (UVCOORD) { 1.0f, 0.0f };
	vertices[2].texcoord0 = (UVCOORD) { 0.0f, 1.0f };

	INDEX indices[3] = { 0, 1, 2 };

	BUFFER * vbuf = buffer_create(VERTEX_BUFFER);
	buffer_set(vbuf, sizeof(vertices), vertices);

	BUFFER * ibuf = buffer_create(INDEX_BUFFER);
	buffer_set(ibuf, sizeof(indices), indices);

	MATERIAL * mtl = mtl_create();
	mtl->color = hexcolor(0xFFFFFFFF);

	uint32_t texture[4];
	texture[0] = 0xFF0000FF;
	texture[1] = 0xFF00FF00;
	texture[2] = 0xFFFF0000;
	texture[3] = 0xFFFFFFFF;
	mtl->colorTexture = bmap_create();
	bmap_set(mtl->colorTexture, 2, 2, RGBA8, texture);

	engine_flags |= CUSTOMDRAW;

	opengl_setIndexBuffer(ibuf);
	opengl_setVertexBuffer(vbuf);
	opengl_setMaterial(mtl);

	while(true)
	{
		wait();
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		opengl_draw(GL_TRIANGLES, 0, 3);

		engine_swap();
	}
}
