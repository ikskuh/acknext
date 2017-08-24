#include <stdio.h>
#include <acknext.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl3w.h>
#include <math.h>

void debug_tools();

MESH mesh;
BUFFER * bonesBuf;

void myview(void * context)
{
	glClearColor(0.3, 0.3, 1.0, 1.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	opengl_setMesh(&mesh);

	GLuint program = shader_getObject(mesh.material->shader);

	GLuint binding_point_index = 4;
	GLint block_index = glGetUniformBlockIndex(
		program,
		"BoneBlock");
	glBindBufferBase(
		GL_UNIFORM_BUFFER,
		binding_point_index,
		buffer_getObject(bonesBuf));
	glUniformBlockBinding(
		program,
		block_index,
		binding_point_index);

	MATRIX matWorld;
	mat_id(&matWorld);
	matWorld.fields[0][0] = 0.5;
	matWorld.fields[1][1] = 0.5;
	matWorld.fields[2][2] = 0.5;

	MATRIX matProj;
	mat_id(&matProj);
	matProj.fields[0][0] = (var)screen_size.height / (var)screen_size.width;

	opengl_setTransform(&matWorld, mat_id(NULL), &matProj);

	MATRIX * bones = buffer_map(bonesBuf, READWRITE);

	mat_id(&bones[0]);
	mat_id(&bones[1]);
	mat_id(&bones[2]);
	mat_id(&bones[3]);

	bones[1].fields[3][1] = 0.25 * sin(total_time);

	quat_to_mat(bones+2, quat_axis_angle(NULL, vector(0, 0, 1), 30 * cos(total_time)));

	buffer_unmap(bonesBuf);

	opengl_draw(GL_TRIANGLES, 0, 12);
}

void gamemain()
{
	filesys_addResource("/home/felix/projects/acknext/scripts/", "/");

	view_create((RENDERCALL)myview, camera);
	task_defer(debug_tools, NULL);
	event_attach(on_escape, engine_shutdown);

	mesh.vertexBuffer = buffer_create(VERTEXBUFFER);
	mesh.indexBuffer = buffer_create(INDEXBUFFER);
	mesh.material = mtl_create();

	mesh.material->shader = shader_create();
	if(!shader_addFileSource(mesh.material->shader, VERTEXSHADER, "bones.vert")) abort();
	if(!shader_addFileSource(mesh.material->shader, FRAGMENTSHADER, "bones.frag")) abort();
	if(!shader_link(mesh.material->shader)) abort();

	buffer_set(mesh.vertexBuffer, sizeof(VERTEX) * 6, NULL);
	buffer_set(mesh.indexBuffer,  sizeof(INDEX) * 12, NULL);

	VERTEX * vertices = buffer_map(mesh.vertexBuffer, WRITEONLY);
	INDEX  * indices = buffer_map(mesh.indexBuffer, WRITEONLY);

	COLOR car[] = {
		*color_hex(0xFF0000),
	    *color_hex(0x00FF00),
	    *color_hex(0x0000FF),
	    *color_hex(0xFFFF00),
	    *color_hex(0x00FFFF),
	    *color_hex(0xFF00FF),
	};
	for(int i = 0; i < 6; i++)
	{
		vertices[i] = (VERTEX) {
			.position = (VECTOR) {-1 + i%3, -1 + 2*(i/3), 0},
			.color = car[i],
			.bones = { 0, 1, 2, 3 },
			.boneWeights = (UBYTE4) { 255, 0, 0, 0 },
		};
	}

	vertices[3].boneWeights = (UBYTE4){ 0, 255,   0, 0 };
	vertices[4].boneWeights = (UBYTE4){ 0,   0, 255, 0 };
	vertices[5].boneWeights = (UBYTE4){ 0,   0, 255, 0 };

	indices[0] = 0;
	indices[1] = 3;
	indices[2] = 4;

	indices[3] = 0;
	indices[4] = 4;
	indices[5] = 1;

	indices[6] = 1;
	indices[7] = 4;
	indices[8] = 5;

	indices[9]  = 1;
	indices[10] = 5;
	indices[11] = 2;

	buffer_unmap(mesh.indexBuffer);
	buffer_unmap(mesh.vertexBuffer);

	bonesBuf = buffer_create(UNIFORMBUFFER);
	buffer_set(bonesBuf, sizeof(MATRIX) * 10, NULL);
}

int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

