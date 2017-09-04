#include <stdio.h>
#include <acknext.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl3w.h>
#include <math.h>

void debug_tools();

BUFFER * bonesBuf;
MODEL * model;
MATERIAL * material;

void putmat(char const * title, MATRIX const * mat)
{
	engine_log("%s\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f",
		title,
		mat->fields[0][0], mat->fields[0][1], mat->fields[0][2], mat->fields[0][3],
		mat->fields[1][0], mat->fields[1][1], mat->fields[1][2], mat->fields[1][3],
		mat->fields[2][0], mat->fields[2][1], mat->fields[2][2], mat->fields[2][3],
		mat->fields[3][0], mat->fields[3][1], mat->fields[3][2], mat->fields[3][3]);
}

void myview(void * context)
{
	glClearColor(0.3, 0.3, 1.0, 1.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if(key_space || key_o) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	GLuint program = material->shader->object;

	GLuint binding_point_index = 4;
	GLint block_index = glGetUniformBlockIndex(
		program,
		"BoneBlock");
	glBindBufferBase(
		GL_UNIFORM_BUFFER,
		binding_point_index,
		bonesBuf->object);
	glUniformBlockBinding(
		program,
		block_index,
		binding_point_index);

	MATRIX matWorld;
	mat_id(&matWorld);
	// matWorld.fields[0][0] = 0.5;
	// matWorld.fields[1][1] = 0.5;
	// matWorld.fields[2][2] = 0.5;

	MATRIX matView, matProj;
	camera_to_matrix(context, &matView, &matProj, NULL);

	MATRIX transforms[ACKNEXT_MAX_BONES];
	transforms[0] = model->bones[0].transform;
	for(int i = 1; i < model->boneCount; i++)
	{
		BONE * bone = &model->bones[i];
		mat_mul(&transforms[i], &bone->transform, &transforms[bone->parent]);
	}

	MATRIX * boneTrafos = buffer_map(bonesBuf, READWRITE);
	for(int i = 0; i < model->boneCount; i++)
	{
		BONE * bone = &model->bones[i];
		mat_mul(&boneTrafos[i], &bone->bindToBoneTransform, &transforms[i]);
	}
	buffer_unmap(bonesBuf);

	opengl_setMaterial(material);

	for(int i = 0; i < model->meshCount; i++)
	{
		MESH * mesh = model->meshes[i];

		opengl_setIndexBuffer(mesh->indexBuffer);
		opengl_setVertexBuffer(mesh->vertexBuffer);

		opengl_setTransform(&matWorld, &matView, &matProj);

		opengl_draw(GL_TRIANGLES, 0, mesh->indexBuffer->size / sizeof(INDEX));
	}

	opengl_drawDebug(&matView, &matProj);
}

MODEL * load_bonestructure(char const * file);

int selection = 0;

void drawlines(MODEL * model)
{
	MATRIX mat;
	mat_id(&mat);

	MATRIX transforms[ACKNEXT_MAX_BONES];
	VECTOR positions[ACKNEXT_MAX_BONES];
	transforms[0] = model->bones[0].transform;

	positions[0] = nullvector;
	vec_transform(&positions[0], &transforms[0]);

	for(int i = 1; i < model->boneCount; i++)
	{
		BONE * bone = &model->bones[i];
		mat_mul(&transforms[i], &bone->transform, &transforms[bone->parent]);

		positions[i] = nullvector;
		vec_transform(&positions[i], &transforms[i]);

		draw_line3d(&positions[i], &positions[bone->parent], &COLOR_GREEN);

		if(i == selection) {
			draw_point3d(&positions[i], &COLOR_RED);
		} else {
			draw_point3d(&positions[i], &COLOR_BLUE);
		}
	}
}

void up() { ++selection; }
void down() { --selection; }

void animate(MODEL * model, char const * name, double frameTime);

void gamemain()
{
	filesys_addResource("/home/felix/projects/acknext/scripts/", "/");

	view_create((RENDERCALL)myview, camera);
	task_defer(debug_tools, NULL);
	event_attach(on_escape, engine_shutdown);

	event_attach(on_kp_plus, up);
	event_attach(on_kp_minus, down);

	bonesBuf = buffer_create(UNIFORMBUFFER);
	buffer_set(bonesBuf, sizeof(MATRIX) * ACKNEXT_MAX_BONES, NULL);

	model = load_bonestructure("/home/felix/projects/acknext/scripts/wuson.x");

	material = mtl_create();
	material->shader = shader_create();
	if(!shader_addFileSource(material->shader, VERTEXSHADER, "bones.vert")) abort();
	if(!shader_addFileSource(material->shader, FRAGMENTSHADER, "bones.frag")) abort();
	if(!shader_link(material->shader)) abort();

	while(model)
	{
		if(selection > 0 && selection < model->boneCount)
		{
			MATRIX mat;
			quat_to_mat(
				&mat,
				quat_axis_angle(
					NULL,
					vector(0,1,0),
					30.0 * (key_kp_9 - key_kp_3) * time_step));

			mat_mul(
				&model->bones[selection].transform,
				&model->bones[selection].transform,
				&mat);
		}

		if(key_n)
		{
			//animate(model, "AnimationSet0", total_time);
			animate(model, "Wuson_Run", total_time);
		}

		drawlines(model);
		task_yield();
	}
}

int main(int argc, char *argv[])
{
	return engine_main(gamemain, argc, argv);
}

